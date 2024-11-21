#ifndef KVSTORE_H
#define KVSTORE_H

#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// Node structure to represent key-value pairs in the tree
struct Node {
  std::string key;
  std::string value;
  Node *left;
  Node *right;

  Node(const std::string &k, const std::string &v, Node *l_ptr = nullptr,
       Node *r_ptr = nullptr)
      : key(k), value(v), left(l_ptr), right(r_ptr) {}

  // Serialize a node to a binary format
  void serialize(std::ofstream &file) const {
    size_t key_size = key.size();
    size_t value_size = value.size();

    // Write key and value size first
    file.write(reinterpret_cast<const char *>(&key_size), sizeof(key_size));
    file.write(key.c_str(), key_size);
    file.write(reinterpret_cast<const char *>(&value_size), sizeof(value_size));
    file.write(value.c_str(), value_size);
  }

  // Deserialize a node from a binary format
  static Node *deserialize(std::ifstream &file) {
    size_t key_size, value_size;
    file.read(reinterpret_cast<char *>(&key_size), sizeof(key_size));

    std::string key(key_size, '\0');
    file.read(&key[0], key_size);

    file.read(reinterpret_cast<char *>(&value_size), sizeof(value_size));
    std::string value(value_size, '\0');
    file.read(&value[0], value_size);

    return new Node(key, value);
  }
};

// Struct to hold the result of operations
struct OperationResult {
  std::string message;
  bool wasSuccess;

  OperationResult(const std::string &msg = "", bool success = false)
      : message(msg), wasSuccess(success) {}
};

// Helper class to handle database operations
class dbModules {
public:
  // Insert a node into the binary search tree
  Node *insertNode(Node *head, const std::string &key,
                   const std::string &value) {
    if (!head)
      return new Node(key, value);
    else if (key < head->key)
      head->left = insertNode(head->left, key, value);
    else
      head->right = insertNode(head->right, key, value);
    return head;
  }

  // Find the in-order successor of a node
  Node *InOrderSuccessor(Node *head) {
    Node *current = head;
    while (current && current->left) {
      current = current->left;
    }
    return current;
  }

  // Delete a node from the binary search tree
  Node *deleteNode(Node *head, const std::string &key) {
    if (!head)
      return nullptr;

    if (key < head->key) {
      head->left = deleteNode(head->left, key);
    } else if (key > head->key) {
      head->right = deleteNode(head->right, key);
    } else {
      if (!head->left) {
        Node *child = head->right;
        delete head;
        return child;
      } else if (!head->right) {
        Node *child = head->left;
        delete head;
        return child;
      }

      Node *successor = InOrderSuccessor(head->right);
      head->key = successor->key;
      head->value = successor->value;
      head->right = deleteNode(head->right, successor->key);
    }
    return head;
  }

  // Save the tree data to a binary file (in-order traversal)
  void saveToFile(Node *node, std::ofstream &file) {
    if (!node)
      return;
    node->serialize(file); // Save node to the file
    saveToFile(node->left, file);
    saveToFile(node->right, file);
  }

  // Search for a node in the binary search tree
  Node *searchNode(Node *head, const std::string &key) {
    if (!head || head->key == key)
      return head;
    if (key < head->key)
      return searchNode(head->left, key);
    return searchNode(head->right, key);
  }
};

// KvStore class to manage the key-value store
class KvStore {
  Node *root;
  dbModules db;

  // Recursive function to delete the entire tree
  void deleteTree(Node *node) {
    if (!node)
      return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
  }

public:
  KvStore() : root(nullptr) {}

  ~KvStore() { deleteTree(root); }

  // Insert a key-value pair
  OperationResult insertKv(const std::string &key, const std::string &value) {
    root = db.insertNode(root, key, value);
    return {"Key-Value pair inserted successfully", true};
  }

  // Delete a key-value pair
  OperationResult deleteKv(const std::string &key) {
    if (!db.searchNode(root, key))
      return {"Key not found for deletion", false};
    root = db.deleteNode(root, key);
    return {"Key deleted successfully", true};
  }

  // Search for a key and return the value
  OperationResult searchKv(const std::string &key) {
    Node *result = db.searchNode(root, key);
    if (result) {
      return {"Value: " + result->value, true};
    }
    return {"Key not found", false};
  }

  // Update the value of an existing key
  OperationResult updateKv(const std::string &key, const std::string &value) {
    Node *node = db.searchNode(root, key);
    if (node) {
      node->value = value;
      return {"Key updated successfully", true};
    }
    return {"Key not found for update", false};
  }

  // Save the tree data to a binary file
  OperationResult saveData() {
    std::ofstream file("./data.bin", std::ios::binary);
    if (!file.is_open())
      return {"Error opening file for saving", false};
    db.saveToFile(root, file);
    file.close();
    return {"Data saved successfully", true};
  }

  // Load data from a binary file and insert into the tree
  OperationResult loadData(const std::string &filename = "./data.bin") {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
      return {"Error opening file for reading: " + filename, false};

    deleteTree(root);
    root = nullptr;

    // Read and construct the tree from the binary file
    while (file.peek() != EOF) {
      Node *node = Node::deserialize(file);
      root = db.insertNode(root, node->key, node->value);
    }
    file.close();
    return {"Data loaded successfully from " + filename, true};
  }

  // Retrieve all key-value pairs (in-order traversal)
  OperationResult getAll() {
    if (!root)
      return {"No data available", false};

    std::vector<std::pair<std::string, std::string>> allData;
    getAllHelper(root, allData);

    std::cout << "All Key-Value Pairs:" << std::endl;
    for (const auto &pair : allData) {
      std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return {"Data retrieved successfully", true};
  }

private:
  // Recursive helper for in-order traversal to collect all key-value pairs
  void getAllHelper(Node *node,
                    std::vector<std::pair<std::string, std::string>> &allData) {
    if (!node)
      return;
    getAllHelper(node->left, allData);
    allData.push_back({node->key, node->value});
    getAllHelper(node->right, allData);
  }
};

#endif // KVSTORE_H
