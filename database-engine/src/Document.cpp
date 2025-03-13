#include "Document.h"

// Constructor for existing documents if they don't have an id it throws
Document::Document(const std::string &input_json,
                   const std::string &file_path) {
  try {
    this->json_value = json::parse(input_json);
  } catch (const std::exception &e) {
    throw std::runtime_error("Parsing Failed: Error to parse JSON");
  }

  this->file_path = file_path;
  if (json_value.contains("id")) {
    this->id = json_value["id"].get<std::string>();
  } else {
    throw std::runtime_error("Document Creation Failed: No id key provided");
  }
  this->deleted = is_deleted();
  json_value.flatten();
}

// Constructor for new documents that may not have a set Id value
Document::Document(const std::string &id, const std::string &input_json,
                   const std::string &file_path) {
  try {
    this->json_value = json::parse(input_json).flatten();
  } catch (const std::exception &e) {
    throw std::runtime_error("Parsing Failed: Error to parse JSON");
  }
  this->file_path = file_path;
  if (json_value.contains("id")) {
    this->id = json_value["id"].get<std::string>();
  } else {
    json_value["id"] = id;
    this->id = id;
  }
  this->deleted = is_deleted();
}

// Helper to check if the document is logically deleted
bool Document::is_deleted() {
  if (json_value.contains("id") && json_value["id"].is_string() &&
      json_value["id"].get<std::string>().back() == 'd') {
    return true;
  }
  return false;
}

// Helper to check the last access time of the document, may throw an exception
void Document::check_access() {
  // There should be a try-catch but I would just re-throw so redundent but I
  // can add if necessary
  if (std::filesystem::exists(file_path)) {
    const auto access_time = std::filesystem::last_write_time(file_path);
    this->last_access_time = std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            access_time.time_since_epoch())};
  } else {
    last_access_time = std::chrono::system_clock::now();
  }
}

// Helper to check if it contains a given key
bool Document::contains_key(const std::string &key) {
  return json_value.contains(key);
}

void Document::updateAccessTime()
{
  last_access_time = std::chrono::system_clock::now();
}

std::chrono::system_clock::time_point Document::getAccessTime()
{
    return last_access_time;
}
