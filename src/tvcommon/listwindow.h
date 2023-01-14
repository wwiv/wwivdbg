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

#ifndef INCLUDED_TVCOMMON_LISTWINDOW_H
#define INCLUDED_TVCOMMON_LISTWINDOW_H

#define Uses_TListViewer
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"
#include "datapane.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class TWCListViewer : public TListViewer {

public:
  TWCListViewer(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb);
  ~TWCListViewer() = default;
  virtual TMenuItem& initContextMenu(TPoint);
  virtual void getText(char* dest, short item, short maxLen);
  virtual TPalette& getPalette() const;
  TColorAttr mapColor(uchar index) noexcept;

};

class DebugProtocol;

class TWCListWindow : public TWindow {

public:
  TWCListWindow(TRect r, TStringView aTitle, short aNumber);
  ~TWCListWindow();
  virtual void handleEvent(TEvent& event) override;
  virtual TPalette& getPalette() const;
  TColorAttr mapColor(uchar index) noexcept;

private:
  TView* fp;
  TScrollBar* hsb, * vsb;
  std::shared_ptr<DebugProtocol> debug_;
};


#endif

