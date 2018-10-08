#pragma once

#include "TextModel/Buffer.h"
#include "Generics/Algorithms.h"
#include <list>
#include <numeric>

namespace TextModel {
  enum class Storage {
    Original, Inserted
  };

  struct Span {
    Storage storage;
    Index start_in_storage;
    Index length; 

    Span(Storage which, Index s, Index l)
    : storage{which}
    , start_in_storage{s}
    , length{l} {}
  };


  class TextBuffer
  : public Buffer
  {
    String original_;
    String inserted_;

    using PieceList = std::list<Span>;
    PieceList pieces_;

    String text_of(Span const& piece) const;

    using ListPosition = std::pair<PieceList::const_iterator, Index>;
    ListPosition piece_at(Index index) const;

    using MutListPosition = std::pair<PieceList::iterator, Index>;
    MutListPosition piece_at(Index index);
  
  public:
    TextBuffer() = default;
    explicit TextBuffer(String);

    void insert(Index index, String text) override;
    void remove(Range const& range) override;
    String text_of(Range const& range) const override;
    Index size() const override;
  };
} // TextModel