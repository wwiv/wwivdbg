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

#ifndef INCLUDED_WWIVDBG_VARS_H
#define INCLUDED_WWIVDBG_VARS_H

#define Uses_TScrollBar
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"

#include <string>
#include <vector>

class Variable;

class TVarsPane : public TScroller {

public:
  TVarsPane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb);
  ~TVarsPane() = default;
  virtual void draw();
  void SetText(const std::vector<Variable> &text);

private:
  std::vector<std::string> lines;
};

class TVarsWindow : public TWindow {
public:
  TVarsWindow(TRect r, const std::shared_ptr<DebugProtocol>& debug);
  ~TVarsWindow();

  virtual void handleEvent(TEvent &event) override;

  void SetText(const std::vector<Variable> &text);

private:
  TVarsPane* fp;
  TScrollBar* hsb, * vsb;
  std::shared_ptr<DebugProtocol> debug_;

};


#endif

