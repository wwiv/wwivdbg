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
#include "tvcommon/datapane.h"
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

#define cpScroller "\x06\x07\x06\x07\x06\x07"

TPalette& TBreakpointsPane::getPalette() const {
 return TListViewer::getPalette();
  static TPalette palette(cpScroller, sizeof(cpScroller) - 1);
  return palette;
}

TColorAttr TBreakpointsPane::mapColor(uchar index) noexcept {
  TPalette& p = getPalette();
  TColorAttr color;
  uchar f = p[0];
  if (p[0] != 0)
  {
    if (0 < index && index <= p[0])
      color = p[index];
    else
      return errorAttr;
  }
  else
    color = index;
  if (color == 0)
    return errorAttr;
  if (owner)
    return owner->mapColor(color);
  return color;
}

TMenuItem& TBreakpointsPane::initContextMenu(TPoint) {
  return *new TMenuItem("Remove ~B~reakpoint", cmBreakpointWindowRemove, kbNoKey);
}

bool TBreakpointsPane::hilightCurrentLine() {
  return focus();
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
    auto it = std::begin(b);
    std::advance(it, index);
    if (it != std::end(b)) {
      b.erase(it);
    }
    clearEvent(event);
    message(TProgram::deskTop, evBroadcast, cmBreakpointsChanged, 0);
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

void TBreakpointsWindow::SetText(const std::vector<std::string> &text) {
  fp->setList(text);
}

void TBreakpointsWindow::UpdateBreakpointWindow() {
  std::vector< std::string> lines;
  for (const auto& b : debug_->breakpoints().breakpoints) {
    const auto l =
      fmt::format("{:<15} | {:<6} | {:04d}", b.module, "line", b.line);
    lines.emplace_back(l);
  }
  fp->setList(lines);
}

// Need to add in more palette entries since this is what we have from the list viewer.
// #define cpListViewer "\x1A\x1A\x1B\x1C\x1D"
// The 2nd line is from cpXXXXXDialog in dialogs.h

#define cpBlueBreakpointWindow cpBlueWindow "\x00\x00\x00\x00\x00\x00\x00\x00" \
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
#define cpCyanBreakpointWindow cpCyanWindow "\x00\x00\x00\x00\x00\x00\x00\x00" \
                               "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
#define cpGrayBreakpointWindow cpGrayWindow "\x00\x00\x00\x00\x00\x00\x00\x00" \
                               "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"

TPalette& TBreakpointsWindow::getPalette() const {
  static TPalette blue(cpBlueBreakpointWindow, sizeof(cpBlueBreakpointWindow) - 1);
  static TPalette cyan(cpCyanBreakpointWindow, sizeof(cpCyanBreakpointWindow) - 1);
  static TPalette gray(cpGrayBreakpointWindow, sizeof(cpGrayBreakpointWindow) - 1);
  static TPalette* palettes[] ={
    &blue,
    &cyan,
    &gray
  };
  return *(palettes[palette]);

}


TColorAttr TBreakpointsWindow::mapColor(uchar index) noexcept {
  return TWindow::mapColor(index);
}
