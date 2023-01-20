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
#define Uses_TListViewer
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
#include "tvcommon/form.h"
#include "tvcommon/inputline.h"
#include "tvcommon/listwindow.h"
#include "fmt/format.h"
#include "commands.h"
#include "breakpoints.h"
#include "protocol.h"
#include "utils.h"
#include <sstream>
#include <string>


TBreakpointsPane::TBreakpointsPane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb)
    : TWCListViewer(bounds, 1, hsb, vsb) {
  hasContextMenu = true;
}


TMenuItem& TBreakpointsPane::initContextMenu(TPoint) {
  return *new TMenuItem("Remove ~B~reakpoint", cmBreakpointWindowRemove, kbNoKey) + newLine() +
    *new TMenuItem("~P~roperties", cmBreakpointWindowProperties, kbNoKey);
}

TBreakpointsWindow::TBreakpointsWindow(TRect r, const std::shared_ptr<DebugProtocol>& debug)
    : TWindowInit(&TWindow::initFrame),
      TWCListWindow(r, "Breakpoints", 0), debug_(debug) {

  insert(fp = new TBreakpointsPane(getClipRect().grow(-1, -1), nullptr, nullptr));
}

TBreakpointsWindow ::~TBreakpointsWindow() = default;

void TBreakpointsWindow::handleBroadcastEvent(TEvent& event) {
  switch (event.message.command) {
  case cmFindWindow:
    if (event.message.infoInt == cmViewBreakpoints) {
      clearEvent(event);
    }
    break;
  case cmBreakpointsChanged:
    UpdateBreakpointWindow();
    clearEvent(event);
    break;
  case cmBroadcastDebugStateChanged: {
    UpdateBreakpointWindow();
    // DO NOT CLEAR clearEvent(event);
  } break;
  default:
    TWindow::handleEvent(event);
  }
}

void TBreakpointsWindow::handleCommandEvent(TEvent& event) {
  switch (event.message.command) {
  case cmBreakpointWindowRemove: { // command
    // currentLine is 1 based, we need 0 based.
    const auto index = fp->focused;
    auto& b = debug_->breakpoints().breakpoints;
    if (b.empty()) {
      return;
    }
    const auto remote_id = b.at(index).remote_id;
    if (!debug_->DeleteBreakpoint(remote_id)) {
      messageBox("Failed to delete breakpoint on the server", mfOKButton | mfError);
      clearEvent(event);
      return;
    }
    auto it = std::begin(b);
    std::advance(it, index);
    if (it != std::end(b)) {
      b.erase(it);
    }
    clearEvent(event);
    message(TProgram::deskTop, evBroadcast, cmBreakpointsChanged, 0);
  } break;
  case cmBreakpointWindowProperties: {
    showInfoDialog(debug_->breakpoints().breakpoints.at(fp->focused));
    clearEvent(event);
  } break;
  default:
    TWindow::handleEvent(event);
  }
}


void TBreakpointsWindow::handleEvent(TEvent& event) {
  switch (event.what) {
  case evBroadcast: handleBroadcastEvent(event); break;
  case evCommand:   handleCommandEvent(event); break;
  default:          TWindow::handleEvent(event); break;
  }
}

void TBreakpointsWindow::UpdateBreakpointWindow() {
  std::vector< std::string> lines;
  for (const auto& b : debug_->breakpoints().breakpoints) {
    if (b.typ == "step") {
      // Ignore step breakpoints for the display, they are internal.
      continue;
    }
    const auto l =
      fmt::format("{:<15} | {:<6} | {:04d}", b.module, b.typ, b.line);
    lines.emplace_back(l);
  }
  fp->setList(lines);
}

void showInfoDialog(Breakpoint& bp) {
  TFormColumn c(12, 12, TFormColumn::LabelPosition::left);
  c.add("~M~odule:", new TFormInputLine(&bp.module, 128));
  c.add("~L~ine:", new TFormNumberInputLine(&bp.line));
  c.add("~R~emote ID:", new TFormNumberInputLine(&bp.remote_id));

  std::vector<CheckBoxItem> items;
  items.emplace_back("Yes", &bp.published);
  c.add("~P~ublished:", new TFormCheckBoxes(&items));

  TForm f;
  f.add(&c);
  f.addButton("~O~K", cmOK, bfDefault);
  f.addButton("C~a~ncel", cmCancel, bfNormal);
  
  if (auto o = f.createDialog("Breakpoints2")) {
    char unused[1000];
    execDialog(o.value(), &unused);
  }
}

