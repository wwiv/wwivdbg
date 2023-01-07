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

#include <string>
#include <optional>

bool
DebugProtocol::Receive(const httplib::Request &req, httplib::Response &res,
  const httplib::ContentReader& content_reader, DebugMessage::Type t) {
  if (req.is_multipart_form_data()) {
    return false;
  }
  std::string body;
  content_reader([&](const char *data, size_t data_length) {
    body.append(data, data_length);
    return true;
  });
  std::lock_guard lock(mu_);
  queue_.emplace_back(t, body);
  res.set_content("ok", "text/plain");
  return true;
}


  DebugProtocol::DebugProtocol() {

  svr_ = std::make_unique<httplib::Server>();

  svr_->Get("/status", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("ok", "text/plain");
  });
  svr_->Post("/stack", [&](const httplib::Request &req, httplib::Response &res,
                           const httplib::ContentReader &content_reader) {
    return Receive(req, res, content_reader, DebugMessage::Type::STACK);
  });
  svr_->Post("/source",
             [&](const httplib::Request &req, httplib::Response &res,
                 const httplib::ContentReader &content_reader) {
    return Receive(req, res, content_reader, DebugMessage::Type::SOURCE);
  });

  svr_->Post("/breakpoints", [&](const httplib::Request &req,
                                 httplib::Response &res,
                                 const httplib::ContentReader &content_reader) {
    return Receive(req, res, content_reader, DebugMessage::Type::BREAKPOINTS);
  });

  srv_thread_ = std::thread([this] {
    svr_->listen("0.0.0.0", 9949);
  });
}

DebugProtocol::~DebugProtocol() { 
  cli_.reset(nullptr);

  // Stop the server then join the thread to wait for it to exit.
  svr_->stop();
  srv_thread_.join();
}

bool DebugProtocol::has_message() const {
  std::lock_guard lock(mu_);
  return !queue_.empty();
}

DebugMessage DebugProtocol::next() {
  std::lock_guard<std::mutex> lock(mu_);
  auto m = queue_.front();
  queue_.pop_front();
  return m;
}

void DebugProtocol::add(DebugMessage&& msg) {
  std::lock_guard<std::mutex> lock(mu_);
  queue_.push_back(msg);
}
