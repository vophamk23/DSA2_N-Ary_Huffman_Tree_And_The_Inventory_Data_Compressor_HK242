/*
Author: NTM - 5/9/2025
All in one
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "heap/Heap.h"
#include "hash/xMap.h"
#include "app/inventory_compressor.h"

using namespace std;

string output = "";
string expect = "";
streambuf *original_cout = std::cout.rdbuf();

// Object ================================================

class Object
{
    int key;
    string val;

public:
    static int count;
    Object() : key(-1), val("") { ++count; }
    Object(int key, string val = "") : key(key), val(val) { ++count; }
    Object(string val) : key(-1), val(val) { ++count; }
    Object(const Object &obj) : key(obj.key), val(obj.val) { ++count; }
    ~Object() { --count; }
    static string ObjectToString(Object obj) { return obj.val; }
    static string ObjectToString(Object *obj) { return obj->val; }
    static bool ObjectEqual(Object &obj1, Object &obj2) { return obj1.val == obj2.val; }
    static bool ObjectEqual(Object *&obj1, Object *&obj2) { return obj1->val == obj2->val; }
    friend ostream &operator<<(ostream &ss, Object &obj);
    friend ostream &operator<<(ostream &ss, Object *obj);
    friend int ObjectHash(Object &obj, int capacity);
    friend int ObjectHash(Object *&obj, int capacity);
    friend int ObjectCompare(Object &obj1, Object &obj2);
    friend int ObjectCompare(Object *&obj1, Object *&obj2);
    bool operator==(Object &obj) { return ObjectEqual(*this, obj); }
    Object &operator=(const Object &obj)
    {
        key = obj.key, val = obj.val;
        return *this;
    }
    bool operator<(Object &obj) { return key < obj.key; }
    bool operator>(Object &obj) { return key > obj.key; }
};

int Object::count = 0;

ostream &operator<<(ostream &ss, Object &obj)
{
    ss << Object::ObjectToString(obj);
    return ss;
}

ostream &operator<<(ostream &ss, Object *obj)
{
    ss << Object::ObjectToString(obj);
    return ss;
}

int ObjectHash(Object &obj, int capacity)
{
    if (obj.key == -1)
        return xMap<int, int>::stringKeyHash(obj.val, capacity);
    else
        return xMap<int, int>::simpleHash(obj.key, capacity);
}

int ObjectHash(Object *&obj, int capacity)
{
    if (obj->key == -1)
        return xMap<int, int>::stringKeyHash(obj->val, capacity);
    else
        return xMap<int, int>::simpleHash(obj->key, capacity);
}

int ObjectCompare(Object &obj1, Object &obj2)
{
    return obj1.key - obj2.key;
}

int ObjectCompare(Object *&obj1, Object *&obj2)
{
    return obj1->key - obj2->key;
}

// Hash ================================================

void hash001()
{
    expect = "==================================================\ncapacity:   10\nsize:       0\n0   : \n1   : \n2   : \n3   : \n4   : \n5   : \n6   : \n7   : \n8   : \n9   : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.println();
}

void hash002()
{
    expect = "1";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    try
    {
        cout << map.get(1);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash003()
{
    expect = "11";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 11);
    try
    {
        cout << map.get(1);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash004()
{
    expect = "111";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(11, 111);
    try
    {
        cout << map.get(11);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash005()
{
    expect = "111";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(11, 111);
    try
    {
        cout << map.get(11);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash006()
{
    // add it to the back
    expect = "==================================================\ncapacity:   10\nsize:       3\n0   : \n1   :  (1,1); (11,1); (21,1)\n2   : \n3   : \n4   : \n5   : \n6   : \n7   : \n8   : \n9   : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(11, 1);
    map.put(21, 1);
    map.println();
}

void hash007()
{
    expect = "key (11) is not found\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 11);
    try
    {
        cout << map.get(11);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash008()
{
    expect = "12";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 11);
    map.put(1, 12);
    try
    {
        cout << map.remove(1);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash009()
{
    expect = "key (12) is not found\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    try
    {
        cout << map.remove(12);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash010()
{
    expect = "2";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    try
    {
        map.remove(1);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
    cout << map.size();
}

void hash011()
{
    expect = "1";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    cout << map.containsKey(2);
}

void hash012()
{
    expect = "0";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    cout << map.containsKey(12);
}

void hash013()
{
    expect = "1";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    cout << map.containsValue(12);
}

void hash014()
{
    expect = "0";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    cout << map.containsValue(0);
}

void hash015()
{
    expect = "0";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    try
    {
        map.remove(1);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
    cout << map.empty();
}

void hash016()
{
    expect = "1";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    try
    {
        map.remove(1);
        map.remove(2);
        map.remove(3);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
    cout << map.empty();
}

void hash017()
{
    // capacity is 10 after clearing
    expect = "==================================================\ncapacity:   10\nsize:       0\n0   : \n1   : \n2   : \n3   : \n4   : \n5   : \n6   : \n7   : \n8   : \n9   : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(0, 1);
    map.put(30, 1);
    map.put(60, 1);
    map.put(90, 1);
    map.put(120, 1);
    map.put(150, 1);
    map.put(180, 1);
    map.put(210, 1);
    map.put(240, 1);
    map.clear();
    map.println();
}

void hash018()
{
    expect = "==================================================\ncapacity:   10\nsize:       3\n0   : \n1   :  (1,1); (11,1); (21,1)\n2   : \n3   : \n4   : \n5   : \n6   : \n7   : \n8   : \n9   : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    map.clear();
    map.put(1, 1);
    map.put(11, 1);
    map.put(21, 1);
    map.println();
}

void hash019()
{
    expect = "[1, 2, 3]\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    auto keys = map.keys();
    keys.println();
}

void hash020()
{
    expect = "[12, 12, 120]\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(3, 120);
    auto values = map.values();
    values.println();
}

void hash021()
{
    expect = "[0, 1, 1, 3, 0, 0, 0, 0, 0, 0]\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(1, 12);
    map.put(2, 1);
    map.put(2, 12);
    map.put(3, 10);
    map.put(13, 10);
    map.put(23, 10);
    map.put(3, 120);
    auto clashes = map.clashes();
    clashes.println();
}

void hash022()
{
    // rehash's work
    expect = "==================================================\ncapacity:   15\nsize:       10\n0   :  (0,1); (30,1); (60,1); (90,1); (120,1); (150,1); (180,1); (210,1); (240,1)\n1   : \n2   : \n3   :  (3,1)\n4   : \n5   : \n6   : \n7   : \n8   : \n9   : \n10  : \n11  : \n12  : \n13  : \n14  : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(0, 1);
    map.put(30, 1);
    map.put(60, 1);
    map.put(90, 1);
    map.put(120, 1);
    map.put(150, 1);
    map.put(180, 1);
    map.put(210, 1);
    map.put(240, 1);
    map.put(3, 1);
    map.println();
}

void hash023()
{
    // ensure that the load factor is assigned
    expect = "==================================================\ncapacity:   15\nsize:       6\n0   :  (0,1); (30,1); (60,1); (90,1); (120,1)\n1   : \n2   : \n3   :  (3,1)\n4   : \n5   : \n6   : \n7   : \n8   : \n9   : \n10  : \n11  : \n12  : \n13  : \n14  : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash, 0.55f);
    map.put(0, 1);
    map.put(30, 1);
    map.put(60, 1);
    map.put(90, 1);
    map.put(120, 1);
    map.put(3, 1);
    map.println();
}

void hash024()
{
    expect = "==================================================\ncapacity:   10\nsize:       3\n0   : \n1   :  (1,1); (11,1)\n2   : \n3   : \n4   : \n5   :  (5,1)\n6   : \n7   : \n8   : \n9   : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(11, 1);
    map.put(5, 1);
    auto map2 = map;
    map2.println();
}

void hash025()
{
    expect = "==================================================\ncapacity:   10\nsize:       3\n0   : \n1   :  (1,1); (11,1)\n2   : \n3   : \n4   : \n5   :  (5,1)\n6   : \n7   : \n8   : \n9   : \n==================================================\n\n";
    xMap<int, int> map(xMap<int, int>::simpleHash);
    map.put(1, 1);
    map.put(11, 1);
    map.put(5, 1);
    xMap<int, int> map2(xMap<int, int>::simpleHash);
    map2 = map;
    map2.println();
}

void hash026()
{
    expect = "345";
    xMap<Object *, int> map(ObjectHash, 0.75f, 0, 0, Object::ObjectEqual, xMap<Object *, int>::freeKey);
    map.put(new Object("ABC"), 123);
    map.put(new Object("abc"), 345);
    try
    {
        Object get("abc");
        cout << map.get(&get);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash027()
{
    expect = "3";
    xMap<Object *, int> map(ObjectHash, 0.75f, 0, 0, Object::ObjectEqual, xMap<Object *, int>::freeKey);
    map.put(new Object("ABC"), 123);
    map.put(new Object("abc"), 345);
    Object same("abc");
    map.put(&same, 654);
    map.put(new Object("654"), 786);
    cout << map.size();
}

void hash028()
{
    expect = "345\n345";
    xMap<Object *, int> map(ObjectHash, 0.75f, 0, 0, Object::ObjectEqual, xMap<Object *, int>::freeKey);
    map.put(new Object("ABC"), 123);
    cout << map.put(new Object("abc"), 345) << endl;
    Object same("abc");
    cout << map.put(&same, 654);
}

void hash029()
{
    expect = "1";
    xMap<Object *, int> map(ObjectHash, 0.75f, 0, 0, Object::ObjectEqual, xMap<Object *, int>::freeKey);
    map.put(new Object("ABC"), 123);
    map.put(new Object("abc"), 345);
    try
    {
        Object remove("abc");
        map.remove(&remove);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
    cout << map.size();
}

void hash030()
{
    expect = "1";
    xMap<Object *, int> map(ObjectHash, 0.75f, 0, 0, Object::ObjectEqual, xMap<Object *, int>::freeKey);
    map.put(new Object("ABC"), 123);
    map.put(new Object("abc"), 345);
    try
    {
        Object key("abc");
        cout << map.containsKey(&key);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash031()
{
    expect = "1";
    xMap<int, Object *> map(xMap<int, Object *>::simpleHash, 0.75f, Object::ObjectEqual, xMap<int, Object *>::freeValue, 0, 0);
    map.put(123, new Object("ABC"));
    map.put(345, new Object("abc"));
    try
    {
        Object value("abc");
        cout << map.containsValue(&value);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash032()
{
    // Wow Dlinkedlist<K>(0, keyEqual)
    expect = "1";
    xMap<Object *, int> map(ObjectHash, 0.75f, 0, 0, Object::ObjectEqual, 0);
    Object obj[] = {
        Object(0, "pass"),
        Object(1, "keyEqual"),
        Object(2, "to"),
        Object(3, "DlinkedList"),
        Object(4, "constructor"),
        Object(5, "."),
    };
    map.put(&obj[0], 1);
    map.put(&obj[1], 1);
    map.put(&obj[2], 1);
    map.put(&obj[3], 1);
    map.put(&obj[4], 1);
    map.put(&obj[5], 1);
    try
    {
        Object key("keyEqual");
        auto keys = map.keys();
        cout << keys.contains(&key);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash033()
{
    // Dlinkedlist<V>(0, valueEqual)
    expect = "1";
    xMap<int, Object *> map(xMap<int, Object *>::simpleHash, 0.75f, Object::ObjectEqual, 0, 0, 0);
    Object obj[] = {
        Object(0, "pass"),
        Object(1, "valueEqual"),
        Object(2, "to"),
        Object(3, "DlinkedList"),
        Object(4, "constructor"),
        Object(5, "."),
    };
    map.put(1, &obj[0]);
    map.put(2, &obj[1]);
    map.put(3, &obj[2]);
    map.put(4, &obj[3]);
    map.put(5, &obj[4]);
    map.put(6, &obj[5]);
    try
    {
        Object value("valueEqual");
        auto values = map.values();
        cout << values.contains(&value);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash034()
{
    // hashCode = map.hashCode
    // valueEqual = map.valueEqual
    // keyEqual = map.keyEqual
    expect = "11";
    xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
    map.put(new Object("a"), new Object("f"));
    map.put(new Object("b"), new Object("e"));
    map.put(new Object("c"), new Object("d"));
    map.put(new Object("d"), new Object("c"));
    map.put(new Object("e"), new Object("b"));
    auto map1 = map;
    map1.put(new Object("f"), new Object("a"));
    try
    {
        Object contains("f");
        cout << map1.containsKey(&contains);
        cout << map1.containsValue(&contains);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash035()
{
    // hashCode = map.hashCode
    // valueEqual = map.valueEqual
    // keyEqual = map.keyEqual
    expect = "11";
    xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
    map.put(new Object("a"), new Object("f"));
    map.put(new Object("b"), new Object("e"));
    map.put(new Object("c"), new Object("d"));
    map.put(new Object("d"), new Object("c"));
    map.put(new Object("e"), new Object("b"));
    xMap<Object *, Object *> map2(ObjectHash);
    map2 = map;
    map2.put(new Object("f"), new Object("a"));
    try
    {
        Object contains("f");
        cout << map2.containsKey(&contains);
        cout << map2.containsValue(&contains);
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash036()
{
    // loadFactor = map.loadFactor
    expect = "[0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0]";
    xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
    map.put(new Object("a"), new Object("f"));
    map.put(new Object("b"), new Object("e"));
    map.put(new Object("c"), new Object("d"));
    map.put(new Object("d"), new Object("c"));
    map.put(new Object("e"), new Object("b"));
    auto map1 = map;
    map.put(new Object("g"), new Object("a"));
    map1.put(new Object("f"), new Object("a"));
    cout << map1.clashes().toString();
}

void hash037()
{
    // loadFactor = map.loadFactor
    expect = "[0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0]";
    xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
    map.put(new Object("a"), new Object("f"));
    map.put(new Object("b"), new Object("e"));
    map.put(new Object("c"), new Object("d"));
    map.put(new Object("d"), new Object("c"));
    map.put(new Object("e"), new Object("b"));
    xMap<Object *, Object *> map2(ObjectHash);
    map2 = map;
    map.put(new Object("g"), new Object("a"));
    map2.put(new Object("f"), new Object("a"));
    cout << map2.clashes().toString();
}

void hash038()
{
    expect = "Leak: 0";
    {
        xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, 0, Object::ObjectEqual, 0);
        map.put(new Object("a"), new Object("f"));
        map.put(new Object("b"), new Object("e"));
        map.put(new Object("c"), new Object("d"));
        map.put(new Object("d"), new Object("c"));
        map.put(new Object("e"), new Object("b"));
        xMap<Object *, Object *> map2(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
        map2.put(new Object("a"), new Object("f"));
        map2.put(new Object("b"), new Object("e"));
        map2.put(new Object("c"), new Object("d"));
        map2.put(new Object("d"), new Object("c"));
        map2.put(new Object("e"), new Object("b"));
        map2 = map;
    }
    cout << "Leak: " << Object::count;
}

void hash039()
{
    expect = "Leak: 0";
    {
        xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
        map.put(new Object("a"), new Object("f"));
        map.put(new Object("b"), new Object("e"));
        map.put(new Object("c"), new Object("d"));
        map.put(new Object("d"), new Object("c"));
        map.put(new Object("e"), new Object("b"));
        Object remove("c");
        try
        {
            delete map.remove(&remove, [](Object *obj)
                              { delete obj; });
        }
        catch (KeyNotFound &e)
        {
            cout << e.what() << endl;
        }
    }
    cout << "Leak: " << Object::count;
}

void hash040()
{
    // remember to delete entry
    expect = "Leak: 0";
    {
        xMap<Object, Object> map(ObjectHash, 0.55f, Object::ObjectEqual, 0, Object::ObjectEqual, 0);
        map.put(Object("a"), Object("f"));
        map.put(Object("b"), Object("e"));
        map.put(Object("c"), Object("d"));
        map.put(Object("d"), Object("c"));
        map.put(Object("e"), Object("b"));
        Object remove("c");
        try
        {
            map.remove(remove);
        }
        catch (KeyNotFound &e)
        {
            cout << e.what() << endl;
        }
    }
    cout << "Leak: " << Object::count;
}

void hash041()
{
    expect = "1\n4";
    xMap<Object, Object> map(ObjectHash, 0.55f, Object::ObjectEqual, 0, Object::ObjectEqual, 0);
    map.put(Object("a"), Object("f"));
    map.put(Object("b"), Object("e"));
    map.put(Object("c"), Object("d"));
    map.put(Object("d"), Object("c"));
    map.put(Object("e"), Object("b"));
    Object removeKey("c");
    Object removeValue("d");
    try
    {
        cout << map.remove(removeKey, removeValue) << endl;
        cout << map.size();
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash042()
{
    expect = "0\n5";
    xMap<Object, Object> map(ObjectHash, 0.55f, Object::ObjectEqual, 0, Object::ObjectEqual, 0);
    map.put(Object("a"), Object("f"));
    map.put(Object("b"), Object("e"));
    map.put(Object("c"), Object("d"));
    map.put(Object("d"), Object("c"));
    map.put(Object("e"), Object("b"));
    Object removeKey("c");
    Object removeValue("c");
    try
    {
        cout << map.remove(removeKey, removeValue) << endl;
        cout << map.size();
    }
    catch (KeyNotFound &e)
    {
        cout << e.what() << endl;
    }
}

void hash043()
{
    // delete entry
    expect = "Leak: 0";
    {
        xMap<Object, Object> map(ObjectHash, 0.55f, Object::ObjectEqual, 0, Object::ObjectEqual, 0);
        map.put(Object("a"), Object("f"));
        map.put(Object("b"), Object("e"));
        map.put(Object("c"), Object("d"));
        map.put(Object("d"), Object("c"));
        map.put(Object("e"), Object("b"));
        Object removeKey("c");
        Object removeValue("d");
        try
        {
            map.remove(removeKey, removeValue);
        }
        catch (KeyNotFound &e)
        {
            cout << e.what() << endl;
        }
    }
    cout << "Leak: " << Object::count;
}

void hash044()
{
    // delete entry
    expect = "Leak: 0";
    {
        xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
        map.put(new Object("a"), new Object("f"));
        map.put(new Object("b"), new Object("e"));
        map.put(new Object("c"), new Object("d"));
        map.put(new Object("d"), new Object("c"));
        map.put(new Object("e"), new Object("b"));
        Object removeKey("c");
        Object removeValue("d");
        try
        {
            map.remove(&removeKey, &removeValue, [](Object *obj)
                       { delete obj; }, [](Object *obj)
                       { delete obj; });
        }
        catch (KeyNotFound &e)
        {
            cout << e.what() << endl;
        }
    }
    cout << "Leak: " << Object::count;
}

void hash045()
{
    // delete entry
    expect = "Leak: 0";
    {
        xMap<Object *, Object *> map(ObjectHash, 0.55f, Object::ObjectEqual, xMap<Object *, Object *>::freeValue, Object::ObjectEqual, xMap<Object *, Object *>::freeKey);
        map.put(new Object("a"), new Object("f"));
        map.put(new Object("b"), new Object("e"));
        map.put(new Object("c"), new Object("d"));
        map.put(new Object("d"), new Object("c"));
        map.put(new Object("e"), new Object("b"));
        map.clear();
        map.put(new Object("a"), new Object("f"));
        map.put(new Object("b"), new Object("e"));
    }
    cout << "Leak: " << Object::count;
}

// Heap ================================================

void heap046()
{
    expect = "[]\n";
    Heap<int> heap;
    heap.println();
}

void heap047()
{
    expect = "[1,2,3]\n";
    Heap<int> heap;
    heap.push(1);
    heap.push(2);
    heap.push(3);
    heap.println();
}

void heap048()
{
    expect = "[1,2,4,3]\n";
    Heap<int> heap;
    heap.push(1);
    heap.push(3);
    heap.push(4);
    heap.push(2);
    heap.println();
}

void heap049()
{
    expect = "[6,10,7,23,15,20,8,50,100,1000]\n";
    Heap<int> heap;
    heap.push(50);
    heap.push(20);
    heap.push(15);
    heap.push(10);
    heap.push(8);
    heap.push(6);
    heap.push(7);
    heap.push(23);
    heap.push(100);
    heap.push(1000);
    heap.println();
}

void heap050()
{
    expect = "[1,2,7,10,6,20,8,50,23,15]\n";
    Heap<int> heap([](int &a, int &b)
                   { return a - b; });
    heap.push(50);
    heap.push(20);
    heap.push(15);
    heap.push(10);
    heap.push(8);
    heap.push(6);
    heap.push(7);
    heap.push(23);
    heap.push(2);
    heap.push(1);
    heap.println();
}

void heap051()
{
    expect = "[50,23,15,20,8,6,7,10,2,1]\n";
    Heap<int> heap([](int &a, int &b)
                   { return b - a; });
    heap.push(50);
    heap.push(20);
    heap.push(15);
    heap.push(10);
    heap.push(8);
    heap.push(6);
    heap.push(7);
    heap.push(23);
    heap.push(2);
    heap.push(1);
    heap.println();
}

void heap052()
{
    expect = "34";
    Heap<int> heap;
    heap.push(50);
    heap.push(34);
    heap.push(78);
    heap.push(13);
    heap.push(56);
    heap.push(67);
    try
    {
        heap.pop();
        cout << heap.pop();
    }
    catch (underflow_error &e)
    {
        cout << e.what();
    }
}

void heap053()
{
    expect = "13";
    Heap<int> heap;
    heap.push(50);
    heap.push(34);
    heap.push(78);
    heap.push(13);
    heap.push(56);
    heap.push(67);
    try
    {
        heap.peek();
        cout << heap.peek();
    }
    catch (underflow_error &e)
    {
        cout << e.what();
    }
}

void heap054()
{
    expect = "Calling to peek with the empty heap.";
    Heap<int> heap;
    heap.push(1);
    try
    {
        heap.pop();
        heap.peek();
    }
    catch (underflow_error &e)
    {
        cout << e.what();
    }
}

void heap055()
{
    expect = "Calling to peek with the empty heap.";
    Heap<int> heap;
    heap.push(1);
    try
    {
        heap.pop();
        heap.pop();
    }
    catch (underflow_error &e)
    {
        cout << e.what();
    }
}

void heap056()
{
    expect = "[1,2,3,4,5,6]\n";
    Heap<int> heap;
    heap.push(1);
    heap.push(2);
    heap.push(3);
    heap.push(4);
    heap.push(5);
    heap.push(6);
    heap.remove(0);
    heap.println();
}

void heap057()
{
    // remember to call reheapUp
    expect = "[1,5,2,8,6,4]\n";
    Heap<int> heap;
    heap.push(1);
    heap.push(6);
    heap.push(2);
    heap.push(8);
    heap.push(9);
    heap.push(4);
    heap.push(5);
    heap.remove(9);
    heap.println();
}

void heap058()
{
    expect = "1";
    Heap<int> heap;
    heap.push(50);
    heap.push(34);
    heap.push(78);
    heap.push(13);
    heap.push(56);
    heap.push(67);
    cout << heap.contains(56);
}

void heap059()
{
    expect = "0";
    Heap<int> heap;
    heap.push(50);
    heap.push(34);
    heap.push(78);
    heap.push(13);
    heap.push(56);
    heap.push(67);
    cout << heap.contains(55);
}

void heap060()
{
    expect = "[6,8,7,10,20,15,50,23]\n";
    Heap<int> heap;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23};
    heap.heapify(arr, sizeof(arr) / sizeof(arr[0]));
    heap.println();
}

void heap061()
{
    expect = "[6,8,7,10,20,15,50,23,10]\n";
    Heap<int> heap;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23};
    heap.heapify(arr, sizeof(arr) / sizeof(arr[0]));
    heap.push(10);
    heap.println();
}

void heap062()
{
    // remember to clear
    expect = "[6,8,7,10,20,15,50,23]\n";
    Heap<int> heap;
    int arr1[] = {50, 20, 15, 10, 8, 6, 7, 23, 10};
    int arr2[] = {50, 20, 15, 10, 8, 6, 7, 23};
    heap.heapify(arr1, sizeof(arr1) / sizeof(arr1[0]));
    heap.heapify(arr2, sizeof(arr2) / sizeof(arr2[0]));
    heap.println();
}

void heap063()
{
    expect = "[1,2,3]\n";
    Heap<int> heap;
    heap.push(1);
    heap.push(2);
    heap.push(3);
    heap.push(4);
    heap.push(5);
    heap.push(6);
    heap.clear();
    heap.push(1);
    heap.push(2);
    heap.push(3);
    heap.println();
}

void heap064()
{
    expect = "1\n0";
    Heap<int> heap;
    cout << heap.empty() << endl;
    heap.push(1);
    heap.push(2);
    heap.push(3);
    heap.push(4);
    heap.push(5);
    heap.push(6);
    cout << heap.empty();
}

void heap065()
{
    expect = "5";
    Heap<int> heap;
    heap.push(1);
    heap.push(2);
    heap.push(3);
    heap.push(4);
    heap.push(5);
    heap.push(6);
    try
    {
        heap.remove(1);
    }
    catch (underflow_error &e)
    {
        cout << e.what();
    }
    cout << heap.size();
}

void heap066()
{
    expect = "[1, 5, 3, 7, 10]\n[1, 3, 3, 7, 10]\n[1, 3, 5, 7, 10]\n[1, 3, 5, 7, 10]\n[1, 3, 5, 7, 10]\n[1, 3, 5, 7, 10]\n";
    Heap<int> heap;
    XArrayList<int> arr;
    arr.add(1);
    arr.add(5);
    arr.add(3);
    arr.add(7);
    arr.add(10);
    heap.heapsort(arr);
    arr.println();
}

void heap067()
{
    expect = "[1, 5, 3, 7, 10]\n[1, 3, 3, 7, 10]\n[1, 3, 5, 7, 10]\n[1, 3, 5, 7, 10]\n[1, 3, 5, 7, 10]\n[1, 3, 5, 7, 10]\n";
    Heap<int> heap;
    heap.push(2);
    heap.push(4); // erase them
    XArrayList<int> arr;
    arr.add(1);
    arr.add(5);
    arr.add(3);
    arr.add(7);
    arr.add(10);
    heap.heapsort(arr);
    arr.println();
}

void heap068()
{
    expect = "[1,2,6,4,5]\nLeak: 0";
    {
        Heap<Object *> heap(ObjectCompare, Heap<Object *>::free);
        heap.push(new Object(1, "1"));
        heap.push(new Object(3, "2"));
        heap.push(new Object(6, "3"));
        heap.push(new Object(6, "4")); // DFS will find it first, but that's wrong
        heap.push(new Object(4, "5"));
        heap.push(new Object(7, "6"));
        try
        {
            Object remove(6, ""); // just compare by key
            heap.remove(&remove, [](Object *obj)
                        { delete obj; }); // delete the object stored in heap, don't delete passed-in pointer
        }
        catch (underflow_error &e)
        {
            cout << e.what();
        }
        heap.println();
    }
    cout << "Leak: " << Object::count;
}

void heap069()
{
    expect = "Leak: 0";
    {
        Heap<Object *> heap(ObjectCompare, Heap<Object *>::free);
        heap.push(new Object(1, "123"));
        heap.push(new Object(2, "234"));
        heap.push(new Object(3, "345"));
        heap.push(new Object(4, "456"));
        heap.push(new Object(5, "567"));
        heap.push(new Object(6, "678"));
        heap.clear();
        heap.push(new Object(5, "567"));
        heap.push(new Object(6, "678"));
    }
    cout << "Leak: " << Object::count;
}

void heap070()
{
    expect = "Leak: 0";
    {
        Heap<Object *> heap(ObjectCompare, Heap<Object *>::free);
        Object *arr1[] = {new Object(1, ""), new Object(2, ""), new Object(3, ""), new Object(4, ""), new Object(5, "")};
        Object *arr2[] = {new Object(1, ""), new Object(2, ""), new Object(3, ""), new Object(4, ""), new Object(5, "")};
        int n = sizeof(arr1) / sizeof(arr1[0]);
        for (int i = 0; i < n; ++i)
            heap.push(arr1[i]);
        heap.heapify(arr2, sizeof(arr2) / sizeof(arr2[0]));
    }
    cout << "Leak: " << Object::count;
}

// HuffmanTree ================================================

void huffmantree071()
{
    expect = "9";
    HuffmanTree<10> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('A', 1));
    tree.build(symbolsFreqs);
    xMap<char, string> table(xMap<char, string>::simpleHash);
    tree.generateCodes(table);
    cout << table.get('A');
}

void huffmantree072()
{
    expect = "f";
    HuffmanTree<16> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('A', 1));
    tree.build(symbolsFreqs);
    xMap<char, string> table(xMap<char, string>::simpleHash);
    tree.generateCodes(table);
    cout << table.get('A');
}

void huffmantree073()
{
    expect = "0";
    HuffmanTree<2> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('A', 1));
    symbolsFreqs.add(make_pair('B', 2));
    tree.build(symbolsFreqs);
    xMap<char, string> table(xMap<char, string>::simpleHash);
    tree.generateCodes(table);
    cout << table.get('A');
}

void huffmantree074()
{
    expect = "A";
    HuffmanTree<10> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('A', 1));
    tree.build(symbolsFreqs);
    cout << tree.decode("9");
}

void huffmantree075()
{
    expect = "A";
    HuffmanTree<2> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('A', 1));
    symbolsFreqs.add(make_pair('B', 2));
    tree.build(symbolsFreqs);
    cout << tree.decode("0");
}

void huffmantree076()
{
    expect = "ABAB";
    HuffmanTree<2> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('A', 1));
    symbolsFreqs.add(make_pair('B', 2));
    tree.build(symbolsFreqs);
    cout << tree.decode("0101");
}

void huffmantree077()
{
    expect = "NXM";
    HuffmanTree<16> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('M', 1));
    symbolsFreqs.add(make_pair('N', 2));
    symbolsFreqs.add(make_pair('A', 3));
    symbolsFreqs.add(make_pair('B', 4));
    symbolsFreqs.add(make_pair('X', 5));
    symbolsFreqs.add(make_pair('Y', 6));
    tree.build(symbolsFreqs);
    cout << tree.decode("bea");
}

void huffmantree078()
{
    expect = "TNM";
    HuffmanTree<3> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('M', 1));
    symbolsFreqs.add(make_pair('N', 2));
    symbolsFreqs.add(make_pair('A', 3));
    symbolsFreqs.add(make_pair('B', 4));
    symbolsFreqs.add(make_pair('C', 5));
    symbolsFreqs.add(make_pair('D', 6));
    symbolsFreqs.add(make_pair('E', 7));
    symbolsFreqs.add(make_pair('F', 8));
    symbolsFreqs.add(make_pair('G', 9));
    symbolsFreqs.add(make_pair('H', 10));
    symbolsFreqs.add(make_pair('X', 11));
    symbolsFreqs.add(make_pair('Y', 12));
    symbolsFreqs.add(make_pair('T', 13));
    tree.build(symbolsFreqs);
    xMap<char, string> table(xMap<char, string>::simpleHash);
    tree.generateCodes(table);
    cout << tree.decode("21001000");
}

void huffmantree079()
{
    expect = "KISSANDMAKEUP";
    HuffmanTree<3> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('M', 1));
    symbolsFreqs.add(make_pair('N', 2));
    symbolsFreqs.add(make_pair('A', 3));
    symbolsFreqs.add(make_pair('B', 4));
    symbolsFreqs.add(make_pair('C', 5));
    symbolsFreqs.add(make_pair('D', 6));
    symbolsFreqs.add(make_pair('E', 7));
    symbolsFreqs.add(make_pair('F', 8));
    symbolsFreqs.add(make_pair('G', 9));
    symbolsFreqs.add(make_pair('H', 10));
    symbolsFreqs.add(make_pair('K', 11));
    symbolsFreqs.add(make_pair('I', 12));
    symbolsFreqs.add(make_pair('U', 13));
    symbolsFreqs.add(make_pair('P', 14));
    symbolsFreqs.add(make_pair('X', 15));
    symbolsFreqs.add(make_pair('Y', 16));
    symbolsFreqs.add(make_pair('T', 17));
    symbolsFreqs.add(make_pair('S', 18));
    tree.build(symbolsFreqs);
    xMap<char, string> table(xMap<char, string>::simpleHash);
    cout << tree.decode("22122212122200220122012201122002212020001");
}

void huffmantree080()
{
    expect = "KISSANDMAKEUP";
    HuffmanTree<16> tree;
    XArrayList<pair<char, int>> symbolsFreqs;
    symbolsFreqs.add(make_pair('M', 1));
    symbolsFreqs.add(make_pair('N', 2));
    symbolsFreqs.add(make_pair('A', 3));
    symbolsFreqs.add(make_pair('B', 4));
    symbolsFreqs.add(make_pair('C', 5));
    symbolsFreqs.add(make_pair('D', 6));
    symbolsFreqs.add(make_pair('E', 7));
    symbolsFreqs.add(make_pair('F', 8));
    symbolsFreqs.add(make_pair('G', 9));
    symbolsFreqs.add(make_pair('H', 10));
    symbolsFreqs.add(make_pair('K', 11));
    symbolsFreqs.add(make_pair('I', 12));
    symbolsFreqs.add(make_pair('U', 13));
    symbolsFreqs.add(make_pair('P', 14));
    symbolsFreqs.add(make_pair('X', 15));
    symbolsFreqs.add(make_pair('Y', 16));
    symbolsFreqs.add(make_pair('T', 17));
    symbolsFreqs.add(make_pair('S', 18));
    tree.build(symbolsFreqs);
    cout << tree.decode("89ff3f3e23d3f84ab");
}

void compressor081()
{
    expect = "Car string: Gadget:(weight: 2.500000), (voltage: 12.000000)\n";
    InventoryManager manager;
    List1D<InventoryAttribute> attrs;
    attrs.add(InventoryAttribute("weight", 2.5));
    attrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(attrs, "Gadget", 50);

    InventoryCompressor<5> compressor(&manager);

    cout << "Car string: " << compressor.productToString(attrs, "Gadget") << endl;
}

void compressor082()
{
    expect = "4414313422002124040442422410102233333321\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute("speed", 88.0));
    manager.addProduct(carAttrs, "Car", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute("capacity", 250.0));
    batteryAttrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(batteryAttrs, "Battery", 5);

    InventoryCompressor<5> compressor(&manager);
    compressor.buildHuffman();
    cout << compressor.encodeHuffman(carAttrs, "Car") << endl;
}

void compressor083()
{
    expect = "e8d4c6ee3aae9c9118ffffff7\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute("speed", 88.0));
    manager.addProduct(carAttrs, "Car", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute("capacity", 250.0));
    batteryAttrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(batteryAttrs, "Battery", 5);

    InventoryCompressor<16> compressor(&manager);
    compressor.buildHuffman();
    cout << compressor.encodeHuffman(carAttrs, "Car") << endl;
}

void compressor084()
{
    expect = "[capacity: 250.000000, voltage: 12.000000]\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute("speed", 88.0));
    manager.addProduct(carAttrs, "Car", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute("capacity", 250.0));
    batteryAttrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(batteryAttrs, "Battery", 5);

    InventoryCompressor<5> compressor(&manager);
    compressor.buildHuffman();
    List1D<InventoryAttribute> AttributeOutput;
    string name = "";
    compressor.decodeHuffman("4404341414013144220114312431144441144224042343223333332123224200301004143443404224233042233333321", AttributeOutput, name);
    cout << AttributeOutput.toString() << endl;
}

void compressor085()
{
    expect = "Battery\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute("speed", 88.0));
    manager.addProduct(carAttrs, "Car", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute("capacity", 250.0));
    batteryAttrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(batteryAttrs, "Battery", 5);

    InventoryCompressor<5> compressor(&manager);
    compressor.buildHuffman();
    List1D<InventoryAttribute> AttributeOutput;
    string name = "";
    compressor.decodeHuffman("4404341414013144220114312431144441144224042343223333332123224200301004143443404224233042233333321", AttributeOutput, name);
    cout << name << endl;
}

void compressor086()
{
    expect = "[capacity: 250.000000, voltage: 12.000000]\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute("speed", 88.0));
    manager.addProduct(carAttrs, "Car", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute("capacity", 250.0));
    batteryAttrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(batteryAttrs, "Battery", 5);

    InventoryCompressor<16> compressor(&manager);
    compressor.buildHuffman();
    List1D<InventoryAttribute> AttributeOutput;
    string name = "";
    compressor.decodeHuffman("e7dbba45c62d3d2ebb5c90e6f8ffffff7e496efedecbdeaac9e508ffffff7", AttributeOutput, name);
    cout << AttributeOutput.toString() << endl;
}

void compressor087()
{
    expect = "Battery\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute("speed", 88.0));
    manager.addProduct(carAttrs, "Car", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute("capacity", 250.0));
    batteryAttrs.add(InventoryAttribute("voltage", 12.0));
    manager.addProduct(batteryAttrs, "Battery", 5);

    InventoryCompressor<16> compressor(&manager);
    compressor.buildHuffman();
    List1D<InventoryAttribute> AttributeOutput;
    string name = "";
    compressor.decodeHuffman("e7dbba45c62d3d2ebb5c90e6f8ffffff7e496efedecbdeaac9e508ffffff7", AttributeOutput, name);
    cout << name << endl;
}

void compressor088()
{
    expect = "fd8fcf4fb6fdef3f9f9fd9fbf118eeeeee7\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute(" s p e e d ", 88.0));
    manager.addProduct(carAttrs, " C a r ", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute(" c a p a c i t y ", 250.0));
    batteryAttrs.add(InventoryAttribute(" v o l t a g e ", 12.0));
    manager.addProduct(batteryAttrs, " B a t t e r y ", 5);

    InventoryCompressor<16> compressor(&manager);
    compressor.buildHuffman();
    cout << compressor.encodeHuffman(carAttrs, " C a r ") << endl;
    // List1D<InventoryAttribute> AttributeOutput;
    // string name = "";
    // compressor.decodeHuffman("e7dbba45c62d3d2ebb5c90e6f8ffffff7e496efedecbdeaac9e508ffffff7", AttributeOutput, name);
    // cout << name;
}

void compressor089()
{
    expect = "fd7fcfafaf9f4f5fb6f2fcf3fcf2fdbfaf5fbf0d6e8eeeeee7d4f6fdffddfdcfafcfdaf9fbfd508eeeeee7\n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute(" s p e e d ", 88.0));
    manager.addProduct(carAttrs, " C a r ", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute(" c a p a c i t y ", 250.0));
    batteryAttrs.add(InventoryAttribute(" v o l t a g e ", 12.0));
    manager.addProduct(batteryAttrs, " B a t t e r y ", 5);

    InventoryCompressor<16> compressor(&manager);
    compressor.buildHuffman();
    cout << compressor.encodeHuffman(batteryAttrs, " B a t t e r y ") << endl;
}

void compressor090()
{
    expect = "[ c a p a c i t y : 250.000000,  v o l t a g e : 12.000000]:  B a t t e r y \n";
    InventoryManager manager;
    List1D<InventoryAttribute> carAttrs;
    carAttrs.add(InventoryAttribute(" s p e e d ", 88.0));
    manager.addProduct(carAttrs, " C a r ", 1);

    List1D<InventoryAttribute> batteryAttrs;
    batteryAttrs.add(InventoryAttribute(" c a p a c i t y ", 250.0));
    batteryAttrs.add(InventoryAttribute(" v o l t a g e ", 12.0));
    manager.addProduct(batteryAttrs, " B a t t e r y ", 5);

    InventoryCompressor<16> compressor(&manager);
    compressor.buildHuffman();
    List1D<InventoryAttribute> AttributeOutput;
    string name = "";
    compressor.decodeHuffman("fd7fcfafaf9f4f5fb6f2fcf3fcf2fdbfaf5fbf0d6e8eeeeee7d4f6fdffddfdcfafcfdaf9fbfd508eeeeee7", AttributeOutput, name);
    cout << AttributeOutput.toString() << ": " << name << endl;
}

void (*func_ptr[])() = {
    hash001,
    hash002,
    hash003,
    hash004,
    hash005,
    hash006,
    hash007,
    hash008,
    hash009,
    hash010,
    hash011,
    hash012,
    hash013,
    hash014,
    hash015,
    hash016,
    hash017,
    hash018,
    hash019,
    hash020,
    hash021,
    hash022,
    hash023,
    hash024,
    hash025,
    hash026,
    hash027,
    hash028,
    hash029,
    hash030,
    hash031,
    hash032,
    hash033,
    /*hash034,
    hash035,
    hash036,
    hash037,
    hash038,
    hash039,*/
    hash040,
    hash041,
    hash042,
    hash043,
    hash044,
    hash045,
    heap046,
    heap047,
    heap048,
    heap049,
    heap050,
    heap051,
    heap052,
    heap053,
    heap054,
    heap055,
    heap056,
    heap057,
    heap058,
    heap059,
    heap060,
    heap061,
    heap062,
    heap063,
    heap064,
    heap065,
    heap066,
    heap067,
    // heap068,
    heap069,
    heap070,
    huffmantree071,
    huffmantree072,
    huffmantree073,
    huffmantree074,
    huffmantree075,
    huffmantree076,
    huffmantree077,
    huffmantree078,
    huffmantree079,
    huffmantree080,
    compressor081,
    compressor082,
    compressor083,

    /*compressor084,
    compressor085,
    compressor086,
    compressor087,*/
    compressor088,
    compressor089,
    // compressor090,
};

