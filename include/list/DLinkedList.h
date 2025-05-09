/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

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
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
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

    // Backward Iterator
    class BWDIterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        BWDIterator(DLinkedList<T> *pList = 0, bool last = true)
        {
            if (last)
            {
                if (pList != 0)
                    this->pNode = pList->tail->prev;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const BWDIterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->next; // MUST next, so iterator-- will go to head
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const BWDIterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix -- overload
        BWDIterator &operator--()
        {
            pNode = pNode->prev;
            return *this;
        }
        // Postfix -- overload
        BWDIterator operator--(int)
        {
            BWDIterator iterator = *this;
            --*this;
            return iterator;
        }
    };
};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////
// Constructor
template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // Khởi tạo danh sách rỗng với head và tail
    // head và tail là các node đặc biệt (sentinel nodes) không chứa dữ liệu người dùng
    this->head = new Node(); // Tạo node head
    this->tail = new Node(); // Tạo node tail

    // Liên kết head và tail với nhau trong danh sách rỗng
    this->head->next = this->tail; // head trỏ next đến tail
    this->tail->prev = this->head; // tail trỏ prev đến head

    // Ban đầu danh sách không có phần tử nào
    this->count = 0;

    // Lưu các con trỏ hàm để so sánh và xóa dữ liệu
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;
}

// Copy constructor
template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // Khởi tạo danh sách rỗng
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;

    // Copy các con trỏ hàm từ danh sách gốc
    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;

    // Copy dữ liệu từ danh sách list
    copyFrom(list);
}

// Toán tử gán
template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // Kiểm tra gán cho chính nó
    if (this == &list)
        return *this;

    // Xóa dữ liệu hiện có trong danh sách
    removeInternalData();

    // Copy dữ liệu từ danh sách list
    copyFrom(list);

    return *this;
}

// Destructor
template <class T>
DLinkedList<T>::~DLinkedList()
{
    // Xóa tất cả các node và dữ liệu nếu cần
    removeInternalData();

    // Xóa các node head và tail
    delete head;
    delete tail;
}

// Thêm phần tử vào cuối danh sách
template <class T>
void DLinkedList<T>::add(T e)
{
    // Tạo node mới chứa dữ liệu e
    Node *newNode = new Node(e);

    // Chèn node mới trước tail
    // Cập nhật liên kết cho node mới
    newNode->next = tail;
    newNode->prev = tail->prev;

    // Cập nhật liên kết cho các node xung quanh
    tail->prev->next = newNode;
    tail->prev = newNode;

    // Tăng kích thước danh sách
    count++;
}

// Thêm phần tử vào vị trí index
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // Kiểm tra index hợp lệ
    if (index < 0 || index > count)
        throw std::out_of_range("Index is out of range!");

    // Nếu thêm vào cuối danh sách, gọi hàm add(T e)
    if (index == count)
    {
        add(e);
        return;
    }

    // Tìm node trước vị trí cần chèn
    Node *prevNode = getPreviousNodeOf(index);

    // Tạo node mới chứa dữ liệu e
    Node *newNode = new Node(e);

    // Chèn node mới vào sau prevNode
    newNode->next = prevNode->next;
    newNode->prev = prevNode;

    // Cập nhật liên kết cho các node xung quanh
    prevNode->next->prev = newNode;
    prevNode->next = newNode;

    // Tăng kích thước danh sách
    count++;
}

// Tìm node trước vị trí index
template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    // Kiểm tra index hợp lệ
    if (index < 0 || index > count)
        throw std::out_of_range("Index is out of range!");

    // Nếu index là 0, trả về head
    if (index == 0)
        return head;

    // Nếu không, duyệt đến node trước vị trí index
    Node *current = head;
    for (int i = 0; i < index; i++)
    {
        current = current->next;
    }

    return current;
}

