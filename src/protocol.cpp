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

bool DebugProtocol::has_message(DebugMessage::Type t) const {
  std::lock_guard lock(mu_);
  if (auto it = queue_.find(t); it != queue_.end()) {
    return !it->second.empty();
  }
  return false;
}

DebugMessage DebugProtocol::next(DebugMessage::Type t) {
  std::lock_guard<std::mutex> lock(mu_);
  if (auto it = queue_.find(t); it != queue_.end()) {
    auto m = it->second.front();
    it->second.pop_front();
    return m;
  }
  return DebugMessage(DebugMessage::Type::STOPPED, "");
}

DebugMessage DebugProtocol::peek(DebugMessage::Type t) {
  std::lock_guard<std::mutex> lock(mu_);
  if (auto it = queue_.find(t); it != queue_.end()) {
    if (!it->second.empty()) {
      return it->second.front();
    }
  }
  return DebugMessage(DebugMessage::Type::STOPPED, "");
}

void DebugProtocol::add(DebugMessage&& msg) {
  bool added{false};
  {
    std::lock_guard<std::mutex> lock(mu_);
    // create or replace
    queue_[msg.msgType].push_back(msg);
    added = true;
  }
  if (added) {
    ptrdiff_t t = static_cast<int>(msg.msgType);
    message(app_, evBroadcast, cmDebugAvail, reinterpret_cast<void *>(t));
  }
}

bool DebugProtocol::UpdateSource() {
  if (auto source = Get("source")) {
    source_ = source.value();
    // Source does not go into the queue
    ptrdiff_t t = static_cast<int>(DebugMessage::Type::SOURCE);
    message(app_, evBroadcast, cmDebugSourceChanged,
            reinterpret_cast<void *>(t));
    return true;
  }
  return false;
}

bool DebugProtocol::UpdateCallStack() { 
  if (auto source = Get("stack")) {
    add({DebugMessage::Type::STACK, source.value()});
    return true;
  }
  return false;
}

using json = nlohmann::json;
bool DebugProtocol::UpdateState(const std::string& s) {
  json j = json::parse(s);
  const auto& loc = j["location"];
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
  Post("tracein");
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
