/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
#include "list/IList.h"
#include <memory.h>
#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

// Định nghĩa toán tử << cho std::pair
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p)
{
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}
template <class T>
class XArrayList : public IList<T>
{
public:
    class Iterator; // forward declaration

protected:
    T *data;                                 // dynamic array to store the list's items
    int capacity;                            // size of the dynamic array
    int count;                               // number of items stored in the array
    bool (*itemEqual)(T &lhs, T &rhs);       // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(XArrayList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    XArrayList(
        void (*deleteUserData)(XArrayList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0,
        int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: BEGIN

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }

    /** free:
     * if T is pointer type:
     *     pass THE address of method "free" to XArrayList<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  XArrayList<Point*> list(&XArrayList<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(XArrayList<T> *list)
    {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

protected:
    void checkIndex(int index);     // check validity of index for accessing
    void ensureCapacity(int index); // auto-allocate if needed

    /** equals:
     * if T: primitive type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     * if T: object type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     *      Therefore, class of type T MUST override operator ==
     * if T: pointer type:
     *      indexOf, contains: will use function pointer "itemEqual"
     *      to: compare two items of T type
     *      Therefore:
     *      (1): must pass itemEqual to the constructor of XArrayList
     *      (2): must define a method for comparing
     *           the content pointed by two pointers of type T
     *          See: definition of "equals" of class Point for more detail
     */
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    void copyFrom(const XArrayList<T> &list);

    void removeInternalData();

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Iterator: BEGIN
    class Iterator
    {
    private:
        int cursor;
        XArrayList<T> *pList;

    public:
        Iterator(XArrayList<T> *pList = 0, int index = 0)
        {
            this->pList = pList;
            this->cursor = index;
        }
        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            T item = pList->removeAt(cursor);
            if (removeItemData != 0)
                removeItemData(item);
            cursor -= 1; // MUST keep index of previous, for ++ later
        }

