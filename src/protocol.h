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

#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class DebugMessage {
public:
  enum class Type { STOPPED, RUNNING, SOURCE, STACK, BREAKPOINTS };
  DebugMessage(Type t, std::string m)
      : msgType(t), message(m) {}

  Type msgType;
  std::string message;
};

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
  DebugProtocol();
  ~DebugProtocol();

  bool has_message() const;
  DebugMessage next();
  void add(DebugMessage&& msg);


private:

  bool
  Receive(const httplib::Request &req, httplib::Response &res,
          const httplib::ContentReader &content_reader, DebugMessage::Type t);

  std::thread srv_thread_;
  std::unique_ptr<httplib::Server> svr_;
  std::thread cli_thread_;
  std::unique_ptr<httplib::Client> cli_;
  std::deque<DebugMessage> queue_;
  mutable std::mutex mu_;
};


#endif
