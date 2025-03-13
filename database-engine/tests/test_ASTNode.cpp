#include "../src/ASTNode.h"
#include "doctest.h"
#include <memory>

namespace doctest {
    template <>
    struct StringMaker<std::shared_ptr<BinaryExpression>> {
        static String convert(const std::shared_ptr<BinaryExpression>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<LogicalExpression>> {
        static String convert(const std::shared_ptr<LogicalExpression>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<NotExpression>> {
        static String convert(const std::shared_ptr<NotExpression>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<Identifier>> {
        static String convert(const std::shared_ptr<Identifier>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<Literal>> {
        static String convert(const std::shared_ptr<Literal>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<NestedIdentifier>> {
        static String convert(const std::shared_ptr<NestedIdentifier>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<SetStatement>> {
        static String convert(const std::shared_ptr<SetStatement>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<GetStatement>> {
        static String convert(const std::shared_ptr<GetStatement>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<UpdateStatement>> {
        static String convert(const std::shared_ptr<UpdateStatement>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };

    template <>
    struct StringMaker<std::shared_ptr<DeleteStatement>> {
        static String convert(const std::shared_ptr<DeleteStatement>& expr) {
            if (!expr) return "nullptr";
            return String(expr->toString().c_str());
        }
    };
}

TEST_CASE("Testing AST Nodes") {
  // Test Identifier
  auto collection = std::make_shared<Identifier>("animals");
  CHECK(collection->name == "animals");

  // Test NestedIdentifier (leader.name)
  auto nestedKey = std::make_shared<NestedIdentifier>("leader");
  nestedKey->addKey("name");
  CHECK(nestedKey->keys.size() == 2);
  CHECK(nestedKey->keys[0] == "leader");
  CHECK(nestedKey->keys[1] == "name");

  // Test Literal
  auto literal = std::make_shared<Literal>("Tom Nook");
  CHECK(literal->value == "Tom Nook");

  // Test BinaryExpression (leader.name = "Tom Nook")
  auto condition = std::make_shared<BinaryExpression>(nestedKey, "=", literal);
  CHECK(condition->op == "=");

  // Test NotExpression (NOT leader.name = "Tom Nook")
  auto notCondition = std::make_shared<NotExpression>(condition);
  auto binaryExpr =
      std::dynamic_pointer_cast<BinaryExpression>(notCondition->expr);
  REQUIRE(binaryExpr);
  CHECK(binaryExpr->op == "=");

  // Test LogicalExpression (leader.name = "Tom Nook" && species = "raccoon")
  auto speciesKey = std::make_shared<NestedIdentifier>("species");
  auto speciesValue = std::make_shared<Literal>("raccoon");
  auto speciesCondition =
      std::make_shared<BinaryExpression>(speciesKey, "=", speciesValue);
  auto andCondition =
      std::make_shared<LogicalExpression>(condition, "&&", speciesCondition);
  CHECK(andCondition->op == "&&");

  // Test SetStatement (SET animals { "leader": "Tom Nook" })
  auto jsonData = std::make_shared<Literal>(R"({ "leader\": \"Tom Nook\" })");
  auto setStatement = std::make_shared<SetStatement>(collection, jsonData);
  CHECK(setStatement->collection->name == "animals");
  CHECK(setStatement->json->value == R"({ "leader\": \"Tom Nook\" })");

  // Test GetStatement (GET animals leader.name = "Tom Nook")
  auto getStatement = std::make_shared<GetStatement>(collection, condition);
  CHECK(getStatement->collection->name == "animals");

  // Test UpdateStatement (UPDATE animals SET leader.name = "Snowball" WHERE
  // leader.name = "Tom Nook")
  auto newLeader = std::make_shared<Literal>("Snowball");
  auto updateStatement = std::make_shared<UpdateStatement>(
      collection, nestedKey, newLeader, getStatement);
  CHECK(updateStatement->updateValue->value == "Snowball");

  // Test DeleteStatement (DELETE * GET leader.name = "Tom Nook")
  auto wildcard = std::make_shared<NestedIdentifier>("*");
  auto deleteStatement = std::make_shared<DeleteStatement>(wildcard, getStatement);
  auto starExpr = std::dynamic_pointer_cast<NestedIdentifier>(deleteStatement->deleteKey);
  REQUIRE(starExpr);
  CHECK(starExpr->keys[0] == "*");
}
