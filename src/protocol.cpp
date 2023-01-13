/*
 * Copyright (C)2023 WWIV Software Services
 *
 * Licensed  under the  Apache License, Version  2.0 (the "License");
 * you may not use this  file  except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless  required  by  applicable  law  or agreed to  in  writing,
 * software  distributed  under  the  License  is  distributed on an
 * "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,
 * either  express  or implied.  See  the  License for  the specific
 * language governing permissions and limitations under the License.
 */

#include "protocol.h"
#include "httplib.h"
#include "strings.h"
#include "commands.h"
#include "fmt/format.h"
#include <string>
#include <optional>

#define Uses_TApplication

#include "tvision/tv.h"
#include <nlohmann/json.hpp>


DebugProtocol::DebugProtocol(TApplication *app, TView *desktop,
                             const std::string &host, int port)
    : app_(app), desktop_(desktop), host_(host), port_(port) {
  cli_ = std::make_unique<httplib::Client>(host, port);
}

DebugProtocol::~DebugProtocol() { 
  cli_.reset(nullptr);
}


bool DebugProtocol::UpdateSource() {
  if (auto source = Get("source")) {
    source_ = source.value();
    // Source does not go into the queue
    message(app_, evBroadcast, cmDebugSourceChanged, 0);
    return true;
  }
  return false;
}

using json = nlohmann::json;
bool DebugProtocol::UpdateState(const std::string& s) {
  json j = json::parse(s);
  const auto& loc = j["location"];
  state_.initial_module = j["initial_module"].get<std::string>();
  state_.module = loc["module"].get<std::string>();
  state_.pos = loc["pos"].get<int>();
  state_.row = loc["row"].get<int>();
  state_.col = loc["col"].get<int>();

  const auto& jvars = j["vars"];
  variables_.clear();
  for (const auto& v : jvars) {
    variables_.push_back(v.get<Variable>());
  }

  const auto& jstack = j["stack"];
  stack_.clear();
  for (const auto& v : jstack) {
    stack_.push_back(v.get<std::string>());
  }

  const auto& jbreakpoints = j["breakpoints"];
  for (const auto& v : jbreakpoints) {
    breakpoints_.UpdateRemote(v);
    variables_.push_back(v.get<Variable>());
  }

  // State does not go into the queue
  message(app_, evBroadcast, cmDebugStateChanged, 0);
  return true;
}

bool DebugProtocol::UpdateState() { 
  if (auto state = Get("state")) {
    return UpdateState(state.value());
  }
  return false;
}

bool DebugProtocol::attached() const {
  std::lock_guard<std::mutex> lock(mu_);
  return attached_;
}

void DebugProtocol::set_attached(bool a) {
  std::lock_guard<std::mutex> lock(mu_);
  attached_ = a;
}

bool DebugProtocol::Attach() {
  if (attached()) {
    return false;
  }
  if (auto body = Post("attach")) {
    json j = json::parse(body.value());
    state_.initial_module = j["initial_module"].get<std::string>();
    set_attached(true);
    message(app_, evBroadcast, cmDebugAttached, 0);
    return true;
  }
  return false;
}

bool DebugProtocol::Detach() {
  if (!attached()) {
    return false;
  }
  if (auto body = Post("detach")) {
    set_attached(false);
    message(app_, evBroadcast, cmDebugDetached, 0);
    return true;
  }
  // detach anyway if we got an error here.
  message(app_, evBroadcast, cmDebugDetached, 0);
  return false;
}

bool DebugProtocol::StepOver() {
  if (!attached()) {
    return false;
  }
  if (auto state = Post("stepover")) {
    return UpdateState(state.value());
  }
  return UpdateState();
}


bool DebugProtocol::TraceIn() {
  if (!attached()) {
    return false;
  }
  if (auto state = Post("tracein")) {
    return UpdateState(state.value());
  }
  return UpdateState();
}

std::optional<std::string> DebugProtocol::Get(const std::string& part) {
  const auto uri = fmt::format("/debug/v1/{}", part);
  if (auto res = cli_->Get(uri)) {
    if (res->status != 200) {
      // TODO(rushfan): Log error.
      return std::nullopt;
    }
    return res->body;
  }
  return std::nullopt;
}

std::optional<std::string> DebugProtocol::Post(const std::string &part) {
  const auto uri = fmt::format("/debug/v1/{}", part);
  if (auto res = cli_->Post(uri)) {
    if (res->status != 200) {
      // TODO(rushfan): Log error.
      return std::nullopt;
    }
    return res->body;
  }
  return std::nullopt;
}


// JSON

void to_json(nlohmann::json& j, const Variable& v) {
  j = nlohmann::json{ 
    {"name", v.name}, 
    {"type", v.type}, 
    {"frame", v.frame}, 
    {"value", v.value} };
}

void from_json(const nlohmann::json& j, Variable& v) {
  j.at("name").get_to(v.name);
  j.at("type").get_to(v.type);
  j.at("frame").get_to(v.frame);
  j.at("value").get_to(v.value);
}


// Breakpoints

/*
"breakpoints": [
  {
    "function_name": "",
    "hit_count": 0,
    "id": 2,
    "line": 10,
    "module": "",
    "typ": "line"
  }
],
*/
void Breakpoints::UpdateRemote(const nlohmann::json& b) {
  std::string module = b["module"];
  int line = b["line"];
  std::string typ = b["typ"];
  int64_t remote_id = b["id"];

  std::vector<Breakpoint> toadd;
  for (auto& lb : breakpoints) {
    if (lb.remote_id == remote_id) {
      // Have one already on the server
      return;
    }
    else if ((lb.module.empty() || lb.module == module) && lb.line == line && typ == "line") {
      // Matching one, not on server.
      // TODO(rushfan): Push this one to the server and update ID
      return;
    }
  }

  // We have a new one, add it.
  Breakpoint newb;
  newb.module =  module;
  newb.line = line;
  newb.published = true;
  newb.remote_id = remote_id;
  breakpoints.emplace_back(newb);
}

void Breakpoints::NewLine(const std::string& module, int line) {
  for (auto& lb : breakpoints) {
    if ((lb.module.empty() || lb.module == module) && lb.line == line) {
      // Matching one, not on server.
      // TODO(rushfan): Push this one to the server and update ID
      return;
    }
  }
  Breakpoint newb;
  newb.module = module;
  newb.line = line;
  newb.published = false;
  breakpoints.emplace_back(newb);

}
