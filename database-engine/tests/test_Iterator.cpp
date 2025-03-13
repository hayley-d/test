#include "../src/JsonFileIterator.h"
#include "doctest.h"
#include <filesystem>
#include <fstream>
#include <string>

TEST_CASE("Testing JsonFileIterator") {

  /*SUBCASE("Test Iterator") {
    const std::string file_path = "storage/iter_test.json";
    std::ofstream outFile(file_path);
    if (!outFile) {
      throw std::runtime_error("Failed to create test file: " + file_path);
    }

    outFile << R"DELIM({
    "0" : "Leshen (Cursed Greenpeace activist)",
    "1" : "Alghoul (Overcaffeinated version of a ghoul)",
    "2" : "Chort (Unhinged Version of a Fiend)",
    "3" : "Fiend (Deer That Hates Your Guts)",
    "4" : "Drowner (Aquatic Serial Killer)", 
    "5" : "Botchling (highly deformed fetus)"
    })DELIM";
    outFile.close();

    JsonFileIterator it(file_path);
    std::vector<std::string> keys;
    std::vector<std::string> values;
    while (!it.isEnd()) {
      auto [key, value] = *it;
      keys.push_back(key);
      values.push_back(value);
      ++it;
    }

    CHECK(keys[0] == "0");
    CHECK(keys[1] == "1");
    CHECK(keys[2] == "2");
    CHECK(keys[3] == "3");
    CHECK(keys[4] == "4");
    CHECK(keys[5] == "5");

    CHECK(values[0] == "Leshen (Cursed Greenpeace activist)");
    CHECK(values[1] == "Alghoul (Overcaffeinated version of a ghoul)");
    CHECK(values[2] == "Chort (Unhinged Version of a Fiend)");
    CHECK(values[3] == "Fiend (Deer That Hates Your Guts)");
    CHECK(values[4] == "Drowner (Aquatic Serial Killer)");
    CHECK(values[5] == "Botchling (highly deformed fetus)");

    if (std::filesystem::exists(file_path)) {
      std::filesystem::remove(file_path);
    }
  }

  SUBCASE("Test Iterator with int") {
    const std::string file_path = "storage/iter_test.json";
    std::ofstream outFile(file_path);
    if (!outFile) {
      throw std::runtime_error("Failed to create test file: " + file_path);
    }

    outFile << R"DELIM({
    "0" : 0,
    "1" : 1,
    "2" : 2,
    "3" : 3,
    "4" : 4, 
    "5" : 5 
    })DELIM";
    outFile.close();

    JsonFileIterator it(file_path);
    std::vector<std::string> keys;
    std::vector<std::string> values;
    while (!it.isEnd()) {
      auto [key, value] = *it;
      keys.push_back(key);
      values.push_back(value);
      ++it;
    }

    CHECK(keys[0] == "0");
    CHECK(keys[1] == "1");
    CHECK(keys[2] == "2");
    CHECK(keys[3] == "3");
    CHECK(keys[4] == "4");
    CHECK(keys[5] == "5");

    CHECK(values[0] == "0");
    CHECK(values[1] == "1");
    CHECK(values[2] == "2");
    CHECK(values[3] == "3");
    CHECK(values[4] == "4");
    CHECK(values[5] == "5");

    if (std::filesystem::exists(file_path)) {
      std::filesystem::remove(file_path);
    }
  }

  SUBCASE("Test Iterator with bool") {
    const std::string file_path = "storage/iter_test.json";
    std::ofstream outFile(file_path);
    if (!outFile) {
      throw std::runtime_error("Failed to create test file: " + file_path);
    }

    outFile << R"DELIM({
    "0" : true,
    "1" : false,
    "2" : true,
    "3" : false,
    "4" : true, 
    "5" : false 
    })DELIM";
    outFile.close();

    JsonFileIterator it(file_path);
    std::vector<std::string> keys;
    std::vector<std::string> values;
    while (!it.isEnd()) {
      auto [key, value] = *it;
      keys.push_back(key);
      values.push_back(value);
      ++it;
    }

    CHECK(keys[0] == "0");
    CHECK(keys[1] == "1");
    CHECK(keys[2] == "2");
    CHECK(keys[3] == "3");
    CHECK(keys[4] == "4");
    CHECK(keys[5] == "5");

    CHECK(values[0] == "true");
    CHECK(values[1] == "false");
    CHECK(values[2] == "true");
    CHECK(values[3] == "false");
    CHECK(values[4] == "true");
    CHECK(values[5] == "false");

    if (std::filesystem::exists(file_path)) {
      std::filesystem::remove(file_path);
    }
  }

  SUBCASE("Test Iterator with float") {
    const std::string file_path = "storage/iter_test.json";
    std::ofstream outFile(file_path);
    if (!outFile) {
      throw std::runtime_error("Failed to create test file: " + file_path);
    }

    outFile << R"DELIM({
    "0" : 4.5,
    "1" : 4.5,
    "2" : 4.5,
    "3" : 44.5,
    "4" : 433.5, 
    "5" : 4.5555 
    })DELIM";
    outFile.close();

    JsonFileIterator it(file_path);
    std::vector<std::string> keys;
    std::vector<std::string> values;
    while (!it.isEnd()) {
      auto [key, value] = *it;
      keys.push_back(key);
      values.push_back(value);
      ++it;
    }

    CHECK(keys[0] == "0");
    CHECK(keys[1] == "1");
    CHECK(keys[2] == "2");
    CHECK(keys[3] == "3");
    CHECK(keys[4] == "4");
    CHECK(keys[5] == "5");

    CHECK(values[0] == "4.5");
    CHECK(values[1] == "4.5");
    CHECK(values[2] == "4.5");
    CHECK(values[3] == "44.5");
    CHECK(values[4] == "433.5");
    CHECK(values[5] == "4.5555");

    if (std::filesystem::exists(file_path)) {
      std::filesystem::remove(file_path);
    }
  }*/

}
