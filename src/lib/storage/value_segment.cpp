#include "value_segment.hpp"

#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "type_cast.hpp"
#include "utils/assert.hpp"

namespace opossum {

template <typename T>
AllTypeVariant ValueSegment<T>::operator[](const ChunkOffset chunk_offset) const {
  DebugAssert(chunk_offset < _valuesegment_values.size(), "Given chunk_offset outside boundaries!");
  return _valuesegment_values.at(chunk_offset);
}

template <typename T>
void ValueSegment<T>::append(const AllTypeVariant& val) {
  // type_cast<> already throws an excpetion if the types mismatch, otherwise appends to the vector
  _valuesegment_values.push_back(type_cast<T>(val));
}

template <typename T>
ChunkOffset ValueSegment<T>::size() const {
  return static_cast<ChunkOffset>(_valuesegment_values.size());
}

EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
