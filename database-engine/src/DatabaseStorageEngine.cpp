#include "DatabaseStorageEngine.h"
#include "JsonFileIterator.h"
#include "simdjson.h"
#include <algorithm>
#include <ctime>

using namespace simdjson;

namespace fs = std::filesystem;

// Instantiates the engine. After this function is called assume the creation of
// a folder with userId name, file for keeping track of operation counter and
// WAL
DatabaseStorageEngine::DatabaseStorageEngine(std::string userId)
    : Database(userId) {
  // Construct directory and file paths
  fs::path dirPath = "storage/" + userId;
  fs::path filePath = dirPath / "operation_counter.txt"; // Append filename
  fs::path walPath = dirPath / "wal.txt";

  // Ensure directory exists
  if (!fs::exists(dirPath)) {
    fs::create_directories(dirPath); // Create full path if necessary

    // Create operation counter file and write 0
    std::ofstream operationCounterFile(filePath);
    if (!operationCounterFile) {
      throw std::runtime_error("Failed to create operation counter file.");
    }
    operationCounterFile << 0 << std::endl;
    operationCounterFile.close();
    operationCounter = 0;

    // Initialize WAL file
    std::ofstream walFile(walPath);
    if (!walFile) {
      throw std::runtime_error("Failed to create WAL file.");
    }
    walFile.close();

  } else {
    // Read operation counter from the existing file
    std::ifstream operationCounterFile(filePath);
    if (!operationCounterFile) {
      throw std::runtime_error(
          "Critical error: Could not open operation counter file.");
    }
    std::string line;
    std::getline(operationCounterFile, line);
    operationCounterFile.close();
    operationCounter = line.empty() ? 0 : std::stoi(line);
  }
}
// Added implementation to clear the document and result map
DatabaseStorageEngine::~DatabaseStorageEngine() {
  /* Not required since shared_ptr
  for (auto &pair : documents) {
    delete pair.second;
  }*/
  documents.clear();
  results.clear();
  fs::path dirPath = "storage/" + userId;
  fs::path filePath = dirPath / "operation_counter.txt";
  std::ofstream operationCounterFile(filePath);
  /*  if (!operationCounterFile) {
      throw std::runtime_error(
          "Failed to create operation counter file or open it.");
    }* Causes a segfault*/
  operationCounterFile << this->operationCounter << std::endl;
  operationCounterFile.close();
}

void DatabaseStorageEngine::flushToDisk() {}

bool DatabaseStorageEngine::setCollection(std::string collectionName) {
  (std::transform(collectionName.begin(), collectionName.end(),
                  collectionName.begin(), ::toupper));
  fs::path collectionPath = fs::path("storage/" + userId) / collectionName;

  if (fs::exists(collectionPath)) {
    this->currentCollection = collectionPath;
    return true;
  } else {
    return false;
  }
}

// Added the [[maybe_unused]] for now for my sanity
void DatabaseStorageEngine::setDocument(
    [[maybe_unused]] const Document &document) {}

// A map of operators to their comparison functions
static const std::unordered_map<
    std::string, std::function<bool(const std::string &, const std::string &)>>
    operators{
        {"==",
         [](const std::string &a, const std::string &b) { return a == b; }},
        {"!=",
         [](const std::string &a, const std::string &b) { return a != b; }},
        {">=",
         [](const std::string &a, const std::string &b) { return a >= b; }},
        {"<=",
         [](const std::string &a, const std::string &b) { return a <= b; }},
        {">", [](const std::string &a, const std::string &b) { return a > b; }},
        {"<",
         [](const std::string &a, const std::string &b) { return a < b; }}};

