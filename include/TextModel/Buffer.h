#pragma once

#include "TextModel/String.h"
#include <cstddef>

namespace TextModel {
  using Index = std::size_t;

  struct Range {
    Index start;
    Index end;
  };

  struct Buffer {
    virtual ~Buffer() = default;
    virtual void insert(Index, String) = 0;
    virtual void remove(Range) = 0;
    virtual String textIn(Range) = 0;
    virtual Index size() const = 0;
  };
} // TextModel