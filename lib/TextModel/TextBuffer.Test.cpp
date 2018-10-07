#include "catch2/catch.hpp"
#include "TextModel/Buffer.h"
#include "Generics/Tree.h"
#include <list>


template<class Container, class ForwardIterator, typename T>
  ForwardIterator InsertAfter(Container c, ForwardIterator it, T value) {
    if (it != c.end()) {
      it++;
    }
    return c.insert(it, value);
  }


namespace TextModel {
  enum class Storage {
    Original, Inserted
  };

  struct Span {
    Storage storage;
    Index start_in_storage;
    Index length; 

    Span(Storage which, Index s, Index l)
    : storage{which}
    , start_in_storage{s}
    , length{l} {}
  };


  class TextBuffer
  : public Buffer
  {
    String original_;
    String inserted_;

    using PieceList = std::list<Span>;
    PieceList pieces_;

    String text_of(Span const& piece) const {
      const auto& from{ piece.storage == Storage::Original
          ? original_
          : inserted_
      };

      return String(
          from.begin() + piece.start_in_storage,
          from.begin() + piece.start_in_storage + piece.length
      );
    }

    using ListPosition = std::pair<PieceList::const_iterator, Index>;
    ListPosition piece_at(Index index) const {
      Index text_position{0};
      auto piece = std::find_if(
          pieces_.begin(), pieces_.end(),
          [&text_position, index](const auto& piece) {
            const auto end_of_piece = text_position + piece.length;
            if (index < end_of_piece) {
              return true;
            }
            else {
              text_position = end_of_piece;
              return false;
            }
          }
      );
      return {piece, text_position};
    }

    using MutListPosition = std::pair<PieceList::iterator, Index>;
    MutListPosition piece_at(Index index) {
      Index text_position{0};
      auto piece = std::find_if(
          pieces_.begin(), pieces_.end(),
          [&text_position, index](const auto& piece) {
            const auto end_of_piece = text_position + piece.length;
            if (index < end_of_piece) {
              return true;
            }
            else {
              text_position = end_of_piece;
              return false;
            }
          }
      );
      return {piece, text_position};
    }

  public:
    void insert(Index index, String text) override {
      const auto append_index = inserted_.size();
      inserted_ += text;
      auto piece = piece_at(index);
      if (piece.first == pieces_.end()) {
        pieces_.emplace_back(Storage::Inserted, append_index, text.size());
      }
      else {
        auto first_split{piece.first};
        const auto relative_position{index - piece.second};
        if (relative_position > 0) {
          auto second_split{InsertAfter(pieces_, first_split, *first_split)};
          first_split->length = relative_position;
          second_split->start_in_storage += relative_position;
          second_split->length -= relative_position;
        }
        InsertAfter(pieces_, first_split, Span{Storage::Inserted, append_index, text.size()});
      }
    }

    void remove(Range) override {}

    String text_of(Range range) const override {
      auto pieces_begin{piece_at(range.start)};
      auto pieces_end{piece_at(range.end)};

      String result;
      const auto relative_start = range.start - pieces_begin.second;
      if (relative_start > 0) {
        const auto first_bit{text_of(*(pieces_begin.first))};
        const auto piece_length = pieces_begin.first == pieces_end.first
            ? range.end - pieces_end.second
            : first_bit.size()
        ;

        result = String(
            first_bit.begin() + relative_start,
            first_bit.begin() + piece_length
        );
        pieces_begin.first++;
        pieces_begin.second = 0;
      }
      return std::accumulate(
          pieces_begin.first, pieces_end.first,
          result,
          [this](String const& lhs, Span const& rhs) {
            return lhs + text_of(rhs);
          }
      );
    }

    Index size() const override {
      return std::accumulate(
          pieces_.begin(), pieces_.end(), Index{0},
          [](Index lhs, Span const& rhs) {
            return lhs + rhs.length;
          }
      );
    }
  };
} // TextModel

TEST_CASE("Building and reading from TextBuffers") {
  TextModel::TextBuffer buffer;
  static const TextModel::String ArbitraryText{"Hello, World!"};
  buffer.insert(0, ArbitraryText);
  REQUIRE(FullTextOf(buffer) == ArbitraryText);
  REQUIRE(buffer.size() == ArbitraryText.size());
}


TEST_CASE("Inserting into the middle of text") {
  TextModel::TextBuffer buffer;
  static const TextModel::String Original{"Hello, World!"};
  static const TextModel::String Inserted{"wonderful "};
  buffer.insert(0, Original);
  buffer.insert(6, Inserted);

  SECTION("makes the size the sum of the pieces") {
    REQUIRE(buffer.size() == Original.size() + Inserted.size());
  }

  SECTION("the full string will contain the inserted string in the middle") {
    const TextModel::Range insertion_range{6, 6 + Inserted.size()};
    REQUIRE(buffer.text_of(insertion_range) == Inserted);
  }
}