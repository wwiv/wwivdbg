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
#include "commands.h"
#include "editor.h"
#include "menu.h"
#include "protocol.h"
#include "source.h"
#include "stack.h"
#include "utils.h"
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
}

void TDebuggerApp::idle() { 
  // TODO(rushfan): Look for debug messages then broadcast to right places.
  TApplication::idle(); 

  if (!debug_->has_message()) {
    std::this_thread::yield();
    return;
  }

  while (debug_->has_message()) {
    auto m = debug_->next();
    switch (m.msgType) {
    case DebugMessage::Type::BREAKPOINTS:
      messageBox("Received breakpoints!", mfOKButton);
      return;
    case DebugMessage::Type::STACK:
      messageBox("Received Call Stack!", mfOKButton);
      return;
    case DebugMessage::Type::SOURCE:
      messageBox("Received Source!", mfOKButton);
      return;
    }
  }
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
  return openStackWindow();
}

TStackWindow *TDebuggerApp::openStackWindow() {
  TRect r = deskTop->getExtent();
  // Move down 70%
  r.a.y = (r.b.y * .7);
  auto wn = ++windowNumber_;
  auto *window = new TStackWindow(r, "Call Stack", wn);
  deskTop->insert(window);
  return window;
}

TSourceWindow* TDebuggerApp::findSourceWindow() {
  ptrdiff_t cmd = cmViewSource;
  if (auto *o = message(deskTop, evBroadcast, cmFindWindow,
                        reinterpret_cast<void *>(cmd))) {
    TSourceWindow *window = reinterpret_cast<TSourceWindow *>(o);
    window->select();
    return window;
  }
  return openSourceWindow();
}

TSourceWindow *TDebuggerApp::openSourceWindow() {
  TRect r = deskTop->getExtent();
  // Move down 70%
  r.b.y = r.b.y * .7;
  auto *window = new TSourceWindow(r);
  deskTop->insert(window);
  return window;
}

TDebuggerApp::TDebuggerApp(int argc, char **argv)
    : TProgInit(TDebuggerApp::initStatusLine, TDebuggerApp::initMenuBar,
                TDebuggerApp::initDeskTop),
      TApplication() {
  UpdateInitialEditorCommandState(this);

  // TODO(rushfan): We should figure out a new command line strategy.
  // Open all files passed on the commandline
  //while (--argc > 0) {         
  //  openEditor(*++argv, True);
  //}
  cascade();

  debug_ = std::make_unique<DebugProtocol>();
}

TDebuggerApp::~TDebuggerApp() { debug_.reset(nullptr); }

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
  if (event.what != evCommand) {
    return;
  }
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
  case cmViewSource: {
    auto* s = findSourceWindow();
    s->set_text({
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",  "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
        "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"
    });
  } break;
  case cmHelpAbout:
    ShowAboutBox();
    break;
  default:
    return;
  }
  clearEvent(event);
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
