#include <string>

// A simple class that is used to store the attributes of a column of a table
class TableColumnAttributes {
 public:
  TableColumnAttributes() = default;
  TableColumnAttributes(const std::string& column_name, const std::string& column_type);

  std::string _column_name;
  std::string _column_type;
};
