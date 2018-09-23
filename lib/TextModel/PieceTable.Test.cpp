#include "catch2/catch.hpp"
#include "TextModel/PieceTable.h"

using TextModel::PieceTable;
using TextModel::String;
using TextModel::Range;
using TextModel::Index;

TEST_CASE("PieceTable are implementing the Buffer interface") {
  SECTION("it's empty by default") {
    PieceTable piece_table;
    REQUIRE(piece_table.size() == 0);
  }

  SECTION("can be initialised by a string") {
    PieceTable piece_table{"some text"};
    Range full_range{0, piece_table.size()};
    REQUIRE(piece_table.text_of(full_range) == "some text");
  }

  SECTION("can be equality-compared") {
    const PieceTable hello_world_text{"Hello, World!"};
    SECTION("for equality") {
      const PieceTable same_hello_world_text{"Hello, World!"};
      REQUIRE(hello_world_text == same_hello_world_text);
    }
    SECTION("for unequality") {
      const PieceTable something_text{"something"};
      REQUIRE(hello_world_text != something_text);
    }
  }
}

TEST_CASE("Inserting into PieceTable") {
  PieceTable piece_table{"Hello, World!"};
  piece_table.insert(7, "wonderful ");
  REQUIRE(text_of(piece_table) == "Hello, wonderful World!");
}