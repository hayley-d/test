#include "QueryParser.h"
#include <cctype>
#include <memory>
#include <stdexcept>

// Returns the current token
Lexer::Token &QueryParser::Parser::currentToken() { return tokens[position]; }

// Returns the current token and increments the position
Lexer::Token &QueryParser::Parser::advance() { return tokens[position++]; }

// Checks if there are any tokens left
bool QueryParser::Parser::hasTokens() {
  return static_cast<size_t>(position) < tokens.size() &&
         currentToken().type != Lexer::TokenType::END;
}

// Parses delete statements (part of the pratt parser)
std::shared_ptr<DeleteStatement> QueryParser::Parser::parseDelete() {
  advance();
  if (currentToken().type == Lexer::TokenType::VALUE) {
    std::vector<std::string> keys;
    keys.push_back(advance().value);
    if (keys.empty()) {
      throw std::runtime_error("Parsing Failed: Missing value");
    }
    std::shared_ptr<NestedIdentifier> key =
        std::make_shared<NestedIdentifier>(keys);
    std::shared_ptr<GetStatement> getStatement = parseGet();
    return std::make_shared<DeleteStatement>(key, getStatement);
  } else if (currentToken().type == Lexer::TokenType::KEY) {
    std::shared_ptr<NestedIdentifier> key = parseNestedIdentifier();
    std::shared_ptr<GetStatement> getStatement = parseGet();
    return std::make_shared<DeleteStatement>(key, getStatement);
  } else {
    throw std::runtime_error("Parsing Failed: Expected a key");
  }
}

// Parses get statements (part of the pratt parser)
std::shared_ptr<GetStatement> QueryParser::Parser::parseGet() {
  advance();
  std::shared_ptr<Identifier> collection = parseIdentifier();
  // can be binaryExp, NotExp or LogicalExp
  Expression condition = parseExpression();
  std::string sort = "";
  if (currentToken().value == "ASC" || currentToken().value == "DESC") {
    sort += advance().value;
  }
  int limit = 0;
  if (currentToken().type == Lexer::TokenType::COMMAND &&
      currentToken().value == "LIMIT") {
    advance();
    if (currentToken().type == Lexer::TokenType::NUMBER) {
      limit += std::stoi(advance().value);
    } else {
      throw std::runtime_error("Parsing Failed: Expected a number");
    }
  } else if (currentToken().type == Lexer::TokenType::OPERATOR &&
             (currentToken().value == "&&" || currentToken().value == "||")) {
    throw std::runtime_error(
        "Parsing Failed: Logical Expressions are not supported");
  }
  return std::make_shared<GetStatement>(collection, condition, sort, limit);
}

// Parses set statements (part of the pratt parser)
std::shared_ptr<SetStatement> QueryParser::Parser::parseSet() {
  advance();
  std::shared_ptr<Identifier> collection = parseIdentifier();
  std::shared_ptr<Literal> json = parseLiteral();
  return std::make_shared<SetStatement>(collection, json);
}

// Parses update statements (part of the pratt parser)
std::shared_ptr<UpdateStatement> QueryParser::Parser::parseUpdate() {
  advance();
  std::shared_ptr<Identifier> collection = parseIdentifier();
  std::shared_ptr<NestedIdentifier> key = parseNestedIdentifier();
  std::shared_ptr<Literal> value = parseLiteral();
  std::shared_ptr<GetStatement> statement = parseGet();
  return std::make_shared<UpdateStatement>(collection, key, value, statement);
}

// Parses indetifiers (collection names)
std::shared_ptr<Identifier> QueryParser::Parser::parseIdentifier() {
  if (currentToken().type == Lexer::TokenType::COLLECTION) {
    return std::make_shared<Identifier>(advance().value);
  } else {
    throw std::runtime_error("Parsing Failed: Missing collection");
  }
}

// Parses nested identifiers (keys like so key.key.key)
std::shared_ptr<NestedIdentifier> QueryParser::Parser::parseNestedIdentifier() {
  std::vector<std::string> keys;
  while (currentToken().type == Lexer::TokenType::KEY) {
    keys.push_back(advance().value);
  }
  if (keys.empty()) {
    throw std::runtime_error("Parsing Failed: Missing keys");
  }
  return std::make_shared<NestedIdentifier>(keys);
}

// Parses literals (either numbers, values or json)
std::shared_ptr<Literal> QueryParser::Parser::parseLiteral() {
  if (currentToken().type == Lexer::TokenType::NUMBER ||
      currentToken().type == Lexer::TokenType::VALUE ||
      currentToken().type == Lexer::TokenType::JSON) {
    std::string &value = advance().value;
    if (value.length() >= 2 && value.front() == '\'' && value.back() == '\'') {
      value = value.substr(1, value.length() - 2);
    }
    return std::make_shared<Literal>(value);
  } else {
    throw std::runtime_error(
        "Parsing Failed: Expected literal but got a different value");
  }
}

// Parses binary expressions
std::shared_ptr<BinaryExpression> QueryParser::Parser::parseBinaryExpression() {
  std::shared_ptr<NestedIdentifier> left = parseNestedIdentifier();
  std::string op;
  if (currentToken().type == Lexer::TokenType::OPERATOR ||
      currentToken().type == Lexer::TokenType::REGEX) {
    op = advance().value;
  } else {
    throw std::runtime_error(
        "Parsing Failed: Expected operator but got a different value");
  }
  std::shared_ptr<Literal> right = parseLiteral();
  return std::make_shared<BinaryExpression>(left, op, right);
}

