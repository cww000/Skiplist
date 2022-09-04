#include "node.h"
template <typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level)
{
    this->key = k;
    this->value = v;
    this->node_level = level;
    //此处使用new 分配内存后，需要对内存空间进行初始化，可以采用直接在new最后加上()来初始化，也可以使用memset。
    // new ()
    this->forward = new Node<K, V> *[level + 1]();
    // memset
    // memset(this->forward,0,sizeof(Node<K,V>)*(level+1));
}

template <typename K, typename V>
Node<K, V>::~Node()
{
    delete[] forward;
}

template <typename K, typename V>
K Node<K, V>::getKey() const
{
    return this->key;
}

template <typename K, typename V>
V Node<K, V>::getValue() const
{
    return this->value;
}

template <typename K, typename V>
void Node<K, V>::setValue(V v)
{
    this->value = v;
}
