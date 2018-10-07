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
    virtual void remove(Range const&) = 0;
    virtual String text_of(Range const&) const = 0;
    virtual Index size() const = 0;
  };

  inline String FullTextOf(Buffer const& buffer) {
    return buffer.text_of(Range{0, buffer.size()});
  }
} // TextModel