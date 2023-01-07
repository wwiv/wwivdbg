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

#ifndef INCLUDED_WWIVDBG_WWIVDBG_H
#define INCLUDED_WWIVDBG_WWIVDBG_H

#include <memory>
#include <string>

class DebugProtocol;

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

class TDebuggerApp : public TApplication {

public:
  TDebuggerApp(int argc, char **argv);
  ~TDebuggerApp();

  virtual void handleEvent(TEvent &event) override;
  static TMenuBar *initMenuBar(TRect);
  static TStatusLine *initStatusLine(TRect);
  virtual void outOfMemory() override;
  virtual void getEvent(TEvent &event) override;
  virtual void idle() override;

private:
  TEditWindow *openEditor(const std::string& fileName, Boolean visible);
  TStackWindow* findStackWindow();
  TStackWindow *openStackWindow();
  TSourceWindow* findSourceWindow();
  TSourceWindow *openSourceWindow();
  void fileOpen();
  void fileNew();
  void changeDir();

  void ShowAboutBox();

  int windowNumber_{0};
  std::unique_ptr<DebugProtocol> debug_;
};

TDialog *createFindDialog();
TDialog *createReplaceDialog();

#endif
