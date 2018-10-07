#include "catch2/catch.hpp"

#include <algorithm>
#include <memory>

template<class T>
  using SharedPtr = std::shared_ptr<T>;


template<class T>
  class Tree {
  public:
    enum class Colour{Red, Black};
    struct Node {
      using NodePtr = SharedPtr<const Node>;


      Colour colour;
      NodePtr left;
      T value;
      NodePtr right;

      Node(Colour c, NodePtr lhs, T v, NodePtr rhs) noexcept
      : colour{c}
      , left{lhs}
      , value{v}
      , right{rhs} {}
    };
    SharedPtr<const Node> root_;

    explicit Tree(SharedPtr<const Node> root) noexcept
    : root_{root} {}

  public:
    Tree() = default;
    Tree(Tree const& lhs, T value, Tree const& rhs) noexcept
    : root_{std::make_shared<const Node>(lhs.root_, value, rhs.root_)} {}

    Tree(std::initializer_list<T> init_list) {
      Tree result;
      for (auto elem : init_list) {
        result = With(result, elem);
      }
      root_ = result.root_;
    }

    template<class InputIterator>
      Tree(InputIterator begin, InputIterator end) {
        Tree result;
        std::for_each(
            begin, end,
            [&result](T const& v) { result = With(result, v); }
        );
        root_ = result.root_;
      }

    bool empty() const noexcept { return !root_; }
    T root() const noexcept { return root_->value; }
    Tree left() const noexcept { return Tree{root_->left}; }
    Tree right() const noexcept { return Tree{root_->right}; }
    Colour root_colour() const noexcept { return root_->colour; }
  };


template<class T>
  Tree<T> With(Tree<T> const& tree, T value) {
    using TreeType = Tree<T>;
    if (tree.empty()) {
      return Tree{TreeType{}, value, TreeType{}};
    }
    else {
      T root{tree.root()};
      if (value < root) {
        return Tree{With(tree.left(), value), root, tree.right()};
      }
      else if (root < value) {
        return Tree{tree.left(), root, With(tree.right(), value)};
      }
      else {
        return tree;
      }
    }
  }


template<class T>
  bool DoubledLeft(Tree<T> const& tree) {
    return !tree.empty()
        && tree.root_colour() == Tree::Colour::Red
        && !tree.left().empty()
        && tree.left().root_colour() == Tree::Colour::Red
    ;
  }


template<class T>
  bool DoubledRight(Tree<T> const& tree) {
    return !tree.empty()
        && tree.root_colour() == Tree::Colour::Red
        && !tree.right().empty()
        && tree.right().root_colour() == Tree::Colour::Red
    ;
  }


template<class T>
  Tree<T> Balanced(
      Tree<T>::Node::Colour colour,
      Tree<T> const& lhs, T value, Tree<T> const& rhs
  ) {
    using TreeType = Tree<T>;
    if (colour == Tree<T>::Node::Colour::Black) {
      if (DoubledLeft(lhs)) {
        return TreeType{
            TreeType::Node::Colour::Red,
            lhs.left().paint(Black),
            lhs.root(),
            TreeType{TreeType::Node::Colour::Black, lhs.right(), value, rhs}
        };
      }
      else if (DoubledRight(lhs)) {
        return TreeType{
            TreeType::Node::Colour::Red,
            TreeType{TreeType::Node::Colour::Black, lhs.left(), left.root(), lhs.right().left()}
            lhs.right().root(),
            TreeType{TreeType::Node::Colour::Black, lhs.right().right(), value, rhs}
        };
      }
      else if (DoubledLeft(rhs)) {
        return TreeType{
            TreeType::Node::Colour::Red,
            TreeType{TreeType::Node::Colour::Black, lhs, value, lhs.right().left()}
            lhs.right().root(),
            TreeType{TreeType::Node::Colour::Black, lhs.right().right(), value, rhs}
        };
      }
    }
  }


template<class T>
  Tree<T> BalanceWith(Tree<T> const& tree, T value) {
    using TreeType = Tree<T>;
    if (tree.empty()) {
      return Tree{TreeType{}, value, TreeType{}};
    }
    else {
      T root{tree.root()};
      const auto colour = root.colour;
      if (value < root) {
        return Balanced{colour, rootBalanbceWith(tree.left(), value), root, tree.right()};
      }
      else if (root < value) {
        return Balanced{colour, tree.left(), root, BalanceWith(tree.right(), value)};
      }
      else {
        return tree;
      }
    }
  }


template<class T>
  bool Has(Tree<T> const& tree, T value) {
    if (tree.empty()) {
      return false;
    }
    else {
      T root{tree.root()};
      if (value < root) {
        return Has(tree.left(), value);
      }
      else if (root < value) {
        return Has(tree.right(), value);
      }
      else {
        return true;
      }
    }
  }

template<class T>
  std::size_t HeightOf(Tree<T> const& tree) {
    if (tree.empty()) {
      return 0;
    }
    else {
      const auto height_of_left = HeightOf(tree.left());
      const auto height_of_right = HeightOf(tree.right());
      return std::max(height_of_left, height_of_right) + 1;
    }
  }

TEST_CASE("Simple binary search tree") {
  using TreeOfIntegers = Tree<int>;
  TreeOfIntegers tree{4, 2, 7};
  REQUIRE(Has(tree, 7));
  REQUIRE(!Has(tree, 13));
}

TEST_CASE("Binary tree can be constructed from any sequence") {
  using TreeOfIntegers = Tree<int>;
  std::vector<int> test_sequence{32, 123, 89, 22};
  TreeOfIntegers tree(test_sequence.begin(), test_sequence.end());
  REQUIRE(Has(tree, 89));
  REQUIRE(!Has(tree, 100));
}


TEST_CASE("The height of a tree") {
  using TreeOfIntegers = Tree<int>;
  TreeOfIntegers empty_tree;
  SECTION("for an empty tree") {
    REQUIRE(HeightOf(empty_tree) == 0);
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
    REQUIRE(HeightOf(tree_of_single_element) < consecutively_less_elements.size());
  }
}