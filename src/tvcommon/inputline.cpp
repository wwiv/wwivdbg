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
#define Uses_TStringCollection
#define Uses_TSubMenu

#include "fmt/format.h"
#include "tvision/tv.h"
#include "inputline.h"
#include "../strings.h"
#include <sstream>
#include <string>

template <size_t SIZE> 
static bool to_char_array_trim(char(&out)[SIZE], const std::string& s) noexcept {
  strncpy(out, s.c_str(), SIZE);
  out[SIZE - 1] = '\0';
  return s.size() <= SIZE;
}

void TFormInputLine::getData(void* rec) {
  // Fetch from internal model
  data_->resize(maxLen + 1);
  TInputLine::getData(&data_->front());
  if (const auto idx = data_->find('\0'); idx != std::string::npos) {
    data_->resize(idx);
  }
}

void TFormInputLine::setData(void* rec) {
  // Set internal model
  auto x = std::make_unique<char[]>(maxLen);
  strncpy(x.get(), data_->c_str(), maxLen);
  TInputLine::setData(x.get());
}


TFormCheckBoxesInteger::TFormCheckBoxesInteger(int32_t* data, TSItem* items) noexcept
  : TCheckBoxes(TRect(0, 0, 10, 1), items), data_(data) {
  auto b = getBounds();
  b.b.y = strings->getCount();
  setBounds(b);
}

TFormCheckBoxes::TFormCheckBoxes(std::vector<CheckBoxItem>* items) noexcept 
  : TCheckBoxes(TRect(0, 0, 10, 1), nullptr), items_(items) {

    delete strings;
    strings = new TStringCollection(items->size(), 10);

    int count = 0;
    for (const auto& i : *items) {
      strings->atInsert(count++, newStr(i.text.c_str()));
    }

    auto b = getBounds();
    b.b.y = count;
    setBounds(b);
}

void TFormCheckBoxes::getData(void* rec) {
  int pos = 0;
  for (auto& item : *items_) {
    bool set = value & (1 << pos++);
    item.value = set;
  }
}

void TFormCheckBoxes::setData(void* rec) {  
  uint32_t v{ 0 };
  int pos = 0;
  for (const auto& item : *items_) {
    if (item.value) {
      v |= (1 << pos);
    }
    ++pos;
  }
  value = v;
}

