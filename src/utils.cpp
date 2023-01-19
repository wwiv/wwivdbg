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

#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TObject
#define Uses_TPoint
#define Uses_TProgram

#include "tvision/tv.h"
#include "utils.h"

short execDialog(TDialog *d, void *data) {
  d->options |= ofCentered;
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

short execDialog(TDialog* d) {
  char unused[1000];
  return execDialog(d, &unused);
}


void* messageInt(TView* receiver, ushort what, ushort command, int info) {
  if (receiver == nullptr) {
    return 0;
  }

  TEvent event{};
  event.what = what;
  event.message.command = command;
  event.message.infoInt = info;
  receiver->handleEvent(event);
  if (event.what == evNothing) {
    return event.message.infoPtr;
  }
  return nullptr;
}
