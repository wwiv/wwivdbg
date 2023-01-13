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

#ifndef INCLUDED_WWIVDBG_SOURCE_H
#define INCLUDED_WWIVDBG_SOURCE_H

#define Uses_TScrollBar
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"
#include "tvcommon/datapane.h"
#include <memory>
#include <string>
#include <vector>

class DebugProtocol;

class TSourcePane : public TDataPane {

public:
  TSourcePane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb, TIndicator* indicator,
              DebugProtocol* debug);
  ~TSourcePane() = default;
  virtual void handleEvent(TEvent& event) override;
  virtual TMenuItem& initContextMenu(TPoint);

  void doUpdate();
  bool hilightCurrentLine();

private:
  DebugProtocol* debug_;
};

class TSourceWindow : public TWindow {
public:
  TSourceWindow(TRect r, const std::string &title,
                const std::string initial_module,
                const std::shared_ptr<DebugProtocol> &debug);
  ~TSourceWindow();

  virtual void handleEvent(TEvent &event) override;
  TScrollBar* standardScrollBar(ushort aOptions);
  void SetText(const std::vector<std::string> &text);
  void SetText(const std::string &text);
  virtual void setState(ushort aState, Boolean enable);

private:
  TSourcePane* fp;
  TScrollBar* hsb, * vsb;
  std::shared_ptr<DebugProtocol> debug_;
  TIndicator* indicator_;
  // current module loaded in the source window.
  std::string module_;
  std::string initial_module_;
};


#endif

