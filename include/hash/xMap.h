#ifndef XMAP_H
#define XMAP_H
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <memory.h>
using namespace std;

#include "list/DLinkedList.h"
#include "hash/IMap.h"

/**
 * xMap<K, V>:
 *  - K: kiểu dữ liệu của khóa (key)
 *  - V: kiểu dữ liệu của giá trị (value)
 *  Ví dụ:
 *    xMap<string, int>: bảng băm ánh xạ từ string sang int
 *
 * Chức năng: Cài đặt bảng băm với cơ chế giải quyết xung đột bằng chuỗi liên kết
 * (Separate Chaining)
 */
template <class K, class V>
class xMap : public IMap<K, V>
{
public:
    class Entry; // Khai báo tiền tố cho lớp Entry

protected:
    DLinkedList<Entry *> *table; // Mảng các danh sách liên kết đôi
    int capacity;                // Kích thước của bảng
    int count;                   // Số lượng phần tử được lưu trữ trong bảng băm
    float loadFactor;            // Hệ số tải xác định số lượng phần tử tối đa có thể lưu (< (loadFactor * capacity))

    int (*hashCode)(K &, int);          // Hàm băm: hashCode(K key, int tableSize) - tableSize là capacity
    bool (*keyEqual)(K &, K &);         // Hàm so sánh khóa: keyEqual(K& lhs, K& rhs) - kiểm tra lhs == rhs
    bool (*valueEqual)(V &, V &);       // Hàm so sánh giá trị: valueEqual(V& lhs, V& rhs) - kiểm tra lhs == rhs
    void (*deleteKeys)(xMap<K, V> *);   // Hàm xóa khóa: deleteKeys(xMap<K,V>* pMap) - xóa tất cả khóa trong pMap
    void (*deleteValues)(xMap<K, V> *); // Hàm xóa giá trị: deleteValues(xMap<K,V>* pMap) - xóa tất cả giá trị trong pMap

public:
    /**
     * Constructor với các tham số tùy chọn
     * @param hashCode: hàm băm bắt buộc
     * @param loadFactor: hệ số tải, mặc định là 0.75
     * @param valueEqual: hàm so sánh giá trị, mặc định là nullptr
     * @param deleteValues: hàm xóa giá trị, mặc định là nullptr
     * @param keyEqual: hàm so sánh khóa, mặc định là nullptr
     * @param deleteKeys: hàm xóa khóa, mặc định là nullptr
     */
    xMap(
        int (*hashCode)(K &, int), // bắt buộc
        float loadFactor = 0.75f,
        bool (*valueEqual)(V &, V &) = 0,
        void (*deleteValues)(xMap<K, V> *) = 0,
        bool (*keyEqual)(K &, K &) = 0,
        void (*deleteKeys)(xMap<K, V> *) = 0);

    xMap(const xMap<K, V> &map);                  // Copy constructor
    xMap<K, V> &operator=(const xMap<K, V> &map); // Toán tử gán
    ~xMap();                                      // Hàm hủy

    // Các phương thức kế thừa từ IMap: PHẦN BẮT ĐẦU
    /**
     * Thêm một cặp key-value vào map
     * @param key: khóa
     * @param value: giá trị
     * @return: giá trị cũ nếu key đã tồn tại, nếu không thì trả về value đầu vào
     */
    V put(K key, V value);

    /**
     * Lấy giá trị ứng với khóa
     * @param key: khóa cần tìm
     * @return: tham chiếu đến giá trị
     * @throw KeyNotFound nếu không tìm thấy khóa
     */
    V &get(K key);

    /**
     * Xóa một cặp key-value theo khóa
     * @param key: khóa cần xóa
     * @param deleteKeyInMap: hàm xóa khóa, mặc định là nullptr
     * @return: giá trị được xóa
     * @throw KeyNotFound nếu không tìm thấy khóa
     */
    V remove(K key, void (*deleteKeyInMap)(K) = 0);

