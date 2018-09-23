#pragma once

#include "TextModel/Buffer.h"
#include <iostream>

namespace TextModel {

class PieceTable
: public Buffer {
  String original_;

public:
  PieceTable() = default;
  PieceTable(String);
  ~PieceTable() override = default;

  void insert(Index, String) override;
  void remove(Range) override;
  String text_of(Range) override;
  Index size() const override;

  bool operator==(PieceTable const& rhs) const {
    return original_ == rhs.original_;
  };
  bool operator!=(PieceTable const& rhs) const {
    return original_ != rhs.original_;
  };

  friend std::ostream& operator<<(std::ostream&, const PieceTable&);
};



} // PieceTable