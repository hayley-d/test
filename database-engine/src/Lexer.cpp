#include "Lexer.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Use this for more efficient comparisons
// Some operators like && and || are not "officially" supported and will cuase
// an exception but they might be added later so I throught I would implement
// just in case
const static std::unordered_set<std::string> operators = {
    "==", "<", ">", "!=", "<=", ">=", "&&", "=", "||"};

// Use this for more efficient comparisons
const static std::unordered_set<std::string> commands = {
    "GET", "SET", "UPDATE", "DELETE", "LIMIT", "ASC", "DESC","NOT"};

/**
 * Tokenizes the input string into a vector of tokens.
 *
 * This function processes the input string by breaking it down into tokens
 * based on defined query grammar rules. Each token is classified into types
 * to simplfy parsing. The resulting vector of tokens is used in the parsing
 * step.
 *
 */
std::vector<Lexer::Token> Lexer::tokenize(const std::string &input) {

  // Trim whitespace before/after and in between
  std::string trimmedInput =
      std::regex_replace(input, std::regex("^ +| +$|( ) +"), "$1");

  if (input.empty()) {
    throw std::runtime_error("Tokenization failed: Input string is empty");
  }
  std::vector<Token> tokens;
  std::istringstream stream(trimmedInput);
  std::string temp;

  bool json_flag = false;
  bool value_flag = false;
  bool regex_flag = false;
  std::string buffer = "";

  while (stream >> temp) {
    if (json_flag &&
        (temp.length() < 2 || temp.substr(temp.length() - 2, 2) != ("}'"))) {
      buffer += temp + " ";
      continue;
    } else if (json_flag) {
      json_flag = false;
      buffer += temp;
      tokens.emplace_back(Token(TokenType::JSON, buffer));
      buffer = "";
      continue;
    }

    if (value_flag &&
        (temp.length() < 1 || temp.substr(temp.length() - 1, 1) != ("'"))) {
      buffer += temp + " ";
      continue;
    } else if (value_flag) {
      value_flag = false;
      buffer += temp;
      tokens.emplace_back(Token(TokenType::VALUE, buffer));
      buffer = "";
      continue;
    }

    if (regex_flag &&
        (temp.length() < 1 || temp.substr(temp.length() - 1, 1) != ("/"))) {
      buffer += temp + " ";
      continue;
    } else if (regex_flag) {
      regex_flag = false;
      buffer += temp;
      tokens.emplace_back(Token(TokenType::REGEX, buffer));
      buffer = "";
      continue;
    }

    if (commands.count(temp)) {
      tokens.emplace_back(Token(TokenType::COMMAND, temp));
    } else if (operators.count(temp)) {
      tokens.emplace_back(Token(TokenType::OPERATOR, temp));
    } else if (temp.substr(0, 1) == "/") {
      if (temp.length() > 1 && temp.substr(temp.length() - 1, 1) == "/") {
        tokens.emplace_back(Token(TokenType::REGEX, temp));
        continue;
      }
      regex_flag = true;
      buffer = "";
      buffer += temp + " ";
    } else if (temp.substr(0, 2) == "'{") {
      if (temp.length() > 1 && temp.substr(temp.length() - 1, 1) == "}'") {
        tokens.emplace_back(Token(TokenType::JSON, temp));
        continue;
      }

      json_flag = true;
      buffer = "";
      buffer += temp + " ";
    } else if (temp.substr(0, 1) == "'") {
      if (temp.length() > 1 && temp.substr(temp.length() - 1, 1) == "'") {
        tokens.emplace_back(Token(TokenType::VALUE, temp));
        continue;
      }
      value_flag = true;
      buffer = "";
      buffer += temp + " ";
      continue;
    } else {
      if (temp == "*") {
        tokens.emplace_back(Token(TokenType::KEY, temp));
        continue;
      }
      if (isInt(temp)) {
        tokens.emplace_back(Token(TokenType::NUMBER, temp));
      } else {
        std::vector<std::string> res = splitByDot(temp);
        if (res.size() > 1) {
          for (size_t i = 0; i < res.size(); i++) {
            if (isAlphanumeric(res[i])) {
              tokens.emplace_back(Token(TokenType::KEY, res[i]));
            } else {
              throw std::runtime_error(
                  "Tokenization failed: Unexpected character in '" + res[i] +
                  "'");
            }
          }
        } else if (res.size() == 1) {
          if (tokens.size() > 1 && tokens[0].value == "UPDATE") {
            if (tokens.back().type == TokenType::COMMAND) {
              tokens.emplace_back(Token(TokenType::COLLECTION, res[0]));
            } else {
              tokens.emplace_back(Token(TokenType::KEY, res[0]));
            }
          } else if (tokens.size() > 1 &&
                     tokens[1].type == TokenType::COLLECTION) {
            if (isAlphanumeric(res[0])) {
              tokens.emplace_back(Token(TokenType::KEY, res[0]));
            } else {
              throw std::runtime_error(
                  "Tokenization failed: Unexpected character in '" + res[0] +
                  "'");
            }
          } else if (!tokens.empty() &&
                     tokens.back().type == TokenType::COMMAND) {
            if (isAlphanumeric(res[0])) {
              tokens.emplace_back(Token(TokenType::COLLECTION, res[0]));
            } else {
              throw std::runtime_error(
                  "Tokenization failed: Unexpected character in '" + res[0] +
                  "'");
            }
          } else if (tokens.size() > 1 && tokens[0].value == "DELETE") {
            if (contains(tokens, TokenType::COLLECTION)) {
              if (isAlphanumeric(res[0])) {
                tokens.emplace_back(Token(TokenType::KEY, res[0]));
              } else {
                throw std::runtime_error(
                    "Tokenization failed: Unexpected character in '" + res[0] +
                    "'");
              }
            } else {
              if (isAlphanumeric(res[0])) {
                tokens.emplace_back(Token(TokenType::COLLECTION, res[0]));
              } else {
                throw std::runtime_error(
                    "Tokenization failed: Unexpected character in '" + res[0] +
                    "'");
              }
            }
          } else {
            throw std::runtime_error(
                "Tokenization failed: Incorrect query format");
          }
        } else {
          throw std::runtime_error(
              "Tokenization failed: Unrecognized character");
        }
      }
    }
  }

  if (json_flag) {
    throw std::runtime_error("Tokenization failed: Unterminated json (missing "
                             "'} to indicate end of json)");
  } else if (regex_flag) {
    throw std::runtime_error("Tokenization failed: Unterminated regex (missing "
                             "/ to indicate end of regex)");
  } else if (value_flag) {
    throw std::runtime_error("Tokenization failed: Unterminated value (missing "
                             "' to indicate end of value)");
  }

  tokens.push_back(Token(TokenType::END, ""));
  return tokens;
}

