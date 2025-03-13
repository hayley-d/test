#include "doctest.h"
#include <iostream>  
#include "../src/DatabaseStorageEngine.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace fs = std::filesystem;


TEST_CASE("Test Storage engine constructor")
{
    fs::path dirPath = "storage/" + std::string("PickleRick");
    fs::path ctrPath = dirPath / "operation_counter.txt";
    fs::path walPath = dirPath / "wal.txt";

    SUBCASE("Create new user database")
    {
        Database* db_1 = new DatabaseStorageEngine("PickleRick");
        delete db_1;
        
        std::ifstream ctrFile(ctrPath);
        CHECK(ctrFile.good());
        ctrFile.close();

        std::ifstream walFile(walPath);
        CHECK(walFile.good());
        walFile.good();
    }

    SUBCASE("Create database for existing user")
    {
        Database* db_2 = new DatabaseStorageEngine("PickleRick");
        delete db_2;

        std::ifstream ctrFile(ctrPath);
        CHECK(ctrFile.good());
        ctrFile.close();

        std::ifstream walFile(walPath);
        CHECK(walFile.good());
        walFile.good();
    }

    // Remove created files and folders
    std::remove(ctrPath.c_str());
    std::remove(walPath.c_str());
    fs::remove(dirPath);
}

TEST_CASE("Test setCollection function")
{   

    SUBCASE("Set existing collection")
    {

        Database* db_3 = new DatabaseStorageEngine("fakeUser");
        fs::path newCollectionPath = "storage/fakeUser/FAKE_COLLECTION";
        fs::create_directories(newCollectionPath);
        
        CHECK(db_3->setCollection("FAKE_COLLECTION") == true);   
        delete db_3;
        fs::remove(fs::path("storage/fakeUser/operation_counter.txt"));
        fs::remove(fs::path("storage/fakeUser/wal.txt"));
        fs::remove(fs::path("storage/fakeUser/FAKE_COLLECTION"));
        fs::remove(fs::path("storage/fakeUser")); 
    }


    SUBCASE("Set non existing collection")
    {
        Database* db_4 = new DatabaseStorageEngine("fakeUser");
        CHECK(db_4->setCollection("FAKE_COLLECTION") == false);   
        delete db_4;
        fs::remove(fs::path("storage/fakeUser/operation_counter.txt"));
        fs::remove(fs::path("storage/fakeUser/wal.txt"));
        fs::remove(fs::path("storage/fakeUser"));
    }

}

/*TEST_CASE("Test writeToKeyFile function")
{   

    DatabaseStorageEngine db_4("VeryEvilMorty");
    fs::path testCollection = "storage/VeryEvilMorty/EVILCOLLECTION";
    fs::create_directories(testCollection);
    db_4.setCollection("EvilCollection");

    SUBCASE("Test writing key : int")
    {
        db_4.writeToKeyFile(4, 6, "ammo");
        db_4.writeToKeyFile(5, 7, "ammo");

        std::string writtenLines; std::string curLine;
        std::fstream file(testCollection / fs::path("ammo.json"));
        while(getline(file, curLine))
        {
            writtenLines += curLine;
        }

        json expectedFile = {
            {"4", 6},
            {"5", 7}
        };

        json writtenFile = json::parse(writtenLines);

        CHECK(writtenFile == expectedFile);
        fs::remove(testCollection / fs::path("ammo.json"));
    }

    SUBCASE("Test writing key : double")
    {
        db_4.writeToKeyFile(1, 350.302, "boozeLeftLiters");
        db_4.writeToKeyFile(5, 309.87, "boozeLeftLiters");

        std::string writtenLines; std::string curLine;
        std::fstream file(testCollection / fs::path("boozeleftliters.json"));
        while(getline(file, curLine))
        {
            writtenLines += curLine;
        }

        json expectedFile = {
            {"1", 350.302},
            {"5", 309.87}
        };

        json writtenFile = json::parse(writtenLines);

        CHECK(writtenFile == expectedFile);
        fs::remove(testCollection / fs::path("boozeleftliters.json"));
    }

    SUBCASE("Test writing key : const char*")
    {
        db_4.writeToKeyFile(1, "C-131", "rickdimension");
        db_4.writeToKeyFile(5, "C-138", "rickdimension");

        std::string writtenLines; std::string curLine;
        std::fstream file(testCollection / fs::path("rickdimension.json"));
        while(getline(file, curLine))
        {
            writtenLines += curLine;
        }

        json expectedFile = {
            {"1", "C-131"},
            {"5", "C-138"}
        };

        json writtenFile = json::parse(writtenLines);

        CHECK(writtenFile == expectedFile);
        fs::remove(testCollection / fs::path("rickdimension.json"));
    }

    SUBCASE("Test writing key : std::string ")
    {
        db_4.writeToKeyFile(1, std::string("C-131"), "rickdimension");
        db_4.writeToKeyFile(5, std::string("C-138"), "rickdimension");

        std::string writtenLines; std::string curLine;
        std::fstream file(testCollection / fs::path("rickdimension.json"));
        while(getline(file, curLine))
        {
            writtenLines += curLine;
        }

        json expectedFile = {
            {"1", "C-131"},
            {"5", "C-138"}
        };

        json writtenFile = json::parse(writtenLines);

        CHECK(writtenFile == expectedFile);
        fs::remove(testCollection / fs::path("rickdimension.json"));
    }
 
    SUBCASE("Test writing key : bool")
    {
        db_4.writeToKeyFile(1, true, "mortyalive");
        db_4.writeToKeyFile(5, false, "mortyalive");

        std::string writtenLines; std::string curLine;
        std::fstream file(testCollection / fs::path("mortyalive.json"));
        while(getline(file, curLine))
        {
            writtenLines += curLine;
        }

        json expectedFile = {
            {"1", true},
            {"5", false}
        };

        json writtenFile = json::parse(writtenLines);

        CHECK(writtenFile == expectedFile);
        fs::remove(testCollection / fs::path("mortyalive.json"));
    }

    SUBCASE("Test writing large mixed file")
    {
        std::vector<std::string> randomStrings = {"Jerry", "Summer", "Beth", "Rick", "Morty", "Mr Meeseeks", "Birdperson"};
        std::vector<int> randomNumbers = {1, 6, 98, 24, 42, 842, 582, 112, 492};

        json expectedFile;

        for(int i=0; i<20; i++)
        {   
            if(i % 2 == 0)
            {
                db_4.writeToKeyFile(i, randomStrings.at(i % randomStrings.size()), "largefile");
                expectedFile[std::to_string(i)] =  randomStrings.at(i % randomStrings.size());
            }
            else
            {
                db_4.writeToKeyFile(i, randomNumbers.at(i % randomNumbers.size()), "largefile");
                expectedFile[std::to_string(i)] = randomNumbers.at(i % randomNumbers.size());
            }
        }

        std::string writtenLines; std::string curLine;
        std::fstream file(testCollection / fs::path("largefile.json"));

        while(getline(file, curLine))
        {
            writtenLines += curLine;
        }

        json writtenFile = json::parse(writtenLines);
        CHECK(writtenFile == expectedFile);
        fs::remove(testCollection / fs::path("largefile.json"));
        
    }
   
}

TEST_CASE("Pseudo test case")
{
    
    fs::remove(fs::path("storage/VeryEvilMorty/operation_counter.txt"));
    fs::remove(fs::path("storage/VeryEvilMorty/wal.txt"));
    fs::remove(fs::path("storage/VeryEvilMorty/EVILCOLLECTION"));
    fs::remove(fs::path("storage/VeryEvilMorty"));
}*/

