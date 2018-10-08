#pragma once

#include "TextModel/Buffer.h"
#include "Generics/Algorithms.h"
#include <list>
#include <numeric>

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
          auto second_split{Generics::InsertAfter(pieces_, first_split, *first_split)};
          first_split->length = relative_position;
          second_split->start_in_storage += relative_position;
          second_split->length -= relative_position;
        }
        Generics::InsertAfter(pieces_, first_split, Span{Storage::Inserted, append_index, text.size()});
      }
    }

    void remove(Range const& range) override {
      auto pieces_begin{piece_at(range.start)};
      auto pieces_end{piece_at(range.end)};

      if (pieces_begin.first == pieces_end.first) {
        pieces_begin.first = pieces_.insert(pieces_begin.first, *pieces_begin.first);
        pieces_begin.first->length = range.start - pieces_begin.second;
        pieces_end.first->start_in_storage += range.end - pieces_begin.second;
        pieces_end.first->length -= range.end - pieces_begin.second;
      }
      else {
        if (range.start - pieces_begin.second > 0) {
          pieces_begin.first->length = range.start - pieces_begin.second;
          pieces_begin.first++;
          pieces_begin.second = 0;
        }
        pieces_.erase(pieces_begin.first, pieces_end.first);

        const auto relative_position{range.end - pieces_end.second};
        pieces_end.first->length -= relative_position;
        pieces_end.first->start_in_storage += relative_position;
      }
    }

    String text_of(Range const& range) const override {
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