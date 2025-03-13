#include "../src/QueryParser.h"
#include "../src/json.hpp"
#include "doctest.h"
#include <iostream>

// Helper function(s)
inline void writeJsonFile(const std::string &filename, const json &content) {
  std::ofstream outFile(filename);
  if (!outFile) {
    throw std::runtime_error("Failed to open file: " + filename);
  }
  outFile << content.dump(4);
  outFile.close();
}

TEST_CASE("Test Get query") {
  DatabaseStorageEngine db("Geralt");
  db.createCollection("COLLECTION");
  QueryParser parser("Geralt", &db);

  const std::string file_path = "storage/Geralt/COLLECTION/name.json";
  std::ofstream outFile(file_path);
  if (!outFile) {
    throw std::runtime_error("Failed to create test file: " + file_path);
  }

  outFile << R"DELIM({
    "0" : "Leshen",
    "1" : "Alghoul",
    "2" : "Chort"
    })DELIM";
  outFile.close();

  const std::string main_path = "storage/Geralt/COLLECTION/main.json";
  std::ofstream outFile2(main_path);

  if (!outFile2) {
    throw std::runtime_error("Failed to create test file: " + main_path);
  }

  outFile2 << R"DELIM([
    {
    	"id": "0",
    	"data": {"name":"Leshen","description":"Cursed Greenpeace activist"}
    },
    {
    	"id": "1",
    	"data": {"name":"Alghoul","description":"Ghouls but spikier"}
    },
    {
    	"id": "2",
    	"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
    }
    ])DELIM";
  outFile2.close();

  SUBCASE("Basic get queries") {
    json tst1 = R"([{
        "id" : "0",
        "data" : {"name":"Leshen","description":"Cursed Greenpeace activist"}
        }])"_json;

    json tst2 = R"([
      	{
      		"id": "2",
      		"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
      	},
      	{
      		"id": "1",
      		"data": {"name":"Alghoul","description":"Ghouls but spikier"}
      	}
        ])"_json;

    json tst3 = R"([
        {
        	"id": "2",
        	"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
        },
        {
        	"id": "1",
        	"data": {"name":"Alghoul","description":"Ghouls but spikier"}
        },
        {
        	"id": "0",
        	"data": {"name":"Leshen","description":"Cursed Greenpeace activist"}
        }
        ])"_json;

    json nullJson;

    CHECK(parser.interpretQuery("GET COLLECTION name == 'Leshen'") == tst1);

    json tst2Result = parser.interpretQuery("GET COLLECTION name != 'Leshen'");
    CHECK(std::is_permutation(tst2Result.begin(), tst2Result.end(),
                              tst2.begin(), tst2.end()));

    json tst3Result = parser.interpretQuery("GET COLLECTION name <= 'Leshen'");
    CHECK(std::is_permutation(tst3Result.begin(), tst3Result.end(),
                              tst3.begin(), tst3.end()));

    //CHECK(parser.interpretQuery("GET COLLECTION name >= 'Leshen'") == tst1);
    json tst4Result = parser.interpretQuery("GET COLLECTION name >= 'Leshen'");
    CHECK(std::is_permutation(tst4Result.begin(), tst4Result.end(),
                              tst1.begin(), tst1.end()));

    CHECK(parser.interpretQuery("GET COLLECTION name > 'Leshen'") == nullJson);

    //CHECK(parser.interpretQuery("GET COLLECTION name < 'Leshen'") == tst2);
    json tst5Result = parser.interpretQuery("GET COLLECTION name < 'Leshen'");
    CHECK(std::is_permutation(tst5Result.begin(), tst5Result.end(),
                              tst2.begin(), tst2.end()));

  }
  SUBCASE("Clean up directory") {
    fs::remove(fs::path("storage/Geralt/operation_counter.txt"));
    fs::remove(fs::path("storage/Geralt/wal.txt"));
    fs::remove(fs::path("storage/Geralt/COLLECTION/name.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION/main.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION"));
    fs::remove(fs::path("storage/Geralt"));
  }
}

