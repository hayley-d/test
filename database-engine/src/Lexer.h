#ifndef LEXER_H
#define LEXER_H
#include <memory>
#include <string>
#include <vector>

class Lexer {
public:
  enum TokenType {
    COMMAND,
    COLLECTION,
    KEY,
    OPERATOR,
    VALUE,
    NUMBER,
    JSON,
    REGEX,
    END 
  };

  struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, const std::string &value)
        : type(type), value(value) {}
  };

  std::vector<Token> tokenize(const std::string &input);

private:
  std::vector<std::string> splitByDot(const std::string &str);
  bool isInt(const std::string &input);
  bool contains(const std::vector<Token> tokens, TokenType type);
  bool isAlphanumeric(const std::string &str);
};

#endif // LEXER_H
