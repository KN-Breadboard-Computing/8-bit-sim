#pragma once

#include <algorithm>
#include <concepts>
#include <string>
#include <type_traits>

template <typename T>
concept HasTestTitle = requires(T t) {
  { t.test_title } -> std::convertible_to<std::string>;
};

#define DOCTEST_VALUE_PARAMETERIZED_DATA(data, data_container)                 \
  static size_t _doctest_subcase_idx = 0;                                      \
  std::for_each(                                                               \
      data_container.begin(), data_container.end(), [&](const auto &in) {      \
        if constexpr (HasTestTitle<decltype(in)>) {                            \
          DOCTEST_SUBCASE(in.test_title.c_str()) { data = in; }                \
        } else {                                                               \
          DOCTEST_SUBCASE((std::string(#data_container "[") +                  \
                           std::to_string(_doctest_subcase_idx++) + "]")       \
                              .c_str()) {                                      \
            data = in;                                                         \
          }                                                                    \
        }                                                                      \
      });                                                                      \
  _doctest_subcase_idx = 0
