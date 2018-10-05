#pragma once

#include "TextModel/Buffer.h"
#include <iostream>
#include <list>

namespace TextModel {

  class PieceTable
  : public Buffer {
    String original_;
    String inserted_;

    struct Piece {
      Index size;
      String& source;
      Index source_begin;
    };
    Index size_{0};

    using Pieces = std::list<Piece>;
    Pieces pieces_;

    struct Piece_Result {
      Pieces::iterator which;
      Index relative_index{0};
    };
    Piece_Result piece_at(Index);

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