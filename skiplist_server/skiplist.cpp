#include "skiplist.h"
#include <string.h>
using namespace std;

//创造节点
// create new node
template<typename K, typename V>
Node<K, V>* Skiplist<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

template <typename K, typename V>
Skiplist<K, V>::Skiplist()
{
    this->_max_level = 32;
    this->_skip_list_level = 0;
    this->_element_count = 0;
    //创建头节点 并将K与V初始化为NULL
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
}

//template<typename K, typename V>
//Skiplist<K, V> &Skiplist<K, V>::operator=(const Skiplist &sp)
//{
//    if(this != &sp){
//        _max_level = sp._max_level;
//        _skip_list_level = sp._skip_list_level;  //普通成员赋值
//        _element_count = sp._element_count;

//        delete _header;   //先销毁左值资源对象---擦除操作
//        _header = new Node<K, V>{*(sp._header)};//复制资源
//    }else {
//        cerr << "   **owner copy assignment.##\n" << endl;
//    }

//    return *this;
//}

//template<typename K, typename V>
//Skiplist<K, V>::Skiplist(const Skiplist &sp)
//    : _max_level(sp._max_level), _skip_list_level(sp._skip_list_level),     _element_count(sp._element_count), _header{new Node<K, V>{*(sp._header)}}
//{

//}



//建造跳表
template <typename K, typename V>
Skiplist<K, V>::Skiplist(int max_level)
{
    _max_level = max_level;
    _skip_list_level = 0;
    _element_count = 0;

    //创建头节点 并将K与V初始化为NULL
    K k;
    V v;
    _header = new Node<K, V>(k, v, _max_level);
}

//析构跳表
template <typename K, typename V>
Skiplist<K, V>::~Skiplist()
{
    if(_file_reader.is_open()){
        _file_reader.close();
    }
    if(_file_writer.is_open()){
        _file_writer.close();
    }
    delete _header;
}

// Insert given key and value in skip list  返回值为int 为后续拓展留下空间
// return 0 means element exists  insert failed
// return 1 means insert successfully

/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/

template <typename K, typename V>
bool Skiplist<K, V>::insert_element(const K key,const V value)
{
 //   lock_guard<mutex> lock(mtx);
    Node<K, V> *current = this->_header;

    // create update array and initialize it
    // update is array which put node that the node->forward[i] should be operated later
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    // start form highest level of skip list
    for(int i = _skip_list_level; i >= 0; i--) {
        while(current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right node, which is desired to insert key.
    current = current->forward[0];

    // if current node have key equal to searched key, we get it
    if (current != nullptr && current->getKey() == key) {
        std::cout << "key:" << key << " has existed, couldn't insert..." << std::endl;
        return 0;
    }

    // if current is NULL that means we have reached to end of the level
    // if current's key is not equal to key that means we have to insert node between update[0] and current node
    if (current == nullptr || current->getKey() != key ) {

        // Generate a random level for node
        int random_level = get_random_level();

        // If random level is greater thar skip list's current level, initialize update value with pointer to header
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // create new node with random level generated
        Node<K, V>* inserted_node = create_node(key, value, random_level);

        // insert node
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count ++;
    }
    //delete[]update;
    return 1;
}

// Search for element in skip list
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template <typename K, typename V>
bool Skiplist<K, V>::search_element(K key,V &value)
{

    //cout << "search_element-------------" << endl;
    Node<K, V> *current = _header;

    //从跳表的最左上角节点开始寻找
    for (int i = _skip_list_level; i >= 0; i--)
    {
        while (current->forward[i] && current->forward[i]->getKey() < key)
        {
            current = current->forward[i];
        }
    }

    //到达第0层
    current = current->forward[0];

    if (current!=nullptr&&current->getKey() == key)
    {
        value=current->getValue();
       // cout << "Found key: " << key << " value: " << current->getValue() << endl;
        return true;
    }
    //cout << "No Found Key: " << key << endl;
    return false;
}

template <typename K, typename V>
bool Skiplist<K, V>::delete_element(K key)
{
  //  lock_guard<mutex> lock(mtx);
    //Node<K, V> *update [_max_level+ 1];
    Node<K, V> *update = new Node<K,V>[_max_level + 1];
    //memset(update, 0 , sizeof(Node<K, V>*) * (_skip_list_level + 1));

    Node<K, V> *current = _header;

    for (int i = _skip_list_level; i >= 0; i--)
    {
        while (current->forward[i] != nullptr && current->forward[i]->getKey() < key)
        {
            current = current->forward[i];
        }
        update[i] = *current;
    }

    current = current->forward[0];

    if (current != nullptr && current->getKey() == key)
    {
        for (int i = 0; i <=_skip_list_level; i++)
        {
            if (update[i].forward[i] != current)
            {
                break;
            }
            update[i].forward[i] = current->forward[i];
        }
        delete current;
        //delete []update;
        //cout << "Successfully deleted key: " << key << endl;
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == nullptr)
        {
            _skip_list_level--;
        }
        _element_count--;
        return 1;
    }
    return 0;
}

template <typename K, typename V>
void Skiplist<K, V>::display_list()
{
    cout<<"SkipList:"<<endl;

    Node<K,V> *current;

    for(int i=_skip_list_level;i>=0;i--){
        current=_header->forward[i];
        cout<< "Level " << i <<':' << endl;
        while(current != nullptr){
            cout<< ' ' <<current->getKey()<< ':' << current->getValue();
            current = current->forward[i];
        }
        cout<<endl;
    }
    return;
}

template<typename K, typename V>
int Skiplist<K, V>::get_random_level(){

    // 该 randomLevel 方法会随机生成 1~ _max_level 之间的数，且 ：
    //        1/2 的概率返回 1
    //        1/4 的概率返回 2
    //        1/8 的概率返回 3 以此类推
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};

template<typename K,typename V>
int Skiplist<K,V>::size(){
    return this->_element_count;
}

template<typename K,typename V>
bool Skiplist<K,V>::is_valid_string(const string& str){
    if(str.empty()){
        return false;
    }
    string delimiter =":";
    if(str.find(delimiter)==string::npos){
       return false;
    }
    return true;
}

template<typename K,typename V>
void Skiplist<K,V>::get_key_value_from_string(const string& str, string& key, string& value){
    if(!is_valid_string(str)){
        return;
    }
    string delimiter =":";
    key=str.substr(0,str.find(delimiter));
    value=str.substr(str.find(delimiter)+1,str.length());
}

template<typename K,typename V>
void Skiplist<K,V>::dump_file(string path){
    //cout<<"dump_file-------------"<<endl;
    _file_writer.open(path);
    Node<K,V> *node=this->_header->forward[0];

    while(node != nullptr){
        _file_writer<<node->getKey()<<":"<<node->getValue()<<"\n";
        //cout<<node->getKey()<<":"<<node->getValue()<<";\n";
        node=node->forward[0];
    }
    _file_writer.flush();
    _file_writer.close();
    return;
}

template<typename K,typename V>
void Skiplist<K,V>::load_file(string path){
   // lock_guard<mutex> lock(mtx);
    //cout<<"load_file-------------"<<endl;
    _file_reader.open(path);
    string line;
    string key, value;
    while(getline(_file_reader,line)){
        get_key_value_from_string(line, key,value);  //分割 ：左边和右边的 key 和 value
        if(key.empty()||value.empty()){
            continue;
        }
        insert_element(key, value);
    }

    _file_reader.close();
}
