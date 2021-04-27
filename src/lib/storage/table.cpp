#include "table.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "value_segment.hpp"

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

Table::Table(const ChunkOffset target_chunk_size) : _target_chunk_size(target_chunk_size) {
  DebugAssert(target_chunk_size <= std::numeric_limits<ChunkOffset>::max() - 1, "Chunk size exceeds maximum size");
  DebugAssert(target_chunk_size > 0, "Chunk size must be greater than 0");

  _chunks.push_back(Chunk());
}

void Table::add_column(const std::string& name, const std::string& type) {
  DebugAssert(_chunks[0].size() == 0, "cannot add a new column in a table with at least one entry in it");

  // store the new column attributes in the attributes list
  TableColumnAttributes attributes = TableColumnAttributes(name, type);
  _column_attributes.push_back(attributes);

  // go through every chunk and add the new column (value_segment) to each chunk
  for (auto& item : _chunks) {
    resolve_data_type(type, [&](const auto data_type_t) {
      using ColumnDataType = typename decltype(data_type_t)::type;
      const auto value_segment = std::make_shared<ValueSegment<ColumnDataType>>();

      item.add_segment(value_segment);
    });
  }
}

void Table::append(const std::vector<AllTypeVariant>& values) {
  auto last_chunk = &_chunks[_chunks.size() - 1];

  // if that last chunk is full regarding the _target_chunk_size, we create a new empty chunk
  if (last_chunk->size() >= _target_chunk_size) {
    // create a new empty chunk
    _chunks.push_back(Chunk());

    // save the valuesegments according to the list column_attributes in the chunk
    for (const auto& item : _column_attributes) {
      resolve_data_type(item._column_type, [&](const auto data_type_t) {
        using ColumnDataType = typename decltype(data_type_t)::type;
        const auto value_segment = std::make_shared<ValueSegment<ColumnDataType>>();

        _chunks[_chunks.size() - 1].add_segment(value_segment);
      });
    }
    _chunks[_chunks.size() - 1].append(values);
    return;
  }
  last_chunk->append(values);
}

ColumnCount Table::column_count() const { return ColumnCount{(_column_attributes.size())}; }

uint64_t Table::row_count() const {
  uint64_t row_count = 0;
  for (const auto& chunk : _chunks) {
    row_count += chunk.size();
  }
  return row_count;
}

ChunkID Table::chunk_count() const { return ChunkID{_chunks.size()}; }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  const auto column_list_size = _column_attributes.size();
  auto column_id = 0ul;
  for (; column_id < column_list_size; ++column_id) {
    if (_column_attributes[column_id]._column_name == column_name) {
      return ColumnID{column_id};
    }
  }
  throw std::runtime_error("Could not find column " + column_name);
}

ChunkOffset Table::target_chunk_size() const { return _target_chunk_size; }

const std::vector<std::string>& Table::column_names() const {
  static std::vector<std::string> column_name_list;
  column_name_list.reserve(_column_attributes.size());
  for (const auto& item : _column_attributes) {
    column_name_list.push_back(item._column_name);
  }
  return column_name_list;
}

const std::string& Table::column_name(const ColumnID column_id) const {
  DebugAssert(column_id < _column_attributes.size() && column_id >= 0, "ColumnID out of range.");
  return (_column_attributes[column_id]._column_name);
}

const std::string& Table::column_type(const ColumnID column_id) const {
  DebugAssert(column_id < _column_attributes.size() && column_id >= 0, "ColumnID out of range.");
  return (_column_attributes[column_id]._column_type);
}

Chunk& Table::get_chunk(ChunkID chunk_id) {
  DebugAssert(chunk_id < _chunks.size(), "ChunkID out of range.");
  return _chunks[chunk_id];
}

const Chunk& Table::get_chunk(ChunkID chunk_id) const {
  DebugAssert(chunk_id < _chunks.size(), "ChunkID out of range.");
  return _chunks[chunk_id];
}

}  // namespace opossum
