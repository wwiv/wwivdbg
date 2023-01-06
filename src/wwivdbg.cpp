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
#include "stack.h"
#include "wwivdbg.h"
#include <memory>
#include <sstream>
#include <string>

short execDialog(TDialog *d, void *data) {
  if (auto *p = TProgram::application->validView(d)) {
    if (data != nullptr) {
      p->setData(data);
    }
    const auto result = TProgram::deskTop->execView(p);
    if (result != cmCancel && data != nullptr) {
      p->getData(data);
    }
    TObject::destroy(p);
    return result;
  }
  return cmCancel;
}

TDialog *createFindDialog() {
  auto *d = new TDialog(TRect(0, 0, 38, 12), "Find");

  d->options |= ofCentered;

  TInputLine *control = new TInputLine(TRect(3, 3, 32, 4), 80);
  d->insert(control);
  d->insert(new TLabel(TRect(2, 2, 15, 3), "~T~ext to find", control));
  d->insert(new THistory(TRect(32, 3, 35, 4), control, 10));

  d->insert(new TCheckBoxes(
      TRect(3, 5, 35, 7),
      new TSItem("~C~ase sensitive", new TSItem("~W~hole words only", 0))));

  d->insert(new TButton(TRect(14, 9, 24, 11), "O~K~", cmOK, bfDefault));
  d->insert(new TButton(TRect(26, 9, 36, 11), "Cancel", cmCancel, bfNormal));

  d->selectNext(False);
  return d;
}

TDialog *createReplaceDialog() {
  TDialog *d = new TDialog(TRect(0, 0, 40, 16), "Replace");

  d->options |= ofCentered;

  TInputLine *control = new TInputLine(TRect(3, 3, 34, 4), 80);
  d->insert(control);
  d->insert(new TLabel(TRect(2, 2, 15, 3), "~T~ext to find", control));
  d->insert(new THistory(TRect(34, 3, 37, 4), control, 10));

  control = new TInputLine(TRect(3, 6, 34, 7), 80);
  d->insert(control);
  d->insert(new TLabel(TRect(2, 5, 12, 6), "~N~ew text", control));
  d->insert(new THistory(TRect(34, 6, 37, 7), control, 11));

  d->insert(new TCheckBoxes(
      TRect(3, 8, 37, 12),
      new TSItem("~C~ase sensitive",
                 new TSItem("~W~hole words only",
                            new TSItem("~P~rompt on replace",
                                       new TSItem("~R~eplace all", 0))))));

  d->insert(new TButton(TRect(17, 13, 27, 15), "O~K~", cmOK, bfDefault));
  d->insert(new TButton(TRect(28, 13, 38, 15), "Cancel", cmCancel, bfNormal));

  d->selectNext(False);

  return d;
}