        T &operator*()
        {
            return pList->data[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->cursor++;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };

    // Iterator: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),
    int capacity)
{
    // Khởi tạo một mảng động trống với capacity được chỉ định
    this->capacity = capacity > 0 ? capacity : 10; // Đảm bảo capacity ít nhất là 10
    this->count = 0;
    this->data = new T[this->capacity];
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)
{
    /*
     * Copies the contents of another XArrayList into this list.
     * Initializes the list with the same capacity as the source list and copies all elements.
     * Also duplicates user-defined comparison and deletion functions, if applicable.
     */
    // Sao chép dữ liệu từ list khác
    this->capacity = list.capacity;
    this->count = list.count;
    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;

    // Cấp phát mảng mới và sao chép dữ liệu
    this->data = new T[this->capacity];
    for (int index = 0; index < this->count; index++)
    {
        this->data[index] = list.data[index];
    }
}

template <class T>
void XArrayList<T>::removeInternalData()
{
    /*
     * Clears the internal data of the list by deleting the dynamic array and any user-defined data.
     * If a custom deletion function is provided, it is used to free the stored elements.
     * Finally, the dynamic array itself is deallocated from memory.
     */
    // Xóa dữ liệu nội bộ của danh sách
    // Nếu deleteUserData được cung cấp, gọi nó để giải phóng các phần tử
    if (this->deleteUserData != nullptr)
    {
        this->deleteUserData(this);
    }

    // Giải phóng mảng động
    delete[] this->data;
    this->data = nullptr;
    this->count = 0;
    this->capacity = 0;
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
    // Constructor sao chép
    // Tạo một danh sách mới và sao chép dữ liệu từ list khác
    this->data = nullptr;
    copyFrom(list);
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{
    // Toán tử gán - kiểm tra tự gán
    if (this != &list)
    {
        // Xóa dữ liệu hiện tại
        removeInternalData();
        // Sao chép dữ liệu từ list khác
        copyFrom(list);
    }
    return *this;
}

template <class T>
XArrayList<T>::~XArrayList()
{
    // Hủy đối tượng - giải phóng bộ nhớ
    removeInternalData();
}

template <class T>
void XArrayList<T>::add(T e)
{
    // Thêm phần tử vào cuối danh sách
    // Đảm bảo có đủ dung lượng
    ensureCapacity(count + 1);
    // Thêm phần tử vào vị trí cuối
    data[count] = e;
    // Tăng số lượng phần tử
    count++;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
    // Thêm phần tử vào vị trí chỉ định
    // Kiểm tra tính hợp lệ của chỉ mục
    if (index < 0 || index > count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    // Đảm bảo có đủ dung lượng
    ensureCapacity(count + 1);

    // Di chuyển các phần tử từ vị trí index trở đi sang phải một vị trí
    for (int i = count; i > index; i--)
    {
        data[i] = data[i - 1];
    }

    // Thêm phần tử vào vị trí index
    data[index] = e;
    count++;
}

template <class T>
T XArrayList<T>::removeAt(int index)
{
    // Xóa phần tử tại vị trí chỉ định và trả về giá trị của nó
    // Kiểm tra tính hợp lệ của chỉ mục
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    // Lưu phần tử cần xóa
    T removedItem = data[index];

    // Di chuyển các phần tử sau vị trí index sang trái một vị trí
    for (int i = index; i < count - 1; i++)
    {
        data[i] = data[i + 1];
    }

    // Giảm số lượng phần tử
    count--;

    return removedItem;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // Tìm và xóa phần tử từ danh sách
    int index = indexOf(item);
    if (index == -1)
        return false;

    // Xóa phần tử tại vị trí tìm thấy
    T removedItem = removeAt(index);

    // Nếu có hàm xóa dữ liệu được cung cấp, gọi nó
    if (removeItemData != nullptr)
    {
        removeItemData(removedItem);
    }

    return true;
}

template <class T>
bool XArrayList<T>::empty()
{
    // Kiểm tra xem danh sách có trống không
    return count == 0;
}

template <class T>
int XArrayList<T>::size()
{
    // Trả về số lượng phần tử trong danh sách
    return count;
}

template <class T>
void XArrayList<T>::clear()
{
    // Xóa tất cả các phần tử khỏi danh sách
    // Nếu có hàm xóa dữ liệu, gọi nó
    if (deleteUserData != nullptr)
    {
        deleteUserData(this);
    }

    // Đặt lại count về 0, nhưng giữ nguyên dung lượng và mảng
    count = 0;
}

template <class T>
T &XArrayList<T>::get(int index)
{
    // Trả về phần tử tại vị trí chỉ định
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("Index is out of range!");
    }
    return data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
    // Tìm vị trí đầu tiên của phần tử trong danh sách
    for (int i = 0; i < count; i++)
    {
        if (equals(data[i], item, itemEqual))
        {
            return i;
        }
    }
    return -1; // Trả về -1 nếu không tìm thấy
}

template <class T>
bool XArrayList<T>::contains(T item)
{
    // Kiểm tra xem danh sách có chứa phần tử không
    return indexOf(item) != -1;
}
/*
 * Converts the array list into a string representation, formatting each element using a user-defined function.
 * If no function is provided, it uses the default string representation for each element.
 * Example: Given an array list with elements {1, 2, 3} and a function that converts integers to strings, calling toString would return "[1, 2, 3]".
 *
 * @param item2str A function pointer for converting items of type T to strings. If null, default to the string conversion of T.
 * @return A string representation of the array list with elements separated by commas and enclosed in square brackets.
 */
template <class T>
string XArrayList<T>::toString(string (*item2str)(T &))
{
    // Chuyển đổi danh sách thành chuỗi
    stringstream ss;
    ss << "[";

    for (int i = 0; i < count; i++)
    {
        if (i > 0)
        {
            ss << ", ";
        }

        if (item2str != nullptr)
        {
            // Sử dụng hàm chuyển đổi tùy chỉnh
            ss << item2str(data[i]);
        }
        else
        {
            // Sử dụng toán tử << mặc định
            ss << data[i];
        }
    }

    ss << "]";
    return ss.str();
}

//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
void XArrayList<T>::checkIndex(int index)
{
    /**
     * Validates whether the given index is within the valid range of the list.
     * Throws an std::out_of_range exception if the index is negative or exceeds the number of elements.
     * Ensures safe access to the list's elements by preventing invalid index operations.
     */
    // Kiểm tra tính hợp lệ của chỉ mục
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("Index is out of range!");
    }
}
template <class T>
void XArrayList<T>::ensureCapacity(int index)
{
    /**
     * Ensures that the list has enough capacity to accommodate the given index.
     * If the index is out of range, it throws an std::out_of_range exception. If the index exceeds the current capacity,
     * reallocates the internal array with increased capacity, copying the existing elements to the new array.
     * In case of memory allocation failure, catches std::bad_alloc.
     */
    int minCapacity = index + 1; // Dung lượng tối thiểu cần thiết để chứa index
    if (minCapacity > capacity)
    {                                     // Nếu vượt quá dung lượng hiện tại
        int newCapacity = capacity * 1.5; // Tăng gấp đôi dung lượng
        if (newCapacity < minCapacity)
            newCapacity = minCapacity; // Đảm bảo đủ chỗ
        T *newData = new T[newCapacity];
        for (int i = 0; i < count; i++)
        {
            newData[i] = data[i]; // Sao chép dữ liệu cũ
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
};

#endif /* XARRAYLIST_H */
