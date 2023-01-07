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
#include "commands.h"
#include "source.h"
#include "utils.h"
#include <sstream>
#include <string>


TSourcePane::TSourcePane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb)
    : TScroller(bounds, hsb, vsb) {
  options |= ofFramed;
  growMode = gfGrowHiY | gfGrowHiX | gfFixed;
}

void TSourcePane::set_text(const std::vector<std::string> &text) {
  lines = text;
  auto max_line_len = 0;
  setLimit(max_line_len, lines.size());
  draw();
}

void TSourcePane::draw() { 
  auto color = getColor(0x0301); 
 for (int i = 0; i < size.y; i++) {
    TDrawBuffer b;
    b.moveChar(0, ' ', color, size.x);
    int j = i + delta.y;
    if (j < lines.size()) {
      // make sure we have this line.
      b.moveStr(0, lines.at(j), color);
    }
    writeBuf(0, i, size.x, 1, b);
  }
}

TSourceWindow::TSourceWindow(TRect r)
    : TWindowInit(TWindow::initFrame),
      TWindow(r, "Source", 0) {

  hsb = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  vsb = standardScrollBar(sbVertical | sbHandleKeyboard);
  TRect ir = getClipRect();
  ir.grow(-1, -1);
  fp = new TSourcePane(ir, hsb, vsb);

  ////insert(hsb);
  //insert(vsb);
  insert(fp);
}

TSourceWindow ::~TSourceWindow() {}

void TSourceWindow::handleEvent(TEvent &event) {
  // Only handle broadcast events in here for now.
  if (event.what != evBroadcast) {
    TWindow::handleEvent(event);
    return;
  }
  switch (event.message.command) {
  case cmFindWindow:
    if (event.message.infoInt == cmViewSource) {
      clearEvent(event);
      return;
    }
  }
  TWindow::handleEvent(event);
}

void TSourceWindow::set_text(const std::vector<std::string>& text) {
  fp->set_text(text);
}
