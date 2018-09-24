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
    REQUIRE(text_of(piece_table) == "some text");
  }

  SECTION("size is the full text size") {
    String arbitrary_text{"Hello, World!"};
    PieceTable piece_table{arbitrary_text};
    REQUIRE(piece_table.size() == arbitrary_text.size());
  }
}

TEST_CASE("Manipulating PieceTables consist of") {
  static const String original_text{"Hello, World!"};
  static const String wedged_text{"wonderful "};
  PieceTable piece_table{original_text};

  SECTION("inserting a piece of string at some position") {
    piece_table.insert(7, wedged_text);
    REQUIRE(text_of(piece_table) == "Hello, wonderful World!");

    SECTION("the size of piece table grows with the length of the inserted"
            "string") {
      const auto new_size = std::size(original_text) + std::size(wedged_text);
      REQUIRE(piece_table.size() == new_size); 
    }
  }

  SECTION("removing substring from a single piece") {
    piece_table.remove({5, 7});
    REQUIRE(text_of(piece_table) == "HelloWorld!");
  }

  SECTION("removing substring that includes a previous insertion") {
    piece_table.insert(7, wedged_text);
    SECTION("entirely") {
      piece_table.remove({7, 7 + std::size(wedged_text)});
      REQUIRE(text_of(piece_table) == original_text);
    }
  }
}