TEST_CASE("TEST writeToMainFile")
{

    DatabaseStorageEngine db_5("GeraltOfRivia");
    fs::path testCollection = "storage/GeraltOfRivia/BEASTIARY";
    fs::create_directories(testCollection);
    db_5.setCollection("beastiary");

    SUBCASE("Writing json objects to main")
    {
        json testObj1 = R"(
            {
            "creatureName" : "Arachnomorphs",
            "description" : "Annoying ass spiders",
            "weakTo" : ["Aard", "Axii", "Insectoid Oil"],
            "details" :    {
                            "legs" : 8,
                            "eyes" : "too many"
                        } 
            }
        )"_json;
        db_5.writeToMainFile("1", testObj1);


        json testObj2 = R"(
            {
                "creatureName" : "Dandelion",
                "description" : "Flowery bard",
                "weakTo" : ["Women", "Women", "Sharp pointy objects", "Bad poetry"],
                "details" : {
                                "height" : "5ft9",
                                "tinderHeight" : "6ft2"
                            }
            }
        
        )"_json;
        db_5.writeToMainFile("2", testObj2);

        std::ifstream file(fs::path("storage/GeraltOfRivia/BEASTIARY/main.json"));
        json filedata = json::parse(file);

        json expectedResult = R"(
            [
            {
            	"id": "1",
            	"data": {"creatureName":"Arachnomorphs","description":"Annoying ass spiders","details":{"eyes":"too many","legs":8},"weakTo":["Aard","Axii","Insectoid Oil"]}
            },
            {
            	"id": "2",
            	"data": {"creatureName":"Dandelion","description":"Flowery bard","details":{"height":"5ft9","tinderHeight":"6ft2"},"weakTo":["Women","Women","Sharp pointy objects","Bad poetry"]}
            }
            ]
        )"_json;

        CHECK(expectedResult == filedata);
    }
}

TEST_CASE("Pseudo test case")
{
    fs::remove(fs::path("storage/GeraltOfRivia/operation_counter.txt"));
    fs::remove(fs::path("storage/GeraltOfRivia/wal.txt"));
    fs::remove(fs::path("storage/GeraltOfRivia/BEASTIARY/main.json"));
    fs::remove(fs::path("storage/GeraltOfRivia/BEASTIARY"));
    fs::remove(fs::path("storage/GeraltOfRivia"));
}

TEST_CASE("DatabaseFileManager Utilities")
{   
    DatabaseStorageEngine db_6("ManButAttorney");
    SUBCASE("Test creating new collection and existing collection")
    {
    
        db_6.createCollection("CoinTossResults");

        fs::path createdCollection("storage/ManButAttorney/COINTOSSRESULTS");

        CHECK(fs::exists(createdCollection));

        fs::remove("storage/ManButAttorney/COINTOSSRESULTS/main.json");
        fs::remove(createdCollection);
    }

    SUBCASE("Test writing to wal w/o timestamp")
    {
        db_6.writeToWal("GET COINTOSSRESULTS BatmanParentStatus != DEAD ASC 0", false);
    }

    SUBCASE("Test writing to wal with timestamp")
    {
        db_6.writeToWal("GET COINTOSSRESULTS RobinsRemaining > 1 DESC 2", true);
    }


    fs::remove(fs::path("storage/ManButAttorney/operation_counter.txt"));
    fs::remove(fs::path("storage/ManButAttorney/wal.txt"));
    fs::remove(fs::path("storage/ManButAttorney"));
}
