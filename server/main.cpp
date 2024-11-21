#include "KVStore.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Invalid number of arguments" << std::endl;
    return 1;
  }

  KvStore kvStore;
  std::string command = argv[1];
  std::string key, value;
  kvStore.loadData();

  if (command == "insertKv") {
    if (argc < 4) {
      std::cerr << "Missing key or value for insertKv" << std::endl;
      return 1;
    }
    key = argv[2];
    value = argv[3];
    OperationResult result = kvStore.insertKv(key, value);
    kvStore.saveData();
    std::cout << result.message << std::endl;
  } else if (command == "deleteKv") {
    if (argc < 3) {
      std::cerr << "Missing key for deleteKv" << std::endl;
      return 1;
    }
    key = argv[2];
    OperationResult result = kvStore.deleteKv(key);
    std::cout << result.message << std::endl;
  } else if (command == "updateKv") {
    if (argc < 4) {
      std::cerr << "Missing key or value for updateKv" << std::endl;
      return 1;
    }
    key = argv[2];
    value = argv[3];
    OperationResult result = kvStore.updateKv(key, value);
    std::cout << result.message << std::endl;
  } else if (command == "loadData") {
    OperationResult result = kvStore.loadData();
    std::cout << result.message << std::endl;
  } else if (command == "getAll") {
    OperationResult result = kvStore.getAll();
    std::cout << result.message << std::endl;
  } else if (command == "searchKv") {
    kvStore.loadData();
    OperationResult result = kvStore.searchKv(argv[2]);
    std::cout << result.message << std::endl;
  } else {
    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
  }

  return 0;
}
