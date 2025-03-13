#ifndef JSONFILEITERATOR_H
#define JSONFILEITERATOR_H
#include <boost/algorithm/string/trim.hpp>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class JsonFileIterator {
private:
  const std::string fileName;
  std::string buffer;
  size_t position;
  char currentChar;
  std::string key;
  std::string value;
  bool end;
  void parseCurrentLine();
  void remove_whitespace(std::string &str);
  void skipWhitespace();
  std::string parseString();
  std::string parseBool();
  std::string parseNumber();
  std::string parseNull();
  std::string parseValue();
  void advance();

public:
  explicit JsonFileIterator(const std::string &FilePath = "");
  ~JsonFileIterator() = default;
  bool operator==(const JsonFileIterator &other) const;
  bool operator!=(const JsonFileIterator &other) const;
  JsonFileIterator &operator++();
  std::pair<std::string, std::string> operator*() const;
  bool isEnd() const;
};

#endif
