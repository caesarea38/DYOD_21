#include <iomanip>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "base_segment.hpp"
#include "chunk.hpp"

#include "utils/assert.hpp"

namespace opossum {

void Chunk::add_segment(std::shared_ptr<BaseSegment> segment) { _chunk_container_for_segments.push_back(segment); }

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  // Check if the number of values matches with the size of one row of the table
  DebugAssert((values.size() == _chunk_container_for_segments.size()),
              "Number of values does not match " + "with the size of one row");
  auto values_iterator = values.cbegin();
  for (const std::shared_ptr<BaseSegment>& segment : _chunk_container_for_segments) {
    // append the 1st value of the values vector to the first segment, the 2nd value to the 2nd segment etc.
    (*segment).append(*values_iterator);
    values_iterator++;
  }
}

std::shared_ptr<BaseSegment> Chunk::get_segment(ColumnID column_id) const {
  return std::atomic_load(&_chunk_container_for_segments.at(column_id));
}

ColumnCount Chunk::column_count() const {
  // the number of columns equals the number of elements in the chunk vector
  return ColumnCount{static_cast<ColumnCount>(_chunk_container_for_segments.size())};
}

ChunkOffset Chunk::size() const {
  if (_chunk_container_for_segments.empty()) {
    return 0;
  }
  return static_cast<ChunkOffset>(get_segment(ColumnID{0})->size());
}

}  // namespace opossum
