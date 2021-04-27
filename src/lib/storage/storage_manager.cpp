#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

StorageManager* StorageManager::_storage_manager = nullptr;

StorageManager& StorageManager::get() {
  // if the storage manager object already exists, return the reference to that object otherwise create a new one
  if (!_storage_manager) {
    _storage_manager = new StorageManager();
  }
  return *(_storage_manager);

  //  return *(new StorageManager());
  // A really hacky fix to get the tests to run - replace this with your implementation
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  const auto table_found = _tables.find(name);
  Assert(table_found == _tables.end(),
         "A table with the given name " + name + "already exists." + "Therefore the given table will not be added.");
  _tables.insert(std::make_pair(name, std::move(table)));
}

void StorageManager::drop_table(const std::string& name) {
  const auto table_found = _tables.find(name);
  Assert(table_found != _tables.end(), "Cannot drop the table, because the name was not found");
  _tables.erase(table_found);
}

// second contains the reference to the table that shall be found while first contains the table name
std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  const auto table_found = _tables.find(name);
  Assert(table_found != _tables.end(), "Table with given name " + name + "could not be found");
  return table_found->second;
}

bool StorageManager::has_table(const std::string& name) const {
  const auto table_found = _tables.find(name);
  return table_found != _tables.end();
}

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> names;
  names.reserve(_tables.size());
  for (const auto& item : _tables) {
    names.push_back(item.first);
  }
  return names;
}

void StorageManager::print(std::ostream& out) const {
  out << "----------" << '\n';
  for (const auto& table : _tables) {
    out << "Table Name:     " + table.first << '\n';
    out << "#columns  :     " + std::to_string(table.second->column_count()) << '\n';
    out << "#rows     :     " + std::to_string(table.second->row_count()) << '\n';
    out << "#chunks   :     " + std::to_string(table.second->chunk_count()) << '\n';
  }
}

void StorageManager::reset() {
  _storage_manager = nullptr;
  _storage_manager = &get();
}

}  // namespace opossum