// Parses query, searches <key>.json and loads results into map
std::vector<std::shared_ptr<Document>>
DatabaseStorageEngine::getDocument(GetStatement stmt) {
  // Set collection, if not exist return empty vector
  if (!this->setCollection(
          this->findStringNoBrackets(stmt.collection->toString()))) {
    return std::vector<std::shared_ptr<Document>>();
  }

  // Set to store all ids matching query
  std::unordered_set<std::string> matching_ids;

  // Check type of expression for key <operation> value
  std::visit(
      [this, &matching_ids](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<BinaryExpression>>) {
          // Binary expression ==> left op right where left is key and right is
          // val
          std::string key_file = (*arg).left->join();

          fs::path key_file_path =
              fs::path(this->currentCollection / fs::path(key_file + ".json"));

          // Iterate through key_file and store in map
          /*JsonFileIterator it(this->currentCollection /
                              fs::path(key_file + ".json"));*/
          std::ifstream main(key_file_path);

          json file = json::parse(main);
          main.close();

          std::vector<std::pair<std::string, std::string>> pairs;

          for (auto &el : file.items()) {
            pairs.push_back({el.key(), el.value()});
          }

          for (/*!it.isEnd()*/ const auto &p : pairs) {
            // auto [key, val] = *it;
            auto key = p.first;
            auto val = p.second;
            // auto [key, val] = [p.first,p.second];
            // std::cout << "KEY: " << key << " VALUE: " << val << "\n";
            auto op_it = operators.find((*arg).op);
            if (op_it != operators.end()) {
              if (op_it->second(val, (*arg).right->value)) {
                matching_ids.insert(key);
                // std::cout << "MATCHING ID: " << key << "\n";
              }
            }

            //++it;
          }

        } else if constexpr (std::is_same_v<
                                 T, std::shared_ptr<LogicalExpression>>) {

          // Sprint 2: Refactor this method to do binary expression recursively
          // and update set of ids

        } else if constexpr (std::is_same_v<T,
                                            std::shared_ptr<NotExpression>>) {
          // Sprint 2: Refactor this method to get set of ids for Binary
          // expression and then remove those elements from set of all ids
        }
      },
      stmt.condition);

  // Use acquired list of query matching ids to find all Documnets to return
  // Check map to see if Document is in cache: If true use cache else remove
  // from matching_ids std::vector<Document*> returnDocuments;
  std::vector<std::shared_ptr<Document>> returnDocuments;
  std::unordered_set<std::string> to_remove;
  auto idIt = matching_ids.begin();
  while (idIt != matching_ids.end()) {
    auto objIt = this->documents.find(*idIt);
    if (objIt != documents.end()) {
      // Cache hit ==> add to output vector and remove from matching_ids
      returnDocuments.push_back(objIt->second);
      objIt->second->updateAccessTime();
      idIt = matching_ids.erase(idIt);
    } else {
      // No hit ==> keep id in matching_ids
      ++idIt;
    }
  }

  // Use matching ids that still remain (i.e. was not found in cache) and search
  // main.json for them
  // std::vector<Document*> newObjsLoadedFromFiles =
  // this->loadIdListInMemory(matching_ids);
  std::vector<std::shared_ptr<Document>> newObjsLoadedFromFiles =
      this->loadIdListInMemory(matching_ids);
  // std::cout << "LOADED " << newObjsLoadedFromFiles.size() << "\n";

  // Append these found objects to results
  returnDocuments.insert(returnDocuments.end(), newObjsLoadedFromFiles.begin(),
                         newObjsLoadedFromFiles.end());

  // Admin
  if (returnDocuments.size() != 0) {
    operationCounter++;
    writeToWal(stmt.toString(), false);
  }

  return returnDocuments;
}

