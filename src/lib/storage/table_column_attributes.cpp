#include "table_column_attributes.hpp"

// A simple class that is used to store the attributes of a column of a table
TableColumnAttributes::TableColumnAttributes(const std::string& column_name, const std::string& column_type)
    : _column_name(column_name), _column_type(column_type) {}
