#include "catch2/catch.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <stack>

template<class T>
  using SharedPtr = std::shared_ptr<T>;


template<class T>
  class Tree {
  public:
    struct Node;
    using NodePtr = SharedPtr<const Node>;
    struct Node {
      NodePtr left;
      T value;
      NodePtr right;

      Node(NodePtr lhs, T v, NodePtr rhs) noexcept
      : left{lhs}
      , value{v}
      , right{rhs} {}
    };
    NodePtr root_;

  public:
    Tree() = default;
    Tree(Tree const& lhs, T value, Tree const& rhs) noexcept
    : root_{std::make_shared<const Node>(lhs.root_, value, rhs.root_)} {}
    explicit Tree(NodePtr root) noexcept : root_{root} {}


    Tree(std::initializer_list<T> init_list) {
      Tree result;
      for (auto elem : init_list) {
        result = Inserted(result, elem);
      }
      root_ = result.root_;
    }

    template<class InputIterator>
      Tree(InputIterator begin, InputIterator end) {
        Tree from_sequence;
        std::for_each(begin, end,
            [&from_sequence](T elem) {
              from_sequence = Inserted(from_sequence, elem);
            }
        );
        root_ = from_sequence.root_;
      }

    bool empty() const noexcept { return !root_; }
    T root() const noexcept { return root_->value; }
    Tree left() const noexcept { return Tree{root_->left}; }
    Tree right() const noexcept { return Tree{root_->right}; }

    class Iterator
    : public std::iterator<std::output_iterator_tag, T> {
      NodePtr current_{nullptr};
      std::stack<NodePtr> path_;

      Iterator(Tree const& tree) {
        current_ = tree.root_;
        while (current_->left) {
          path_.push(current_);
          current_ = current_->left;
        }
      }

      Iterator() = default;

      friend class Tree;

    public:
      Iterator(Iterator const& other)
      : current_{other.current_}
      , path_{other.path_} {}

      T const& operator*() const {
        return current_->value;
      }

      T const* operator->() const {
        return current_.get();
      }

      Iterator& operator++() {
        if (!current_) {
          return *this;
        }
        else if (path_.empty()) {
          current_.reset();
          return *this;
        }
        else if (current_ == path_.top()->left) {
          current_ = path_.top();
          return *this;
        }
        else if (current_ == path_.top() && current_->right) {
          current_ = current_->right;
          while (current_->left) {
            current_ = current_->left;
          }
          return *this;
        }
        else {
          path_.pop();
          current_ = path_.empty() ? NodePtr{} : path_.top();
          return *this;
        }
      }

      Iterator operator++(int) const {
        Iterator result = *this;
        result++;
        return result;
      }

      Iterator& operator=(Iterator const& rhs) = default;
      bool operator==(Iterator const& rhs) const {
        return current_ == rhs.current_;
      }
      bool operator!=(Iterator const& rhs) const {
        return current_ != rhs.current_;
      }
    };

    Iterator begin() const { return Iterator{*this}; }
    Iterator end() const { return Iterator{}; }
    bool operator==(Tree const& rhs) const { return root_ == rhs.root_; }
    bool operator!=(Tree const& rhs) const { return root_ != rhs.root_; }
  };


template<class T>
  Tree<T> Inserted(Tree<T> const& tree, T value) {
    using TreeType = Tree<T>;
    if (tree.empty()) {
      return Tree{TreeType{}, value, TreeType{}};
    }
    else {
      T root{tree.root()};
      if (value < root) {
        return Tree{Inserted(tree.left(), value), root, tree.right()};
      }
      else if (root < value) {
        return Tree{tree.left(), root, Inserted(tree.right(), value)};
      }
      else {
        return tree;
      }
    }
  }

template<class T>
  Tree<T> Removed(Tree<T> const& tree, T value) {
    using TreeType = Tree<T>;
    if (tree.empty()) {
      return tree;
    }
    else {
      const T root{tree.root()};
      if (value < root) {
        const auto left = Removed(tree.left(), value);
        if (left != tree.left()) {
          return Tree{Removed(tree.left(), value), root, tree.right()};
        }
        else {
          return tree;
        }
      }
      else if (root < value) {
        const auto right = Removed(tree.right(), value);
        if (right != tree.right()) {
          return Tree{tree.left(), root, Removed(tree.right(), value)};
        }
        else {
          return tree;
        }
      }
      else {
        if (!tree.left().empty()) {
          return Tree{tree.left().left(), tree.left().root(), tree.right()};
        }
        else if (!tree.right().empty()) {
          return TreeType{TreeType{}, tree.right().root(), tree.right().right()};
        }
        else {
          return {};
        }
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
  std::all_of(test_sequence.begin(), test_sequence.end(), [&tree](const auto v) { return Has(tree, v); });
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
  using TreeOfIntegers = Tree<int>;

  static const std::vector<int> ArbitraryIntegers{2, 8, 1, 99, 123, 23, 12};
  const TreeOfIntegers tree{ArbitraryIntegers.begin(), ArbitraryIntegers.end()};
  const auto SortedIntegers{Sorted(ArbitraryIntegers)};
  REQUIRE(std::equal(tree.begin(), tree.end(), SortedIntegers.begin()));
}


TEST_CASE("Trees can be created from a Tree by removing an element") {
  using Tree = Tree<int>;

  const Tree numbers{321, 123, 231, 132};
  SECTION("if the element doesn't exist it will return the same Tree") {
    REQUIRE(Removed(numbers, 2) == numbers);
  }

  SECTION("tree with an element removed it will have it no more") {
    static constexpr int ArbitraryElement{231};
    const auto reduced{Removed(numbers, ArbitraryElement)};
    REQUIRE(!Has(reduced, ArbitraryElement));
  }
}