// Parses expressions (logical, binary or not)
Expression QueryParser::Parser::parseExpression() {
  if (currentToken().value == "NOT") {
    throw std::runtime_error(
        "Parsing Failed: Not expressions are not supported");
    // return parseNotExpression();
  } else if (currentToken().type == Lexer::TokenType::KEY) {
    return parseBinaryExpression();
  } else {
    throw std::runtime_error(
        "Parsing Failed: Logical expressions are not supported");
    // return parseLogicalExpression();
  }
}

// Parses not expressions
std::shared_ptr<NotExpression> QueryParser::Parser::parseNotExpression() {
  std::shared_ptr<BinaryExpression> bin_exp = parseBinaryExpression();
  return std::make_shared<NotExpression>(bin_exp);
}

// Parses logical expressions
std::shared_ptr<LogicalExpression>
QueryParser::Parser::parseLogicalExpression() {
  throw std::runtime_error(
      "Parsing Failed: Logical expressions are not supported");

  std::shared_ptr<BinaryExpression> left = parseBinaryExpression();
  std::string op;
  if (currentToken().type == Lexer::TokenType::OPERATOR ||
      currentToken().type == Lexer::TokenType::REGEX) {
    op = advance().value;
  } else {
    throw std::runtime_error(
        "Parsing Failed: Expected operator but got a different value");
  }
  std::shared_ptr<BinaryExpression> right = parseBinaryExpression();
  return std::make_shared<LogicalExpression>(left, op, right);
}

json QueryParser::interpretQuery(const std::string &query) {
  Lexer lex;
  std::vector<Lexer::Token> tokens = lex.tokenize(query);
  Statement stmt = parse(tokens);

  json result;
  std::visit(
      [this, &result](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<SetStatement>>) {
          // Handle SET statement
        } else if constexpr (std::is_same_v<T, std::shared_ptr<GetStatement>>) {
          std::vector<std::shared_ptr<Document>> resultingDocuments =
              getDocument((*arg));

          for (const auto &doc : resultingDocuments) {
            result.push_back(doc.get()->json_value);
          }
        } else if constexpr (std::is_same_v<T,
                                            std::shared_ptr<UpdateStatement>>) {
          std::vector<std::shared_ptr<Document>> resultingDocuments =
              updateDocument((*arg));

          for (const auto &doc : resultingDocuments) {
            result.push_back(doc.get()->json_value);
          }

        } else if constexpr (std::is_same_v<T,
                                            std::shared_ptr<DeleteStatement>>) {
          std::vector<std::shared_ptr<Document>> resultingDocuments =
              deleteDocument((*arg));

          for (const auto &doc : resultingDocuments) {
            result.push_back(doc.get()->json_value);
          }

        } else {
          throw std::runtime_error("Unexpected statement type");
        }
      },
      stmt);
  return result;
}

// Parse is entry point into parsing
Statement QueryParser::parse(const std::vector<Lexer::Token> &tokens) {
  Parser p(tokens);
  return p.parse();
}

// This is the actual parser parse
Statement QueryParser::Parser::parse() {
  // Rule 0: Must include tokens
  if (tokens.size() > 0) {
    Parser p = Parser(tokens);
    // Rule 1: First token is a COMMAND
    if (p.currentToken().type == Lexer::TokenType::COMMAND) {
      std::shared_ptr<ASTNode> statment;
      if (p.currentToken().value == "GET") {
        return p.parseGet();
      } else if (p.currentToken().value == "SET") {
        return p.parseSet();
      } else if (p.currentToken().value == "UPDATE") {
        return p.parseUpdate();
      } else if (p.currentToken().value == "DELETE") {
        return p.parseDelete();
      } else {
        throw std::runtime_error("Parsing Failed: Invalid command");
      }
    } else {
      throw std::runtime_error(
          "Parsing Failed: Query must start with a command");
    }
  } else {
    throw std::runtime_error("Parsing Failed: No tokens to parse");
  }
}

bool QueryParser::setCollection(std::string collectionName) {
  return dbStorageSystem->setCollection(collectionName);
}

void QueryParser::setDocument([[maybe_unused]] const Document &document) {
  /*try {
    auto stmt = std::get<std::shared_ptr<SetStatement>>(stmt);
    // Do things
  } catch (const std::bad_variant_access &) {
  }*/
}

// Returns a vector of Document pointers - throws if document access fails for
// whatever reason
std::vector<std::shared_ptr<Document>>
QueryParser::getDocument(GetStatement stmt) {
  try {
    return dbStorageSystem->getDocument(stmt);
  } catch (const std::bad_variant_access &) {
    throw std::runtime_error("Failed to get document.");
  }
  return std::vector<std::shared_ptr<Document>>();
}

std::vector<std::shared_ptr<Document>>
QueryParser::updateDocument(const UpdateStatement stmt) {
  try {
    return dbStorageSystem->updateDocument(stmt);
  } catch (const std::bad_variant_access &) {
    throw std::runtime_error("Failed to update document.");
  }
  return std::vector<std::shared_ptr<Document>>();
}

std::vector<std::shared_ptr<Document>>
QueryParser::deleteDocument(const DeleteStatement stmt) {
  try {
    return dbStorageSystem->deleteDocument(stmt);
  } catch (const std::bad_variant_access &) {
    throw std::runtime_error("Failed to get document.");
  }
  return std::vector<std::shared_ptr<Document>>();
}
