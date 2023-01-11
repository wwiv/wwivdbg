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

#ifndef INCLUDED_WWIVDBG_PROTOCOL_H
#define INCLUDED_WWIVDBG_PROTOCOL_H

#include <nlohmann/json_fwd.hpp>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

// copied from wwiv
struct Variable {
  std::string name;
  std::string type;
  std::string value;
};
void to_json(nlohmann::json& j, const Variable& p);
void from_json(const nlohmann::json& j, Variable& p);

class DebugMessage {
public:
  enum class Type { STOPPED, RUNNING, SOURCE, STACK, STATE, BREAKPOINTS };
  DebugMessage(Type t, std::string m)
      : msgType(t), message(m) {}

  Type msgType;
  std::string message;
};

class DebugState {
public:
  std::string module;
  int pos{0};
  int row{0};
  int col{0};
};

class TApplication;
class TView;

namespace httplib {
class Client;
class ContentReader;
class Response;
class Request;
class Server;
}

/**
 * @brief  Handles debug communication with scripte runner.
 * 
 * Debugee  port 9948 (WWIV on touch pad)
 * Debugger port 9949 (WWIV + 1 on touch pad)
 */
class DebugProtocol {
public:
  DebugProtocol(TApplication* app, TView *desktop, const std::string &host, int port);
  ~DebugProtocol();

  bool has_message(DebugMessage::Type t) const;
  DebugMessage next(DebugMessage::Type t);
  DebugMessage peek(DebugMessage::Type t);

  void add(DebugMessage&& msg);

  bool UpdateSource();
  // Latest updated source
  std::string source() { return source_;  }
  std::vector<Variable> vars() { return variables_; }
  DebugState state() { return state_; }
  bool UpdateCallStack();
  bool UpdateState(const std::string& state);
  bool UpdateState();
  bool Attach();
  bool Detach();
  bool StepOver();
  bool TraceIn();
  bool attached() const;
  void set_attached(bool a);


private:
  std::optional<std::string> Get(const std::string &part);
  std::optional<std::string> Post(const std::string &part);

  std::unique_ptr<httplib::Client> cli_;
  std::map<DebugMessage::Type, std::deque<DebugMessage>> queue_;
  mutable std::mutex mu_;
  TApplication *app_; 
  TView *desktop_;
  const std::string host_;
  const int port_;
  bool attached_{false};

  std::string source_;
  std::vector<Variable> variables_;
  DebugState state_;
};


#endif
