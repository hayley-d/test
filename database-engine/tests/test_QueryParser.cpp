#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_DISABLE_PARALLEL
#include "../src/Lexer.h"
#include "doctest.h"
#include <iostream>
#include <vector>

#include "test_ASTNode.cpp"
#include "test_Parser.cpp"
#include "test_HTTPServer.cpp"
#include "test_DatabaseStorageEngine.cpp"
#include "test_Iterator.cpp"
//#include "test_Queries.cpp"


void printVector(const std::vector<Lexer::Token> &vec) {
  std::cout << "Vector contents: ";
  for (const auto &item : vec) {
    std::cout << item.value << " : ";
  }
  std::cout << "\n";
}

TEST_CASE("Testing Lexing") {
  Lexer lex;

  SUBCASE("Test token length") {
    // Test simple SET
    auto tokens = lex.tokenize("SET INVENTIONS '{name: Meeseeks}'");
    CHECK(tokens.size() == 4);

    tokens = lex.tokenize("DELETE weakness.comment GET COLLECTION2 weakness.oil == 'Necrophage Oil'");
    CHECK(tokens.size() == 10);

    tokens = lex.tokenize("DELETE description GET COLLECTION name == 'Alghoul'");
    CHECK(tokens.size() == 8);

    // Test SET with multi-line json
    tokens = lex.tokenize(R"(SET MORTYS '{
                        "name" : "Evil Morty",
                        "leader" : true,
                        "approval" : 100,
                        "address" : {
                                        "street" : "74",
                                        "city" : "Citadle"
                                    }
                    }')");
    CHECK(tokens.size() == 4);

    // Test simple GET
    tokens = lex.tokenize("GET INVENTIONS name == 'Plumbus'");
    CHECK(tokens.size() == 6);

    tokens = lex.tokenize("GET COLLECTION name == 'Leshen'");
    CHECK(tokens.size() == 6);

    // Test simple DELETE with wildcard
    tokens = lex.tokenize("DELETE * GET RICKS age >= 100");
    CHECK(tokens.size() == 8);

    // Test DELETE with regex
    tokens = lex.tokenize("DELETE /^ +| +$|( ) +/ GET RICKS age >= 100");
    CHECK(tokens.size() == 8);

    // Test simple DELETE
    tokens =
        lex.tokenize("DELETE rick.sayings GET rick.saysings == 'Shwifty'");
    CHECK(tokens.size() == 9);

    // TEST GET with SORT and LIMIT
    tokens = lex.tokenize("GET PIRATES ship.type == 'Frigate' DESC LIMIT 7");
    CHECK(tokens.size() == 10);

    // Test GET with many keys
    tokens = lex.tokenize("GET PIRATES ship.type.a.b.c == 'Frigate' DESC 4 7");
    CHECK(tokens.size() == 13);
  }

  SUBCASE("Test Token Type") {
    auto tokens = lex.tokenize("SET collection '{key: value}'");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "SET");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "collection");
    CHECK(tokens[2].type == Lexer::TokenType::JSON);
    CHECK(tokens[2].value == "'{key: value}'");

    tokens = lex.tokenize("GET COLLECTION key.key == 'The Black Pearl'");
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
    CHECK(tokens[5].value == "'The Black Pearl'");

    tokens = lex.tokenize(R"(SET PIRATES '{
                        "name" : "Joshamee Gibs",
                        "rank" : "First Mate",
                        "age" : 53,
                        "address" : {
                                        "street" : "7-th Lane",
                                        "city" : "Tortuga"
                                    }
                    }')");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "SET");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "PIRATES");
    CHECK(tokens[2].type == Lexer::TokenType::JSON);
    CHECK(
        tokens[2].value ==
        R"('{ "name" : "Joshamee Gibs", "rank" : "First Mate", "age" : 53, "address" : { "street" : "7-th Lane", "city" : "Tortuga" } }')");

    tokens = lex.tokenize("DELETE /*/ GET PIRATES age >= 100");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "DELETE");
    CHECK(tokens[1].type == Lexer::TokenType::REGEX);
    CHECK(tokens[1].value == R"(/*/)");
    CHECK(tokens[2].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[2].value == "GET");
    CHECK(tokens[3].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[3].value == "PIRATES");
    CHECK(tokens[4].type == Lexer::TokenType::KEY);
    CHECK(tokens[4].value == "age");
    CHECK(tokens[5].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[5].value == ">=");
    CHECK(tokens[6].type == Lexer::TokenType::NUMBER);
    CHECK(tokens[6].value == "100");

    tokens = lex.tokenize("DELETE /^ +| +$|( ) +/ GET PIRATES age >= 100");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "DELETE");
    CHECK(tokens[1].type == Lexer::TokenType::REGEX);
    CHECK(tokens[1].value == R"(/^ +| +$|( ) +/)");
    CHECK(tokens[2].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[2].value == "GET");
    CHECK(tokens[3].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[3].value == "PIRATES");
    CHECK(tokens[4].type == Lexer::TokenType::KEY);
    CHECK(tokens[4].value == "age");
    CHECK(tokens[5].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[5].value == ">=");
    CHECK(tokens[6].type == Lexer::TokenType::NUMBER);
    CHECK(tokens[6].value == "100");

    tokens = lex.tokenize(
        "DELETE COLLECTION ship.nickname GET ship.nickname == 'Small Boat'");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "DELETE");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "COLLECTION");
    CHECK(tokens[2].type == Lexer::TokenType::KEY);
    CHECK(tokens[2].value == "ship");
    CHECK(tokens[3].type == Lexer::TokenType::KEY);
    CHECK(tokens[3].value == "nickname");
    CHECK(tokens[4].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[4].value == "GET");
    CHECK(tokens[5].type == Lexer::TokenType::KEY);
    CHECK(tokens[5].value == "ship");
    CHECK(tokens[6].type == Lexer::TokenType::KEY);
    CHECK(tokens[6].value == "nickname");
    CHECK(tokens[7].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[7].value == "==");
    CHECK(tokens[8].type == Lexer::TokenType::VALUE);
    CHECK(tokens[8].value == "'Small Boat'");

    tokens = lex.tokenize("GET PIRATES ship.type == 'Frigate' DESC 4 7");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "GET");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "PIRATES");
    CHECK(tokens[2].type == Lexer::TokenType::KEY);
    CHECK(tokens[2].value == "ship");
    CHECK(tokens[3].type == Lexer::TokenType::KEY);
    CHECK(tokens[3].value == "type");
    CHECK(tokens[4].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[4].value == "==");
    CHECK(tokens[5].type == Lexer::TokenType::VALUE);
    CHECK(tokens[5].value == "'Frigate'");
    CHECK(tokens[6].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[6].value == "DESC");
    CHECK(tokens[7].type == Lexer::TokenType::NUMBER);
    CHECK(tokens[7].value == "4");
    CHECK(tokens[8].type == Lexer::TokenType::NUMBER);
    CHECK(tokens[8].value == "7");

    tokens = lex.tokenize("GET PIRATES ship.type.a.b.c == 'Frigate' DESC 4 7");
    CHECK(tokens[0].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[0].value == "GET");
    CHECK(tokens[1].type == Lexer::TokenType::COLLECTION);
    CHECK(tokens[1].value == "PIRATES");
    CHECK(tokens[2].type == Lexer::TokenType::KEY);
    CHECK(tokens[2].value == "ship");
    CHECK(tokens[3].type == Lexer::TokenType::KEY);
    CHECK(tokens[3].value == "type");
    CHECK(tokens[4].type == Lexer::TokenType::KEY);
    CHECK(tokens[4].value == "a");
    CHECK(tokens[5].type == Lexer::TokenType::KEY);
    CHECK(tokens[5].value == "b");
    CHECK(tokens[6].type == Lexer::TokenType::KEY);
    CHECK(tokens[6].value == "c");

    CHECK(tokens[7].type == Lexer::TokenType::OPERATOR);
    CHECK(tokens[7].value == "==");
    CHECK(tokens[8].type == Lexer::TokenType::VALUE);
    CHECK(tokens[8].value == "'Frigate'");
    CHECK(tokens[9].type == Lexer::TokenType::COMMAND);
    CHECK(tokens[9].value == "DESC");
    CHECK(tokens[10].type == Lexer::TokenType::NUMBER);
    CHECK(tokens[10].value == "4");
    CHECK(tokens[11].type == Lexer::TokenType::NUMBER);
    CHECK(tokens[11].value == "7");
  }

  SUBCASE("Empty Input") {
    CHECK_THROWS_AS(lex.tokenize(""), std::runtime_error);
  }

  SUBCASE("Negative Integer") {
    CHECK_THROWS_AS(lex.tokenize("SET -10"), std::runtime_error);
  }

  SUBCASE("Unsupported Character") {
    CHECK_THROWS_AS(lex.tokenize("SET ANIMAL -sloth"), std::runtime_error);
  }

  SUBCASE("Unsupported Character") {
    CHECK_THROWS_AS(lex.tokenize("SET * sloth"), std::runtime_error);
  }

  SUBCASE("Unterminated") {
    CHECK_THROWS_AS(
        lex.tokenize("DELETE /^ +| +$|( ) + GET ANIMALS type  == 'sloth'"),
        std::runtime_error);

    CHECK_THROWS_AS(
        lex.tokenize("DELETE /^ +| +$|( ) +/ GET ANIMALS type  == 'sloth"),
        std::runtime_error);

    CHECK_THROWS_AS(lex.tokenize("SET ANIMAL '{type: sloth"),
                    std::runtime_error);

    CHECK_THROWS_AS(lex.tokenize("SET ANIMAL '{type: sloth}"),
                    std::runtime_error);

    CHECK_THROWS_AS(lex.tokenize("GET ANIMALS type == 'sloth' DESC 04 7"),
                    std::runtime_error);

    CHECK_THROWS_AS(lex.tokenize("GET ANIMALS type == 'sloth' DESC 4 -7"),
                    std::runtime_error);
  }

  SUBCASE("Incorrect Format") {
    CHECK_THROWS_AS(lex.tokenize("ANIMAL SET sloth"), std::runtime_error);
  }
}
