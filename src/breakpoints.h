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

#ifndef INCLUDED_WWIVDBG_BREAKPOINTS_H
#define INCLUDED_WWIVDBG_BREAKPOINTS_H

#define Uses_TListViewer
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"
#include "tvcommon/datapane.h"
#include "tvcommon/listwindow.h"
#include <memory>
#include <string>
#include <vector>

struct Breakpoint;

class TBreakpointsPane : public TWCListViewer {

public:
  TBreakpointsPane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb);
  ~TBreakpointsPane() = default;
  virtual TMenuItem& initContextMenu(TPoint);
  bool hilightCurrentLine();
  virtual TPalette& getPalette() const;
  TColorAttr mapColor(uchar index) noexcept;

};

class DebugProtocol;

class TBreakpointsWindow : public TWCListWindow {

public:
  TBreakpointsWindow(TRect r, const std::shared_ptr<DebugProtocol>& debug);
  ~TBreakpointsWindow();
  void handleBroadcastEvent(TEvent& event);
  void handleCommandEvent(TEvent& event);
  virtual void handleEvent(TEvent& event) override;
  virtual TPalette& getPalette() const;
  TColorAttr mapColor(uchar index) noexcept;

  void SetText(const std::vector<std::string> &text);
  void UpdateBreakpointWindow();
  void showInfoDialog(int index);

private:
  TBreakpointsPane* fp;
  TScrollBar* hsb, * vsb;
  std::shared_ptr<DebugProtocol> debug_;
};

void showInfoDialog2(Breakpoint& bp);


#endif

