#ifndef QUERYPARSER_H
#define QUERYPARSER_H
#include "ASTNode.h"
#include "Database.h"
#include "DatabaseStorageEngine.h"
#include "Lexer.h"
#include <memory>
#include <string>
#include <vector>

class QueryParser : public Database {
  DatabaseStorageEngine *dbStorageSystem;

public:
  QueryParser(std::string userId, DatabaseStorageEngine *db)
      : Database(userId), dbStorageSystem(db) {}
  ~QueryParser() = default;
  bool setCollection(std::string collectionName) override;
  void setDocument(const Document &document) override;
  std::vector<std::shared_ptr<Document>>
  getDocument(GetStatement stmt) override;
  std::vector<std::shared_ptr<Document>> updateDocument(const UpdateStatement stmt)
      override; // Added update if we decide to use it
  std::vector<std::shared_ptr<Document>> deleteDocument(const DeleteStatement stmt) override;
  Statement parse(const std::vector<Lexer::Token> &tokens);
  json interpretQuery(const std::string &query);
  struct Parser {
    std::vector<Lexer::Token> tokens;
    int position;

    explicit Parser(const std::vector<Lexer::Token> &tokens)
        : tokens(tokens), position(0) {}

    Lexer::Token &currentToken();
    Lexer::Token &advance();
    bool hasTokens();
    Statement parse();
    std::shared_ptr<DeleteStatement> parseDelete();
    std::shared_ptr<GetStatement> parseGet();
    std::shared_ptr<SetStatement> parseSet();
    std::shared_ptr<UpdateStatement> parseUpdate();
    std::shared_ptr<Identifier> parseIdentifier();
    std::shared_ptr<NestedIdentifier> parseNestedIdentifier();
    std::shared_ptr<Literal> parseLiteral();
    std::shared_ptr<BinaryExpression> parseBinaryExpression();
    Expression parseExpression();
    std::shared_ptr<NotExpression> parseNotExpression();
    std::shared_ptr<LogicalExpression> parseLogicalExpression();
  };

private:
  void executeAST(const std::unique_ptr<ASTNode> &node);
  std::unique_ptr<ASTNode> buildQueryAST();
};

#endif // QUERYPARSER_H
