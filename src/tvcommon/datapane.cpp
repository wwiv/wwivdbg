#include "datapane.h"
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
#define Uses_TIndicator
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

#include "datapane.h"
#include "tvision/tv.h"
#include <sstream>
#include <string>


TDataPane::TDataPane(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb,
  TIndicator* indicator)
  : TScroller(bounds, hsb, vsb), indicator_(indicator) {
  options |= ofFramed;
  growMode = gfGrowHiY | gfGrowHiX | gfFixed;
  normalCursor();
  showCursor();
}

// Split a string into lines
static std::vector<std::string> split_string_by_newline(const std::string& str) {
  auto result = std::vector<std::string>{};
  auto ss = std::stringstream{ str };

  for (std::string line; std::getline(ss, line, '\n');)
    result.push_back(line);

  return result;
}

void TDataPane::SetText(const std::string& text) {
  const auto lines = split_string_by_newline(text);
  SetText(lines);
}

void TDataPane::SetText(const std::vector<std::string>& text) {
  lines = text;
  auto max_line_len = 0;
  for (const auto& l : lines) {
    if (l.size() > max_line_len) {
      max_line_len = l.size();
    }
  }
  setLimit(max_line_len, lines.size());
  draw();
}

// hacky way to fix up tabs and stuff since it renders as 'o'
static std::string replace_tabs(const std::string& l) {
  std::string out;
  out.reserve(l.size() + 20);
  for (const auto c : l) {
    if (c < 0) {
      // high ascii?
    }
    else if (c < 32) {
      switch (c) {
      case '\t': {
        while ((out.size() % 8) != 0) {
          out.push_back(' ');
        }
      } break;
      }
      // control chars?
    }
    else {
      out.push_back(c);
    }
  }
  return out;
}

void TDataPane::UpdateLocation(int pos, int row, int col) {
  pos_ = pos;
  curPos_.y = std::max(0, row - 1);
  curPos_.x = col;
  trackCursor();
  doUpdate();
}

void TDataPane::doUpdate() {
  setCursor(curPos_.x - delta.x, curPos_.y - delta.y);
  if (indicator_) {
    indicator_->setValue(curPos_, false);
  }
}

void TDataPane::trackCursor() {
  const auto x = std::max(curPos_.x - size.x + 1, min(delta.x, curPos_.x));
  const auto y = std::max(curPos_.y - size.y + 1, min(delta.y, curPos_.y));
  scrollTo(x, y);
}

void TDataPane::handleEvent(TEvent& event) {
  TScroller::handleEvent(event);
  if (event.what & evMouseDown) {
    if ((event.mouse.buttons & 0x01) && mouseInView(event.mouse.where)) {
      do {
        auto p = makeLocal(event.mouse.where);
        setCurPos(p.x + delta.x, p.y + delta.y);
        this->showCursor();
      } while (mouseEvent(event, evMouseMove));
      clearEvent(event);
      doUpdate();
      return;
    }
  } else  if (event.what == evKeyboard) {
    switch (event.keyDown.keyCode) {
    case kbHome:
      setCurPos(0, curPos_.y);
      break;
    case kbEnd:
      setCurPos(limit.x - 1, curPos_.y);
      break;
    case kbUp:
      if (curPos_.y > 0) {
        curPos_.y--;
      }
      break;
    case kbDown:
      if (curPos_.y < limit.y - 1) {
        curPos_.y++;
      }
      break;
    case kbPgUp: {
      curPos_.y -= (size.y - 1);
      if (curPos_.y < 0) {
        curPos_.y = 0;
      }
    } break;
    case kbPgDn:
      if (curPos_.y < limit.y - 1) {
        curPos_.y += (size.y - 1);
        if (curPos_.y > limit.y - 1) {
          curPos_.y = limit.y - 1;
        }
      }
      break;
    case kbLeft:
      if (curPos_.x > 0) {
        --curPos_.x;
      }
      break;
    case kbRight:
      if (curPos_.x < limit.x - 1) {
        curPos_.x++;
      }
      break;
    default:
      break;
    }
    clearEvent(event);
    trackCursor();
    doUpdate();
  }
}


void TDataPane::draw() {
  const auto hilight_current = hilightCurrentLine();
  const auto normal_color = getColor(0x0301);
  const auto selected_color = getColor(0x201);
  for (int i = 0; i < size.y; i++) {
    int j = i + delta.y;
    const auto c = (hilight_current && j == curPos_.y) ? 2 : 1;
    auto color = getColor(c);
    TDrawBuffer b;
    b.moveChar(0, ' ', color, size.x);
    // make sure we have this line.
    if (j < lines.size()) {
      auto& l = lines.at(j);
      if (delta.x >= std::ssize(l)) {
        l.clear();
      }
      else {
        l = l.substr(delta.x);
      }
      l = replace_tabs(l);
      b.moveStr(0, l, color);
    }
    writeLine(0, i, size.x, 1, b);
  }
}