std::vector<std::shared_ptr<Document>>
DatabaseStorageEngine::updateDocument(const UpdateStatement stmt) {
  // Set collection, if not exist return empty vector
  if (!this->setCollection(
          this->findStringNoBrackets(stmt.collection->toString()))) {
    return std::vector<std::shared_ptr<Document>>();
  }

  // Set to store all ids matching query
  std::unordered_set<std::string> matching_ids;

  // Check type of expression for key <operation> value
  std::visit(
      [this, &matching_ids](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<BinaryExpression>>) {
          std::string key_file = (*arg).left->join();
          fs::path key_file_path =
              fs::path(this->currentCollection / fs::path(key_file + ".json"));

          // Iterate through the key-value file and extract the key-value pairs
          std::ifstream main(key_file_path);
          json file = json::parse(main);
          main.close();

          std::vector<std::pair<std::string, std::string>> pairs;
          for (auto &el : file.items()) {
            if (el.value().is_string()) {
              pairs.push_back({el.key(), el.value()});
            } else {
              pairs.push_back(
                  {el.key(), std::to_string(el.value().get<int>())});
            }
          }
          for (const auto &p : pairs) {
            auto key = p.first;
            auto val = p.second;
            // std::cout << "KEY : " << key << " VALUE: " << val << "\n";
            auto op_it = operators.find((*arg).op);
            if (op_it != operators.end()) {
              if (op_it->second(val, (*arg).right->value)) {
                matching_ids.insert(key);
              }
            }
          }

        } else if constexpr (std::is_same_v<
                                 T, std::shared_ptr<LogicalExpression>>) {

          // Sprint 2: Refactor this method to do binary expression recursively
          // and update set of ids

        } else if constexpr (std::is_same_v<T,
                                            std::shared_ptr<NotExpression>>) {
          // Sprint 2: Refactor this method to get set of ids for Binary
          // expression and then remove those elements from set of all ids
        }
      },
      stmt.statement->condition);

  // Use acquired list of query matching ids to find all Documnets to return
  // Check map to see if Document is in cache: If true use cache else remove
  // from matching_ids std::vector<Document*> returnDocuments;
  std::vector<std::shared_ptr<Document>> returnDocuments;
  // Iterate through the matching IDs and check if the document is in memory, if
  // so remove from matching_ids vec
  auto idIt = matching_ids.begin();
  while (idIt != matching_ids.end()) {
    auto objIt = this->documents.find(*idIt);
    if (objIt != documents.end()) {
      update_nested_key(objIt->second->json_value["data"], stmt.updateKey->keys,
                        stmt.updateValue->value);
      returnDocuments.push_back(objIt->second);
      objIt->second->updateAccessTime();
      idIt = matching_ids.erase(idIt);
      // Delete from cache if deleted
      if (objIt->second->is_deleted()) {
        std::string idStr = *idIt;
        idStr = idStr.substr(0, idStr.length() - 2);
        documents.erase(idStr);
      }
    } else {
      ++idIt;
    }
  }
  //

  // Use matching ids that still remain (i.e. was not found in cache) and search
  // main.json for them
  std::vector<std::shared_ptr<Document>> newObjsLoadedFromFiles =
      this->getObjByIdAndUpdate(matching_ids, stmt.updateKey->keys,
                                stmt.updateValue->value);

  // Append these found objects to results
  returnDocuments.insert(returnDocuments.end(), newObjsLoadedFromFiles.begin(),
                         newObjsLoadedFromFiles.end());

  // Append the updates to the main (I can make this a new function but it's 3
  // lines....
  for (const auto &doc : returnDocuments) {
    writeToMainFile(doc->id, doc->json_value["data"]);
  }

  // Admin
  if (returnDocuments.size() != 0) {
    operationCounter++;
    writeToWal(stmt.toString(), false);
  }

  return returnDocuments;
}

