#include "TextModel/TextModel.h"
#include "catch2/catch.hpp"

TEST_CASE("TextModel represents a simple model of a text flow object") {
  TextModel::TextModel text_model{"Hello,World"};
  REQUIRE(text_model.line_count() == 1);
}