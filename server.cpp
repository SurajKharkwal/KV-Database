#include "KVStore.h"
#include "crow.h"
#include <iostream>
#include <string>

int main() {
  crow::SimpleApp app; // Initialize Crow app
  KvStore kv;          // Create an instance of KvStore

  // Default route
  CROW_ROUTE(app, "/")([]() { return "Key-Value Store API"; });

  // Route to set key-value pairs
  CROW_ROUTE(app, "/setKv")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body || !body["key"] || !body["value"]) {
          return crow::response(400, R"({"message": "Invalid JSON"})");
        }
        std::string key = body["key"].s();
        std::string value = body["value"].s();
        auto result = kv.insertKv(key, value);
        kv.saveData(); // Save data to file
        return crow::response(
            crow::json::wvalue{{"message", result.message}, {"key", key}});
      });

  // Route to get a value by key
  CROW_ROUTE(app, "/getKv/<string>")
  ([&](std::string key) {
    auto result = kv.searchKv(key);
    if (result.wasSuccess) {
      return crow::response(
          crow::json::wvalue{{"key", key}, {"value", result.message}});
    }
    std::cout << result.message << std::endl;
    return crow::response(404);
  });

  // Route to delete a key
  CROW_ROUTE(app, "/deleteKv/<string>")
  ([&](std::string key) {
    auto result = kv.deleteKv(key);
    if (result.wasSuccess) {
      kv.saveData(); // Save data after deletion
      return crow::response(crow::json::wvalue{{"message", result.message}});
    }

    std::cout << result.message << std::endl;
    return crow::response(404);
  });

  // Route to update a key-value pair
  CROW_ROUTE(app, "/updateKv")
      .methods(crow::HTTPMethod::PUT)([&](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body || !body["key"] || !body["value"]) {
          return crow::response(400, R"({"message": "Invalid JSON"})");
        }
        std::string key = body["key"].s();
        std::string value = body["value"].s();
        auto result = kv.updateKv(key, value);
        if (result.wasSuccess) {
          kv.saveData(); // Save data after update
          return crow::response(
              crow::json::wvalue{{"message", result.message}, {"key", key}});
        }

        std::cout << result.message << std::endl;
        return crow::response(404);
      });

  // Route to load data from a file
  CROW_ROUTE(app, "/loadData")
  ([&]() {
    auto result = kv.loadData("./data.bat");
    if (result.wasSuccess) {
      return crow::response(crow::json::wvalue{{"message", result.message}});
    }
    std::cout << result.message << std::endl;
    return crow::response(500);
  });

  // Run the app on port 18080 with multithreading
  app.port(18080).multithreaded().run();
}
