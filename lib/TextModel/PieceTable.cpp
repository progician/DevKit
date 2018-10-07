#include "TextModel/PieceTable.h"
#include <numeric>
#include <stdexcept>


namespace TextModel {

  PieceTable::Piece_Result
  PieceTable::piece_at(Index index) {
    Index current_start{0};
    auto piece_at_index = std::find_if(
        pieces_.begin(), pieces_.end(),
        [&current_start, index](const auto& piece) {
          if (current_start <= index && index < current_start + piece.size) {
            return true;
          }
          current_start += piece.size;
          return false;
        }
    );

    return {piece_at_index, index - current_start};
  }

  PieceTable::PieceTable(String original)
  : original_{std::move(original)}
  , size_{original_.size()}
  , pieces_{{original_.size(), original_, 0}} {}


  void PieceTable::insert(Index index, String piece) {
    const Index appending_at{inserted_.size()};
    const Index piece_size{piece.size()};
    inserted_.append(std::move(piece));

    auto insertion_point = piece_at(index);
    if (insertion_point.which == pieces_.end()) {
      pieces_.push_back(Piece{piece_size, inserted_, appending_at});
    }
    else {
      auto insertion_it = insertion_point.which;
      const auto piece_at_insertion = *insertion_it;
      insertion_it->size = insertion_point.relative_index;

      ++insertion_it;
      insertion_it = pieces_.insert(insertion_it, {
          piece_size, inserted_, appending_at
      });

      ++insertion_it;
      pieces_.insert(insertion_it, {
          piece_at_insertion.size - insertion_point.relative_index,
          piece_at_insertion.source,
          piece_at_insertion.source_begin + insertion_point.relative_index
      });
    }
    size_ += piece_size;
  }


  void PieceTable::remove(Range const& range) {
    auto start_piece = piece_at(range.start);
    auto end_piece = piece_at(range.end);

    const auto piece_to_split = *(end_piece.which);

    auto it = start_piece.which;
    if (start_piece.relative_index > 0) {
      it->size = range.start;
      ++it;
    }

    if (it != std::end(pieces_)) {
      pieces_.erase(it, end_piece.which);
    }

    if (end_piece.relative_index > 0) {
      pieces_.push_back(Piece{
          piece_to_split.size - end_piece.relative_index,
          piece_to_split.source,
          piece_to_split.source_begin + end_piece.relative_index
      });
    }
  }

  String PieceTable::text_of(Range const&) const {
    return std::accumulate(
        pieces_.begin(), pieces_.end(), String{},
        [](String lhs, Piece rhs) {
          const String piece{&rhs.source[0] + rhs.source_begin, rhs.size};
          lhs.append(piece);
          return lhs;
        }
    );
  }

  Index PieceTable::size() const {
    return size_;
  }

  String text_of(PieceTable const& piece_table) {
    return piece_table.text_of({0, piece_table.size()});
  }

  std::ostream& operator<<(std::ostream& lhs, PieceTable const& rhs) {
    lhs << rhs.original_;
    return lhs;
  }

} // TextModel