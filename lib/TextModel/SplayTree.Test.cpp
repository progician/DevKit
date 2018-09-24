#include "catch2/catch.hpp"

#include <memory>

template<class T>
  using SharedPtr = std::shared_ptr<T>;


template<class T>
  class Tree {
    struct Node {
      using NodePtr = SharedPtr<const Node>;

      NodePtr left;
      T value;
      NodePtr right;

      Node(NodePtr lhs, T v, NodePtr rhs) noexcept
      : left{lhs}
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
            [&result](T const& v) {
              result = With(result, v);
            }
        );
        root_ = result.root_;
      }

    bool empty() const noexcept { return !root_; }
    T root() const noexcept { return root_->value; }
    Tree left() const noexcept { return Tree{root_->left}; }
    Tree right() const noexcept { return Tree{root_->right}; }
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