    /**
     * Xóa một cặp key-value cụ thể
     * @param key: khóa
     * @param value: giá trị
     * @param deleteKeyInMap: hàm xóa khóa, mặc định là nullptr
     * @param deleteValueInMap: hàm xóa giá trị, mặc định là nullptr
     * @return: true nếu xóa thành công, false nếu không tìm thấy cặp key-value
     */
    bool remove(K key, V value, void (*deleteKeyInMap)(K) = 0, void (*deleteValueInMap)(V) = 0);

    /**
     * Kiểm tra xem khóa có tồn tại trong map không
     * @param key: khóa cần kiểm tra
     * @return: true nếu tồn tại, false nếu không
     */
    bool containsKey(K key);

    /**
     * Kiểm tra xem giá trị có tồn tại trong map không
     * @param value: giá trị cần kiểm tra
     * @return: true nếu tồn tại, false nếu không
     */
    bool containsValue(V value);

    /**
     * Kiểm tra xem map có rỗng không
     * @return: true nếu rỗng, false nếu không
     */
    bool empty();

    /**
     * Lấy số lượng phần tử trong map
     * @return: số lượng phần tử
     */
    int size();

    /**
     * Xóa tất cả phần tử trong map
     */
    void clear();

    /**
     * Chuyển map thành chuỗi
     * @param key2str: hàm chuyển khóa thành chuỗi, mặc định là nullptr
     * @param value2str: hàm chuyển giá trị thành chuỗi, mặc định là nullptr
     * @return: chuỗi biểu diễn map
     */
    string toString(string (*key2str)(K &) = 0, string (*value2str)(V &) = 0);

    /**
     * Lấy danh sách các khóa trong map
     * @return: danh sách liên kết chứa các khóa
     */
    DLinkedList<K> keys();

    /**
     * Lấy danh sách các giá trị trong map
     * @return: danh sách liên kết chứa các giá trị
     */
    DLinkedList<V> values();

    /**
     * Lấy danh sách số lượng phần tử ở mỗi vị trí trong bảng băm
     * @return: danh sách chứa số lượng phần tử tại mỗi vị trí
     */
    DLinkedList<int> clashes();
    // Các phương thức kế thừa từ IMap: PHẦN KẾT THÚC

    /**
     * Hiển thị map lên màn hình
     * @param key2str: hàm chuyển khóa thành chuỗi, mặc định là nullptr
     * @param value2str: hàm chuyển giá trị thành chuỗi, mặc định là nullptr
     */
    void println(string (*key2str)(K &) = 0, string (*value2str)(V &) = 0)
    {
        cout << this->toString(key2str, value2str) << endl;
    }

    /**
     * Lấy kích thước của bảng băm
     * @return: kích thước bảng băm (capacity)
     */
    int getCapacity()
    {
        return capacity;
    }
    ///////////////////////////////////////////////////
    // CÁC PHƯƠNG THỨC TĨNH: PHẦN BẮT ĐẦU
    //      * Được sử dụng để tạo đối tượng xMap
    ///////////////////////////////////////////////////
    /**
     * Hàm băm đơn giản cho các khóa kiểu số nguyên
     */
    static int simpleHash(K &key, int capacity)
    {
        return key % capacity;
    }

    /**
     * Hàm băm cho khóa kiểu số nguyên
     */
    static int intKeyHash(int &key, int capacity)
    {
        return key % capacity;
    }

    /**
     * Hàm băm cho khóa kiểu chuỗi
     */
    static int stringKeyHash(string &key, int capacity)
    {
        long long int sum = 0;
        for (int idx = 0; idx < key.length(); idx++)
            sum += key[idx];
        return sum % capacity;
    }

