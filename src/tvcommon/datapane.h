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

#ifndef INCLUDED_TVCOMMON_DATAPANE_H
#define INCLUDED_TVCOMMON_DATAPANE_H

#define Uses_TIndicator
#define Uses_TScrollBar
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"

#include <memory>
#include <string>
#include <vector>

class TDataPane : public TScroller {

public:
  TDataPane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb, TIndicator* indicator);
  ~TDataPane() = default;
  virtual void draw() override;
  virtual void handleEvent(TEvent& event) override;

  // Sets the text buffer either as lines or text.
  virtual void SetText(const std::string& text);
  virtual void SetText(const std::vector < std::string > & text);
  virtual void doUpdate();
  virtual bool hilightCurrentLine() { return false; }

  void UpdateLocation(int pos, int row, int col);
  virtual void trackCursor();
  void setCurPos(int x, int y) { curPos_.x = x; curPos_.y = y; }

private:
  std::vector<std::string> lines;
  int pos_{ 0 };
  TIndicator* indicator_{ nullptr };
  TPoint curPos_{ 0, 0 };
};


#endif

