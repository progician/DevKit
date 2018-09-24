#include "TextModel/PieceTable.h"
#include <numeric>
#include <stdexcept>


namespace TextModel {

  PieceTable::OperationAt
  PieceTable::operation_at(Index index) {
    Index current_start{0};
    auto operation_at_index = std::find_if(
        operations_.begin(), operations_.end(),
        [&current_start, index](const auto& operation) {
          if (current_start <= index && index < current_start + operation.size) {
            return true;
          }
          current_start += operation.size;
          return false;
        }
    );

    return {operation_at_index, index - current_start};
  }

  PieceTable::PieceTable(String original)
  : original_{std::move(original)}
  , size_{original_.size()}
  , operations_{{original_.size(), original_, 0}} {}


  void PieceTable::insert(Index index, String piece) {
    const Index appending_at{inserted_.size()};
    const Index piece_size{piece.size()};
    inserted_.append(std::move(piece));

    auto insertion_point = operation_at(index);
    if (insertion_point.which == operations_.end()) {
      operations_.push_back(Operation{piece_size, inserted_, appending_at});
    }
    else {
      auto insertion_it = insertion_point.which;
      const auto operation_at_insertion = *insertion_it;
      insertion_it->size = insertion_point.relative_index;

      ++insertion_it;
      insertion_it = operations_.insert(insertion_it, {
          piece_size, inserted_, appending_at
      });

      ++insertion_it;
      operations_.insert(insertion_it, {
          operation_at_insertion.size - insertion_point.relative_index,
          operation_at_insertion.source,
          operation_at_insertion.source_begin + insertion_point.relative_index
      });
    }
    size_ += piece_size;
  }


  void PieceTable::remove(Range range) {
    auto start_operation = operation_at(range.start);
    auto end_operation = operation_at(range.end);

    const auto op_to_split = *(end_operation.which);

    auto it = start_operation.which;
    if (start_operation.relative_index > 0) {
      it->size = range.start;
      ++it;
    }

    if (it != std::end(operations_)) {
      operations_.erase(it, end_operation.which);
    }

    if (end_operation.relative_index > 0) {
      operations_.push_back(Operation{
          op_to_split.size - end_operation.relative_index,
          op_to_split.source,
          op_to_split.source_begin + end_operation.relative_index
      });
    }
  }

  String PieceTable::text_of(Range) const {
    return std::accumulate(
        operations_.begin(), operations_.end(), String{},
        [](String lhs, Operation rhs) {
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