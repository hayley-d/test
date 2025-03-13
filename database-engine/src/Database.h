#ifndef DATABASE_H
#define DATABASE_H

#include "Document.h"
#include <string>

#include "ASTNode.h"

/*using Statement =
    std::variant<std::shared_ptr<GetStatement>, std::shared_ptr<SetStatement>,
                 std::shared_ptr<DeleteStatement>,
                 std::shared_ptr<UpdateStatement>>;*/

class Database {
protected:
  std::string userId;

public:
  Database(std::string userId) : userId(userId) {}
  virtual ~Database() = default;
  virtual bool setCollection(std::string collectionName) = 0; // Set collection on which queries execute, return true if collection exist else false
  virtual void setDocument(const Document &document) = 0;
  virtual std::vector<std::shared_ptr<Document>> getDocument(GetStatement stmt) = 0;
  virtual std::vector<std::shared_ptr<Document>> updateDocument(const UpdateStatement stmt) = 0;
  virtual std::vector<std::shared_ptr<Document>> deleteDocument(const DeleteStatement stmt) = 0;
};

#endif // DATABASE_H
