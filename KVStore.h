#ifndef KVSTORE_H
#define KVSTORE_H

#include <fstream>
#include <iostream>
#include <optional>
#include <string>

// Node structure
struct Node {
  std::string key;
  std::string value;
  Node *left;
  Node *right;

  Node(const std::string &k, const std::string &v, Node *l_ptr = nullptr, Node *r_ptr = nullptr)
      : key(k), value(v), left(l_ptr), right(r_ptr) {}
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
  Node *insertNode(Node *head, const std::string &key, const std::string &value) {
    if (!head)
      return new Node(key, value);
    else if (key < head->key)
      head->left = insertNode(head->left, key, value);
    else
      head->right = insertNode(head->right, key, value);
    return head;
  }

  Node *InOrderSuccessor(Node *head) {
    Node *current = head;
    while (current && current->left) {
      current = current->left;
    }
    return current;
  }

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

  void saveToFile(Node *node, std::ofstream &file) {
    if (!node)
      return;
    file << node->key << " " << node->value << "\n";
    saveToFile(node->left, file);
    saveToFile(node->right, file);
  }

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

  void deleteTree(Node *node) {
    if (!node)
      return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
  }

  dbModules db;

public:
  KvStore() : root(nullptr) {}

  ~KvStore() { deleteTree(root); }

  OperationResult insertKv(const std::string &key, const std::string &value) {
    root = db.insertNode(root, key, value);
    return {"Key-Value pair inserted successfully", true};
  }

  OperationResult deleteKv(const std::string &key) {
    if (!db.searchNode(root, key))
      return {"Key not found for deletion", false};
    root = db.deleteNode(root, key);
    return {"Key deleted successfully", true};
  }

  OperationResult searchKv(const std::string &key) {
    Node *result = db.searchNode(root, key);
    if (result) {
      return {"Value: " + result->value, true};
    }
    return {"Key not found", false};
  }

  OperationResult updateKv(const std::string &key, const std::string &value) {
    Node *node = db.searchNode(root, key);
    if (node) {
      node->value = value;
      return {"Key updated successfully", true};
    }
    return {"Key not found for update", false};
  }

  OperationResult saveData() {
    std::ofstream file("./data.bat");
    if (!file.is_open())
      return {"Error opening file for saving", false};
    db.saveToFile(root, file);
    file.close();
    return {"Data saved successfully", true};
  }

  OperationResult loadData(const std::string &filename) {
    std::ifstream file(filename);
    if (!file)
      return {"Error opening file for reading: " + filename, false};

    deleteTree(root);
    root = nullptr;

    std::string key, value;
    while (file >> key >> value) {
      insertKv(key, value);
    }
    file.close();
    return {"Data loaded successfully from " + filename, true};
  }
};

#endif // KVSTORE_H