std::vector<std::shared_ptr<Document>>
DatabaseStorageEngine::deleteDocument(const DeleteStatement stmt) {
  // Set collection, if not exist return empty vector
  if (!this->setCollection(
          this->findStringNoBrackets(stmt.statement->collection->toString()))) {
    return std::vector<std::shared_ptr<Document>>();
  }

  // Set to store all ids matching query
  std::unordered_set<std::string> matching_ids;

  // Check type of expression for key <operation> value
  std::visit(
      [this, &matching_ids](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<BinaryExpression>>) {
          std::string key_file = (*arg).left->join();
          fs::path key_file_path =
              fs::path(this->currentCollection / fs::path(key_file + ".json"));

          // Iterate through the key-value file and extract the key-value pairs
          std::ifstream main(key_file_path);

          json file = json::parse(main);
          main.close();

          std::vector<std::pair<std::string, std::string>> pairs;

          for (auto &el : file.items()) {
            pairs.push_back({el.key(), el.value()});
          }

          for (const auto &p : pairs) {
            auto key = p.first;
            auto val = p.second;
            auto op_it = operators.find((*arg).op);
            if (op_it != operators.end()) {
              if (op_it->second(val, (*arg).right->value)) {
                matching_ids.insert(key);
              }
            }
          }

        } else if constexpr (std::is_same_v<
                                 T, std::shared_ptr<LogicalExpression>>) {

          // Sprint 2: Refactor this method to do binary expression recursively
          // and update set of ids

        } else if constexpr (std::is_same_v<T,
                                            std::shared_ptr<NotExpression>>) {
          // Sprint 2: Refactor this method to get set of ids for Binary
          // expression and then remove those elements from set of all ids
        }
      },
      stmt.statement->condition);

  // Use acquired list of query matching ids to find all Documnets to return
  // Check map to see if Document is in cache: If true use cache else remove
  // from matching_ids std::vector<Document*> returnDocuments;
  std::vector<std::shared_ptr<Document>> returnDocuments;

  std::unordered_set<std::string> to_remove;
  for (const std::string &id : matching_ids) {
    if (!id.empty() && id.back() == 'd') {
      to_remove.insert(id);
      to_remove.insert(id.substr(0, id.size() - 2));
    }
  }

  for (const auto &id : to_remove) {
    matching_ids.erase(id);
  }

  // Iterate through the matching IDs and check if the document is in memory, if
  // so remove from matching_ids vec
  auto idIt = matching_ids.begin();
  while (idIt != matching_ids.end()) {
    auto objIt = this->documents.find(*idIt);
    if (objIt != documents.end() && !objIt->second->is_deleted()) {
      if (stmt.deleteKey->join() == "*") {
        objIt->second->json_value["id"] =
            objIt->second->json_value["id"].get<std::string>() + "d";
      } else {
        delete_nested_key(objIt->second->json_value["data"],
                          stmt.deleteKey->keys);
      }
      returnDocuments.push_back(objIt->second);
      objIt->second->updateAccessTime();
      idIt = matching_ids.erase(idIt);
      // Delete from cache if deleted
      if (objIt->second->is_deleted()) {
        std::string idStr = *idIt;
        idStr = idStr.substr(0, idStr.length() - 2);
        documents.erase(idStr);
      }
    } else {
      ++idIt;
    }
  }

  // Use matching ids that still remain (i.e. was not found in cache) and search
  // main.json for them
  std::vector<std::shared_ptr<Document>> newObjsLoadedFromFiles =
      this->getObjByIdAndDelete(matching_ids, stmt.deleteKey->keys);

  // Append these found objects to results
  returnDocuments.insert(returnDocuments.end(), newObjsLoadedFromFiles.begin(),
                         newObjsLoadedFromFiles.end());

  // Append the updates to the main (I can make this a new function but it's 3
  // lines....
  for (const auto &doc : returnDocuments) {
    writeToMainFile(doc->id, doc->json_value["data"]);
  }

  // Admin
  if (returnDocuments.size() != 0) {
    operationCounter++;
    writeToWal(stmt.toString(), false);
  }

  return returnDocuments;
}

// Function to delete a key based on a dot-separated path
void DatabaseStorageEngine::delete_nested_key(
    json &obj, const std::vector<std::string> &keys) {
  std::string token;

  if (keys.empty())
    return;

  json *current = &obj;

  // Traverse to the second-last key
  for (size_t i = 0; i < keys.size() - 1; i++) {
    if (current->contains(keys[i]) && (*current)[keys[i]].is_object()) {
      current = &(*current)[keys[i]];
    } else {
      throw std::runtime_error("Failed to delete key");
    }
  }

  current->erase(keys.back());
}

// Function to update a key based on a dot-separated path
void DatabaseStorageEngine::update_nested_key(
    json &obj, const std::vector<std::string> &keys, const std::string &value) {
  std::string token;

  if (keys.empty())
    return;

  json *current = &obj;

  // Traverse to the second-last key
  for (size_t i = 0; i < keys.size() - 1; ++i) {
    if (current->contains(keys[i]) && (*current)[keys[i]].is_object()) {
      current = &(*current)[keys[i]];
    } else {
      throw std::runtime_error("Failed to delete key");
    }
  }

  // Try to convert value to a number
  if (!value.empty() && std::all_of(value.begin(), value.end(), [](char c) {
        return std::isdigit(c) || c == '.';
      })) {
    if (value.find('.') != std::string::npos) {
      (*current)[keys.back()] = std::stod(value);
    } else {
      (*current)[keys.back()] = std::stoll(value);
    }
  } else {
    (*current)[keys.back()] = value;
  }
}

