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

#ifndef INCLUDED_TVCOMMON_INPUTLINE_H
#define INCLUDED_TVCOMMON_INPUTLINE_H

#define Uses_TListViewer
#define Uses_TInputLine
#define Uses_TDialog
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"
#include "fmt/format.h"
#include <cstring>
#include <optional>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace detail {
  template <typename T, typename std::enable_if<std::is_unsigned<T>::value, T>::type* = nullptr>
  T to_number(const std::string& s, int b = 10) {
    char* end;
    auto result = strtoul(s.c_str(), &end, b);
    if (errno == ERANGE) {
      return 0;
    }
    if (result > std::numeric_limits<T>::max()) {
      return std::numeric_limits<T>::max();
    }
    if (result < std::numeric_limits<T>::min()) {
      return std::numeric_limits<T>::min();
    }
    return static_cast<T>(result);
  }

  template <typename T, typename std::enable_if<std::is_signed<T>::value, T>::type* = nullptr>
  T to_number(const std::string& s, int b = 10) {
    char* end;
    auto result = strtol(s.c_str(), &end, b);
    if (errno == ERANGE) {
      return 0;
    }
    if (result > std::numeric_limits<T>::max()) {
      return std::numeric_limits<T>::max();
    }
    if (result < std::numeric_limits<T>::min()) {
      return std::numeric_limits<T>::min();
    }
    return static_cast<T>(result);
  }

  template <size_t SIZE> bool to_char_array_trim(char(&out)[SIZE], const std::string& s) noexcept {
    strncpy(out, s.c_str(), SIZE);
    out[SIZE - 1] = '\0';
    return s.size() <= SIZE;
  }

}

class TFormInputLine : public TInputLine {
public:
  TFormInputLine(const TRect& bounds, std::string* data, unsigned int aMaxLen, TValidator* aValid = nullptr)
    : TInputLine(bounds, aMaxLen, aValid), data_(data) {}
  TFormInputLine(std::string* data, unsigned int aMaxLen, TValidator* aValid = nullptr)
    : TInputLine(TRect(0, 0, 10, 1), aMaxLen, aValid), data_(data) {}
  virtual ~TFormInputLine() = default;
  virtual void getData(void* rec);
  virtual void setData(void* rec);

private:
  std::string* data_{ nullptr };
};

template <typename T,
          typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class TFormNumberInputLine : public TInputLine {

public:
  TFormNumberInputLine(const TRect& bounds, T* data, TValidator* aValid = nullptr)
    : TInputLine(bounds, std::numeric_limits<T>::digits10 + 2, aValid), data_(data) {}
  TFormNumberInputLine(T* data, TValidator* aValid)
    : TInputLine(TRect(0, 0, 10, 1), std::numeric_limits<T>::digits10 + 2, aValid), data_(data) {}
  TFormNumberInputLine(T* data) : TFormNumberInputLine(TRect(0, 0, 10, 1), data, nullptr) {}
  virtual ~TFormNumberInputLine() = default;

  virtual void getData(void* rec) {
    char x[std::numeric_limits<T>::digits10 + 2] = {};
    TInputLine::getData(&x);
    *data_ = detail::to_number<int>(x);
  }

  virtual void setData(void* rec) {
    char x[std::numeric_limits<T>::digits10 + 2] = {};
    const auto s = fmt::format("{:d}", *data_);
    detail::to_char_array_trim(x, s);
    TInputLine::setData(&x);
  }

private:
  T* data_{ nullptr };
};


class TFormCheckBoxes : public TCheckBoxes {
public:
  // TODO(rushfan): Should really change this to a better data structure.
  TFormCheckBoxes(int32_t *data, TSItem *items) noexcept
      : TCheckBoxes(TRect(0, 0, 10, 1), items), data_(data) {
  }

  virtual void getData(void* rec) {
    *data_ = value;
  }

  virtual void setData(void* rec) {
    value = *data_;
  }
private:
  int32_t* data_{ nullptr };
};
#endif

