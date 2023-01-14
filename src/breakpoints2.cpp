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
#include "breakpoints2.h"
#include "protocol.h"
#include "utils.h"
#include <sstream>
#include <string>


TBreakpointsPane2::TBreakpointsPane2(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb)
    : TListViewer(bounds, 1, hsb, vsb) {
  range = 2;
}

//#define cpDialogX "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
#define cpDialogX "\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
#define cpScroller "\x06\x07\x06\x07\x06\x07"

TPalette& TBreakpointsPane2::getPalette() const {
 return TListViewer::getPalette();
  static TPalette palette(cpScroller, sizeof(cpScroller) - 1);
  return palette;
}

TColorAttr TBreakpointsPane2::mapColor(uchar index) noexcept {
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

TMenuItem& TBreakpointsPane2::initContextMenu(TPoint) {
  return *new TMenuItem("Remove ~B~reakpoint", cmBreakpointWindowRemove, kbNoKey);
}

bool TBreakpointsPane2::hilightCurrentLine() {
  return focus();
}

void TBreakpointsPane2::getText(char* dest, short item, short maxLen) {
  strcpy(dest, "Test");
}


TBreakpointsWindow2::TBreakpointsWindow2(TRect r, const std::shared_ptr<DebugProtocol>& debug)
    : TWindowInit(&TWindow::initFrame),
      TWindow(r, "Breakpoints", 0), debug_(debug) {

  palette = wpCyanWindow;
  fp = new TBreakpointsPane2(getClipRect().grow(-1, -1), nullptr, nullptr);
  fp->growMode = gfGrowHiX | gfGrowHiY;
  insert(fp);
  errorAttr = 0xFF;
}

TBreakpointsWindow2 ::~TBreakpointsWindow2() = default;

void TBreakpointsWindow2::handleBroadcastEvent(TEvent& event) {
  switch (event.message.command) {
  case cmFindWindow:
    if (event.message.infoInt == cmViewBreakpoints) {
      clearEvent(event);
    }
    break;
  case cmBreakpointsChanged:
    UpdateBreakpointWindow();
    break;
  case cmDebugStateChanged: {
    UpdateBreakpointWindow();
    // DO NOT CLEAR clearEvent(event);
  } break;
  default:
    TWindow::handleEvent(event);
  }
}

void TBreakpointsWindow2::handleCommandEvent(TEvent& event) {
  switch (event.message.command) {
  case cmBreakpointWindowRemove: { // command
    // currentLine is 1 based, we need 0 based.
    const auto index = 0; // HACK std::max<int>(0, fp->currentLine() - 1);
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


void TBreakpointsWindow2::handleEvent(TEvent& event) {
  switch (event.what) {
  case evBroadcast: handleBroadcastEvent(event); break;
  case evCommand:   handleCommandEvent(event); break;
  default:          TWindow::handleEvent(event); break;
  }
}

void TBreakpointsWindow2::SetText(const std::vector<std::string> &text) {
//  fp->SetText(text);
}

void TBreakpointsWindow2::UpdateBreakpointWindow() {
  std::vector< std::string> lines;
  for (const auto& b : debug_->breakpoints().breakpoints) {
    const auto l =
      fmt::format("{:<15} | {:<6} | {:04d}", b.module, "line", b.line);
    lines.emplace_back(l);
  }
//  fp->SetText(lines);
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

TPalette& TBreakpointsWindow2::getPalette() const {
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


TColorAttr TBreakpointsWindow2::mapColor(uchar index) noexcept {
  return TWindow::mapColor(index);
}