// Create Collection collectionName if not exists and set current collection.
// Also initializes main.json
void DatabaseStorageEngine::createCollection(std::string collectionName) {
  std::transform(collectionName.begin(), collectionName.end(),
                 collectionName.begin(), ::toupper);
  fs::path collectionPath("storage/" + this->userId + "/" + collectionName);
  fs::create_directory(collectionPath);
  this->setCollection(collectionName);

  std::ofstream mainJson(this->currentCollection / fs::path("main.json"));
  if (!mainJson) {
    throw std::runtime_error(
        "Could not initialize main.json file for new collection");
  }
}

void DatabaseStorageEngine::writeToWal(std::string message,
                                       bool writeCheckpoint) {
  // Open file stream to wal
  fs::path walPath("storage/" + userId + "/wal.txt");
  std::fstream wal(walPath, std::ios::in | std::ios::app);

  if (!wal) {
    std::cerr << "Could not open WAL\n";
    return;
  }

  // Seek to end of file
  wal.seekg(0, std::ios::end);
  wal << message << "\n";

  // Check if checkpoint is required
  if (writeCheckpoint) {
    time_t curTime;
    time(&curTime);
    wal << "CHECKPOINT: " << curTime << "\n";
  }
}

// Sets the size of the map to newSize, if it currently contains more elements
// (n) than newSize removes n - newSize elements
void DatabaseStorageEngine::setMaximumMapSize(u_int newSize) {
  if (this->documents.size() > newSize) {
    this->removeLRU(this->documents.size() - newSize);
  }

  this->maximumMapSize = newSize;
}

std::streampos DatabaseStorageEngine::findClosingCharacter(std::string fileName,
                                                           char c) {
  // Check if no collection set and early return
  if (this->currentCollection.empty()) {
    return -1;
  }

  std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
  fileName += ".json";
  fs::path newFilePath = this->currentCollection / fileName;

  std::fstream newFile(newFilePath,
                       std::ios::in | std::ios::out | std::ios::app);

  if (!newFile) {
    std::cerr << "Could not create or open file\n";
    return -1;
  }

  newFile.seekg(0, std::ios::end); // Move to file end
  std::streampos fileSize = newFile.tellg();

  // Empty file => newly created file => write {} and return 1
  if (fileSize == 0) {
    newFile << "{}";
    newFile.flush();
    return 1;
  }

  // Move backwards to find last "x"
  for (std::streamoff offset = 1; offset <= fileSize; ++offset) {
    newFile.seekg(-offset, std::ios::end);
    char ch;
    newFile.get(ch);

    if (ch == c) {
      return newFile.tellg() - std::streampos(1);
    }
  }

  return std::streampos(-1); // Not found: Should not happen
}

// Loads the contents of a key-value file into the results map to be filtered
// through to find the matching values
void DatabaseStorageEngine::loadJsonIntoMap(const std::string &fileName) {
  JsonFileIterator it(fileName);
  while (!it.isEnd()) {
    auto [key, value] = *it;
    results.insert(std::make_pair(key, value));
    ++it;
  }
}