// Function to check if the tokens contain a specfic token type
bool Lexer::contains(const std::vector<Lexer::Token> tokens,
                     Lexer::TokenType type) {
  for (const auto &token : tokens) {
    if (token.type == type) {
      return true;
    }
  }
  return false;
}

// Function to split keys with . delimiter
std::vector<std::string> Lexer::splitByDot(const std::string &str) {
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, '.')) {
    result.push_back(token);
  }

  return result;
}

// Checks if a literal is an int
bool Lexer::isInt(const std::string &input) {
  bool negative = false;
  if (input.empty()) {
    return false;
  }

  size_t i = 0;

  if (input[i] == '-') {
    negative = true;
    i++;
  }

  for (; i < input.length(); i++) {
    if (!isdigit(input[i])) {
      if (negative) {
        throw std::runtime_error("Tokenization failed: Invalid symbol '-'");
      }
      return false;
    }
  }

  if (negative) {
    throw std::runtime_error("Tokenization failed: Query language does not "
                             "support negative integers");
  }

  if (input.length() > 1 && input[0] == '0') {
    throw std::runtime_error(
        "Tokenization failed: Invalid integer with leading zero");
  }

  return true;
}

// Checks if a literal is alphanumeric or is also allowed to contain `_`
bool Lexer::isAlphanumeric(const std::string &str) {
  return std::all_of(str.begin(), str.end(), [](unsigned char c) {
    return std::isalnum(c) || c == '_';
  });
}
