#pragma once

#include "TextModel/Buffer.h"

namespace TextModel {

class PieceTable
: public Buffer {
public:
  ~PieceTable() override = default;

  void insert(Index, String) override;
  void remove(Range) override;
  String textIn(Range) override;
  Index size() const override;
};

} // PieceTable