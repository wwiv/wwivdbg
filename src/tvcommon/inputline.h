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
  TFormInputLine(const TRect& bounds, std::string* data, unsigned int maxLen, TValidator* valid = nullptr)
    : TInputLine(bounds, maxLen, valid), data_(data) {}
  TFormInputLine(std::string* data, unsigned int maxLen, TValidator* valid = nullptr)
    : TInputLine(TRect(0, 0, 10, 1), maxLen, valid), data_(data) {}
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
  TFormNumberInputLine(const TRect& bounds, T* data, TValidator* valid)
    : TInputLine(bounds, std::numeric_limits<T>::digits10 + 2, valid), data_(data) {}
  TFormNumberInputLine(T* data, TValidator* valid)
    : TFormNumberInputLine(TRect(0, 0, 10, 1), valid) {}
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

// TCheckBoxes like class that takes data as an integer value.
class TFormCheckBoxesInteger : public TCheckBoxes {
public:
  TFormCheckBoxesInteger(int32_t* data, TSItem* items) noexcept;

  virtual void getData(void *rec) { *data_ = value; }
  virtual void setData(void *rec) { value = *data_; }

private:
  int32_t* data_{ nullptr };
};


// One item (label and data value) for use in a TFormCheckBoxes control.
struct CheckBoxItem {
  // Text for the checkbox in the UI
  std::string text;
  // pointer to the value for this checkbox item.
  bool* value;
};

// TCheckBoxes like class that takes a vector of CheckBoxItems
class TFormCheckBoxes : public TCheckBoxes {
public:
  TFormCheckBoxes(std::vector<CheckBoxItem>* items) noexcept;

  virtual void getData(void* rec);
  virtual void setData(void* rec);

private:
  std::vector<CheckBoxItem>* items_;
};

#endif