TMenuBar *TDebuggerApp::initMenuBar(TRect r) {
  TSubMenu &menuFile =
      *new TSubMenu("~F~ile", kbAltF) +
      *new TMenuItem("~O~pen", cmOpen, kbCtrlO, hcNoContext, "Ctrl-O") +
      *new TMenuItem("~N~ew", cmNew, kbCtrlN, hcNoContext, "Ctrl-N") +
      *new TMenuItem("~S~ave", cmSave, kbCtrlS, hcNoContext, "Ctrl-S") +
      *new TMenuItem("S~a~ve as...", cmSaveAs, kbNoKey) + newLine() +
      *new TMenuItem("~C~hange dir...", cmChangeDrct, kbNoKey) +
      *new TMenuItem("~D~OS shell", cmDosShell, kbNoKey) +
      *new TMenuItem("E~x~it", cmQuit, kbAltF4, hcNoContext, "Alt-F4");

  TSubMenu &menuEdit =
      *new TSubMenu("~E~dit", kbAltE) +
      *new TMenuItem("~U~ndo", cmUndo, kbCtrlZ, hcNoContext, "Ctrl-Z") +
      newLine() +
      *new TMenuItem("Cu~t~", cmCut, kbCtrlX, hcNoContext, "Ctrl-X") +
      *new TMenuItem("~C~opy", cmCopy, kbCtrlC, hcNoContext, "Ctrl-C") +
      *new TMenuItem("~P~aste", cmPaste, kbCtrlV, hcNoContext, "Ctrl-V") +
      newLine() +
      *new TMenuItem("~C~lear", cmClear, kbNoKey);

  TSubMenu &menuSearch =
      *new TSubMenu("~S~earch", kbAltS) +
      *new TMenuItem("~F~ind...", cmFind, kbCtrlF,  hcNoContext, "Ctrl-F") +
      *new TMenuItem("~R~eplace...", cmReplace, kbCtrlH, hcNoContext, "Ctrl-H") +
      *new TMenuItem("~S~earch again", cmSearchAgain, kbF3, hcNoContext, "F3");

  TSubMenu &menuView =
      *new TSubMenu("~V~iew", kbAltV) +
      *new TMenuItem("~B~reakpoints...", cmViewBreakpoints, kbAltB, hcNoContext, "Alt-B") +
      *new TMenuItem("S~t~ack...", cmViewStack, kbAltT, hcNoContext, "Alt-T") +
      *new TMenuItem("S~o~urce", cmViewSource, kbAltO, hcNoContext, "Alt-O");

  TSubMenu &menuWindows =
      *new TSubMenu("~W~indows", kbAltW) +
      *new TMenuItem("~S~ize/move", cmResize, kbCtrlF5, hcNoContext,
                     "Ctrl-F5") +
      *new TMenuItem("~Z~oom", cmZoom, kbF5, hcNoContext, "F5") +
      *new TMenuItem("~T~ile", cmTile, kbNoKey) +
      *new TMenuItem("C~a~scade", cmCascade, kbNoKey) +
      *new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6") +
      *new TMenuItem("~P~revious", cmPrev, kbShiftF6, hcNoContext, "Shift-F6") +
      *new TMenuItem("~C~lose", cmClose, kbCtrlW, hcNoContext, "Ctrl+W");

  TSubMenu &menuHelp = *new TSubMenu("~H~elp", kbAltH) +
                       *new TMenuItem("~A~bout...", cmHelpAbout, kbNoKey);

  r.b.y = r.a.y + 1;
  return new TMenuBar(r, menuFile + menuEdit + menuSearch + menuView +
                             menuWindows + menuHelp);
}

TStatusLine *TDebuggerApp::initStatusLine(TRect r) {
  r.a.y = r.b.y - 1;
  return new TStatusLine(
      r, *new TStatusDef(0, 0xFFFF) + *new TStatusItem(0, kbAltX, cmQuit) +
             *new TStatusItem("~F2~ Save", kbF2, cmSave) +
             *new TStatusItem("~F3~ Open", kbF3, cmOpen) +
             *new TStatusItem("~Ctrl-W~ Close", kbAltF3, cmClose) +
             *new TStatusItem("~F5~ Zoom", kbF5, cmZoom) +
             *new TStatusItem("~F6~ Next", kbF6, cmNext) +
             *new TStatusItem("~F10~ Menu", kbF10, cmMenu) +
             *new TStatusItem(0, kbShiftDel, cmCut) +
             *new TStatusItem(0, kbCtrlIns, cmCopy) +
             *new TStatusItem(0, kbShiftIns, cmPaste) +
             *new TStatusItem(0, kbCtrlF5, cmResize));
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
}

