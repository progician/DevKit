#pragma once

namespace Generics {

  template<class Container, class ForwardIterator, typename T>
    ForwardIterator InsertAfter(Container c, ForwardIterator it, T value) {
      if (it != c.end()) {
        it++;
      }
      return c.insert(it, value);
    }

} // Generics