// Xóa phần tử tại vị trí index
template <class T>
T DLinkedList<T>::removeAt(int index)
{
    // Kiểm tra index hợp lệ
    if (index < 0 || index >= count)
        throw std::out_of_range("Index is out of range!");

    T data;

    // Trường hợp đặc biệt: xóa phần tử đầu tiên
    if (index == 0)
    {
        Node *nodeToRemove = head;
        data = nodeToRemove->data;

        head = head->next;
        if (head != nullptr)
            head->prev = nullptr;
        else
            tail = nullptr; // Nếu danh sách chỉ có 1 phần tử

        delete nodeToRemove;
    }
    // Trường hợp đặc biệt: xóa phần tử cuối cùng
    else if (index == count - 1)
    {
        Node *nodeToRemove = tail;
        data = nodeToRemove->data;

        tail = tail->prev;
        tail->next = nullptr;

        delete nodeToRemove;
    }
    // Trường hợp thông thường: xóa phần tử ở giữa
    else
    {
        // Tìm node trước vị trí cần xóa
        Node *prevNode = getPreviousNodeOf(index);

        // Node cần xóa là node sau prevNode
        Node *nodeToRemove = prevNode->next;

        // Lưu dữ liệu để trả về
        data = nodeToRemove->data;

        // Cập nhật liên kết
        prevNode->next = nodeToRemove->next;
        nodeToRemove->next->prev = prevNode;

        // Xóa node
        delete nodeToRemove;
    }

    // Giảm kích thước danh sách
    count--;

    return data;
}

// Kiểm tra danh sách rỗng
template <class T>
bool DLinkedList<T>::empty()
{
    return count == 0;
}

// Trả về kích thước danh sách
template <class T>
int DLinkedList<T>::size()
{
    return count;
}

// Xóa tất cả phần tử trong danh sách
template <class T>
void DLinkedList<T>::clear()
{
    // Xóa tất cả các node và dữ liệu nếu cần
    removeInternalData();

    // Khởi tạo lại danh sách rỗng
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
}

// Lấy phần tử tại vị trí index
template <class T>
T &DLinkedList<T>::get(int index)
{
    // Kiểm tra index hợp lệ
    if (index < 0 || index >= count)
        throw std::out_of_range("Index is out of range!");

    // Tìm node tại vị trí index
    Node *node = getPreviousNodeOf(index)->next;

    return node->data;
}

// Tìm vị trí đầu tiên của phần tử item
template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // Duyệt danh sách để tìm item
    Node *current = head->next;
    int index = 0;

    while (current != tail)
    {
        // So sánh phần tử current với item
        if (equals(current->data, item, itemEqual))
            return index;

        current = current->next;
        index++;
    }

    // Không tìm thấy
    return -1;
}

// Xóa phần tử item đầu tiên tìm thấy
template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // Duyệt danh sách để tìm item
    Node *current = head->next;

    while (current != tail)
    {
        // So sánh phần tử current với item
        if (equals(current->data, item, itemEqual))
        {
            // Cập nhật liên kết
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // Xóa dữ liệu nếu cần
            if (removeItemData != 0)
                removeItemData(current->data);

            // Xóa node
            delete current;

            // Giảm kích thước danh sách
            count--;

            return true;
        }

        current = current->next;
    }

    // Không tìm thấy
    return false;
}

// Kiểm tra danh sách có chứa phần tử item không
template <class T>
bool DLinkedList<T>::contains(T item)
{
    return indexOf(item) != -1;
}

// Chuyển danh sách thành chuỗi
template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    stringstream ss;
    ss << "[";

    // Duyệt danh sách
    Node *current = head->next;

    while (current != tail)
    {
        // Chuyển đổi phần tử thành chuỗi
        if (item2str != 0)
            ss << item2str(current->data);
        else
            ss << current->data; // Sử dụng toán tử << mặc định

        // Thêm dấu phẩy nếu không phải phần tử cuối cùng
        current = current->next;
        if (current != tail)
            ss << ", ";
    }

    ss << "]";
    return ss.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // Khởi tạo lại danh sách rỗng
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;

    // Copy dữ liệu từ danh sách gốc
    Node *current = list.head->next;

    while (current != list.tail)
    {
        // Thêm phần tử vào cuối danh sách
        add(current->data);
        current = current->next;
    }
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    // Kiểm tra xem có cần xóa dữ liệu người dùng không
    if (deleteUserData != 0)
        deleteUserData(this);

    // Xóa tất cả các node ngoại trừ head và tail
    Node *current = head->next;

    while (current != tail)
    {
        Node *next = current->next;
        delete current;
        current = next;
    }

    // Cập nhật liên kết cho danh sách rỗng
    head->next = tail;
    tail->prev = head;
    count = 0;
}

#endif /* DLINKEDLIST_H */