// Reads the JSON file and turns it into a Document object, adding it to the
// document map
void DatabaseStorageEngine::loadDocumentIntoMap(const std::string &id,
                                                const std::string &fileName) {
  if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".json") {
    std::ifstream file(fileName);
    if (!file) {
      throw std::runtime_error("Error: Could not open file");
    }
    std::string contents((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    documents.insert(std::make_pair(id, new Document(contents, fileName)));
  } else {
    throw std::runtime_error("Error: Not a JSON file");
  }
}

// Extract the string from inside a pair of brackets
std::string
DatabaseStorageEngine::findStringNoBrackets(const std::string originalString) {
  auto firstBracket = originalString.find('(');
  auto secondBracket = originalString.find(')');

  if (firstBracket != std::string::npos && secondBracket != std::string::npos &&
      secondBracket > firstBracket) {
    std::string newString = originalString.substr(
        firstBracket + 1, secondBracket - firstBracket - 1);
    return newString;
  }

  return originalString;
}

// NOTE: Add compaction in sprint two for main.json so that it does not get too
// large
std::vector<std::shared_ptr<Document>>
DatabaseStorageEngine::loadIdListInMemory(
    const std::unordered_set<std::string> &idToFind) {
  std::vector<std::shared_ptr<Document>> newObjectsLoadedIntoMemory;
  fs::path mainPath = this->currentCollection / fs::path("main.json");
  std::ifstream main(mainPath);

  json mainFile = json::parse(main);
  main.close();
  std::unordered_set<std::string> to_remove;

  for (const auto &obj : mainFile) {
    if (!obj.contains("id") || !obj.contains("data")) {
      throw std::runtime_error("JSON object is missing required fields");
    }

    std::string idStr = obj["id"].get<std::string>();
    if (!idStr.empty() && idStr.back() == 'd') {
      to_remove.insert(idStr);
      to_remove.insert(idStr.substr(0, idStr.size() - 1));
    }

    if (documents.count(idStr) > 0 && idToFind.count(idStr)) {
      newObjectsLoadedIntoMemory.push_back(documents[idStr]);
    } else if (idToFind.count(idStr) > 0) {
      auto returnDoc =
          std::make_shared<Document>(obj.dump(), mainPath.string());

      if (this->documents.size() != maximumMapSize) {
        // If choose the log based approch then duplicates get overrrden in the
        // map anyway eliminating a check for that
        // Dont add deleted documents
        if (!returnDoc->is_deleted() &&
            (to_remove.find(idStr) == to_remove.end())) {
          this->documents[idStr] = returnDoc;
        } else {
          // Check if older version of the deleted obj are in the map and remove
          // This should remove the d from the end of the id value
          idStr = idStr.substr(0, idStr.length() - 2);
          // Remove from the map if it exists
          if (documents.count(idStr) > 0) {
            documents.erase(idStr);
          }
        }
      } else {
        // Evict 40% of cache to free space
        this->removeLRU(int(this->documents.size() * 0.4));
      }

      newObjectsLoadedIntoMemory.push_back(returnDoc);
    }
  }

  newObjectsLoadedIntoMemory.erase(std::remove_if(newObjectsLoadedIntoMemory.begin(), newObjectsLoadedIntoMemory.end(),
                              [&](const std::shared_ptr<Document> &doc) {
                                return doc && to_remove.find(doc->id) !=
                                                  to_remove.end();
                              }),
               newObjectsLoadedIntoMemory.end());

  return newObjectsLoadedIntoMemory;
}

// Searches for documents by ID and stores them in a vector (without caching)
// for efficiency it will update while iterating so it only needs to be done
// once preferably in sprint two simdjson will be used so the whole main does
// not need to be brought into memory
std::vector<std::shared_ptr<Document>>
DatabaseStorageEngine::getObjByIdAndUpdate(
    std::unordered_set<std::string> &idList,
    const std::vector<std::string> &keys, const std::string &value) {
  std::vector<std::shared_ptr<Document>> buffer;
  fs::path mainPath = this->currentCollection / fs::path("main.json");
  std::ifstream main(mainPath);

  json mainFile = json::parse(main);
  main.close();
  std::unordered_set<std::string> to_remove;

  for (const auto &obj : mainFile) {
    if (!obj.contains("id") || !obj.contains("data")) {
      throw std::runtime_error("JSON object is missing required fields");
    }

    std::string idStr = obj["id"].get<std::string>();
    if (!idStr.empty() && idStr.back() == 'd') {
      to_remove.insert(idStr);
      to_remove.insert(idStr.substr(0, idStr.size() - 2));
    }
    if (idList.count(idStr) > 0) {
      auto returnDoc =
          std::make_shared<Document>(obj.dump(), mainPath.string());
      // Skip if already has a tombstone
      if (!returnDoc->is_deleted() &&
          (to_remove.find(idStr) == to_remove.end())) {
        update_nested_key(returnDoc->json_value["data"], keys, value);
        buffer.push_back(returnDoc);
      } else {
        // Remove from the buffer if it has been marked at a later entry
        idStr = idStr.substr(0, idStr.length() - 2);
        buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
                                    [&](const std::shared_ptr<Document> &doc) {
                                      return doc->id == idStr;
                                    }),
                     buffer.end());
      }
    }
  }

  buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
                              [&](const std::shared_ptr<Document> &doc) {
                                return doc && to_remove.find(doc->id) !=
                                                  to_remove.end();
                              }),
               buffer.end());

  return buffer;
}

