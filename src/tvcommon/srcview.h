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

#include <map>
#include <memory>
#include <string>
#include <vector>

class TWCSourceViewer : public TScroller {

public:
  TWCSourceViewer(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb, TIndicator* indicator);
  ~TWCSourceViewer() = default;
  virtual void draw() override;
  virtual void handleEvent(TEvent& event) override;

  // Override to populate a context menu.
  virtual TMenuItem& initContextMenu(TPoint);
  virtual int getLineForMousePoint(TPoint m);

  // Sets the text buffer either as lines or text.
  virtual void SetText(const std::string& text);
  virtual void SetText(const std::vector<std::string> &text);

  // Should the current line be hilighted
  virtual bool hilightCurrentLine() { return false; }
  virtual TColorAttr mapColor(uchar) noexcept;
  void setLineColorMap(const std::map<int, int>& m) { lineColorMap = m; }
  int lineColor(int line);

  // Sets the current position and also selects the row.
  void SetSelectedPosition(int pos, int row, int col);
  // Move the cursor to match the current position.
  virtual void trackCursor();
  // Update the scrolling, cursor, etc after a position update.
  virtual void doUpdate();
  // Update the internal database 
  void setCurPos(int x, int y);
  // Current line number, 1 based at origin.
  int currentLine() const { return curPos_.y + 1; }
  

protected:
  std::vector<std::string> lines;
  // Should this view offer a context menu?
  bool context_menu_enabled{ false };

private:
  int pos_{ 0 };
  int selected_row_{ -1 };
  TIndicator* indicator_{ nullptr };
  TPoint curPos_{ 0, 0 };
  std::map<int, int> lineColorMap;
};


#endif

