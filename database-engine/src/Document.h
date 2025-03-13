#ifndef DOCUMENT_H
#define DOCUMENT_H
#include "json.hpp"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <string>

class Document {
  using json = nlohmann::json;

  void check_access();

public:
  json json_value;
  std::string id;
  bool deleted;
  std::chrono::system_clock::time_point last_access_time;
  std::string file_path; // Added to check timestamp

  Document(const std::string &input_json, const std::string &file_path);
  Document(const std::string &id, const std::string &input_json,
           const std::string &file_path);
  bool contains_key(const std::string &key);
  void updateAccessTime(); // Sets access time to current system time
  std::chrono::system_clock::time_point getAccessTime();
  ~Document() = default;
  bool is_deleted();
};
#endif // DOCUMENT_H
