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
#include "editor.h"
#include "utils.h"
#include <sstream>
#include <string>

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


void UpdateInitialEditorCommandState(TView* view) {
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
  view->disableCommands(ts);

  TEditor::editorDialog = doEditDialog;
}