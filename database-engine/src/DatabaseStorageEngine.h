#ifndef DatabaseStorageEngine_H
#define DatabaseStorageEngine_H

#include "Database.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

using json = nlohmann::json;
namespace fs = std::filesystem;

class DatabaseStorageEngine : public Database {

  // Stores the results of a key-value look up
  std::unordered_map<std::string, std::string> results;
  // Stores the documents
  std::unordered_map<std::string, std::shared_ptr<Document>> documents;
  u_int maximumMapSize = 10;
  int operationCounter;
  std::filesystem::path
      currentCollection; // The current collection that queries affect

public:
  DatabaseStorageEngine(std::string userId);
  ~DatabaseStorageEngine();
  void flushToDisk();
  bool setCollection(std::string collectionName) override;
  void setDocument(const Document &document) override;
  std::vector<std::shared_ptr<Document>>
  getDocument(GetStatement stmt) override;
  std::vector<std::shared_ptr<Document>> updateDocument(const UpdateStatement stmt)
      override; // Added update if we decide to use it
  std::vector<std::shared_ptr<Document>> deleteDocument(const DeleteStatement stmt) override;
  void writeToMainFile(
      const std::string& objId,
      json jsonObj); // Function that writes <key : jsonObj> to main.json
  // Templatized write to key file function
  template <typename T>
  void writeToKeyFile(int objId, T value, std::string key);
  void createCollection(std::string collectionName);
  void writeToWal(std::string message, bool writeCheckpoint);
  void setMaximumMapSize(u_int newSize);

private:
  std::streampos
  findClosingCharacter(std::string fileName,
                       char c); // Helper function to open file at end to not
                                // have to loop through it
  void loadJsonIntoMap(const std::string &fileName);
  void loadDocumentIntoMap(const std::string &id, const std::string &fileName);
  std::string findStringNoBrackets(const std::string originalString);
  std::vector<std::shared_ptr<Document>>
  loadIdListInMemory(const std::unordered_set<std::string> &idToFind);
  void removeLRU(int nrElements);
  std::vector<std::shared_ptr<Document>> getObjByIdAndUpdate(std::unordered_set<std::string> &idList,const std::vector<std::string>& keys, const std::string& value);
  void appendUpdatesToFile(const std::vector<std::shared_ptr<Document>> &buffer);
  void delete_nested_key(json& obj, const std::vector<std::string>& keys);
  void update_nested_key(json& obj, const std::vector<std::string>& keys, const std::string& value);
  std::vector<std::shared_ptr<Document>> getObjByIdAndDelete(std::unordered_set<std::string> &idList,const std::vector<std::string> &keys);
};

// Function to format different types correctly
template <typename T> std::string formatValue(T value) {
  return std::to_string(value);
}

// Explicit specialization for std::string (quotes needed)
template <> std::string formatValue<std::string>(std::string value);

template <> std::string formatValue<const char *>(const char *value);

// Specialization for bool
template <> std::string formatValue<bool>(bool value);

template <typename T>
void DatabaseStorageEngine::writeToKeyFile(int objId, T value,
                                           std::string key) {
  if (this->currentCollection.empty()) {
    return;
  }

  std::streampos lastBracePosition = findClosingCharacter(key, '}');
  if (lastBracePosition == -1) {
    throw std::runtime_error("JSON file had no }");
  }

  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  fs::path filename = this->currentCollection / (key + ".json");
  std::fstream file(filename, std::ios::in | std::ios::out);

  if (!file) {
    std::cerr << "Error opening " << filename << " for writing\n";
    return;
  }

  bool isEmpty = (lastBracePosition == 1);
  file.seekp(lastBracePosition);

  if (!isEmpty) {
    file.seekp(-1, std::ios::cur);
    char prevChar;
    file.get(prevChar);

    if (prevChar != '{') {
      file.seekp(-1, std::ios::cur);
      file << ",\n";
    }
  } else {
    file.seekp(0, std::ios::beg);
    file << "{\n";
  }

  file << "\t\"" << objId << "\" : " << formatValue(value);
  file << "\n}";
  file.close();
}

#endif // DATABASESTORAGEENGINE_H
