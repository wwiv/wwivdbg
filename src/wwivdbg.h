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

#ifndef INCLUDED_WWIVDBG_H
#define INCLUDED_WWIVDBG_H

#include <string>

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

constexpr int cmStartUserCommands = 100;
constexpr int cmChangeDrct = 102;
// View
constexpr int cmViewBreakpoints = 103;
constexpr int cmViewStack = 104;
constexpr int cmViewSource = 105;
// Help
constexpr int cmHelpAbout = 106;

class TDebuggerApp : public TApplication {

public:
  TDebuggerApp(int argc, char **argv);

  virtual void handleEvent(TEvent &event);
  static TMenuBar *initMenuBar(TRect);
  static TStatusLine *initStatusLine(TRect);
  virtual void outOfMemory();

private:
  TEditWindow *openEditor(const std::string& fileName, Boolean visible);
  void fileOpen();
  void fileNew();
  void changeDir();

  void ShowAboutBox();
};

TDialog *createFindDialog();
TDialog *createReplaceDialog();

#endif
