#include "../src/ASTNode.h"
#include "../src/Lexer.h"
#include "../src/QueryParser.h"
#include "doctest.h"
#include <memory>

TEST_CASE("Testing Parsing") {

  SUBCASE("Test Parsing Error Missing Colleciton") {
    Lexer lex;
    QueryParser::Parser parser = QueryParser::Parser(
        lex.tokenize("DELETE /^ +| +$|( ) +/ GET type  == 'sloth'"));
    CHECK_THROWS_AS(parser.parse(), std::runtime_error);
  }

  SUBCASE("Test Parsing Error Missing Literal") {
    Lexer lex;
    QueryParser::Parser parser = QueryParser::Parser(
        lex.tokenize("DELETE /^ +| +$|( ) +/ GET ANIMALS type  =="));
    CHECK_THROWS_AS(parser.parse(), std::runtime_error);
  }

  SUBCASE("Test Parsing Error Logical and NOT") {
    Lexer lex;
    std::vector<Lexer::Token> tokens = lex.tokenize("GET COLLECTION key.key == 'key' && thing.thing == 'thing'");
    CHECK(tokens.size() == 12);
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "GET");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "COLLECTION");
    CHECK(tokens[2].type == Lexer::TokenType::KEY);
    CHECK(tokens[2].value == "key");
    CHECK(tokens[3].type == Lexer::TokenType::KEY);
    CHECK(tokens[3].value == "key");
    CHECK(tokens[4].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[4].value == "==");
    CHECK(tokens[5].type == Lexer::TokenType::VALUE);
    CHECK(tokens[5].value == "'key'");
    CHECK(tokens[6].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[6].value == "&&");
    CHECK(tokens[7].type == Lexer::TokenType::KEY);
    CHECK(tokens[7].value == "thing");
    CHECK(tokens[8].type == Lexer::TokenType::KEY);
    CHECK(tokens[8].value == "thing");
    CHECK(tokens[9].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[9].value == "==");
    CHECK(tokens[10].type == Lexer::TokenType::VALUE);
    CHECK(tokens[10].value == "'thing'");

    QueryParser::Parser parser = QueryParser::Parser(
        QueryParser::Parser(lex.tokenize("GET COLLECTION key.key == 'key' && thing.thing == 'thing'")));
    CHECK_THROWS_AS(parser.parse(), std::runtime_error);

    QueryParser::Parser parser_2 = QueryParser::Parser(
        QueryParser::Parser(lex.tokenize("GET COLLECTION key.key == 'key' || thing.thing == 'thing'")));
    CHECK_THROWS_AS(parser.parse(), std::runtime_error);

    QueryParser::Parser parser_3 = QueryParser::Parser(
        QueryParser::Parser(lex.tokenize("GET COLLECTION NOT thing.thing == 'thing'")));
    CHECK_THROWS_AS(parser.parse(), std::runtime_error);
  }


  SUBCASE("Test Parsing SET") {
    Lexer lex;
    QueryParser::Parser parser =
        QueryParser::Parser(lex.tokenize("SET PEOPLE '{name : Hayley}'"));
    Statement smt = parser.parse();
    CHECK(std::holds_alternative<std::shared_ptr<SetStatement>>(smt));

    // I know its risky (but I'm a bit lazy to handle to try catch)
    auto stmt = std::get<std::shared_ptr<SetStatement>>(smt);
    CHECK(stmt != nullptr);
    CHECK(stmt->collection->name == "PEOPLE");
    CHECK(stmt->json->value == "{name : Hayley}");
  }

  SUBCASE("Test Parsing GET") {
    Lexer lex;
    QueryParser::Parser parser = QueryParser::Parser(
        lex.tokenize("GET PIRATES ship.type == 'Frigate' DESC LIMIT 7"));
    Statement smt = parser.parse();
    CHECK(std::holds_alternative<std::shared_ptr<GetStatement>>(smt));

    auto stmt = std::get<std::shared_ptr<GetStatement>>(smt);
    CHECK(stmt != nullptr);
    CHECK(stmt->collection->name == "PIRATES");
    CHECK(stmt->sortOrder == "DESC");
    CHECK(stmt->limit == 7);

    CHECK(std::holds_alternative<std::shared_ptr<BinaryExpression>>(
        stmt->condition));
    auto condition =
        std::get<std::shared_ptr<BinaryExpression>>(stmt->condition);
    CHECK(stmt != nullptr);
    CHECK(condition->left->join() == "ship.type");
  }

  SUBCASE("Test Parsing Update") {
    Lexer lex;
    std::vector<Lexer::Token> tokens =
        lex.tokenize("UPDATE PIRATES days_since_last_bath '++1' GET PIRATES "
                     "ship.name == 'The Flying Dutchmen'");
    CHECK(tokens.size() == 11);
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "UPDATE");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "PIRATES");
    CHECK(tokens[2].type == Lexer::TokenType::KEY);
    CHECK(tokens[2].value == "days_since_last_bath");
    CHECK(tokens[3].type == Lexer::TokenType::VALUE);
    CHECK(tokens[3].value == "'++1'");
    CHECK(tokens[4].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[4].value == "GET");
    CHECK(tokens[5].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[5].value == "PIRATES");
    CHECK(tokens[6].type == Lexer::TokenType::KEY);
    CHECK(tokens[6].value == "ship");
    CHECK(tokens[7].type == Lexer::TokenType::KEY);
    CHECK(tokens[7].value == "name");
    CHECK(tokens[8].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[8].value == "==");
    CHECK(tokens[9].type == Lexer::TokenType::VALUE);
    CHECK(tokens[9].value == "'The Flying Dutchmen'");

    QueryParser::Parser parser = QueryParser::Parser(
        lex.tokenize("UPDATE PIRATES days_since_last_bath '++1' GET PIRATES "
                     "ship.name == 'The Flying Dutchmen'"));
    Statement smt = parser.parse();
    CHECK(std::holds_alternative<std::shared_ptr<UpdateStatement>>(smt));

    auto stmt = std::get<std::shared_ptr<UpdateStatement>>(smt);
    CHECK(stmt != nullptr);
    CHECK(stmt->collection->name == "PIRATES");
    CHECK(stmt->updateKey->join() == "days_since_last_bath");
    CHECK(stmt->updateValue->value == "++1");
    CHECK(stmt->statement->collection->name == "PIRATES");
    CHECK(stmt->statement->sortOrder == "");
    CHECK(stmt->statement->limit == 0);
    CHECK(std::holds_alternative<std::shared_ptr<BinaryExpression>>(
        stmt->statement->condition));
    auto condition =
        std::get<std::shared_ptr<BinaryExpression>>(stmt->statement->condition);
    CHECK(stmt != nullptr);
    CHECK(condition->left->join() == "ship.name");
    CHECK(condition->op == "==");
    CHECK(condition->right->value == "The Flying Dutchmen");
  }

  SUBCASE("Test Parsing DELETE") {
    Lexer lex;
    QueryParser::Parser parser = QueryParser::Parser(
        lex.tokenize("DELETE thing.thing GET RICKS age >= 100"));
    Statement smt = parser.parse();
    CHECK(std::holds_alternative<std::shared_ptr<DeleteStatement>>(smt));

    // I know its risky (but I'm a bit lazy to handle to try catch)
    auto stmt = std::get<std::shared_ptr<DeleteStatement>>(smt);
    CHECK(stmt != nullptr);

    auto starExpr =
        std::dynamic_pointer_cast<NestedIdentifier>(stmt->deleteKey);
    REQUIRE(starExpr);

    CHECK(stmt->statement->collection->name == "RICKS");
    CHECK(stmt->statement->sortOrder == "");
    CHECK(stmt->statement->limit == 0);

    CHECK(std::holds_alternative<std::shared_ptr<BinaryExpression>>(
        stmt->statement->condition));
    auto condition =
        std::get<std::shared_ptr<BinaryExpression>>(stmt->statement->condition);
    CHECK(stmt != nullptr);
    CHECK(condition->left->join() == "age");
    CHECK(condition->op == ">=");
    CHECK(condition->right->value == "100");
  }
}