std::vector<std::shared_ptr<Document>>
DatabaseStorageEngine::getObjByIdAndDelete(
    std::unordered_set<std::string> &idList,
    const std::vector<std::string> &keys) {
  std::vector<std::shared_ptr<Document>> buffer;
  fs::path mainPath = this->currentCollection / fs::path("main.json");
  std::ifstream main(mainPath);

  json mainFile = json::parse(main);
  main.close();
  std::unordered_set<std::string> to_remove;

  for (const auto &obj : mainFile) {
    if (!obj.contains("id") || !obj.contains("data")) {
      throw std::runtime_error("JSON object is missing required fields");
    }

    std::string idStr = obj["id"].get<std::string>();
    if (!idStr.empty() && idStr.back() == 'd') {
      to_remove.insert(idStr);
      to_remove.insert(idStr.substr(0, idStr.size() - 2));
    }

    if (idList.count(idStr) > 0) {
      auto returnDoc =
          std::make_shared<Document>(obj.dump(), mainPath.string());
      // Skip if already has a tombstone
      if (!returnDoc->is_deleted() &&
          (to_remove.find(idStr) == to_remove.end())) {
        if (keys.size() == 1 && keys[0] == "*") {
          returnDoc->json_value["id"] =
              returnDoc->json_value["id"].get<std::string>() + "d";
        } else {
          delete_nested_key(returnDoc->json_value["data"], keys);
        }
        buffer.push_back(returnDoc);
      } else {
        // Remove from the buffer if it has been marked at a later entry
        idStr = idStr.substr(0, idStr.length() - 2);
        buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
                                    [&](const std::shared_ptr<Document> &doc) {
                                      return doc->id == idStr;
                                    }),
                     buffer.end());
      }
    }
  }
  buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
                              [&](const std::shared_ptr<Document> &doc) {
                                return doc && to_remove.find(doc->id) !=
                                                  to_remove.end();
                              }),
               buffer.end());
  return buffer;
}

// Removes nrElements of objects from documents map. Removes at most
// documents.size() number of element
void DatabaseStorageEngine::removeLRU(int nrElements) {
  int i = 0;
  int startingSize = documents.size();
  while ((i < nrElements) && (i != startingSize)) {
    // This returns the smallest map element:
    // https://stackoverflow.com/questions/2659248/how-can-i-find-the-minimum-value-in-a-map
    auto it = std::min_element(
        documents.begin(), documents.end(), [](const auto &l, const auto &r) {
          return l.second->getAccessTime() < r.second->getAccessTime();
        });
    documents.erase(it);
    i++;
  }
}

// Writes a json object to the end of main.json
void DatabaseStorageEngine::writeToMainFile(const std::string &objId,
                                            json jsonObj) {
  if (this->currentCollection.empty()) {
    return;
  }

  fs::path mainFilePath = this->currentCollection / "main.json";

  // Find the last closing bracket ']'
  std::streampos lastBracketPosition = findClosingCharacter("main", ']');

  // Open or create the file in read/write mode
  std::fstream mainFile(mainFilePath, std::ios::in | std::ios::out);

  if (!mainFile) {
    throw std::runtime_error("Could not open or create main.json inside " +
                             std::string(currentCollection.c_str()) + "\n");
  }

  bool isEmpty = (lastBracketPosition == 1);
  mainFile.seekp(lastBracketPosition);

  if (!isEmpty) {
    mainFile.seekp(-1, std::ios::cur);
    char prevChar;
    mainFile.get(prevChar);

    if (prevChar != '[') {
      mainFile.seekp(-1, std::ios::cur);
      mainFile << ",\n";
    }
  } else {
    mainFile.seekp(0, std::ios::beg);
    mainFile << "[\n";
  }

  mainFile << "{\n\t\"id\": \"" << objId
           << "\",\n\t\"data\": " << jsonObj.dump() << "\n}";
  mainFile << "\n]";
  mainFile.close();
}

template <> std::string formatValue<std::string>(std::string value) {
  return "\"" + value + "\"";
}

template <> std::string formatValue(const char *value) {
  return "\"" + std::string(value) + "\"";
}

template <> std::string formatValue(bool value) {
  return value ? "true" : "false";
}
