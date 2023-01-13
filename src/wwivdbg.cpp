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
#include "fmt/format.h"
#include "breakpoints.h"
#include "commands.h"
#include "editor.h"
#include "menu.h"
#include "protocol.h"
#include "source.h"
#include "stack.h"
#include "utils.h"
#include "vars.h"
#include "wwivdbg.h"
#include <memory>
#include <string>
#include <thread>



TMenuBar *TDebuggerApp::initMenuBar(TRect r) { 
  return CreateMenuBar(r); 
}

TStatusLine *TDebuggerApp::initStatusLine(TRect r) {
  return CreateStatusLine(r);
}

void TDebuggerApp::outOfMemory() {
  messageBox("Not enough memory for this operation.", mfError | mfOKButton);
}

void TDebuggerApp::getEvent(TEvent& event) {

  TApplication::getEvent(event);
  if (event.what == evBroadcast && event.message.command == cmDebugAttached) {
    //OOOO
#ifdef _WIN32
    // OutputDebugString("cmDebugAttached");
#endif
  }
}

void TDebuggerApp::idle() { 
  // TODO(rushfan): Look for debug messages then broadcast to right places.
  TApplication::idle(); 
}



TEditWindow *TDebuggerApp::openEditor(const std::string& fileName, Boolean visible) {
  TRect r = deskTop->getExtent();
  if (auto *p = ValidView(this, new TEditWindow(r, fileName, wnNoNumber))) {
    if (!visible) {
      p->show();
    }
    deskTop->insert(p);
    return p;
  }
  return nullptr;
}

TStackWindow *TDebuggerApp::findStackWindow() {
  ptrdiff_t cmd = cmViewStack;
  if (auto *o = message(deskTop, evBroadcast, cmFindWindow,
                        reinterpret_cast<void *>(cmd))) {
    auto *window = reinterpret_cast<TStackWindow *>(o);
    window->select();
    return window;
  } 
  //openStackWindow
  TRect r = deskTop->getExtent();
  // Move down 70%
  r.a.y = (r.b.y * .7);
  auto *window = new TStackWindow(r, debug_);
  deskTop->insert(window);
  return window;
}

TSourceWindow* TDebuggerApp::findSourceWindow() {
  ptrdiff_t cmd = cmViewSource;
  if (auto *o = message(deskTop, evBroadcast, cmFindWindow,
                        reinterpret_cast<void *>(cmd))) {
    auto *window = reinterpret_cast<TSourceWindow *>(o);
    window->select();
    return window;
  }
  
  // openSourceWindow(
  TRect r = deskTop->getExtent();
  // cap botton at 70%
  r.b.y = r.b.y * .7;
  const auto title = fmt::format("Source: {}", debug_->state().initial_module);
  auto *window = new TSourceWindow(r, title, debug_);
  deskTop->insert(window);
  return window;
}

TVarsWindow* TDebuggerApp::findVarsWindow() {
  ptrdiff_t cmd = cmViewVars;
  if (auto* o = message(deskTop, evBroadcast, cmFindWindow,
    reinterpret_cast<void*>(cmd))) {
    auto* window = reinterpret_cast<TVarsWindow*>(o);
    window->select();
    return window;
  }

  TRect r = deskTop->getExtent();
  // Move down 70% 
  r.a.y = (r.b.y * 0.7);
  auto* window = new TVarsWindow(r, debug_);
  deskTop->insert(window);
  return window;
}

TBreakpointsWindow *TDebuggerApp::findBreakpointsWindow() {
  ptrdiff_t cmd = cmViewBreakpoints;
  if (auto *o = message(deskTop, evBroadcast, cmFindWindow,
                        reinterpret_cast<void *>(cmd))) {
    TBreakpointsWindow *window = reinterpret_cast<TBreakpointsWindow *>(o);
    window->select();
    return window;
  }

  // create breakpoints window
  TRect r = deskTop->getExtent();
  // Move down 70%
  r.a.y = (r.b.y * .7);
  auto *window = new TBreakpointsWindow(r);
  deskTop->insert(window);
  return window;
}

TDebuggerApp::TDebuggerApp(int argc, char **argv)
    : TProgInit(TDebuggerApp::initStatusLine, TDebuggerApp::initMenuBar,
                TDebuggerApp::initDeskTop),
      TApplication() {
  UpdateInitialEditorCommandState(this);
  cascade();

  debug_ = std::make_shared<DebugProtocol>(this, deskTop, "127.0.0.1", 9948);
  disableCommand(cmDebugDetach);
  attached_cmds_ += cmDebugDetach;
  attached_cmds_ += cmDebugRun;
  attached_cmds_ += cmDebugStepOver;
  attached_cmds_ += cmDebugTraceIn;
  disableCommands(attached_cmds_);

  detached_cmds_ += cmDebugAttach;
  enableCommands(detached_cmds_);
}