    /**
     * freeKey(xMap<K,V> *pMap):
     *  Mục đích: hàm điển hình để xóa các khóa được lưu trữ trong map
     *  KHI nào sử dụng:
     *      1. K là kiểu con trỏ; VÀ
     *      2. Người dùng cần xMap giải phóng các khóa
     */
    static void freeKey(xMap<K, V> *pMap)
    {
        for (int idx = 0; idx < pMap->capacity; idx++)
        {
            DLinkedList<Entry *> list = pMap->table[idx];
            for (auto pEntry : list)
            {
                delete pEntry->key;
            }
        }
    }

    /**
     * freeValue(xMap<K,V> *pMap):
     *  Mục đích: hàm điển hình để xóa các giá trị được lưu trữ trong map
     *  KHI nào sử dụng:
     *      1. V là kiểu con trỏ; VÀ
     *      2. Người dùng cần xMap giải phóng các giá trị
     */
    static void freeValue(xMap<K, V> *pMap)
    {
        for (int idx = 0; idx < pMap->capacity; idx++)
        {
            DLinkedList<Entry *> list = pMap->table[idx];
            for (auto pEntry : list)
            {
                delete pEntry->value;
            }
        }
    }

    /**
     * deleteEntry(Entry* ptr): con trỏ hàm để xóa con trỏ đến Entry
     */
    static void deleteEntry(Entry *ptr)
    {
        delete ptr;
    }
    ///////////////////////////////////////////////////
    // CÁC PHƯƠNG THỨC TĨNH: PHẦN KẾT THÚC
    ///////////////////////////////////////////////////
protected:
    ////////////////////////////////////////////////////////
    ////////////////////////  TIỆN ÍCH ////////////////////
    ////////////////////////////////////////////////////////
    /*
     * Đảm bảo hệ số tải không vượt quá ngưỡng
     * @param minCapacity: số lượng phần tử tối thiểu cần xét
     */
    void ensureLoadFactor(int minCapacity);

    /*
     * Tổ chức lại bảng băm với kích thước mới
     * @param newCapacity: kích thước mới của bảng băm
     */
    void rehash(int newCapacity);

    /*
     * Xóa tất cả dữ liệu nội bộ của map
     */
    void removeInternalData();

    /*
     * Sao chép dữ liệu từ map khác
     * @param map: map nguồn để sao chép
     */
    void copyMapFrom(const xMap<K, V> &map);

    /*
     * Di chuyển các phần tử từ bảng cũ sang bảng mới
     * @param oldTable: bảng cũ
     * @param oldCapacity: kích thước bảng cũ
     * @param newTable: bảng mới
     * @param newCapacity: kích thước bảng mới
     */
    void moveEntries(
        DLinkedList<Entry *> *oldTable, int oldCapacity,
        DLinkedList<Entry *> *newTable, int newCapacity);

    /*
     * So sánh hai khóa
     * @param lhs: khóa thứ nhất
     * @param rhs: khóa thứ hai
     * @return: true nếu hai khóa bằng nhau, false nếu không
     */
    bool keyEQ(K &lhs, K &rhs)
    {
        if (keyEqual != 0)
            return keyEqual(lhs, rhs);
        else
            return lhs == rhs;
    }

    /*
     * So sánh hai giá trị
     * @param lhs: giá trị thứ nhất
     * @param rhs: giá trị thứ hai
     * @return: true nếu hai giá trị bằng nhau, false nếu không
     */
    bool valueEQ(V &lhs, V &rhs)
    {
        if (valueEqual != 0)
            return valueEqual(lhs, rhs);
        else
            return lhs == rhs;
    }

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  ĐỊNH NGHĨA LỚP INNER ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Entry: BEGIN
    class Entry
    {
    private:
        K key;
        V value;
        friend class xMap<K, V>;

    public:
        Entry(K key, V value)
        {
            this->key = key;
            this->value = value;
        }
    };
    // Entry: END
};
//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////
/**
 * Constructor với các tham số
 * - Khởi tạo bảng băm với capacity mặc định là 10
 * - Thiết lập các con trỏ hàm và tham số
 */