ushort doEditDialog(int dialog, ...) {
  va_list arg;
  char buf[1000] = {};
  std::ostringstream os(buf, sizeof(buf) - 1);
  switch (dialog) {
  case edOutOfMemory:
    return messageBox("Not enough memory for this operation",
                      mfError | mfOKButton);
  case edReadError: {
    va_start(arg, dialog);
    os << "Error reading file " << va_arg(arg, char *) << ".";
    va_end(arg);
    return messageBox(buf, mfError | mfOKButton);
  }
  case edWriteError: {
    va_start(arg, dialog);
    os << "Error writing file " << va_arg(arg, char *) << ".";
    va_end(arg);
    return messageBox(buf, mfError | mfOKButton);
  }
  case edCreateError: {
    va_start(arg, dialog);
    os << "Error creating file " << va_arg(arg, char *) << ".";
    va_end(arg);
    return messageBox(buf, mfError | mfOKButton);
  }
  case edSaveModify: {
    va_start(arg, dialog);
    os << va_arg(arg, char *) << " has been modified. Save?";
    va_end(arg);
    return messageBox(buf, mfInformation | mfYesNoCancel);
  }
  case edSaveUntitled:
    return messageBox("Save untitled file?", mfInformation | mfYesNoCancel);
  case edSaveAs: {
    va_start(arg, dialog);
    return execDialog(
        new TFileDialog("*.*", "Save file as", "~N~ame", fdOKButton, 101),
        va_arg(arg, char *));
  }

  case edFind: {
    va_start(arg, dialog);
    return execDialog(createFindDialog(), va_arg(arg, char *));
  }

  case edSearchFailed:
    return messageBox("Search string not found.", mfError | mfOKButton);
  case edReplace: {
    va_start(arg, dialog);
    return execDialog(createReplaceDialog(), va_arg(arg, char *));
  }

  case edReplacePrompt:
    //  Avoid placing the dialog on the same line as the cursor
    TRect r(0, 1, 40, 8);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2, 0);
    TPoint t = TProgram::deskTop->makeGlobal(r.b);
    t.y++;
    va_start(arg, dialog);
    TPoint *pt = va_arg(arg, TPoint *);
    if (pt->y <= t.y) {
      r.move(0, TProgram::deskTop->size.y - r.b.y - 2);
    }
    va_end(arg);
    return messageBoxRect(r, "Replace this occurence?",
                          mfYesNoCancel | mfInformation);
  }
  return cmCancel;
}

TEditWindow *TDebuggerApp::openEditor(const std::string& fileName, Boolean visible) {
  TRect r = deskTop->getExtent();
  TView *p = validView(new TEditWindow(r, fileName, wnNoNumber));
  if (!visible) {
    p->hide();
  }
  deskTop->insert(p);
  return (TEditWindow *)p;
}

int TDebuggerApp::openStackWindow() { 
  TRect r = deskTop->getExtent(); 
  // Move down 70%
  r.a.y = (r.b.y * .7);
  auto wn = ++windowNumber_;
  auto *window = new TStackWindow(r, "Call Stack", wn);
  deskTop->insert(window);
  return wn;
}

TDebuggerApp::TDebuggerApp(int argc, char **argv)
    : TProgInit(TDebuggerApp::initStatusLine, TDebuggerApp::initMenuBar,
                TDebuggerApp::initDeskTop),
      TApplication() {
  TCommandSet ts;
  ts.enableCmd(cmSave);
  ts.enableCmd(cmSaveAs);
  ts.enableCmd(cmCut);
  ts.enableCmd(cmCopy);
  ts.enableCmd(cmPaste);
  ts.enableCmd(cmClear);
  ts.enableCmd(cmUndo);
  ts.enableCmd(cmFind);
  ts.enableCmd(cmReplace);
  ts.enableCmd(cmSearchAgain);
  disableCommands(ts);

  TEditor::editorDialog = doEditDialog;

  // Open all files passed on the commandline
  while (--argc > 0) {         
    openEditor(*++argv, True);
  }
  cascade();
}

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
    if (auto *o = message(deskTop, evBroadcast, cmFindWindow, nullptr)) {
      TWindow *window = reinterpret_cast<TWindow *>(o);
      window->select();
    } else {
      openStackWindow();
    }
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