TDebuggerApp::~TDebuggerApp() { debug_.reset(); }

void TDebuggerApp::fileOpen() {
  char fileName[MAXPATH];
  strcpy(fileName, "*.*");

  if (execDialog(
          new TFileDialog("*.*", "Open file", "~N~ame", fdOpenButton, 100),
          fileName) != cmCancel) {
    openEditor(fileName, True);
  }
}

void TDebuggerApp::fileNew() { openEditor("", True); }

void TDebuggerApp::changeDir() { execDialog(new TChDirDialog(cdNormal, 0), 0); }

void TDebuggerApp::handleEvent(TEvent &event) {
  TApplication::handleEvent(event);
  switch (event.what) { 
  case evCommand:
    handleCommand(event);
    break;
  case evBroadcast:
    handleBroadcast(event);
    break;
  }
}
void TDebuggerApp::handleBroadcast(TEvent &event) {
  switch (event.message.command) { 
  case cmDebugAttached: {
    // Ensure the source window is open
    findSourceWindow();
    debug_->UpdateSource();

    enableCommands(attached_cmds_);
    disableCommands(detached_cmds_);
  } break;
  case cmDebugDetached: {
    enableCommands(detached_cmds_);
    disableCommands(attached_cmds_);
    messageBox("Debugger detached", mfInformation | mfOKButton);
  }
  break;
  default:
    return;
  }
  clearEvent(event);
}

void TDebuggerApp::handleCommand(TEvent &event) {
  switch (event.message.command) {
  case cmOpen:
    fileOpen();
    break;
  case cmNew:
    fileNew();
    break;
  case cmChangeDrct:
    changeDir();
    break;
  case cmViewStack: {
    findStackWindow();
  } break;
  case cmViewBreakpoints: {
    findBreakpointsWindow();
  } break;
  case cmViewSource: {
    findSourceWindow();
  } break;
  case cmViewVars: {
    findVarsWindow();
  } break;
  case cmDebugRun:
    messageBox("Implement Run", mfOKButton | mfError);
    break;
  case cmDebugTraceIn:
    if (!debug_->TraceIn()) {
      messageBox("Error in  RunTraceIn", mfOKButton | mfError);
    }
    break;
  case cmDebugStepOver:
    if (!debug_->StepOver()) {
      messageBox("Error in RunStepOver", mfOKButton | mfError);
    }
    break;
  case cmDebugAttach: {
    if (!debug_->Attach()) {
      messageBox("Error attaching to BBS.", mfOKButton | mfError);
    }
  } break;
  case cmDebugDetach: {
    if (!debug_->Detach()) {
      messageBox("Error detaching to BBS.", mfOKButton | mfError);
    }
  } break;
  case cmHelpAbout:
    ShowAboutBox();
    break;
  case cmHelpFoo: {
    auto *s = findSourceWindow();
    s->SetText("this\nis\na\ntest\n\nHello\nWorld\nthis\nis\na\ntest\n\nHello\n"
               "World\nthis\nis\na\ntest\n\nHello\nWorld\nthis\nis\na\ntest\n\n"
               "Hello\nWorld\n\n");
  } break;
  default:
    return;
  }
  clearEvent(event);
}

void TDebuggerApp::eventError(TEvent &event) {
  if (event.what == evBroadcast && event.message.command >= 100) {
    auto text = fmt::format("Event Error: cmd:{} data:{}\n",
                            event.message.command, event.message.infoLong);
#ifdef _WIN32
    OutputDebugString(text.c_str());
#endif
  }
}

bool TDebuggerApp::valid(ushort cmd) { 
  if (cmd == cmQuit && debug_->attached()) {
    debug_->Detach();
  }
  return TApplication::valid(cmd); 
}



void TDebuggerApp::ShowAboutBox() {
  TDialog *aboutBox = new TDialog(TRect(0, 0, 59, 11), "About");
  aboutBox->insert(new TStaticText(TRect(9, 2, 50, 7),
                                   "\003WWIVbasic Debugger\n\n"
                                   "\003Copyright (c) 2023\n\n"
                                   "\003WWIV Software Services"));

  aboutBox->insert(new TButton(TRect(24, 8, 35, 10), " OK", cmOK, bfDefault));
  aboutBox->options |= ofCentered;

  executeDialog(aboutBox);
}

int main(int argc, char **argv) {
  TDebuggerApp editorApp(argc, argv);
  editorApp.run();
  editorApp.shutDown();
  return 0;
}
