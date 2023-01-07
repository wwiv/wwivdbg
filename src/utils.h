
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

#ifndef INCLUDED_WWIVDBG_UTILS_H
#define INCLUDED_WWIVDBG_UTILS_H

#include "tvision/tv.h"

/**
 * @brief  Ensures that the view `p` is valid, otherwise returns nullptr
 * @tparam T The type of the view
 * @param program pointer to TProgram to use for validating view
 * @param p the view to return
 * @return the view or nullptr if p is not valid.
*/
template <typename T> 
T *ValidView(TProgram *program, T *p) noexcept {
  auto* t = program->validView(p);
  if (!t) {
    return nullptr;
  }
  return static_cast<T *>(t);
}

short execDialog(TDialog *d, void *data);

#endif
