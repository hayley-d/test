#include "JsonFileIterator.h"
#include <sstream>

// Constructor will initialize the file reader and set the current char to the
// first char in the file
JsonFileIterator::JsonFileIterator(const std::string &fileName)
    : fileName(fileName), position(0), end(false) {

  if (fileName.empty()) {
    end = true;
    return;
  }

  if (!std::filesystem::exists(fileName)) {
    throw std::runtime_error("File not found: " + fileName);
  }

  std::ifstream file(fileName, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file " + fileName);
  }

  if (!file.good()) {
    throw std::runtime_error("File error!");
  }

  std::ostringstream ss;
  ss << file.rdbuf();
  buffer = ss.str();

  if (buffer.empty()) {
    end = true;
    return;
  }

  /*std::cout << "Buffer Raw Data (in integer):\n";
  for (size_t i = 0; i < buffer.size(); ++i) {
    std::cout << static_cast<int>(buffer[i]) << " ";
  }
  std::cout << std::endl;

  std::cout << "File Contents:\n" << buffer << std::endl;*/

  currentChar = buffer[position];

  while (key.empty() && !end) {
    parseCurrentLine();
  }

  //std::cout << "MADE IT TO THE END OF THE CONSTRUCTOR\n";
}

bool JsonFileIterator::isEnd() const { return end; }

bool JsonFileIterator::operator==(const JsonFileIterator &other) const {
  return currentChar == other.currentChar && end == other.end &&
         fileName == other.fileName;
}

bool JsonFileIterator::operator!=(const JsonFileIterator &other) const {
  return !(*this == other);
}

void JsonFileIterator::skipWhitespace() {
  while (isspace(currentChar)) {
    if (!end) {
      this->advance();
    } else {
      return;
    }
  }
}

void JsonFileIterator::advance() {
  if (end)
    return;
  if (++position >= buffer.size()) {
    end = true;
    std::cout << "End of buffer reached.\n";
    return;
  }
  currentChar = buffer[position];
  std::cout << "CURRENT CHAR : " << currentChar << " (ASCII: " << static_cast<int>(currentChar) << ")\n";
}

JsonFileIterator &JsonFileIterator::operator++() {
  if (!end) {
    advance();
  }
  return *this;
}

std::pair<std::string, std::string> JsonFileIterator::operator*() const {
  return {key, value};
}

// Helper: Parse the key and value from the current line
void JsonFileIterator::parseCurrentLine() {
  if (end)
    return;

  this->skipWhitespace();

  // If the closing brace is encountered then its the end of the file
  if (currentChar == '}' || end) {
    end = true;
    return;
  }

  if (currentChar == '{') {
    if (!end) {
      advance();
    } else {
      return;
    }
  }

  if (currentChar == '"') {
    key = parseString();
    this->skipWhitespace();

    if (currentChar != ':') {
      throw std::runtime_error("Expected colon after key: " + key);
    }

    if (!end) {
      this->advance();
    } else {
      return;
    }

    this->skipWhitespace();
    value = parseValue();
    this->skipWhitespace();

    if (currentChar == ',') {
      if (!end) {
        this->advance();
      } else {
        return;
      }
    } else {
        end = true;
    }

    this->skipWhitespace();
  }
}

// Parses a string enclosed in double quotes
std::string JsonFileIterator::parseString() {
  std::string temp;
  advance();

  while (currentChar != '"') {
    temp += currentChar;
    if (!end) {
      this->advance();
    } else {
      return temp;
    }
  }

  // throw away the end "
  advance();
  return temp;
}

// Parses a bool
std::string JsonFileIterator::parseBool() {
  std::string temp;

  while (isalpha(currentChar) && currentChar != EOF) {
    temp += currentChar;
    if (!end) {
      this->advance();
    } else {
      return temp;
    }
  }

  if (temp == "true" || temp == "false") {
    return temp;
  } else {
    throw std::runtime_error("Invalid boolean value");
  }
}

// Parses a number
std::string JsonFileIterator::parseNumber() {
  std::string temp;
  while (isdigit(currentChar) || currentChar == '.' || currentChar == '-') {
    temp += currentChar;
    if (!end) {
      this->advance();
    } else {
      return temp;
    }
  }
  return temp;
}

// Parses null values
std::string JsonFileIterator::parseNull() {
  std::string temp;
  while (isalpha(currentChar)) {
    temp += currentChar;
    advance();
  }

  if (temp == "null") {
    return temp;
  } else {
    throw std::runtime_error("Invalid null value");
  }
}

std::string JsonFileIterator::parseValue() {
  static const std::unordered_map<char, std::function<std::string()>> parsers =
      {{'"', [this]() { return parseString(); }},
       {'t', [this]() { return parseBool(); }},
       {'f', [this]() { return parseBool(); }},
       {'n', [this]() { return parseNull(); }},
       {'-', [this]() { return parseNumber(); }}};

  auto it = parsers.find(currentChar);
  if (it != parsers.end()) {
    return it->second();
  } else if (isdigit(currentChar)) {
    return parseNumber();
  } else {
    throw std::runtime_error("Unexpected character when parsing value.");
  }
}