TEST_CASE("Test more complicated GET Queries") {

  DatabaseStorageEngine db2("Geralt");
  db2.createCollection("COLLECTION2");
  QueryParser parser("Geralt", &db2);
  db2.setMaximumMapSize(4); // Set map size to 4 to test removeLRU functionality

  const std::string file_path = "storage/Geralt/COLLECTION2/main.json";

  std::ofstream outFile(file_path);
  if (!outFile) {
    throw std::runtime_error("Failed to create test file: " + file_path);
  }

  outFile << R"DELIM(
    [
      {
        "id" : "0",
        "data" :  {
                    "name" : "Nekker",
                    "class" : "Ogroids",
                    "weakness" :  {
                                    "oil" : "Ogroid Oil",
                                    "comment" : "Free mutagen farm"
                                  },
                    "dangerLevel" : 3
                  }
      }, 
      {
        "id" : "1",
        "data" :  {
                    "name" : "Siren",
                    "class" : "Hybrids",
                    "weakness" :  {
                                    "oil" : "Hybrid Oil",
                                    "comment" : "It's rude to stare"
                                  },
                    "dangerLevel" : 2 
                  }
      }, 
      {
        "id" : "2",
        "data" :  {
                    "name" : "Rotfiend",
                    "class" : "Necrophage",
                    "weakness" :  {
                                    "oil" : "Necrophage Oil",
                                    "comment" : "Suspected in the activities of September the 11th"
                                  },
                    "dangerLevel" : 6 
                  }
      }, 
      {
        "id" : "3",
        "data" :  {
                    "name" : "Golem",
                    "class" : "Elementa",
                    "weakness" :  {
                                    "oil" : "Elemental oil",
                                    "comment" : "Rocky horror picture show"
                                  },
                    "dangerLevel" : 7 
                  }
      }, 
      {
        "id" : "4",
        "data" :  {
                    "name" : "Wolves",
                    "class" : "Beasts",
                    "weakness" :  {
                                    "oil" : "Hunting rifle oil",
                                    "comment" : "Geralt's 2nd greatest foe"
                                  },
                    "dangerLevel" : 9 
                  }
      }, 
      {
        "id" : "5",
        "data" :  {
                    "name" : "Gravity",
                    "class" : "Newtonian",
                    "weakness" :  {
                                    "oil" : "Olive",
                                    "comment" : "Geralt's greatest foe"
                                  },
                    "dangerLevel" : 999 
                  }
      } 
    ]
    )DELIM";
  outFile.close();

  // Create all key.json files automatically
  std::ifstream inFile(file_path);
  if (!inFile) {
    std::cerr << "Failed to open input file: " << file_path << std::endl;
  }

  json mainJson;
  inFile >> mainJson;
  inFile.close();

  json nameJson, classJson, oilJson, commentJson, dangerLevelJson, idJson;

  for (const auto &creature : mainJson) {
    std::string id = creature["id"].get<std::string>();
    nameJson[id] = creature["data"]["name"];
    classJson[id] = creature["data"]["class"];
    oilJson[id] = creature["data"]["weakness"]["oil"];
    commentJson[id] = creature["data"]["weakness"]["comment"];
    dangerLevelJson[id] = creature["data"]["dangerLevel"];
    idJson[id] = id;
  }

  writeJsonFile("storage/Geralt/COLLECTION2/name.json", nameJson);
  writeJsonFile("storage/Geralt/COLLECTION2/class.json", classJson);
  writeJsonFile("storage/Geralt/COLLECTION2/weakness.oil.json", oilJson);
  writeJsonFile("storage/Geralt/COLLECTION2/weakness.comment.json",
                commentJson);
  writeJsonFile("storage/Geralt/COLLECTION2/dangerLevel.json", dangerLevelJson);
  writeJsonFile("storage/Geralt/COLLECTION2/id.json", idJson);

  json tst1 = R"([
        {
            "id": "5",
            "data": {
                "name": "Gravity",
                "class": "Newtonian",
                "weakness": {
                    "oil": "Olive",
                    "comment": "Geralt's greatest foe"
                },
                "dangerLevel": 999
            }
        }
    ])"_json;

  json tst2 = R"([
        {
            "id": "2",
            "data": {
                "name": "Rotfiend",
                "class": "Necrophage",
                "weakness": {
                    "oil": "Necrophage Oil",
                    "comment": "Suspected in the activities of September the 11th"
                },
                "dangerLevel": 6
            }
        }
    ])"_json;

  json tst3 = nullptr; // I actually don't know why this returns the wrong thing

  json tst4 = R"([
        {
            "id": "3",
            "data": {
                "name": "Golem",
                "class": "Elementa",
                "weakness": {
                    "oil": "Elemental oil",
                    "comment": "Rocky horror picture show"
                },
                "dangerLevel": 7
            }
        }
    ])"_json;

  SUBCASE("Run 4 get queries to fill cache") {
    CHECK(parser.interpretQuery("GET COLLECTION2 weakness.oil == 'Olive'") == tst1);
    CHECK(parser.interpretQuery("GET COLLECTION2 name == 'Rotfiend'") == tst2);
    // CHECK(parser.interpretQuery("GET COLLECTION dangerLevel == 9") == tst3);
    CHECK(parser.interpretQuery("GET COLLECTION2 id == '3'") == tst4);
  }

  SUBCASE("Run a different query to ensure cache needs to be cleared") {
    parser.interpretQuery("GET COLLECTION2 name != 'Siren'");
  }

  /*SUBCASE("Check difference between response time with cahce hit and no cache "
          "hit") {
    std::chrono::steady_clock::time_point noCacheStart =
        std::chrono::steady_clock::now();
    parser.interpretQuery("GET COLLECTION2 name == 'Siren'");
    std::chrono::steady_clock::time_point noCacheEnd =
        std::chrono::steady_clock::now();

    std::chrono::steady_clock::time_point cacheStart =
        std::chrono::steady_clock::now();
    parser.interpretQuery("GET COLLECTION2 name == 'Siren'");
    std::chrono::steady_clock::time_point cacheEnd =
        std::chrono::steady_clock::now();

    // std::cout << "Time with no cache: " <<
    // (std::chrono::duration_cast<std::chrono::microseconds>(noCacheEnd-
    // noCacheStart).count()) << "\n"; std::cout << "Time with cache: " <<
    // (std::chrono::duration_cast<std::chrono::microseconds>(cacheEnd -
    // cacheStart).count()) << "\n";
  }*/
  SUBCASE("Clear up directory (x2)") {
    fs::remove(fs::path("storage/Geralt/operation_counter.txt"));
    fs::remove(fs::path("storage/Geralt/wal.txt"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/main.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/name.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/class.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/weakness.oil.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/weakness.comment.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/dangerLevel.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/id.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2"));
    fs::remove(fs::path("storage/Geralt"));
  }
}

TEST_CASE("Test Simple Update query") {
  DatabaseStorageEngine db("Geralt");
  db.createCollection("COLLECTION");
  QueryParser parser("Geralt", &db);

  const std::string file_path = "storage/Geralt/COLLECTION/name.json";
  std::ofstream outFile(file_path);
  if (!outFile) {
    throw std::runtime_error("Failed to create test file: " + file_path);
  }

  outFile << R"DELIM({
    "0" : "Leshen",
    "1" : "Alghoul",
    "2" : "Chort"
    })DELIM";
  outFile.close();

  const std::string main_path = "storage/Geralt/COLLECTION/main.json";
  std::ofstream outFile2(main_path);

  if (!outFile2) {
    throw std::runtime_error("Failed to create test file: " + main_path);
  }

  outFile2 << R"DELIM([
    {
    	"id": "0",
    	"data": {"name":"Leshen","description":"Cursed Greenpeace activist"}
    },
    {
    	"id": "1",
    	"data": {"name":"Alghoul","description":"Ghouls but spikier"}
    },
    {
    	"id": "2",
    	"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
    }
    ])DELIM";
  outFile2.close();

  SUBCASE("Basic update queries") {
    json tst1 = R"([{
        "id" : "2",
        "data" : {"name":"Chort","description":"Headbanged so hard it grew extra horns"}
        }])"_json;

    json tst2 = R"([
      	{
      		"id": "2",
      		"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
      	},
      	{
      		"id": "1",
      		"data": {"name":"Alghoul","description":"Ghouls but spikier"}
      	}
        ])"_json;

    json tst3 = R"([
        {
        	"id": "1",
        	"data": {"name":"Alghoul","description":"A fancy ghoul"}
        }])"_json;

    json nullJson;

    CHECK(parser.interpretQuery(
              "UPDATE COLLECTION description 'Headbanged so hard it grew extra "
              "horns' GET COLLECTION name == 'Chort'") == tst1);

    CHECK(parser.interpretQuery("UPDATE COLLECTION description 'A fancy ghoul' "
                                "GET COLLECTION name == 'Alghoul'") == tst3);

    CHECK(parser.interpretQuery(
              "UPDATE COLLECTION description 'Killing one is now a war crime' "
              "GET COLLECTION name == 'Sloth'") == nullJson);
  }

  SUBCASE("Clean up directory") {
    fs::remove(fs::path("storage/Geralt/operation_counter.txt"));
    fs::remove(fs::path("storage/Geralt/wal.txt"));
    fs::remove(fs::path("storage/Geralt/COLLECTION/name.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION/main.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION"));
    fs::remove(fs::path("storage/Geralt"));
  }
}

