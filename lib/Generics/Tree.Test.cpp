#include "catch2/catch.hpp"

#include "Generics/Tree.h"

TEST_CASE("Simple binary search tree") {
  using TreeOfIntegers = Generics::Tree<int>;
  TreeOfIntegers tree{4, 2, 7};
  REQUIRE(Generics::Has(tree, 7));
  REQUIRE(!Generics::Has(tree, 13));
}

TEST_CASE("Binary tree can be constructed from any sequence") {
  using TreeOfIntegers = Generics::Tree<int>;
  std::vector<int> test_sequence{32, 123, 89, 22};
  TreeOfIntegers tree(test_sequence.begin(), test_sequence.end());
  REQUIRE(std::all_of(
      test_sequence.begin(), test_sequence.end(),
      [&tree](const auto v) { return Generics::Has(tree, v); }
  ));
  REQUIRE(Generics::Has(tree, 89));
  REQUIRE(!Generics::Has(tree, 100));
}


TEST_CASE("The height of a tree") {
  using TreeOfIntegers = Generics::Tree<int>;
  TreeOfIntegers empty_tree;
  SECTION("for an empty tree") {
    REQUIRE(Generics::HeightOf(empty_tree) == 0);
  }

  SECTION("single element tree") {
    const TreeOfIntegers tree_of_single_element{123};
    REQUIRE(HeightOf(tree_of_single_element) == 1);
  }

  SECTION("consecutively less elements") {
    std::vector<int> consecutively_less_elements{10, 9, 8};
    const TreeOfIntegers tree_of_single_element{
      consecutively_less_elements.begin(), consecutively_less_elements.end()
    };
    REQUIRE(HeightOf(tree_of_single_element) == consecutively_less_elements.size());
  }
}


template<typename Container>
  Container Sorted(Container v) {
    Container result(std::move(v));
    std::sort(result.begin(), result.end());
    return result;
  }


TEST_CASE("Trees can be iterated through inorder traversal") {
  using TreeOfIntegers = Generics::Tree<int>;

  static const std::vector<int> ArbitraryIntegers{2, 8, 1, 99, 123, 23, 12};
  const TreeOfIntegers tree{ArbitraryIntegers.begin(), ArbitraryIntegers.end()};
  const auto SortedIntegers{Sorted(ArbitraryIntegers)};
  REQUIRE(std::equal(tree.begin(), tree.end(), SortedIntegers.begin()));
}


TEST_CASE("Trees can be created from a Tree by removing an element") {
  using Tree = Generics::Tree<int>;

  const Tree numbers{321, 123, 231, 132};
  SECTION("if the element doesn't exist it will return the same Tree") {
    REQUIRE(Generics::Removed(numbers, 2) == numbers);
  }

  SECTION("tree with an element removed it will have it no more") {
    static constexpr int ArbitraryElement{231};
    const auto reduced{Generics::Removed(numbers, ArbitraryElement)};
    REQUIRE(!Generics::Has(reduced, ArbitraryElement));
  }
}