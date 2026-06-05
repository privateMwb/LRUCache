#pragma once 

template<typename K, typename V>
struct Node {
    K key;
    V value;
    
    Node* prev = nullptr;
    Node* next = nullptr;
    
    explicit Node(const K& key, const V& value) :
    key(key),
    value(value) {}
    
    Node() = default;
};