TEST_CASE("Test more complicated UPDATE Queries") {
  DatabaseStorageEngine db2("Geralt");
  db2.createCollection("COLLECTION2");
  QueryParser parser("Geralt", &db2);
  db2.setMaximumMapSize(4); // Set map size to 4 to test removeLRU functionality

  const std::string file_path = "storage/Geralt/COLLECTION2/main.json";

  std::ofstream outFile(file_path);
  if (!outFile) {
    throw std::runtime_error("Failed to create test file: " + file_path);
  }

  outFile << R"DELIM(
    [
      {
        "id" : "0",
        "data" :  {
                    "name" : "Nekker",
                    "class" : "Ogroids",
                    "weakness" :  {
                                    "oil" : "Ogroid Oil",
                                    "comment" : "Free mutagen farm"
                                  },
                    "dangerLevel" : 3
                  }
      }, 
      {
        "id" : "1",
        "data" :  {
                    "name" : "Siren",
                    "class" : "Hybrids",
                    "weakness" :  {
                                    "oil" : "Hybrid Oil",
                                    "comment" : "It's rude to stare"
                                  },
                    "dangerLevel" : 2 
                  }
      }, 
      {
        "id" : "2",
        "data" :  {
                    "name" : "Rotfiend",
                    "class" : "Necrophage",
                    "weakness" :  {
                                    "oil" : "Necrophage Oil",
                                    "comment" : "Suspected in the activities of September the 11th"
                                  },
                    "dangerLevel" : 6 
                  }
      }, 
      {
        "id" : "3",
        "data" :  {
                    "name" : "Golem",
                    "class" : "Elementa",
                    "weakness" :  {
                                    "oil" : "Elemental oil",
                                    "comment" : "Rocky horror picture show"
                                  },
                    "dangerLevel" : 7 
                  }
      }, 
      {
        "id" : "4",
        "data" :  {
                    "name" : "Wolves",
                    "class" : "Beasts",
                    "weakness" :  {
                                    "oil" : "Hunting rifle oil",
                                    "comment" : "Geralt's 2nd greatest foe"
                                  },
                    "dangerLevel" : 9 
                  }
      }, 
      {
        "id" : "5",
        "data" :  {
                    "name" : "Gravity",
                    "class" : "Newtonian",
                    "weakness" :  {
                                    "oil" : "Olive",
                                    "comment" : "Geralt's greatest foe"
                                  },
                    "dangerLevel" : 999 
                  }
      } 
    ]
    )DELIM";
  outFile.close();

  // Create all key.json files automatically
  std::ifstream inFile(file_path);
  if (!inFile) {
    std::cerr << "Failed to open input file: " << file_path << std::endl;
  }

  json mainJson;
  inFile >> mainJson;
  inFile.close();

  json nameJson, classJson, oilJson, commentJson, dangerLevelJson, idJson;

  for (const auto &creature : mainJson) {
    std::string id = creature["id"].get<std::string>();
    nameJson[id] = creature["data"]["name"];
    classJson[id] = creature["data"]["class"];
    oilJson[id] = creature["data"]["weakness"]["oil"];
    commentJson[id] = creature["data"]["weakness"]["comment"];
    dangerLevelJson[id] = creature["data"]["dangerLevel"];
    idJson[id] = id;
  }

  writeJsonFile("storage/Geralt/COLLECTION2/name.json", nameJson);
  writeJsonFile("storage/Geralt/COLLECTION2/class.json", classJson);
  writeJsonFile("storage/Geralt/COLLECTION2/weakness.oil.json", oilJson);
  writeJsonFile("storage/Geralt/COLLECTION2/weakness.comment.json",
                commentJson);
  writeJsonFile("storage/Geralt/COLLECTION2/dangerLevel.json", dangerLevelJson);
  writeJsonFile("storage/Geralt/COLLECTION2/id.json", idJson);


  json tst1 = R"([
    {
        "id" : "2",
        "data" :  {
                    "name" : "Rotfiend",
                    "class" : "Necrophage",
                    "weakness" :  {
                                    "oil" : "Necrophage Oil",
                                    "comment" : "CONFIRMED participant in the activities of September the 11th"
                                  },
                    "dangerLevel" : 6 
                  }
      }
    ])"_json;

  json tst3 = nullptr;

  json tst4 = R"([
      {
        "id" : "5",
        "data" :  {
                    "name" : "Gravity",
                    "class" : "Newtonian",
                    "weakness" :  {
                                    "oil" : "Olive",
                                    "comment" : "Geralt's greatest foe"
                                  },
                    "dangerLevel" : 1000000 
                  }
      }
    ])"_json;

  SUBCASE("Test Update queries") {
    CHECK(parser.interpretQuery("UPDATE COLLECTION2 weakness.comment 'CONFIRMED participant in the activities of September the 11th' GET COLLECTION2 weakness.oil == 'Necrophage Oil'") == tst1);
    CHECK(parser.interpretQuery("UPDATE COLLECTION2 dangerLevel '1000000' GET COLLECTION name == 'Gravity'") == tst4);
  }

  SUBCASE("Clear up directory (x2)") {
    fs::remove(fs::path("storage/Geralt/operation_counter.txt"));
    fs::remove(fs::path("storage/Geralt/wal.txt"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/main.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/name.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/class.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/weakness.oil.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/weakness.comment.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/dangerLevel.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/id.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2"));
    fs::remove(fs::path("storage/Geralt"));
  }
}

/*TEST_CASE("Test Simple Delete query") {
  DatabaseStorageEngine db("Geralt");
  db.createCollection("COLLECTION");
  QueryParser parser("Geralt", &db);

  const std::string file_path = "storage/Geralt/COLLECTION/name.json";
  std::ofstream outFile(file_path);
  if (!outFile) {
    throw std::runtime_error("Failed to create test file: " + file_path);
  }

  outFile << R"DELIM({
    "0" : "Leshen",
    "1" : "Alghoul",
    "2" : "Chort"
    })DELIM";
  outFile.close();

  const std::string main_path = "storage/Geralt/COLLECTION/main.json";
  std::ofstream outFile2(main_path);

  if (!outFile2) {
    throw std::runtime_error("Failed to create test file: " + main_path);
  }

  outFile2 << R"DELIM([
    {
    	"id": "0",
    	"data": {"name":"Leshen","description":"Cursed Greenpeace activist"}
    },
    {
    	"id": "1",
    	"data": {"name":"Alghoul","description":"Ghouls but spikier"}
    },
    {
    	"id": "2",
    	"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
    }
    ])DELIM";
  outFile2.close();

  SUBCASE("Basic update queries") {
    json tst1 = R"([{
        "id" : "2d",
        "data" : {"name":"Chort","description":"Top 10 metal album covers of all time"}
        }])"_json;

    json tst2 = R"([
      	{
      		"id": "2",
      		"data": {"name":"Chort","description":"Top 10 metal album covers of all time"}
      	},
      	{
      		"id": "1",
      		"data": {"name":"Alghoul","description":"Ghouls but spikier"}
      	}
        ])"_json;

    json tst3 = R"([
        {
        	"id": "1",
        	"data": {"name":"Alghoul"}
        }])"_json;

    json nullJson;

    CHECK(parser.interpretQuery("DELETE * GET COLLECTION name == 'Chort'") == tst1);
    CHECK(parser.interpretQuery("DELETE * GET COLLECTION name == 'Chort'") == nullJson);


    //CHECK(parser.interpretQuery("DELETE description GET COLLECTION name == 'Alghoul'") == tst3);
    //CHECK(parser.interpretQuery("GET COLLECTION name == 'Alghoul'") == tst3);


    //CHECK(parser.interpretQuery("DELETE description GET COLLECTION name == 'Sloth'") == nullJson);
  }

  SUBCASE("Clean up directory") {
    fs::remove(fs::path("storage/Geralt/operation_counter.txt"));
    fs::remove(fs::path("storage/Geralt/wal.txt"));
    fs::remove(fs::path("storage/Geralt/COLLECTION/name.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION/main.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION"));
    fs::remove(fs::path("storage/Geralt"));
  }
}

TEST_CASE("Test more complicated UPDATE Queries") {
  DatabaseStorageEngine db2("Geralt");
  db2.createCollection("COLLECTION2");
  QueryParser parser("Geralt", &db2);
  db2.setMaximumMapSize(4); // Set map size to 4 to test removeLRU functionality

  const std::string file_path = "storage/Geralt/COLLECTION2/main.json";

  std::ofstream outFile(file_path);
  if (!outFile) {
    throw std::runtime_error("Failed to create test file: " + file_path);
  }

  outFile << R"DELIM(
    [
      {
        "id" : "0",
        "data" :  {
                    "name" : "Nekker",
                    "class" : "Ogroids",
                    "weakness" :  {
                                    "oil" : "Ogroid Oil",
                                    "comment" : "Free mutagen farm"
                                  },
                    "dangerLevel" : 3
                  }
      }, 
      {
        "id" : "1",
        "data" :  {
                    "name" : "Siren",
                    "class" : "Hybrids",
                    "weakness" :  {
                                    "oil" : "Hybrid Oil",
                                    "comment" : "It's rude to stare"
                                  },
                    "dangerLevel" : 2 
                  }
      }, 
      {
        "id" : "2",
        "data" :  {
                    "name" : "Rotfiend",
                    "class" : "Necrophage",
                    "weakness" :  {
                                    "oil" : "Necrophage Oil",
                                    "comment" : "Suspected in the activities of September the 11th"
                                  },
                    "dangerLevel" : 6 
                  }
      }, 
      {
        "id" : "3",
        "data" :  {
                    "name" : "Golem",
                    "class" : "Elementa",
                    "weakness" :  {
                                    "oil" : "Elemental oil",
                                    "comment" : "Rocky horror picture show"
                                  },
                    "dangerLevel" : 7 
                  }
      }, 
      {
        "id" : "4",
        "data" :  {
                    "name" : "Wolves",
                    "class" : "Beasts",
                    "weakness" :  {
                                    "oil" : "Hunting rifle oil",
                                    "comment" : "Geralt's 2nd greatest foe"
                                  },
                    "dangerLevel" : 9 
                  }
      }, 
      {
        "id" : "5",
        "data" :  {
                    "name" : "Gravity",
                    "class" : "Newtonian",
                    "weakness" :  {
                                    "oil" : "Olive",
                                    "comment" : "Geralt's greatest foe"
                                  },
                    "dangerLevel" : 999 
                  }
      } 
    ]
    )DELIM";
  outFile.close();

  // Create all key.json files automatically
  std::ifstream inFile(file_path);
  if (!inFile) {
    std::cerr << "Failed to open input file: " << file_path << std::endl;
  }

  json mainJson;
  inFile >> mainJson;
  inFile.close();

  json nameJson, classJson, oilJson, commentJson, dangerLevelJson, idJson;

  for (const auto &creature : mainJson) {
    std::string id = creature["id"].get<std::string>();
    nameJson[id] = creature["data"]["name"];
    classJson[id] = creature["data"]["class"];
    oilJson[id] = creature["data"]["weakness"]["oil"];
    commentJson[id] = creature["data"]["weakness"]["comment"];
    dangerLevelJson[id] = creature["data"]["dangerLevel"];
    idJson[id] = id;
  }

  writeJsonFile("storage/Geralt/COLLECTION2/name.json", nameJson);
  writeJsonFile("storage/Geralt/COLLECTION2/class.json", classJson);
  writeJsonFile("storage/Geralt/COLLECTION2/weakness.oil.json", oilJson);
  writeJsonFile("storage/Geralt/COLLECTION2/weakness.comment.json",
                commentJson);
  writeJsonFile("storage/Geralt/COLLECTION2/dangerLevel.json", dangerLevelJson);
  writeJsonFile("storage/Geralt/COLLECTION2/id.json", idJson);


  json tst1 = R"([
    {
        "id" : "2",
        "data" :  {
                    "name" : "Rotfiend",
                    "class" : "Necrophage",
                    "weakness" :  {
                                    "oil" : "Necrophage Oil"
                                  },
                    "dangerLevel" : 6 
                  }
      }
    ])"_json;

  json tst2 = R"([
    {
        "id" : "2d",
        "data" :  {
                    "name" : "Rotfiend",
                    "class" : "Necrophage",
                    "weakness" :  {
                                    "oil" : "Necrophage Oil"
                                  },
                    "dangerLevel" : 6 
                  }
      }
    ])"_json;


  json tst3 = nullptr;

  json tst4 = R"([
      {
        "id" : "5",
        "data" :  {
                    "name" : "Gravity",
                    "class" : "Newtonian",
                    "weakness" :  {
                                    "oil" : "Olive",
                                    "comment" : "Geralt's greatest foe"
                                  }
                  }
      }
    ])"_json;

  json tst5 = R"([
      {
        "id" : "5d",
        "data" :  {
                    "name" : "Gravity",
                    "class" : "Newtonian",
                    "weakness" :  {
                                    "oil" : "Olive",
                                    "comment" : "Geralt's greatest foe"
                                  }
                  }
      }
    ])"_json;


  SUBCASE("Test Delete queries") {
    CHECK(parser.interpretQuery("DELETE weakness.comment GET COLLECTION2 weakness.oil == 'Necrophage Oil'") == tst1);
    CHECK(parser.interpretQuery("GET COLLECTION2 weakness.oil == 'Necrophage Oil'") == tst1);
    CHECK(parser.interpretQuery("DELETE * GET COLLECTION2 weakness.oil == 'Necrophage Oil'") == tst2);
    CHECK(parser.interpretQuery("GET COLLECTION2 weakness.oil == 'Necrophage Oil'") == tst3);


    CHECK(parser.interpretQuery("DELETE dangerLevel GET COLLECTION name == 'Gravity'") == tst4);
    CHECK(parser.interpretQuery("GET COLLECTION name == 'Gravity'") == tst4);
    CHECK(parser.interpretQuery("DELETE * GET COLLECTION name == 'Gravity'") == tst5);
    CHECK(parser.interpretQuery("GET COLLECTION name == 'Gravity'") == tst3);
  }

  SUBCASE("Clear up directory (x2)") {
    fs::remove(fs::path("storage/Geralt/operation_counter.txt"));
    fs::remove(fs::path("storage/Geralt/wal.txt"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/main.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/name.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/class.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/weakness.oil.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/weakness.comment.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/dangerLevel.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2/id.json"));
    fs::remove(fs::path("storage/Geralt/COLLECTION2"));
    fs::remove(fs::path("storage/Geralt"));
  }
}*/
