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

#ifndef INCLUDED_TVCOMMON_FORM_H
#define INCLUDED_TVCOMMON_FORM_H

#define Uses_TListViewer
#define Uses_TDialog
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>


class TFormColumn {
public:
  enum class LabelPosition { left, above };

  TFormColumn(int x, int y, int pad, int labelWidth, int controlWidth,
              LabelPosition labelPos);
  TFormColumn(int labelWidth, int controlWidth, LabelPosition labelPos)
      : TFormColumn(0, 0, 2, labelWidth, controlWidth, labelPos) {}
  ~TFormColumn();

  bool add(const std::string& labelText, TView* control);

  int width() const;
  int height() const;
  void set_x(int x) { x_ = x; }
  int x() const { return x_; }
  int moveX(int r) { x_ += r; return x_; }

  void updateLabelWidths();
  TDialog* insertTo(TDialog* dialog);

private:
  struct Item {
    std::string labelText;
    TView *control;
  };

  TDialog* insertLabelLeftTo(TDialog* dialog, int btnPad, int btnX, int y);
  TDialog* insertLabelBelowTo(TDialog* dialog, int btnPad, int btnX, int y);

  int x_{ 0 };
  int y_{ 0 };
  int pad_{ 0 };
  int labelWidth_{ 0 };
  int controlWidth_{ 0 };
  LabelPosition labelPos_{ LabelPosition::left };
  
  std::vector<Item> items_;
};

class TForm {
public:
  TForm() = default;
  ~TForm() = default;
  void add(TFormColumn* c);
  void addButton(const std::string& label, int command, int flags);
  std::optional<TDialog*> createDialog(const std::string& title);
private:

  struct Button {
    std::string label;
    int command;
    int flags;
  };

  std::vector< TFormColumn*> cols_;
  std::vector<Button> buttons_;
};

TDialog* createDialog(TFormColumn* c, const std::string& title, int buttons);
TDialog* createDialog(TFormColumn* c1, TFormColumn* c2, const std::string& title, int buttons);

#endif

