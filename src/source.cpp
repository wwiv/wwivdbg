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

#include "tvision/tv.h"
#include "fmt/format.h"
#include "commands.h"
#include "protocol.h"
#include "source.h"
#include "utils.h"
#include <sstream>
#include <string>


TSourcePane::TSourcePane(const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb,
                         TIndicator *indicator, DebugProtocol *debug)
    : TDataPane(bounds, hsb, vsb, indicator), debug_(debug) {
  context_menu_enabled = true;
  normalCursor();
}

bool TSourcePane::hilightCurrentLine() {
  return debug_ && debug_->attached();
}


void TSourcePane::doUpdate() {
  TDataPane::doUpdate();
}

void TSourcePane::handleEvent(TEvent& event) {
  TDataPane::handleEvent(event);
}

TMenuItem& TSourcePane::initContextMenu(TPoint) {
  return // *new TMenuItem("~C~opy", cmCopy, kbCtrlC, hcNoContext, "Ctrl-C") + newLine() +
    *new TMenuItem("Add ~B~reakpoint", cmBreakpointAdd, kbCtrlB, hcNoContext, "Ctrl-B");
}

TScrollBar* TSourceWindow::standardScrollBar(ushort aOptions)
{
  TRect r = getExtent();
  if ((aOptions & sbVertical) != 0)
    r = TRect(r.b.x - 1, r.a.y + 1, r.b.x, r.b.y - 1);
  else
    r = TRect(r.a.x + 18, r.b.y - 1, r.b.x - 2, r.b.y);

  TScrollBar* s;
  insert(s = new TScrollBar(r));
  if ((aOptions & sbHandleKeyboard) != 0) {
    s->options |= ofPostProcess;
  }
  return s;
}


TSourceWindow::TSourceWindow(TRect r, const std::string &title,
                             const std::string initial_module,
                             const std::shared_ptr<DebugProtocol> &debug)
    : TWindowInit(TWindow::initFrame), TWindow(r, title, 0), debug_(debug),
      module_(initial_module), initial_module_(initial_module) {

  hsb = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  vsb = standardScrollBar(sbVertical | sbHandleKeyboard);
  insert(indicator_ = new TIndicator(TRect(2, size.y - 1, 16, size.y)));
  insert(fp = new TSourcePane(getClipRect().grow(-1, -1), hsb, vsb, indicator_, debug.get()));
  fp->SetText(debug->source());
}

TSourceWindow ::~TSourceWindow() = default;

// Only handle broadcast events in here for now.
void TSourceWindow::handleEvent(TEvent &event) {
  if (event.what != evBroadcast && event.what != evCommand) {
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
  case cmBreakpointAdd: {
    const auto line = fp->currentLine();
    debug_->breakpoints().NewLine(module_, line);
    clearEvent(event);
    message(TProgram::deskTop, evBroadcast, cmBreakpointsChanged, 0);
    return;
  } break;
  case cmDebugSourceChanged:
    fp->SetText(debug_->source());
    clearEvent(event);
    return;
  case cmBroadcastDebugStateChanged: {
    const auto &s = debug_->state();
    fp->SetSelectedPosition(s.pos, s.row, s.col);
    module_ = s.module;
    fp->draw();
    // DO NOT CLEAR event
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

void TSourceWindow::setState(ushort state, Boolean enable) {
  TWindow::setState(state, enable);
  if (frame == nullptr) {
    // Was getting a random crash on closing the window because of getting
    // called here to make these not visible while shutting down the view.
    return;
  }
  switch (state) {
  case sfActive:
    if (hsb != 0)
      hsb->setState(sfVisible, enable);
    if (vsb != 0)
      vsb->setState(sfVisible, enable);
    if (indicator_) {
      indicator_->setState(sfVisible, enable);
    }
    //updateCommands();
    break;

  case sfExposed:
    //if (enable == True)
    //  unlock();
    break;
  }
}


