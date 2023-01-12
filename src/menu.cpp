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
#include "utils.h"
#include <sstream>
#include <string>

TMenuBar *CreateMenuBar(TRect &r) {
  TSubMenu &menuFile =
      *new TSubMenu("~F~ile", kbAltF) +
      *new TMenuItem("~O~pen", cmOpen, kbCtrlO, hcNoContext, "Ctrl-O") +
      *new TMenuItem("~N~ew", cmNew, kbCtrlN, hcNoContext, "Ctrl-N") +
      *new TMenuItem("~S~ave", cmSave, kbCtrlS, hcNoContext, "Ctrl-S") +
      *new TMenuItem("S~a~ve as...", cmSaveAs, kbNoKey) + newLine() +
      *new TMenuItem("~C~hange dir...", cmChangeDrct, kbNoKey) +
      *new TMenuItem("~D~OS shell", cmDosShell, kbNoKey) + newLine() +
      *new TMenuItem("E~x~it", cmQuit, kbAltF4, hcNoContext, "Alt-F4");

  TSubMenu &menuEdit =
      *new TSubMenu("~E~dit", kbAltE) +
      *new TMenuItem("~U~ndo", cmUndo, kbCtrlZ, hcNoContext, "Ctrl-Z") +
      newLine() +
      *new TMenuItem("Cu~t~", cmCut, kbCtrlX, hcNoContext, "Ctrl-X") +
      *new TMenuItem("~C~opy", cmCopy, kbCtrlC, hcNoContext, "Ctrl-C") +
      *new TMenuItem("~P~aste", cmPaste, kbCtrlV, hcNoContext, "Ctrl-V") +
      newLine() + *new TMenuItem("~C~lear", cmClear, kbNoKey);

  TSubMenu &menuSearch =
      *new TSubMenu("~S~earch", kbAltS) +
      *new TMenuItem("~F~ind...", cmFind, kbCtrlF, hcNoContext, "Ctrl-F") +
      *new TMenuItem("~R~eplace...", cmReplace, kbCtrlH, hcNoContext,
                     "Ctrl-H") +
      *new TMenuItem("~S~earch again", cmSearchAgain, kbF3, hcNoContext, "F3");

  TSubMenu &menuView =
      *new TSubMenu("~V~iew", kbAltV) +
      *new TMenuItem("~B~reakpoints", cmViewBreakpoints, kbAltB, hcNoContext,
                     "Alt-B") +
      *new TMenuItem("Call S~t~ack", cmViewStack, kbNoKey) +
      *new TMenuItem("~L~ocal Variables", cmViewVars, kbNoKey) +
      *new TMenuItem("Basic S~o~urce", cmViewSource, kbAltO, hcNoContext, "Alt-O");

  TSubMenu &menuDebug =
      *new TSubMenu("~D~ebug", kbAltD) +
      *new TMenuItem("~R~un", cmDebugRun, kbF9, hcNoContext, "F9") +
      *new TMenuItem("~T~race Into", cmDebugTraceIn, kbF7, hcNoContext, "F7") +
      *new TMenuItem("~S~tep Over", cmDebugStepOver, kbF8, hcNoContext, "F8") +
      newLine() + 
      *new TMenuItem("~A~ttach to BBS", cmDebugAttach, kbNoKey) +
      *new TMenuItem("D~e~tach from BBS", cmDebugDetach, kbNoKey);


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
                       *new TMenuItem("Test Data", cmHelpFoo, kbNoKey) +
                       newLine() +
                       *new TMenuItem("~A~bout...", cmHelpAbout, kbNoKey);

  r.b.y = r.a.y + 1;
  return new TMenuBar(r, menuFile + menuEdit + menuSearch + menuView +
                             menuDebug + menuWindows + menuHelp);
}


TStatusLine *CreateStatusLine(TRect &r) {
  r.a.y = r.b.y - 1;
  return new TStatusLine(
      r, *new TStatusDef(0, 0xFFFF) + *new TStatusItem(0, kbAltX, cmQuit) +
             //*new TStatusItem("~F2~ Save", kbF2, cmSave) +
             //*new TStatusItem("~F3~ Open", kbF3, cmOpen) +
             *new TStatusItem("~Ctrl-W~ Close", kbAltF3, cmClose) +
             *new TStatusItem("~F5~ Zoom", kbF5, cmZoom) +
             //*new TStatusItem("~F6~ Next", kbF6, cmNext) +
             *new TStatusItem("~F10~ Menu", kbF10, cmMenu) +
             *new TStatusItem(0, kbShiftDel, cmCut) +
             *new TStatusItem(0, kbCtrlIns, cmCopy) +
             *new TStatusItem(0, kbShiftIns, cmPaste) +
             *new TStatusItem(0, kbCtrlF5, cmResize));

}

