#ifndef ASTNODE_H
#define ASTNODE_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

// Forward Declarations
struct BinaryExpression;
struct LogicalExpression;
struct NotExpression;
struct Identifier;
struct Literal;
struct SetStatement;
struct GetStatement;
struct UpdateStatement;
struct DeleteStatement;
struct NestedIdentifier;

// Base class for all AST nodes.
struct ASTNode {
  virtual ~ASTNode() = default;
  virtual std::string toString() const = 0;
};

// Expression type that can hold various types of expressions in the AST.
using Expression = std::variant<std::shared_ptr<BinaryExpression>,
                                std::shared_ptr<LogicalExpression>,
                                std::shared_ptr<NotExpression>>;

// Statememt type that can hold various types of statements in the AST.
using Statement =
    std::variant<std::shared_ptr<GetStatement>, std::shared_ptr<SetStatement>,
                 std::shared_ptr<DeleteStatement>,
                 std::shared_ptr<UpdateStatement>>;

// Represents an identifier (collection name).
struct Identifier : public ASTNode {
  const std::string name;

  explicit Identifier(const std::string &n) : name(n) {}

  std::string toString() const override { return "Identifier(" + name + ")"; }
};

// Represents a nested JSON key
struct NestedIdentifier : public ASTNode {
  std::vector<std::string> keys;

  explicit NestedIdentifier(const std::string &key) { keys.push_back(key); }
  explicit NestedIdentifier(const std::vector<std::string> &keys)
      : keys(keys) {}

  void addKey(const std::string &key) { keys.push_back(key); }

  std::string toString() const override {
    std::string out = "NestedIdentifier(";

    for (auto it = keys.begin(); it != keys.end(); ++it) {
      out += *it;
      if (it + 1 != keys.end())
        out += ".";
    }
    out += ")";
    return out;
  }

  std::string join() {
    std::string out = "";
    for (size_t i = 0; i < keys.size(); ++i) {
      out += keys[i];
      if (i != keys.size() - 1) {
        out += ".";
      }
    }
    return out;
  }
};

// Represents a literal value (number, string, JSON).
struct Literal : public ASTNode {
  const std::string value;

  explicit Literal(const std::string &v) : value(v) {}

  std::string toString() const override { return "Literal(" + value + ")"; }
};

// Represents a binary comparison expression
struct BinaryExpression : public ASTNode {
  std::shared_ptr<NestedIdentifier> left;
  std::string op; // ==, !=, >, <, <=, >=
  std::shared_ptr<Literal> right;

  BinaryExpression(std::shared_ptr<NestedIdentifier> left,
                   const std::string &op, std::shared_ptr<Literal> right)
      : left(std::move(left)), op(op), right(std::move(right)) {}

  std::string toString() const override {
    return "BinaryExpression(" + left->toString() + " " + op + " " +
           right->toString() + ")";
  }
};

// Represents a logical expression (`&&` for AND, `||` for OR)
// This is if we decide to implement this functionality in sprint 2
struct LogicalExpression : public ASTNode {
  std::shared_ptr<BinaryExpression> left;
  std::string op;
  std::shared_ptr<BinaryExpression> right;

  LogicalExpression(std::shared_ptr<BinaryExpression> l, const std::string &o,
                    std::shared_ptr<BinaryExpression> r)
      : left(std::move(l)), op(o), right(std::move(r)) {}

  std::string toString() const override {
    return "LogicalExpression(" + left->toString() + " " + op + " " +
           right->toString() + ")";
  }
};

// Represents a `NOT` expression (`NOT <key> <operator> <value>`).
// This is if we decide to add this functionality later
struct NotExpression : public ASTNode {
  std::shared_ptr<BinaryExpression> expr;

  explicit NotExpression(std::shared_ptr<BinaryExpression> expression)
      : expr(std::move(expression)) {}

  std::string toString() const override {
    return "NotExpression(" + expr->toString() + ")";
  }
};

// Represents a `SET` statement
struct SetStatement : public ASTNode {
  std::shared_ptr<Identifier> collection;
  std::shared_ptr<Literal> json;

  SetStatement(std::shared_ptr<Identifier> collection,
               std::shared_ptr<Literal> json)
      : collection(std::move(collection)), json(std::move(json)) {}

  std::string toString() const override {
    return "SetStatement(collection=" + collection->toString() +
           ", json=" + json->toString() + ")";
  }
};

// Represents a `GET` statement
struct GetStatement : public ASTNode {
  std::shared_ptr<Identifier> collection;
  Expression condition;
  std::string sortOrder;
  int limit;

  GetStatement(std::shared_ptr<Identifier> collection, Expression cond)
      : collection(std::move(collection)), condition(std::move(cond)),
        sortOrder(""), limit(0) {}

  GetStatement(std::shared_ptr<Identifier> collection, Expression cond,
               const std::string &sort)
      : collection(std::move(collection)), condition(std::move(cond)),
        sortOrder(sort), limit(0) {}

  GetStatement(std::shared_ptr<Identifier> collection, Expression cond,
               const std::string &sort, int limit)
      : collection(std::move(collection)), condition(std::move(cond)),
        sortOrder(sort), limit(limit) {}

  std::string toString() const override {
    return "GetStatement(collection=" + collection->toString() +
           ", condition=" + ")";
  }
};

// Represents a `UPDATE` statement
struct UpdateStatement : public ASTNode {
  std::shared_ptr<Identifier> collection;
  std::shared_ptr<NestedIdentifier> updateKey;
  std::shared_ptr<Literal> updateValue;
  std::shared_ptr<GetStatement> statement;

  UpdateStatement(std::shared_ptr<Identifier> collection,
                  std::shared_ptr<NestedIdentifier> key,
                  std::shared_ptr<Literal> value,
                  std::shared_ptr<GetStatement> smt)
      : collection(std::move(collection)), updateKey(std::move(key)),
        updateValue(value), statement(smt) {}

  std::string toString() const override {
    return "UpdateStatement(collection=" + collection->toString() +
           ", updateKey=" + updateKey->toString() +
           ", updateValue=" + updateValue->toString() +
           ", statement=" + statement->toString() + ")";
  }
};

// Represents a `DELETE` statement
struct DeleteStatement : public ASTNode {
  std::shared_ptr<NestedIdentifier> deleteKey;
  std::shared_ptr<GetStatement> statement;

  DeleteStatement(std::shared_ptr<NestedIdentifier> deleteKey,
                  std::shared_ptr<GetStatement> smt)
      : deleteKey(std::move(deleteKey)), statement(std::move(smt)) {}

  std::string toString() const override {
    return "DeleteStatement(key=" + deleteKey->toString() +
           ", statement=" + statement->toString() + ")";
  }
};

#endif // ASTNODE_H
