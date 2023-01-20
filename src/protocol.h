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
#include <vector>

// copied from wwiv
struct Variable {
  std::string name;
  std::string type;
  std::string value;
  int frame;
};
void to_json(nlohmann::json& j, const Variable& p);
void from_json(const nlohmann::json& j, Variable& p);

class DebugState {
public:
  std::string initial_module;
  std::string module;
  int pos{0};
  int row{0};
  int col{0};

  void clear() {
    initial_module.clear();
    module.clear();
    pos = row = col = 0;
  }
};

// Local
struct Breakpoint {
  // remote ID
  int remote_id{ -1 };
  int line{ -1 };
  std::string module;
  // always line so far
  std::string typ;
  bool published{ false };
  int hit_count{ 0 };
};


class TApplication;
class TView;

namespace httplib {
class Client;
class ContentReader;
using Params = std::multimap<std::string, std::string>;
class Response;
class Request;
class Server;
}

class Breakpoints {
public:
  void UpdateRemote(const nlohmann::json& b);
  void NewLocalLine(const std::string& module, int line);
  std::vector<Breakpoint> breakpoints;
};
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

  bool UpdateSource();
  // Latest updated source
  std::string source() { return source_;  }
  std::vector<Variable> vars() { return variables_; }
  std::vector<std::string> stack() { return stack_; }
  Breakpoints& breakpoints() { return breakpoints_; }
  DebugState state() { return state_; }
  bool UpdateState(const std::string& state, bool fireEvent = true);
  bool UpdateState();
  // Gets the state, good for being called asynchronously.
  std::pair<std::string, bool> GetState();
  bool Attach();
  bool Detach();
  bool CreateBreakpoint(const std::string& module, const std::string& typ, int line);
  bool DeleteBreakpoint(int id);
  bool StepOver();
  bool TraceIn();
  bool Run();
  bool attached() const;
  void set_attached(bool a);
  void NewLineBreakpoint(const std::string& module, int line);
  void UpdateLocalBreakpoints();
  void setHost(const std::string& host) { host_ = host; }
  void setPort(int port) { port_ = port; }

private:
  std::optional<std::string> Get(const std::string &part, bool handleError = true);
  std::optional<std::string> Delete(const std::string &part);
  std::optional<std::string> Post(const std::string &part);
  std::optional<std::string> Post(const std::string &part, const httplib::Params& params);

  std::unique_ptr<httplib::Client> cli_;
  mutable std::mutex mu_;
  TApplication* app_{ nullptr };
  TView* desktop_{ nullptr };
  std::string host_{ "127.0.0.1" };
  int port_{ 9948 };
  bool attached_{false};

  std::string source_;
  std::vector<Variable> variables_;
  std::vector<std::string> stack_;
  Breakpoints breakpoints_;
  DebugState state_{};
};


#endif
