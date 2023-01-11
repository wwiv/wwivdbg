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
#define Uses_MsgBox
#define Uses_TApplication
#define Uses_TButton
#define Uses_TChDirDialog
#define Uses_TCheckBoxes
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TEditor
#define Uses_TEditWindow
#define Uses_TFileDialog
#define Uses_TFileEditor
#define Uses_THistory
#define Uses_TInputLine
#define Uses_TKeys
#define Uses_TLabel
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TObject
#define Uses_TPoint
#define Uses_TProgram
#define Uses_TRect
#define Uses_TSItem
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TSubMenu

#include "tvision/tv.h"
#include "fmt/format.h"
#include "commands.h"
#include "protocol.h"
#include "vars.h"
#include "utils.h"
#include <sstream>
#include <string>


TVarsPane::TVarsPane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb)
    : TScroller(bounds, hsb, vsb) {
  options |= ofFramed;
  growMode = gfGrowHiY | gfGrowHiX | gfFixed;
}

void TVarsPane::SetText(const std::vector<Variable> &vars) {
  lines.clear();
  for (const auto& v : vars) {
    lines.push_back(fmt::format("{} ({}): '{}'", v.name, v.type, v.value));
  }
  auto max_line_len = 0;
  for (const auto &l : lines) {
    if (l.size() > max_line_len) {
      max_line_len = l.size();
    }
  }
  setLimit(max_line_len, lines.size());
  draw();
}

void TVarsPane::draw() { 
  const auto normal_color = getColor(0x0301);
  const auto selected_color = getColor(0x201);
  for (int i = 0; i < size.y; i++) {
    int j = i + delta.y;
    TDrawBuffer b;
    b.moveChar(0, ' ', normal_color, size.x);
    // make sure we have this line.
    if (j < lines.size()) {
      auto& l = lines.at(j);
      if (delta.x >= std::ssize(l)) {
        l.clear();
      } else {
        l = l.substr(delta.x);
      }
      b.moveStr(0, l, normal_color);
    }
    writeLine(0, i, size.x, 1, b);
  }
}

TVarsWindow::TVarsWindow(TRect r, const std::shared_ptr<DebugProtocol>& debug)
    : TWindowInit(TWindow::initFrame),
      TWindow(r, "Vars", 0), debug_(debug) {

  hsb = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  vsb = standardScrollBar(sbVertical | sbHandleKeyboard);
  insert(fp = new TVarsPane(getClipRect().grow(-1, -1), hsb, vsb));
  SetText(debug->vars());
}

TVarsWindow ::~TVarsWindow() = default;

void TVarsWindow::handleEvent(TEvent &event) {
  // Only handle broadcast events in here for now.
  if (event.what != evBroadcast) {
    TWindow::handleEvent(event);
    return;
  }
  switch (event.message.command) {
  case cmFindWindow:
    if (event.message.infoInt == cmViewVars) {
      clearEvent(event);
      return;
    }
    break;
  case cmDebugStateChanged: {
    fp->SetText(debug_->vars());
    fp->draw();
    // DO NOT CLEAR clearEvent(event);
  } break;

  }
  TWindow::handleEvent(event);
}

void TVarsWindow::SetText(const std::vector<Variable> &text) {
  fp->SetText(text);
}
