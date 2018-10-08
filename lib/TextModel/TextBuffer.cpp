#include "TextModel/TextBuffer.h"

namespace TextModel {
  String TextBuffer::text_of(Span const& piece) const {
    const auto& from{ piece.storage == Storage::Original
        ? original_
        : inserted_
    };

    return String(
        from.begin() + piece.start_in_storage,
        from.begin() + piece.start_in_storage + piece.length
    );
  }


  TextBuffer::ListPosition TextBuffer::piece_at(Index index) const {
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


  TextBuffer::MutListPosition TextBuffer::piece_at(Index index) {
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


  void TextBuffer::insert(Index index, String text) {
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


  void TextBuffer::remove(Range const& range) {
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


  String TextBuffer::text_of(Range const& range) const {
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

  Index TextBuffer::size() const {
    return std::accumulate(
        pieces_.begin(), pieces_.end(), Index{0},
        [](Index lhs, Span const& rhs) {
          return lhs + rhs.length;
        }
    );
  }

} // TextModel