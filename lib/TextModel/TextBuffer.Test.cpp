#include "catch2/catch.hpp"
#include "TextModel/TextBuffer.h"
#include <random>

TEST_CASE("Building and reading from TextBuffers", "[unit]") {
  TextModel::TextBuffer buffer;
  static const TextModel::String ArbitraryText{"Hello, World!"};
  buffer.insert(0, ArbitraryText);
  REQUIRE(FullTextOf(buffer) == ArbitraryText);
  REQUIRE(buffer.size() == ArbitraryText.size());
}


TEST_CASE("Inserting into the middle of text", "[unit]") {
  TextModel::TextBuffer buffer;
  static const TextModel::String Original{"Hello, World!"};
  static const TextModel::String Inserted{"wonderful "};
  buffer.insert(0, Original);
  buffer.insert(6, Inserted);

  SECTION("makes the size the sum of the pieces", "[unit]") {
    REQUIRE(buffer.size() == Original.size() + Inserted.size());
  }

  SECTION("the full string will contain the inserted string in the middle") {
    const TextModel::Range insertion_range{6, 6 + Inserted.size()};
    REQUIRE(buffer.text_of(insertion_range) == Inserted);
  }
}

TEST_CASE("Removing from a text", "[unit]") {
  TextModel::TextBuffer buffer;
  static const TextModel::String ArbitraryText{"Hello, World!"};
  static const TextModel::Index RangeLength{2};
  buffer.insert(0, ArbitraryText);
  buffer.remove(TextModel::Range{5, 5 + RangeLength});

  SECTION("the size gets reduced by the length of the range") {
    REQUIRE(buffer.size() == ArbitraryText.size() - RangeLength);
  }

  SECTION("the string will be removed from the text") {
    REQUIRE(TextModel::FullTextOf(buffer) == "HelloWorld!");
  }
}

TEST_CASE("Removing a string of insertions", "[unit]") {
  TextModel::TextBuffer buffer;
  TextModel::String pieces[] = {
    "Hello",
    ", ",
    "World",
    "!"
  };
  for (const auto& piece : pieces) {
    buffer.insert(buffer.size(), piece);
  }

  REQUIRE(TextModel::FullTextOf(buffer) == "Hello, World!");

  SECTION("will work exactly as it was in the middle of a single insertion") {
    const TextModel::Range range{2, buffer.size() - 1};
    buffer.remove(range);
    REQUIRE(TextModel::FullTextOf(buffer) == "He!");
  }
}

TEST_CASE("Benchmark text buffer", "![benchmark]") {
  static constexpr size_t SufficientIteration{10000};
  std::random_device rd;
  std::mt19937 mt(rd());
  TextModel::TextBuffer buffer;
  BENCHMARK("insertion single characters at random position") {
    for (auto loop_count = 0; loop_count < SufficientIteration; ++loop_count) {
      std::uniform_int_distribution<TextModel::Index> dist(0, loop_count);
      const TextModel::Index where = dist(mt);
      buffer.insert(where, "ABC");
    }
  }
}