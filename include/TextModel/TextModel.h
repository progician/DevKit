#pragma once

#include <string>
#include <vector>

namespace TextModel {
  class TextModel {
  public:
    TextModel(std::string);

    std::size_t line_count() const noexcept { return 1; }
  };
} // TextModel