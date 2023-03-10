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
#include "form.h"
#include "fmt/format.h"
#include <sstream>
#include <string>

TFormColumn::TFormColumn(int x, int y, int pad, int labelWidth,
                         int controlWidth, TFormColumn::LabelPosition labelPos)
    : x_(x), y_(y), pad_(pad), labelWidth_(labelWidth),
      controlWidth_(controlWidth), labelPos_(labelPos) {}

TFormColumn::~TFormColumn() = default;

bool TFormColumn::add(const std::string& labelText, TView* control) {
  items_.emplace_back(labelText, control);
  return true;
}

int TFormColumn::width() const {
  if (labelPos_ == TFormColumn::LabelPosition::left) {
    return pad_ + labelWidth_ + pad_ + controlWidth_ + pad_ + 1;
  } else {
    return (pad_ * 3) + std::max<int>(labelWidth_, controlWidth_) +  1;
  }
}

int TFormColumn::height() const {
  int height = 3;
  
  if (labelPos_ == TFormColumn::LabelPosition::left) {
    for (const auto& item : items_) {
      auto bounds = item.control->getBounds();
      height += (bounds.b.y - bounds.a.y) + 1;
    }
    return height;
  } 
  
  for (const auto& item : items_) {
    auto bounds = item.control->getBounds();
    height += (bounds.b.y - bounds.a.y) + 2;
  }
  return height;
}

void TFormColumn::updateLabelWidths() {
  for (const auto& item : items_) {
    labelWidth_ = std::max<int>(labelWidth_, item.labelText.size());
  }
}

TDialog* TFormColumn::insertLabelLeftTo(TDialog* dialog, int btnPad, int btnX, int y) {
  const int ctrlX = pad_ + labelWidth_ + pad_;
  TRect lr(pad_, y, pad_ + labelWidth_, y + 1);
  for (auto& item : items_) {
    const auto bounds = item.control->getBounds();
    const auto height = bounds.b.y - bounds.a.y;
    TRect lr(pad_, y, pad_ + labelWidth_, y + 1);
    TRect cr(ctrlX, y, ctrlX + controlWidth_, y + height);
    item.control->setBounds(cr);
    dialog->insert(item.control);
    dialog->insert(new TLabel(lr, item.labelText, item.control));
    y += height + 1;
  }
  return dialog;
}

TDialog* TFormColumn::insertLabelBelowTo(TDialog* dialog, int btnPad, int btnX, int y) {
  TRect r(pad_, y, pad_ + std::max<int>(labelWidth_, controlWidth_), y + 1);
  for (const auto& item : items_) {
    auto lr = r;
    auto cr = r;
    cr.move(0, 1);
    item.control->setBounds(cr);
    dialog->insert(item.control);
    dialog->insert(new TLabel(lr, item.labelText, item.control));
    r.move(0, 3);
  }
  return dialog;
}

TDialog* TFormColumn::insertTo(TDialog* dialog) {
  updateLabelWidths();
  const int btnPad = pad_ + 1;
  const int btnX = labelWidth_ + pad_ + controlWidth_ + btnPad;
  if (labelPos_ == TFormColumn::LabelPosition::left) {
    return insertLabelLeftTo(dialog, btnPad, btnX, y_ + 2);
  }
  return insertLabelBelowTo(dialog, btnPad, btnX, y_ + 2);
}

bool TFormColumn::selectFirstControl() {
  if (items_.empty()) {
    return false;
  }
  items_.front().control->select();
  return true;
}


void TForm::add(TFormColumn* c) {
  cols_.emplace_back(c);
}

void TForm::addButton(const std::string& label, int command, int flags) {
  buttons_.emplace_back(label, command, flags);
}

void TForm::addOKButton() { addButton("~O~K", cmOK, bfDefault); }
void TForm::addCancelButton() { addButton("C~a~ncel", cmCancel, bfNormal); }


std::optional<TDialog*> TForm::createDialog(const std::string& title) {
  if (cols_.empty()) {
    return std::nullopt;
  }
  const int buttonHeight = 3;
  int width = cols_.size();
  int height = buttonHeight;
  for (const auto* c : cols_) {
    width += c->width();
    if (c->height() > height) {
      height = c->height();
    }
  }
  if (!buttons_.empty()) {
    height += buttonHeight;
  }
  TRect bounds(0, 0, width, height);
  TDialog* d = new TDialog(bounds, title);
  int x = 0;
  for (auto* c : cols_) {
    c->moveX(x);
    c->insertTo(d);
    x += c->x() + c->width();
  }

  // Add buttons
  constexpr int btnPadding = 4;
  constexpr int btnWidth = 10;
  const int y = d->getBounds().b.y - 3;
  int end_x = d->getBounds().b.x - 3;
  for (auto& b : buttons_) {
    TRect r(end_x - btnWidth, y, end_x, y + 2);
    d->insert(new TButton(r, b.label, b.command, b.flags));
    end_x -= (btnWidth + btnPadding);
  }

  cols_.front()->selectFirstControl();
  return d;
}

