#ifndef HEAP_H
#define HEAP_H

#include <stdexcept>
#include <sstream>
#include <iostream>
#include "heap/IHeap.h"
#include "list/XArrayList.h"

using namespace std;

template <class T>
class Heap : public IHeap<T>
{
public:
    /*──────────────  forward iterator  ─────────────*/
    class Iterator
    {
    public:
        Iterator(T *p, Heap<T> *h) : ptr(p), heap(h) {}
        bool operator!=(const Iterator &other) const { return ptr != other.ptr; }
        Iterator &operator++()
        {
            ++ptr;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++ptr;
            return tmp;
        }
        T &operator*() const { return *ptr; }
        void remove(void (*removeItemData)(T) = 0)
        {
            if (heap)
                heap->remove(*ptr, removeItemData);
        }

    private:
        T *ptr;
        Heap<T> *heap;
    };

    /*──────────────  data members  ─────────────*/
protected:
    T *elements;
    int capacity;
    int count;
    int (*comparator)(T &, T &);
    void (*deleteUserData)(Heap<T> *);

    /*──────────────  ctor / dtor  ─────────────*/
public:
    explicit Heap(int (*comp)(T &, T &) = 0,
                  void (*del)(Heap<T> *) = 0)
        : capacity(10), count(0),
          comparator(comp), deleteUserData(del)
    {
        elements = new T[capacity];
    }

    Heap(const Heap<T> &other) { copyFrom(other); }

    Heap<T> &operator=(const Heap<T> &other)
    {
        if (this != &other)
        {
            removeInternalData();
            copyFrom(other);
        }
        return *this;
    }

    ~Heap() { removeInternalData(); }

    /*──────────────  core APIs  ─────────────*/
    void push(T item) override
    {
        ensureCapacity(count + 1);
        elements[count++] = item;
        reheapUpRec(count - 1);
    }

    T pop() override
    {
        if (count == 0)
            throw underflow_error("Calling pop on empty heap");
        T root = elements[0];
        elements[0] = elements[count - 1];
        --count;
        reheapDownRec(0);
        return root;
    }

    const T peek() override
    {
        if (count == 0)
            throw underflow_error("Calling to peek with the empty heap.");
        return elements[0];
    }

    void remove(T item, void (*rm)(T) = 0) override
    {
        int idx = indexOfRec(0, item);
        if (idx < 0)
            return;
        if (rm)
            rm(elements[idx]);
        elements[idx] = elements[count - 1];
        --count;
        reheapDownRec(idx);
        reheapUpRec(idx);
    }

    bool contains(T item) override { return indexOfRec(0, item) >= 0; }

    int size() override { return count; }

    /*──────────────  heapify (O(n))  ─────────────*/
    void heapify(T array[], int n) override
    {
        clear();
        ensureCapacity(n);
        copyRawRec(array, 0, n); // copy input array
        count = n;
        buildBottomUpRec((count - 2) / 2); // Floyd
    }

    /*──────────────  misc APIs  ─────────────*/
    void clear() override
    {
        removeInternalData();
        capacity = 10;
        count = 0;
        elements = new T[capacity];
    }

    bool empty() override { return count == 0; }

    string toString(string (*item2str)(T &) = 0) override
    {
        stringstream os;
        os << "[";
        buildStringRec(os, 0, item2str);
        os << "]";
        return os.str();
    }

    void println(string (*item2str)(T &) = 0) { cout << toString(item2str) << '\n'; }

    Iterator begin() { return Iterator(elements, this); }
    Iterator end() { return Iterator(elements + count, this); }

    /*──────────────  optional helper  ─────────────*/
    static void free(Heap<T> *p) { freeRec(p->elements, 0, p->count); }

    /*──────────────  heapsort  ─────────────*/
    void heapsort(XArrayList<T> &list)
    {
        int n = list.size();
        T *tmp = new T[n];
        copyListToArrayRec(list, tmp, 0, n);

        clear();
        pushArrayRec(tmp, 0, n);
        delete[] tmp;

        list.clear();
        popToListRec(list, n);
    }

    /*──────────────  private helpers  ─────────────*/
private:
    /* comparisons ----------------------------------------------------------*/
    int compare(T &a, T &b) const
    {
        if (comparator)
            return comparator(a, b);
        if (a < b)
            return -1;
        if (a > b)
            return 1;
        return 0;
    }
    bool aLTb(T &a, T &b) const { return compare(a, b) < 0; }