template <class K, class V>
xMap<K, V>::xMap(
    int (*hashCode)(K &, int),
    float loadFactor,
    bool (*valueEqual)(V &lhs, V &rhs),
    void (*deleteValues)(xMap<K, V> *),
    bool (*keyEqual)(K &lhs, K &rhs),
    void (*deleteKeys)(xMap<K, V> *pMap))
{
    // Khởi tạo bảng băm với capacity mặc định là 10
    this->capacity = 10;
    this->count = 0;
    this->loadFactor = loadFactor;

    // Khởi tạo các con trỏ hàm
    this->hashCode = hashCode;
    this->valueEqual = valueEqual;
    this->keyEqual = keyEqual;
    this->deleteKeys = deleteKeys;
    this->deleteValues = deleteValues;

    // Tạo bảng chứa các danh sách liên kết kép
    this->table = new DLinkedList<Entry *>[capacity];
}

/**
 * Copy constructor - Tạo một bản sao của map
 * @param map: map nguồn để sao chép
 */
template <class K, class V>
xMap<K, V>::xMap(const xMap<K, V> &map)
{
    // Khởi tạo các biến thành viên để tránh lỗi trong removeInternalData
    this->capacity = 0;
    this->count = 0;
    this->table = nullptr;

    // Sao chép các con trỏ hàm
    this->hashCode = map.hashCode;
    this->keyEqual = map.keyEqual;
    this->valueEqual = map.valueEqual;
    this->deleteKeys = map.deleteKeys;
    this->deleteValues = map.deleteValues;
    this->loadFactor = map.loadFactor;

    // Sao chép dữ liệu từ map nguồn
    copyMapFrom(map);
}

/**
 * Toán tử gán - Gán giá trị của map khác cho map hiện tại
 * @param map: map nguồn để gán
 * @return: tham chiếu đến map hiện tại
 */
template <class K, class V>
xMap<K, V> &xMap<K, V>::operator=(const xMap<K, V> &map)
{
    // Kiểm tra xem có tự gán không (self-assignment)
    if (this != &map)
    {
        // Sao chép các con trỏ hàm
        this->hashCode = map.hashCode;
        this->keyEqual = map.keyEqual;
        this->valueEqual = map.valueEqual;
        this->deleteKeys = map.deleteKeys;
        this->deleteValues = map.deleteValues;
        this->loadFactor = map.loadFactor;

        // Sao chép dữ liệu từ map nguồn
        copyMapFrom(map);
    }
    return *this;
}

/**
 * Hàm hủy - Giải phóng bộ nhớ
 */
