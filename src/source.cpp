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
#include "protocol.h"
#include "source.h"
#include "utils.h"
#include <sstream>
#include <string>


TSourcePane::TSourcePane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb,
                         DebugProtocol* debug)
    : TScroller(bounds, hsb, vsb), debug_(debug) {
  options |= ofFramed;
  growMode = gfGrowHiY | gfGrowHiX | gfFixed;
}

// Split a string into lines
static std::vector<std::string> split_string_by_newline(const std::string &str) {
  auto result = std::vector<std::string>{};
  auto ss = std::stringstream{str};

  for (std::string line; std::getline(ss, line, '\n');)
    result.push_back(line);

  return result;
}

void TSourcePane::SetText(const std::string &text) {
  const auto lines = split_string_by_newline(text);
  SetText(lines);
}

void TSourcePane::SetText(const std::vector<std::string> &text) {
  lines = text;
  auto max_line_len = 0;
  for (const auto &l : lines) {
    if (l.size() > max_line_len) {
      max_line_len = l.size();
    }
  }
  setLimit(max_line_len, lines.size());
  draw();
}

// hacky way to fix up tabs and stuff since it renders as 'o'
static std::string replace_tabs(const std::string &l) { 
  std::string out;
  out.reserve(l.size() + 20);
  for (const auto c : l) {
    if (c < 0) {
      // high ascii?
    } else if (c < 32) {
      switch (c) {
      case '\t': {
        while ((out.size() % 8) != 0) {
          out.push_back(' ');
        }
      } break;
      }
      // control chars?
    } else {
      out.push_back(c);
    }
  }
  return out;
}

void TSourcePane::draw() { 
  //auto palette = getPalette();
  const auto attached = debug_->attached();
  const auto normal_color = getColor(0x0301);
  const auto selected_color = getColor(0x201);
  for (int i = 0; i < size.y; i++) {
    int j = i + delta.y;
    const auto c = (attached && j == current_line_) ? 2 : 1;
    auto color = getColor(c);
    TDrawBuffer b;
    b.moveChar(0, ' ', color, size.x);
    // make sure we have this line.
    if (j < lines.size()) {
      auto& l = lines.at(j);
      if (delta.x >= std::ssize(l)) {
        l.clear();
      } else {
        l = l.substr(delta.x);
      }
      l = replace_tabs(l);
      b.moveStr(0, l, color);
    }
    writeLine(0, i, size.x, 1, b);
  }
}

TSourceWindow::TSourceWindow(TRect r, const std::shared_ptr<DebugProtocol>& debug)
    : TWindowInit(TWindow::initFrame),
      TWindow(r, "Source", 0), debug_(debug) {

  hsb = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  vsb = standardScrollBar(sbVertical | sbHandleKeyboard);
  insert(fp = new TSourcePane(getClipRect().grow(-1, -1), hsb, vsb, debug.get()));
  fp->SetText(debug->source());
}

TSourceWindow ::~TSourceWindow() = default;

// Only handle broadcast events in here for now.
void TSourceWindow::handleEvent(TEvent &event) {
  if (event.what != evBroadcast) {
    TWindow::handleEvent(event);
    return;
  }
  switch (event.message.command) {
  case cmFindWindow: {
    if (event.message.infoInt == cmViewSource) {
      clearEvent(event);
      return;
    }
  } break;
  case cmDebugSourceChanged:
    fp->SetText(debug_->source());
    clearEvent(event);
    return;
  case cmDebugStateChanged: {
    const auto &s = debug_->state();
    fp->UpdateLocation(s.pos, s.line, s.col);
    fp->draw();
    clearEvent(event);
  } break;
  }
  TWindow::handleEvent(event);
}

void TSourceWindow::SetText(const std::vector<std::string> &text) {
  fp->SetText(text);
}

void TSourceWindow::SetText(const std::string &text) {
  fp->SetText(text);
}
