#ifndef XMAP_H
#define XMAP_H

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;

#include "list/DLinkedList.h"
#include "hash/IMap.h"

template <class K, class V>
class xMap : public IMap<K, V> {
public:
    class Entry;

protected:
    DLinkedList<Entry *> *table;
    int capacity;
    int count;
    float loadFactor;

    int  (*hashCode)(K &, int);
    bool (*keyEqual)(K &, K &);
    bool (*valueEqual)(V &, V &);
    void (*deleteKeys)(xMap<K, V> *);
    void (*deleteValues)(xMap<K, V> *);

public:
    xMap(int  (*hashCode)(K &, int), float loadFactor = 0.75f,
         bool (*valueEqual)(V &, V &)            = 0,
         void (*deleteValues)(xMap<K, V> *)      = 0,
         bool (*keyEqual)(K &, K &)              = 0,
         void (*deleteKeys)(xMap<K, V> *)        = 0);
    xMap(const xMap<K, V> &map);
    xMap<K, V> &operator=(const xMap<K, V> &map);
    ~xMap();

    V   put(K key, V value);
    V  &get(K key);
    V   remove(K key, void (*deleteKeyInMap)(K) = 0);
    bool remove(K key, V value,
                void (*deleteKeyInMap)(K)   = 0,
                void (*deleteValueInMap)(V) = 0);
    bool containsKey(K key);
    bool containsValue(V value);
    bool empty();
    int  size();
    void clear();
    string toString(string (*key2str)(K &)   = 0,
                    string (*value2str)(V &) = 0);
    DLinkedList<K> keys();
    DLinkedList<V> values();
    DLinkedList<int> clashes();
    void println(string (*key2str)(K &) = 0,
                 string (*value2str)(V &) = 0) {
        cout << this->toString(key2str, value2str) << endl;
    }
    int  getCapacity() { return capacity; }

    static int simpleHash(K &key, int capacity)       { return key % capacity; }
    static int intKeyHash(int &key, int capacity)     { return key % capacity; }
    static int stringKeyHash(string &key, int capacity) {
        long long sum = 0;
        for (int pos = 0; pos < static_cast<int>(key.length()); ++pos)
            sum += key[pos];
        return static_cast<int>(sum % capacity);
    }
    static void freeKey(xMap<K, V> *pMap) {
        for (int i = 0; i < pMap->capacity; ++i) {
            DLinkedList<Entry *> &lst = pMap->table[i];
            for (int j = 0; j < lst.size(); ++j) {
                delete lst.get(j)->key;
            }
        }
    }
    static void freeValue(xMap<K, V> *pMap) {
        for (int i = 0; i < pMap->capacity; ++i) {
            DLinkedList<Entry *> &lst = pMap->table[i];
            for (int j = 0; j < lst.size(); ++j) {
                delete lst.get(j)->value;
            }
        }
    }
    static void deleteEntry(Entry *ptr) { delete ptr; }

protected:
    void ensureLoadFactor(int minCapacity);
    void rehash(int newCapacity);
    void removeInternalData();
    void copyMapFrom(const xMap<K, V> &map);
    void moveEntries(DLinkedList<Entry *> *oldTable, int oldCap,
                     DLinkedList<Entry *> *newTable, int newCap);
    bool keyEQ(K &a, K &b)    { return keyEqual   ? keyEqual(a, b)   : (a == b); }
    bool valueEQ(V &a, V &b)  { return valueEqual ? valueEqual(a, b) : (a == b); }

public:
    class Entry {
    private:
        K key;
        V value;
        friend class xMap<K, V>;
    public:
        Entry(K k, V v) : key(k), value(v) {}
    };
};

/* -------------------------------------------------------------------------- */
/*                             IMPLEMENTATIONS                                */
/* -------------------------------------------------------------------------- */

template <class K, class V>
xMap<K, V>::xMap(int (*hashCode)(K &, int), float loadFactor,
                 bool (*valueEqual)(V &, V &),
                 void (*deleteValues)(xMap<K, V> *),
                 bool (*keyEqual)(K &, K &),
                 void (*deleteKeys)(xMap<K, V> *))
    : table(new DLinkedList<Entry *>[10]),
      capacity(10),
      count(0),
      loadFactor(loadFactor),
      hashCode(hashCode),
      keyEqual(keyEqual),
      valueEqual(valueEqual),
      deleteKeys(deleteKeys),
      deleteValues(deleteValues) {}