template <class K, class V>
xMap<K, V>::~xMap()
{
    // Giải phóng toàn bộ bộ nhớ
    removeInternalData();
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////// TRIỂN KHAI CÁC PHƯƠNG THỨC CỦA IMap //////////////////////
//////////////////////////////////////////////////////////////////////////////////
/**
 * Thêm một cặp key-value vào map
 * - Nếu key đã tồn tại, cập nhật giá trị và trả về giá trị cũ
 * - Nếu key chưa tồn tại, thêm cặp key-value mới và trả về value đầu vào
 * - Kiểm tra và đảm bảo hệ số tải khi thêm phần tử mới
 */
template <class K, class V>
V xMap<K, V>::put(K key, V value)
{
    // Tính chỉ số của key trong bảng băm
    int index = this->hashCode(key, capacity);
    V retValue = value;

    // Lấy danh sách tại vị trí index
    DLinkedList<Entry *> &list = table[index];
    typename DLinkedList<Entry *>::Iterator it;

    // Kiểm tra xem key đã tồn tại chưa
    bool found = false;
    for (it = list.begin(); it != list.end(); it++)
    {
        Entry *entry = *it;
        if (keyEQ(entry->key, key))
        {
            // Key đã tồn tại, lưu lại giá trị cũ để trả về
            retValue = entry->value;
            // Cập nhật giá trị mới
            entry->value = value;
            found = true;
            break;
        }
    }

    // Nếu key chưa tồn tại, tạo entry mới và thêm vào danh sách
    if (!found)
    {
        Entry *newEntry = new Entry(key, value);
        list.add(newEntry);
        count++; // Tăng số lượng phần tử

        // Kiểm tra và đảm bảo hệ số tải
        ensureLoadFactor(count);
    }
    return retValue;
}

/**
 * Lấy giá trị ứng với khóa
 * - Nếu tìm thấy key, trả về tham chiếu đến giá trị tương ứng
 * - Nếu không tìm thấy, ném ngoại lệ KeyNotFound
 */
template <class K, class V>
V &xMap<K, V>::get(K key)
{
    // Tính chỉ số của key trong bảng băm
    int index = hashCode(key, capacity);

    // Lấy danh sách tại vị trí index
    DLinkedList<Entry *> &list = table[index];

    // Duyệt danh sách để tìm key
    for (auto entry : list)
    {
        if (keyEQ(entry->key, key))
        {
            // Trả về tham chiếu đến giá trị
            return entry->value;
        }
    }

    // key không tìm thấy, ném ngoại lệ
    stringstream os;
    os << "key (" << key << ") is not found";
    throw KeyNotFound(os.str());
}
/**
 * Xóa một cặp key-value theo khóa
 * - Nếu tìm thấy key, xóa cặp key-value và trả về giá trị được xóa
 * - Nếu không tìm thấy, ném ngoại lệ KeyNotFound
 * - Nếu deleteKeyInMap không phải là nullptr, gọi hàm này để xóa key
 */
template <class K, class V>
V xMap<K, V>::remove(K key, void (*deleteKeyInMap)(K))
{
    // Tính chỉ số của key trong bảng băm
    int index = hashCode(key, capacity);

    // Lấy danh sách tại vị trí index
    DLinkedList<Entry *> &list = table[index];

    // Duyệt danh sách để tìm key
    typename DLinkedList<Entry *>::Iterator it;
    for (it = list.begin(); it != list.end(); it++)
    {
        Entry *entry = *it;
        if (keyEQ(entry->key, key))
        {
            // Lưu giá trị để trả về
            V retValue = entry->value;

            // Xóa key nếu được yêu cầu
            if (deleteKeyInMap != nullptr)
            {
                deleteKeyInMap(entry->key);
            }
            // Xóa entry khỏi danh sách và giải phóng bộ nhớ
            // Giải phóng bộ nhớ cho key và value nếu cần
            list.removeItem(*it, deleteEntry);
            count--; // Giảm số lượng phần tử

            return retValue;
        }
    }

    // key không tìm thấy, ném ngoại lệ
    stringstream os;
    os << "key (" << key << ") is not found";
    throw KeyNotFound(os.str());
}
/**
 * Xóa một cặp key-value cụ thể
 * - Nếu tìm thấy cặp key-value, xóa nó và trả về true
 * - Nếu không tìm thấy, trả về false
 * - Nếu deleteKeyInMap hoặc deleteValueInMap không phải là nullptr, gọi hàm này để xóa key hoặc value
 */
template <class K, class V>
bool xMap<K, V>::remove(K key, V value, void (*deleteKeyInMap)(K), void (*deleteValueInMap)(V))
{
    // Tính chỉ số của key trong bảng băm
    int index = hashCode(key, capacity);

    // Lấy danh sách tại vị trí index
    DLinkedList<Entry *> &list = table[index];

    // Duyệt danh sách để tìm cặp key-value
    typename DLinkedList<Entry *>::Iterator it;
    for (it = list.begin(); it != list.end(); it++)
    {
        Entry *entry = *it;
        if (keyEQ(entry->key, key) && valueEQ(entry->value, value))
        {
            // Xóa key nếu được yêu cầu
            if (deleteKeyInMap != nullptr)
            {
                deleteKeyInMap(entry->key);
            }

            // Xóa value nếu được yêu cầu
            if (deleteValueInMap != nullptr)
            {
                deleteValueInMap(entry->value);
            }

            // Giải phóng bộ nhớ cho key và value nếu cần
            list.removeItem(*it, deleteEntry);
            count--; // Giảm số lượng phần tử

            return true;
        }
    }

    // Không tìm thấy cặp key-value
    return false;
}
/**
 * Kiểm tra xem khóa có tồn tại trong map không
 * - Trả về true nếu tìm thấy key
 * - Trả về false nếu không tìm thấy
 */
template <class K, class V>
bool xMap<K, V>::containsKey(K key)
{
    // Tính chỉ số của key trong bảng băm
    int index = hashCode(key, capacity);

    // Lấy danh sách tại vị trí index
    DLinkedList<Entry *> &list = table[index];

    // Duyệt danh sách để tìm key
    for (auto entry : list)
    {
        if (keyEQ(entry->key, key))
        {
            return true;
        }
    }

    // Không tìm thấy key
    return false;
}

/**
 * Kiểm tra xem giá trị có tồn tại trong map không
 * - Trả về true nếu tìm thấy value
 * - Trả về false nếu không tìm thấy
 */
template <class K, class V>
bool xMap<K, V>::containsValue(V value)
{
    // Duyệt qua tất cả các danh sách
    for (int i = 0; i < capacity; i++)
    {
        DLinkedList<Entry *> &list = table[i];

        // Duyệt danh sách để tìm value
        for (auto entry : list)
        {
            if (valueEQ(entry->value, value))
            {
                return true;
            }
        }
    }

    // Không tìm thấy value
    return false;
}

/**
 * Kiểm tra xem map có rỗng không
 * - Trả về true nếu không có phần tử nào
 * - Trả về false nếu có ít nhất một phần tử
 */
template <class K, class V>
bool xMap<K, V>::empty()
{
    // YOUR CODE IS HERE
    // Bảng rỗng khi không có phần tử nào
    return count == 0;
}

/**
 * Lấy số lượng phần tử trong map
 * - Trả về số lượng phần tử
 */
template <class K, class V>
int xMap<K, V>::size()
{
    // YOUR CODE IS HERE
    // Trả về số lượng phần tử
    return count;
}

/**
 * Xóa tất cả phần tử trong map
 * - Giải phóng toàn bộ bộ nhớ
 * - Khởi tạo lại bảng rỗng với capacity mặc định là 10
 */
template <class K, class V>
void xMap<K, V>::clear()
{
    // Giải phóng toàn bộ bộ nhớ
    removeInternalData();

    // Khởi tạo lại bảng rỗng với capacity mặc định là 10
    capacity = 10;
    count = 0;
    table = new DLinkedList<Entry *>[capacity];
}

/**
 * Lấy danh sách các khóa trong map
 * - Trả về danh sách chứa các khóa
 */
template <class K, class V>
DLinkedList<K> xMap<K, V>::keys()
{
    // Truyền tham số keyEqual vào constructor của DLinkedList
    DLinkedList<K> keyList(0, keyEqual);

    // Duyệt qua tất cả các danh sách
    for (int i = 0; i < capacity; i++)
    {
        DLinkedList<Entry *> &list = table[i];

        // Thêm các key vào danh sách kết quả
        typename DLinkedList<Entry *>::Iterator it;
        for (it = list.begin(); it != list.end(); it++)
        {
            Entry *entry = *it;
            keyList.add(entry->key);
        }
    }

    return keyList;
}

/**
 * Lấy danh sách các giá trị trong map
 * - Duyệt qua tất cả các danh sách trong bảng băm
 * - Thêm các giá trị vào danh sách kết quả
 * - Trả về danh sách chứa các giá trị
 */
template <class K, class V>
DLinkedList<V> xMap<K, V>::values()
{
    // Truyền tham số valueEqual vào constructor của DLinkedList
    DLinkedList<V> valueList(0, valueEqual);

    // Duyệt qua tất cả các danh sách
    for (int i = 0; i < capacity; i++)
    {
        DLinkedList<Entry *> &list = table[i];

        // Thêm các value vào danh sách kết quả
        typename DLinkedList<Entry *>::Iterator it;
        for (it = list.begin(); it != list.end(); it++)
        {
            Entry *entry = *it;
            valueList.add(entry->value);
        }
    }

    return valueList;
}
/**
 * Lấy danh sách số lượng phần tử ở mỗi vị trí trong bảng băm
 * - Trả về danh sách chứa số lượng phần tử tại mỗi vị trí
 */
template <class K, class V>
DLinkedList<int> xMap<K, V>::clashes()
{
    // YOUR CODE IS HERE
    DLinkedList<int> clashList;

    // Duyệt qua tất cả các danh sách
    for (int i = 0; i < capacity; i++)
    {
        // Thêm số lượng phần tử tại mỗi vị trí vào danh sách kết quả
        clashList.add(table[i].size());
    }

    return clashList;
}

/**
 * Chuyển map thành chuỗi
 * - Nếu key2str hoặc value2str không phải là nullptr, sử dụng chúng để chuyển đổi
 * - Trả về chuỗi biểu diễn map
 */
template <class K, class V>
string xMap<K, V>::toString(string (*key2str)(K &), string (*value2str)(V &))
{
    stringstream os;
    string mark(50, '=');
    os << mark << endl;
    os << setw(12) << left << "capacity: " << capacity << endl;
    os << setw(12) << left << "size: " << count << endl;
    for (int idx = 0; idx < capacity; idx++)
    {
        DLinkedList<Entry *> list = table[idx];

        os << setw(4) << left << idx << ": ";
        stringstream itemos;
        for (auto pEntry : list)
        {
            itemos << " (";

            if (key2str != 0)
                itemos << key2str(pEntry->key);
            else
                itemos << pEntry->key;
            itemos << ",";
            if (value2str != 0)
                itemos << value2str(pEntry->value);
            else
                itemos << pEntry->value;

            itemos << ");";
        }
        string valuestr = itemos.str();
        if (valuestr.length() > 0)
            valuestr = valuestr.substr(0, valuestr.length() - 1);
        os << valuestr << endl;
    }
    os << mark << endl;

    return os.str();
}

////////////////////////////////////////////////////////
//                  CÁC TIỆN ÍCH HỖ TRỢ
//              (Mã được cung cấp sẵn)
////////////////////////////////////////////////////////

/**
 * moveEntries:
 * Mục đích: Di chuyển tất cả các phần tử từ bảng băm cũ (oldTable) sang bảng mới (newTable)
 * - Tính toán lại vị trí mới cho mỗi phần tử dựa trên capacity mới
 */
template <class K, class V>
void xMap<K, V>::moveEntries(
    DLinkedList<Entry *> *oldTable, int oldCapacity,
    DLinkedList<Entry *> *newTable, int newCapacity)
{
    // Duyệt qua từng vị trí trong bảng cũ
    for (int old_index = 0; old_index < oldCapacity; old_index++)
    {
        DLinkedList<Entry *> &oldList = oldTable[old_index];
        // Duyệt qua từng phần tử trong danh sách tại vị trí old_index
        for (auto oldEntry : oldList)
        {
            // Tính chỉ số mới dựa trên capacity mới
            int new_index = this->hashCode(oldEntry->key, newCapacity);
            DLinkedList<Entry *> &newList = newTable[new_index];
            // Thêm phần tử vào danh sách tại vị trí mới
            newList.add(oldEntry);
        }
    }
}

/**
 * ensureLoadFactor:
 * Mục đích: Đảm bảo hệ số tải,
 * nghĩa là số lượng phần tử tối đa không vượt quá "loadFactor*capacity"
 * - Nếu số phần tử hiện tại vượt quá ngưỡng, thực hiện rehash với capacity lớn hơn
 */
template <class K, class V>
void xMap<K, V>::ensureLoadFactor(int current_size)
{
    // Tính số lượng phần tử tối đa có thể lưu trữ với capacity hiện tại
    int maxSize = (int)(loadFactor * capacity);

    // Nếu số phần tử hiện tại vượt quá ngưỡng
    if (current_size > maxSize)
    {
        int oldCapacity = capacity;
        // Tăng capacity lên 1.5 lần
        int newCapacity = 1.5 * oldCapacity;
        // Thực hiện rehash với capacity mới
        rehash(newCapacity);
    }
}

/**
 * rehash(int newCapacity)
 * Mục đích:
 *   1. Tạo bảng băm mới với kích thước newCapacity
 *   2. Di chuyển tất cả các phần tử từ bảng cũ sang bảng mới
 *   3. Giải phóng bộ nhớ của bảng cũ
 */
template <class K, class V>
void xMap<K, V>::rehash(int newCapacity)
{
    // Lưu lại tham chiếu đến bảng cũ
    DLinkedList<Entry *> *pOldMap = this->table;
    int oldCapacity = capacity;

    // Tạo bảng mới
    this->table = new DLinkedList<Entry *>[newCapacity];
    this->capacity = newCapacity; // giữ nguyên "count"

    // Di chuyển các phần tử từ bảng cũ sang bảng mới
    moveEntries(pOldMap, oldCapacity, this->table, newCapacity);

    // Xóa dữ liệu cũ: chỉ xóa các node trong danh sách, không xóa entry
    for (int idx = 0; idx < oldCapacity; idx++)
    {
        DLinkedList<Entry *> &list = pOldMap[idx];
        list.clear();
    }
    // Xóa bảng cũ
    delete[] pOldMap;
}
/**
 * removeInternalData:
 * Mục đích:
 *   1. Xóa tất cả các khóa và giá trị nếu người dùng yêu cầu
 *      (tức là deleteKeys và deleteValues không phải là nullptr)
 *   2. Xóa tất cả các entry
 *   3. Xóa bảng
 */
template <class K, class V>
void xMap<K, V>::removeInternalData()
{
    // Xóa dữ liệu của người dùng nếu được yêu cầu
    if (deleteKeys != 0)
        deleteKeys(this);
    if (deleteValues != 0)
        deleteValues(this);

    // Xóa tất cả các entry trong map hiện tại
    for (int idx = 0; idx < this->capacity; idx++)
    {
        DLinkedList<Entry *> &list = this->table[idx];
        for (auto pEntry : list)
            delete pEntry;
        list.clear();
    }

    // Xóa bảng
    delete[] table;
}

/**
 * copyMapFrom(const xMap<K,V>& map):
 * Mục đích:
 *   1. Xóa tất cả các entry của bảng băm hiện tại
 *   2. Sao chép (chỉ shallow-copy) tất cả các entry trong map đầu vào
 *      vào bảng hiện tại
 */
template <class K, class V>
void xMap<K, V>::copyMapFrom(const xMap<K, V> &map)
{
    // Xóa dữ liệu nội bộ hiện tại
    removeInternalData();

    // Khởi tạo bảng mới với capacity giống map đầu vào
    this->capacity = map.capacity;
    this->count = 0;
    this->table = new DLinkedList<Entry *>[capacity];

    // Sao chép các con trỏ hàm và tham số
    this->hashCode = hashCode;
    this->loadFactor = loadFactor;

    this->valueEqual = valueEqual;
    this->keyEqual = keyEqual;
    // KHÔNG NÊN SAO CHÉP: deleteKeys, deleteValues => chỉ xóa MỘT LẦN trong map nếu cần

    // Sao chép các entry
    for (int idx = 0; idx < map.capacity; idx++)
    {
        DLinkedList<Entry *> &list = map.table[idx];
        for (auto pEntry : list)
        {
            // Sử dụng phương thức put để thêm từng cặp key-value
            this->put(pEntry->key, pEntry->value);
        }
    }
}
#endif /* XMAP_H */
