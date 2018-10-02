#pragma once

#include "TextModel/Buffer.h"
#include <iostream>
#include <list>

namespace TextModel {

  class PieceTable
  : public Buffer {
    String original_;
    String inserted_;

    struct Operation {
      Index size;
      String& source;
      Index source_begin;
    };
    Index size_{0};

    using OperationList = std::list<Operation>;
    OperationList operations_;

    struct Piece {
      OperationList::iterator which;
      Index relative_index{0};
    };
    Piece piece_at(Index);

  public:
    PieceTable() = default;
    PieceTable(String);
    ~PieceTable() override = default;

    void insert(Index, String) override;
    void remove(Range) override;

    String text_of(Range) const override;
    Index size() const override;

    friend std::ostream& operator<<(std::ostream&, const PieceTable&);
  };

  String text_of(PieceTable const&);
} // PieceTable