template <class K, class V>
xMap<K, V>::xMap(const xMap<K, V> &map)
    : table(new DLinkedList<Entry *>[map.capacity]),
      capacity(map.capacity),
      count(0),
      loadFactor(map.loadFactor),
      hashCode(map.hashCode),
      keyEqual(map.keyEqual),
      valueEqual(map.valueEqual),
      deleteKeys(nullptr),
      deleteValues(nullptr) {

    for (int i = 0; i < map.capacity; ++i) {
        DLinkedList<Entry *> &lst = map.table[i];
        for (int j = 0; j < lst.size(); ++j) {
            put(lst.get(j)->key, lst.get(j)->value);
        }
    }
}

template <class K, class V>
xMap<K, V> &xMap<K, V>::operator=(const xMap<K, V> &map) {
    if (this != &map) {
        removeInternalData();

        capacity     = map.capacity;
        count        = 0;
        loadFactor   = map.loadFactor;
        hashCode     = map.hashCode;
        keyEqual     = map.keyEqual;
        valueEqual   = map.valueEqual;
        deleteKeys   = nullptr;
        deleteValues = nullptr;
        table        = new DLinkedList<Entry *>[capacity];

        for (int i = 0; i < map.capacity; ++i) {
            DLinkedList<Entry *> &lst = map.table[i];
            for (int j = 0; j < lst.size(); ++j) {
                put(lst.get(j)->key, lst.get(j)->value);
            }
        }
    }
    return *this;
}

template <class K, class V>
xMap<K, V>::~xMap() { removeInternalData(); }

template <class K, class V>
V xMap<K, V>::put(K key, V value) {
    int idx = hashCode(key, capacity);
    DLinkedList<Entry *> &bucket = table[idx];

    for (int k = 0; k < bucket.size(); ++k) {
        Entry *e = bucket.get(k);
        if (keyEQ(e->key, key)) {
            V old = e->value;
            e->value = value;
            return old;
        }
    }
    bucket.add(new Entry(key, value));
    ++count;
    ensureLoadFactor(count);
    return value;
}

template <class K, class V>
V &xMap<K, V>::get(K key) {
    int idx = hashCode(key, capacity);
    DLinkedList<Entry *> &bucket = table[idx];

    for (int k = 0; k < bucket.size(); ++k) {
        Entry *e = bucket.get(k);
        if (keyEQ(e->key, key)) return e->value;
    }
    stringstream os;
    os << "key (" << key << ") is not found";
    throw KeyNotFound(os.str());
}

template <class K, class V>
V xMap<K, V>::remove(K key, void (*deleteKeyInMap)(K)) {
    int idx = hashCode(key, capacity);
    DLinkedList<Entry *> &bucket = table[idx];

    for (int k = 0; k < bucket.size(); ++k) {
        Entry *e = bucket.get(k);
        if (keyEQ(e->key, key)) {
            V old = e->value;
            if (deleteKeyInMap) deleteKeyInMap(e->key);
            bucket.removeAt(k);
            delete e;
            --count;
            return old;
        }
    }
    stringstream os;
    os << "key (" << key << ") is not found";
    throw KeyNotFound(os.str());
}

template <class K, class V>
bool xMap<K, V>::remove(K key, V value,
                        void (*deleteKeyInMap)(K),
                        void (*deleteValueInMap)(V)) {
    int idx = hashCode(key, capacity);
    DLinkedList<Entry *> &bucket = table[idx];

    for (int k = 0; k < bucket.size(); ++k) {
        Entry *e = bucket.get(k);
        if (keyEQ(e->key, key) && valueEQ(e->value, value)) {
            if (deleteKeyInMap)   deleteKeyInMap(e->key);
            if (deleteValueInMap) deleteValueInMap(e->value);
            bucket.removeAt(k);
            delete e;
            --count;
            return true;
        }
    }
    return false;
}

template <class K, class V>
bool xMap<K, V>::containsKey(K key) {
    int idx = hashCode(key, capacity);
    DLinkedList<Entry *> &bucket = table[idx];

    for (int k = 0; k < bucket.size(); ++k)
        if (keyEQ(bucket.get(k)->key, key)) return true;

    return false;
}

template <class K, class V>
bool xMap<K, V>::containsValue(V value) {
    for (int i = 0; i < capacity; ++i) {
        DLinkedList<Entry *> &bucket = table[i];
        for (int k = 0; k < bucket.size(); ++k)
            if (valueEQ(bucket.get(k)->value, value)) return true;
    }
    return false;
}

template <class K, class V>
bool xMap<K, V>::empty() { return count == 0; }

