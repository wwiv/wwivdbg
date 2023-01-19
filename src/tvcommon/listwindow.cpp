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
#include "listwindow.h"
#include "fmt/format.h"
#include <sstream>
#include <string>


TWCListViewer::TWCListViewer(const TRect &bounds, ushort numColumns, TScrollBar *hsb, TScrollBar *vsb)
    : TListViewer(bounds, numColumns, hsb, vsb) {
  growMode = gfGrowHiX | gfGrowHiY;
}

TMenuItem& TWCListViewer::initContextMenu(TPoint) {
  return *new TMenuItem("<empty>", 0, kbNoKey);
}

void TWCListViewer::setList(const std::vector<std::string>& l) {
  list = l;
  setRange(static_cast<short>(list.size()));
}

void TWCListViewer::getText(char* dest, short item, short maxLen) {
  if (item >= list.size()) {
    strcpy(dest, "");
  }
  else {
    strncpy(dest, list.at(item).c_str(), maxLen);
  }
}

bool TWCListViewer::isContextMenuTrigger(TEvent& event) {
  return ((event.what & evMouseDown) && (event.mouse.buttons & mbRightButton) && mouseInView(event.mouse.where));

}
void TWCListViewer::handleEvent(TEvent &event) {
  // Since TListViewer will clear the mouse events, send a copy of the
  // event for it to update the selection, etc.  Then check if it's
  // an appropriate context menu trigger.
  TEvent e{ event };
  TListViewer::handleEvent(e);
  if (!hasContextMenu || list.empty()) {
    return;
  }
  if (!isContextMenuTrigger(event)) {
    return;
  }
  auto &menu = initContextMenu(event.mouse.where);
  popupMenu(event.mouse.where, menu, owner);
  clearEvent(event);
}

TWCListWindow::TWCListWindow(TRect r, TStringView aTitle, short windowNumber)
    : TWindowInit(&TWindow::initFrame),
      TWindow(r, aTitle, windowNumber) {

  // NOTE(rushfan): This only works with cyan so far, need to figure the palette out
  palette = wpCyanWindow;
}

TWCListWindow ::~TWCListWindow() = default;


void TWCListWindow::handleEvent(TEvent& event) {
  TWindow::handleEvent(event);
}


// NOTE(rushfan): These are not entirely right yet.
// This only works with cyan so far, need to figure the palette out

// Need to add in more palette entries since this is what we have from the list viewer.
// #define cpListViewer "\x1A\x1A\x1B\x1C\x1D"
// The 2nd line is from cpXXXXXDialog in dialogs.h

#define cpBlueBreakpointWindow cpBlueWindow "\x00\x00\x00\x00\x00\x00\x00\x00" \
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
#define cpCyanBreakpointWindow cpCyanWindow "\x00\x00\x00\x00\x00\x00\x00\x00" \
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
#define cpGrayBreakpointWindow cpGrayWindow "\x00\x00\x00\x00\x00\x00\x00\x00" \
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"

TPalette& TWCListWindow::getPalette() const {
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

TColorAttr TWCListWindow::mapColor(uchar index) noexcept {
  return TWindow::mapColor(index);
}
