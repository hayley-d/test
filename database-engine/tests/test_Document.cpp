#include "../src/ASTNode.h"
#include "../src/Document.h"
#include "../src/Lexer.h"
#include "../src/QueryParser.h"

#include "../src/json.hpp"
#include "doctest.h"
#include <iostream>

TEST_CASE("Testing Document Class") {
  std::string test_json = R"({
    "monsters": [
        { 
            "type": "Griffin", 
            "location": "White Orchard", 
            "difficulty": "Moderate", 
            "weaknesses": ["Grapeshot Bomb", "Hybrid Oil", "Aard Sign"]
        }, 
        { 
            "type": "Leshen", 
            "location": "Crookback Bog", 
            "difficulty": "High", 
            "weaknesses": ["Relict Oil", "Igni Sign", "Burning Down the Forest"]
        }
    ]
})";
  SUBCASE("Test Document Construction") {
    Lexer lex;
    QueryParser::Parser parser =
        QueryParser::Parser(lex.tokenize("SET collection '" + test_json + "'"));

    std::string test_json =
        std::get<std::shared_ptr<SetStatement>>(parser.parse())->json->value;

    // Test constructon without an id key
    CHECK_THROWS_AS(Document doc(test_json, "monsters"), std::runtime_error);
    CHECK_NOTHROW(Document doc("1", test_json, "monsters"));
    Document doc("1", test_json, "monsters");
    CHECK(doc.id == "1");
    CHECK(doc.deleted == false);

    Document doc2("1d", test_json, "monsters");
    CHECK(doc2.id == "1d");
    CHECK(doc2.deleted == true);
  }

  SUBCASE("Test Document Contains") {
    Lexer lex;
    QueryParser::Parser parser =
        QueryParser::Parser(lex.tokenize("SET collection '" + test_json + "'"));

    std::string test_json =
        std::get<std::shared_ptr<SetStatement>>(parser.parse())->json->value;
    Document doc("1", test_json, "monsters");
    CHECK(doc.contains_key("/monsters/0/type") == true);
    CHECK(doc.contains_key("/monsters/1/type") == true);

  }

  SUBCASE("Test Document Helpers") {}
}
