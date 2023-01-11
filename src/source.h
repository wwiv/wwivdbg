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

#ifndef INCLUDED_WWIVDBG_SOURCE_H
#define INCLUDED_WWIVDBG_SOURCE_H

#define Uses_TScrollBar
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"

#include <memory>
#include <string>
#include <vector>

class DebugProtocol;

class TSourcePane : public TScroller {

public:
  TSourcePane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb,
              DebugProtocol* debug);
  ~TSourcePane() = default;
  virtual void draw();

  // Sets the text buffer either as lines or text.
  void SetText(const std::string &text);
  void SetText(const std::vector<std::string> &text);

  void UpdateLocation(int pos, int row, int col) {
    current_pos_ = pos;
    current_line_ = std::max(0, row - 1);
    current_col_ = col;
    scrollTo(current_col_, current_line_);
  }

private:
  std::vector<std::string> lines;
  int current_pos_{0};
  int current_line_{0};
  int current_col_{0};
  DebugProtocol *debug_;
};

class TSourceWindow : public TWindow {
private:
  TSourcePane *fp;
  TScrollBar *hsb, *vsb;
  std::shared_ptr<DebugProtocol> debug_;

public:
  TSourceWindow(TRect r, const std::shared_ptr<DebugProtocol>& debug);
  ~TSourceWindow();

  virtual void handleEvent(TEvent &event) override;
  TScrollBar* standardScrollBar(ushort aOptions);
  void SetText(const std::vector<std::string> &text);
  void SetText(const std::string &text);
};


#endif

