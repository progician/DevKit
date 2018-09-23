#include "catch2/catch.hpp"
#include "TextModel/PieceTable.h"

TEST_CASE("PieceTable are implementing the Buffer interface") {
  TextModel::PieceTable piece_table;
  SECTION("it's empty by default") {
    REQUIRE(piece_table.size() == 0);
  }
}