    /* capacity -------------------------------------------------------------*/
    void ensureCapacity(int minCap)
    {
        if (minCap <= capacity)
            return;
        int newCap = capacity + (capacity >> 1);
        if (newCap < minCap)
            newCap = minCap;
        T *newArr = new T[newCap];
        copyArrayRec(elements, newArr, 0, count);
        delete[] elements;
        elements = newArr;
        capacity = newCap;
    }

    /* swapping -------------------------------------------------------------*/
    void swapElm(int i, int j)
    {
        T tmp = elements[i];
        elements[i] = elements[j];
        elements[j] = tmp;
    }

    /* reheap‑up / reheap‑down ---------------------------------------------*/
    void reheapUpRec(int idx)
    {
        if (idx == 0)
            return;
        int parent = (idx - 1) / 2;
        if (aLTb(elements[idx], elements[parent]))
        {
            swapElm(idx, parent);
            reheapUpRec(parent);
        }
    }

    void reheapDownRec(int idx)
    {
        int left = 2 * idx + 1;
        if (left >= count)
            return; // no children

        int right = 2 * idx + 2;
        int child = left;
        if (right < count && aLTb(elements[right], elements[left]))
            child = right;

        if (aLTb(elements[child], elements[idx]))
        {
            swapElm(child, idx);
            reheapDownRec(child);
        }
    }

    /* bottom‑up build ------------------------------------------------------*/
    void buildBottomUpRec(int idx)
    {
        if (idx < 0)
            return;
        reheapDownRec(idx);
        buildBottomUpRec(idx - 1);
    }

    /* element search -------------------------------------------------------*/
    int indexOfRec(int i, T item) const
    {
        if (i >= count)
            return -1;
        if (compare(elements[i], item) == 0)
            return i;
        return indexOfRec(i + 1, item);
    }

    /* removeInternalData ---------------------------------------------------*/
    void removeInternalData()
    {
        if (deleteUserData)
        {
            deleteUserData(this);
            deleteUserData = 0;
        }
        delete[] elements;
    }

    /* deep copy ------------------------------------------------------------*/
    void copyFrom(const Heap<T> &h)
    {
        capacity = h.capacity;
        count = h.count;
        comparator = h.comparator;
        deleteUserData = 0;
        elements = new T[capacity];
        copyArrayRec(h.elements, elements, 0, count);
    }

    /* generic array copy recursions ---------------------------------------*/
    static void copyArrayRec(T *src, T *dst, int idx, int n)
    {
        if (idx >= n)
            return;
        dst[idx] = src[idx];
        copyArrayRec(src, dst, idx + 1, n);
    }

    void copyRawRec(T *src, int idx, int n)
    {
        if (idx >= n)
            return;
        elements[idx] = src[idx];
        copyRawRec(src, idx + 1, n);
    }

    static void freeRec(T *arr, int idx, int n)
    {
        if (idx >= n)
            return;
        delete arr[idx];
        freeRec(arr, idx + 1, n);
    }

    /* toString helper ------------------------------------------------------*/
    void buildStringRec(stringstream &os, int idx,
                        string (*item2str)(T &)) const
    {
        if (idx >= count)
            return;
        if (idx)
            os << ",";
        if (item2str)
            os << item2str(elements[idx]);
        else
            os << elements[idx];
        buildStringRec(os, idx + 1, item2str);
    }

    /* heapsort helpers -----------------------------------------------------*/
    static void copyListToArrayRec(XArrayList<T> &list, T *arr, int idx, int n)
    {
        if (idx >= n)
            return;
        arr[idx] = list.get(idx);
        copyListToArrayRec(list, arr, idx + 1, n);
    }

    void pushArrayRec(T *arr, int idx, int n)
    {
        if (idx >= n)
            return;
        push(arr[idx]);
        pushArrayRec(arr, idx + 1, n);
    }

    void popToListRec(XArrayList<T> &list, int remaining)
    {
        if (remaining == 0)
            return;
        list.add(pop());
        popToListRec(list, remaining - 1);
    }
};

#endif /* HEAP_H */
