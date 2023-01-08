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

#include "commands.h"
#include "fmt/format.h"
#include <string>
#include <optional>

#define Uses_TApplication

#include "tvision/tv.h"


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

bool DebugProtocol::UpdateState() { 
  if (auto state = Get("state")) {
    state_ = state.value();
    // Source does not go into the queue
    ptrdiff_t t = static_cast<int>(DebugMessage::Type::STATE);
    message(app_, evBroadcast, cmDebugStateChanged,
            reinterpret_cast<void *>(t));
    return true;
  }
  return false;
}

bool DebugProtocol::attached() const {
  std::lock_guard<std::mutex> lock(mu_);
  return attached_;
}

bool DebugProtocol::Attach() {
  {
    std::lock_guard<std::mutex> lock(mu_);
    if (attached_) {
      return false;
    }
  }
  if (auto body = Post("attach")) {
    {
      std::lock_guard<std::mutex> lock(mu_);
      attached_ = true;
    }
    //TEvent event{};
    //event.what = evBroadcast;
    //event.message.command = cmDebugAttached;
    //desktop_->handleEvent(event);
    auto handler = message(app_, evBroadcast, cmDebugAttached, 0);
    return true;
  }
  return false;
}

bool DebugProtocol::Detach() {
  {
    std::lock_guard<std::mutex> lock(mu_);
    if (!attached_) {
      return false;
    }
  }
  if (auto body = Post("detach")) {
    {
      std::lock_guard<std::mutex> lock(mu_);
      attached_ = true;
    }
    message(desktop_, evBroadcast, cmDebugDetached, 0);
    return true;
  }
  return false;
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
  if (auto res = cli_->Post(uri, "wwivdbg v0", "text/plain")) {
    if (res->status != 200) {
      // TODO(rushfan): Log error.
      return std::nullopt;
    }
    return res->body;
  }
  return std::nullopt;
}
