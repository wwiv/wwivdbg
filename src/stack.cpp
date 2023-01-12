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
#include "tvcommon/datapane.h"
#include "fmt/format.h"
#include "commands.h"
#include "stack.h"
#include "strings.h"
#include <memory>
#include <sstream>
#include <string>

TStackInterior::TStackInterior(const TRect &r, TScrollBar *hsb, TScrollBar *vsb)
    : TDataPane(r, hsb, vsb, nullptr) {}

TStackWindow::TStackWindow(const TRect &r, const std::shared_ptr<DebugProtocol>& debug)
  : TWindowInit(&TWindow::initFrame), TWindow(r, "Call Stack", 0), debug_(debug) {
  auto inner = getClipRect();
  inner.grow(-1, -1);

  hsb = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  vsb = standardScrollBar(sbVertical | sbHandleKeyboard);
  insert(fp = new TStackInterior(inner, hsb, vsb));
  UpdateStack(debug_->stack());
};

TStackWindow::~TStackWindow() = default;

void TStackWindow::handleEvent(TEvent& event) {
  // Only handle broadcast events in here for now.
  if (event.what != evBroadcast) {
    TWindow::handleEvent(event);
    return;
  }
  switch (event.message.command) { 
  case cmFindWindow:
    if (event.message.infoInt == cmViewStack) {
      clearEvent(event);
      return;
    }
  case cmDebugStateChanged: {
    UpdateStack(debug_->stack());
    // DO NOT CLEAR clearEvent(event);
  } break;
  }
  TWindow::handleEvent(event);
}


void TStackWindow::UpdateStack(const std::vector<std::string>& stack) {
  std::vector<std::string> lines;
  lines.reserve(stack.size());
  int frame = 0;
  for (const auto& v : stack) {
    lines.push_back(fmt::format("F{:03d} | {}", frame++, v));
  }
  fp->SetText(lines);
}
