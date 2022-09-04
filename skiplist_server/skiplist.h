#ifndef SKIPLIST_H
#define SKIPLIST_H
//#define RANDOM


#include<fstream>
#include <mutex>
#include <iostream>
#include<string>
#include "node.h"
#include "node.cpp"

// skiplist类

template <typename K, typename V>
class Skiplist {

public:
    Skiplist(int);
    Skiplist();
  //  Skiplist<K, V> &operator=(const Skiplist &);
  //  Skiplist(const Skiplist& sp);
    ~Skiplist();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    bool insert_element(const K, const V);
    void display_list();
    bool search_element(K, V&);
    bool delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const std::string& str, std::string& key, std::string& value);
    bool is_valid_string(const std::string& str);

private:
    // Maximum level of the skip list
    int _max_level;

    // current level of skip list
    int _skip_list_level;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;
    std::mutex mtx;

    // skiplist current element count
    int _element_count;

    // pointer to header node
    Node<K, V> *_header;
};


#endif