template <class K, class V>
int xMap<K, V>::size() { return count; }

template <class K, class V>
void xMap<K, V>::clear() {
    removeInternalData();
    capacity = 10;
    count    = 0;
    table    = new DLinkedList<Entry *>[capacity];
}

template <class K, class V>
DLinkedList<K> xMap<K, V>::keys() {
    DLinkedList<K> out;
    for (int i = 0; i < capacity; ++i) {
        DLinkedList<Entry *> &bucket = table[i];
        for (int k = 0; k < bucket.size(); ++k)
            out.add(bucket.get(k)->key);
    }
    return out;
}

template <class K, class V>
DLinkedList<V> xMap<K, V>::values() {
    DLinkedList<V> out;
    for (int i = 0; i < capacity; ++i) {
        DLinkedList<Entry *> &bucket = table[i];
        for (int k = 0; k < bucket.size(); ++k)
            out.add(bucket.get(k)->value);
    }
    return out;
}

template <class K, class V>
DLinkedList<int> xMap<K, V>::clashes() {
    DLinkedList<int> out;
    for (int i = 0; i < capacity; ++i)
        out.add(table[i].size());
    return out;
}

template <class K, class V>
string xMap<K, V>::toString(string (*key2str)(K &),
                            string (*value2str)(V &)) {
    stringstream os;
    string mark(50, '=');
    os << mark << endl;
    os << setw(12) << left << "capacity: " << capacity << endl;
    os << setw(12) << left << "size: "      << count    << endl;

    for (int idx = 0; idx < capacity; ++idx) {
        os << setw(4) << left << idx << ": ";
        stringstream itemos;
        DLinkedList<Entry *> &bucket = table[idx];

        for (int k = 0; k < bucket.size(); ++k) {
            Entry *pEntry = bucket.get(k);
            string ks = key2str   ? key2str(pEntry->key)
                                  : (static_cast<ostringstream&>(ostringstream() << pEntry->key).str());
            string vs = value2str ? value2str(pEntry->value)
                                  : (static_cast<ostringstream&>(ostringstream() << pEntry->value).str());
            itemos << " (" << ks << "," << vs << ");";
        }
        string s = itemos.str();
        if (!s.empty()) s.pop_back();
        os << s << endl;
    }
    os << mark << endl;
    return os.str();
}

template <class K, class V>
void xMap<K, V>::moveEntries(DLinkedList<Entry *> *oldTable, int oldCap,
                             DLinkedList<Entry *> *newTable, int newCap) {
    for (int i = 0; i < oldCap; ++i) {
        DLinkedList<Entry *> &bucket = oldTable[i];
        for (int k = 0; k < bucket.size(); ++k) {
            Entry *e = bucket.get(k);
            newTable[hashCode(e->key, newCap)].add(e);
        }
    }
}

template <class K, class V>
void xMap<K, V>::ensureLoadFactor(int current_size) {
    if (current_size > static_cast<int>(loadFactor * capacity))
        rehash(static_cast<int>(capacity * 1.5f));
}

template <class K, class V>
void xMap<K, V>::rehash(int newCapacity) {
    DLinkedList<Entry *> *old = table;
    int oldCap = capacity;

    table    = new DLinkedList<Entry *>[newCapacity];
    capacity = newCapacity;
    moveEntries(old, oldCap, table, newCapacity);

    for (int i = 0; i < oldCap; ++i)
        old[i].clear();
    delete[] old;
}

template <class K, class V>
void xMap<K, V>::removeInternalData() {
    if (deleteKeys)   deleteKeys(this);
    if (deleteValues) deleteValues(this);

    for (int i = 0; i < capacity; ++i) {
        DLinkedList<Entry *> &bucket = table[i];
        for (int k = 0; k < bucket.size(); ++k)
            delete bucket.get(k);
        bucket.clear();
    }
    delete[] table;
}

template <class K, class V>
void xMap<K, V>::copyMapFrom(const xMap<K, V> &map) {
    removeInternalData();

    capacity     = map.capacity;
    count        = 0;
    loadFactor   = map.loadFactor;
    hashCode     = map.hashCode;
    keyEqual     = map.keyEqual;
    valueEqual   = map.valueEqual;
    table        = new DLinkedList<Entry *>[capacity];

    for (int i = 0; i < map.capacity; ++i) {
        DLinkedList<Entry *> &bucket = map.table[i];
        for (int k = 0; k < bucket.size(); ++k) {
            Entry *e = bucket.get(k);
            put(e->key, e->value);
        }
    }
}

#endif  // XMAP_H
