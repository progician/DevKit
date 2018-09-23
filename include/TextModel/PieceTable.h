#pragma once

#include "TextModel/Buffer.h"
#include <iostream>
#include <list>

namespace TextModel {

  class PieceTable
  : public Buffer {
    String original_;
    String inserted_;

    struct Operation {
      Index size;
      String& source;
      Index source_begin;
    };
    using OperationList = std::list<Operation>;
    OperationList operations_;

    struct OperationAt {
      OperationList::iterator which;
      Index relative_index{0};
    };
    OperationAt operation_at(Index);

  public:
    PieceTable() = default;
    PieceTable(String);
    ~PieceTable() override = default;

    void insert(Index, String) override;
    void remove(Range) override;

    String text_of(Range) const override;
    Index size() const override;

    bool operator==(PieceTable const& rhs) const {
      return original_ == rhs.original_;
    };
    bool operator!=(PieceTable const& rhs) const {
      return original_ != rhs.original_;
    };

    friend std::ostream& operator<<(std::ostream&, const PieceTable&);
  };

  String text_of(PieceTable const&);
} // PieceTable