bool run(int func_idx)
{
    try
    {
        Object::count = 0;
        cout << "Testcase " << func_idx + 1 << ": ";
        ostringstream oss;
        cout.rdbuf(oss.rdbuf());
        func_ptr[func_idx]();
        cout.rdbuf(original_cout);
        output = oss.str();
        bool ans = expect == output;
        cout << (ans ? "Pass\n" : "Fail\n");
        if (!ans)
        {
            cout << "Expect:\n"
                 << expect << endl;
            cout << "Output:\n"
                 << output << endl;
            cout << "====================================================================================================================\n";
        }
        return ans;
    }
    catch (...)
    {
        cout << "Error\n";
        return 0;
    }
}

int main(int argc, char **argv)
{
    cout << "Assignment-2" << endl;
    if (argc == 1)
    {
        int result = 0;
        int n = sizeof(func_ptr) / sizeof(func_ptr[0]);
        for (int i = 0; i != n; ++i)
            result += run(i);
        cout << "Result: " << result << " / " << n << endl;
    }
    else
    {
        run(stoi(argv[1]));
    }
    return 0;
}

/*#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "test/tc_xmap.h"
#include "test/tc_heap.h"
#include "test/tc_compressor.h"

using namespace std;

void (*func_ptr[17])() = {
    hashDemo1,
    hashDemo2,
    hashDemo3,
    hashDemo4,
    hashDemo5,
    hashDemo6,
    hashDemo7,
    heapDemo1,
    heapDemo2,
    heapDemo3,
    tc_huffman1001,
    tc_huffman1002,
    tc_huffman1003,
    tc_huffman1004,
    tc_huffman1005,
    tc_compressor1001,
    tc_compressor1002};

void run(int func_idx)
{
    func_ptr[func_idx]();
}

int main(int argc, char **argv)
{
    // cout << "Assignment-2" << endl;
    if (argc == 1)
    {
        hashDemo1();
        hashDemo2();
        hashDemo3();
        hashDemo4();
        hashDemo5();
        hashDemo6();
        hashDemo7();
        heapDemo1();
        heapDemo2();
        heapDemo3();
        tc_huffman1001();
        tc_huffman1002();
        tc_huffman1003();
        tc_huffman1004();
        tc_huffman1005();
        tc_compressor1001();
        tc_compressor1002();
    }

    return 0;
}
*/