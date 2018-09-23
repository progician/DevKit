#include "TextModel/PieceTable.h"

namespace TextModel {

  PieceTable::PieceTable(String original)
  : original_{std::move(original)} {}

  void PieceTable::insert(Index, String) {}
  void PieceTable::remove(Range) {}
  String PieceTable::text_of(Range) { return original_; }

  Index PieceTable::size() const {
    return 0;
  }

} // TextModel