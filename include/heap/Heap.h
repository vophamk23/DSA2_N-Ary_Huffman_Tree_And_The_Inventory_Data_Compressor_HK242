/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Heap.h
 * Author: LTSACH
 *
 * Created on 22 August 2020, 18:18
 */

#ifndef HEAP_H
#define HEAP_H
#include <memory.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "heap/IHeap.h"
#include "list/XArrayList.h"
using namespace std;
/*
 * function pointer: int (*comparator)(T& lhs, T& rhs)
 *      compares objects of type T given in lhs and rhs.
 *      return: sign of (lhs - rhs)
 *              -1: lhs < rhs
 *              0 : lhs == rhs
 *              +1: ls > rhs
 *
 * function pointer: void (*deleteUserData)(Heap<T>* pHeap)
 *      remove user's data in case that T is a pointer type
 *      Users should pass &Heap<T>::free for "deleteUserData"
 *
 */
template <class T>
class Heap : public IHeap<T>
{
public:
  class Iterator; // forward declaration

protected:
  T *elements;                            // a dynamic array to contain user's data
  int capacity;                           // size of the dynamic array
  int count;                              // current count of elements stored in this heap
  int (*comparator)(T &lhs, T &rhs);      // see above
  void (*deleteUserData)(Heap<T> *pHeap); // see above

public:
  Heap(int (*comparator)(T &, T &) = 0, void (*deleteUserData)(Heap<T> *) = 0);

  Heap(const Heap<T> &heap);               // copy constructor
  Heap<T> &operator=(const Heap<T> &heap); // assignment operator

  ~Heap();

  // Inherit from IHeap: BEGIN
  void push(T item);
  T pop();
  const T peek();
  void remove(T item, void (*removeItemData)(T) = 0);
  bool contains(T item);
  int size();
  void heapify(T array[], int size);
  void clear();
  bool empty();
  string toString(string (*item2str)(T &) = 0);
  // Inherit from IHeap: END
  void heapsort(XArrayList<T> &arrayList);
  void println(string (*item2str)(T &) = 0)
  {
    cout << toString(item2str) << endl;
  }

  Iterator begin() { return Iterator(this, true); }
  Iterator end() { return Iterator(this, false); }

public:
  /* if T is pointer type:
   *     pass the address of method "free" to Heap<T>'s constructor:
   *     to:  remove the user's data (if needed)
   * Example:
   *  Heap<Point*> heap(&Heap<Point*>::free);
   *  => Destructor will call free via function pointer "deleteUserData"
   */
  static void free(Heap<T> *pHeap)
  {
    for (int idx = 0; idx < pHeap->count; idx++)
      delete pHeap->elements[idx];
  }

private:
  bool aLTb(T &a, T &b) { return compare(a, b) < 0; }
  int compare(T &a, T &b)
  {
    if (comparator != 0)
      return comparator(a, b);
    else
    {
      if (a < b)
        return -1;
      else if (a > b)
        return 1;
      else
        return 0;
    }
  }
  void ensureCapacity(int minCapacity);
  void swap(int a, int b);
  void reheapUp(int position);
  void reheapDown(int position);
  int getItem(T item);
  void removeInternalData();
  void copyFrom(const Heap<T> &heap);

  // Helper method: buildHeap
  // This method is not required but helps to make the code clearer
  void buildHeap(int startIdx = -1);
  //////////////////////////////////////////////////////////////////////
  ////////////////////////  INNER CLASSES DEFNITION ////////////////////
  //////////////////////////////////////////////////////////////////////

public:
  // Iterator: BEGIN
  class Iterator
  {
  private:
    Heap<T> *heap;
    int cursor;

  public:
    Iterator(Heap<T> *heap = 0, bool begin = 0)
    {
      this->heap = heap;
      if (begin && (heap != 0))
        cursor = 0;
      if (!begin && (heap != 0))
        cursor = heap->size();
    }
    Iterator &operator=(const Iterator &iterator)
    {
      this->heap = iterator.heap;
      this->cursor = iterator.cursor;
      return *this;
    }

    T &operator*() { return this->heap->elements[cursor]; }
    bool operator!=(const Iterator &iterator)
    {
      return this->cursor != iterator.cursor;
    }
    // Prefix ++ overload
    Iterator &operator++()
    {
      cursor++;
      return *this;
    }
    // Postfix ++ overload
    Iterator operator++(int)
    {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
    void remove(void (*removeItemData)(T) = 0)
    {
      this->heap->remove(this->heap->elements[cursor], removeItemData);
    }
  };
  // Iterator: END
};

//////////////////////////////////////////////////////////////////////
//////////////////////// METHOD DEFNITION - Public ///////////////////
//////////////////////////////////////////////////////////////////////

// Hàm khởi tạo heap với một hàm so sánh và hàm xóa dữ liệu người dùng
template <class T>
Heap<T>::Heap(
    int (*comparator)(T &, T &),
    void (*deleteUserData)(Heap<T> *))
{
  capacity = 10;                         // Dung lượng ban đầu là 10
  count = 0;                             // Số phần tử ban đầu là 0
  elements = new T[capacity];            // Tạo mảng động mới
  this->comparator = comparator;         // Gán hàm so sánh
  this->deleteUserData = deleteUserData; // Gán hàm xóa dữ liệu
}

// Hàm khởi tạo sao chép
template <class T>
Heap<T>::Heap(const Heap<T> &heap)
{
  copyFrom(heap); // Sao chép dữ liệu từ heap khác
}

// Toán tử gán
template <class T>
Heap<T> &Heap<T>::operator=(const Heap<T> &heap)
{
  removeInternalData(); // Xóa dữ liệu hiện tại
  copyFrom(heap);       // Sao chép từ heap khác
  return *this;
}

// Hàm hủy
template <class T>
Heap<T>::~Heap()
{
  removeInternalData(); // Xóa dữ liệu nội bộ
}

// Thêm phần tử vào heap
template <class T>
void Heap<T>::push(T item)
{
  ensureCapacity(count + 1); // Đảm bảo đủ dung lượng
  elements[count] = item;    // Thêm phần tử vào cuối
  count += 1;                // Tăng số lượng phần tử
  reheapUp(count - 1);       // Sắp xếp lại heap từ dưới lên
}
/*
Ví dụ hoạt động của push:
      18
     /  \
    15   13
    /
  25
 =>
      25
     /  \
    18   13
    /
  15
=> mảng: [25, 18, 13, 15, , ]
          0   1   2   3
*/

// Lấy và xóa phần tử ở đỉnh heap
template <class T>
T Heap<T>::pop()
{
  if (count == 0)
    throw std::underflow_error("Calling to peek with the empty heap.");

  T item = elements[0];              // Lưu phần tử đỉnh
  elements[0] = elements[count - 1]; // Đưa phần tử cuối lên đỉnh
  count -= 1;                        // Giảm số lượng phần tử
  reheapDown(0);                     // Sắp xếp lại heap từ trên xuống
  return item;                       // Trả về phần tử đỉnh
}

/*
Ví dụ hoạt động của pop:
      15
     /  \
    18   13
 => ReheapDown
      18
     /  \
    15   13
=> Mảng: [18, 15, 13, , , ]
*/

// Xem phần tử ở đỉnh heap
template <class T>
const T Heap<T>::peek()
{
  if (count == 0)
    throw std::underflow_error("Calling to peek with the empty heap.");
  return elements[0]; // Trả về phần tử ở đỉnh heap
}
// Xóa một phần tử khỏi heap
template <class T>
void Heap<T>::remove(T item, void (*removeItemData)(T))
{
  int foundIdx = this->getItem(item); // Tìm vị trí của phần tử

  // TH 1: Không tìm thấy
  if (foundIdx == -1)
    return;

  // TH 2: Tìm thấy tại foundIdx
  elements[foundIdx] = elements[count - 1]; // Thay thế bằng phần tử cuối
  count -= 1;                               // Giảm số lượng phần tử

  // Kiểm tra xem cần reheapUp hay reheapDown
  if (foundIdx > 0 && elements[foundIdx] < elements[(foundIdx - 1) / 2])
  {
    // Nếu phần tử mới nhỏ hơn phần tử cha, cần reheapUp
    reheapUp(foundIdx);
  }
  else
  {
    // Ngược lại, cần kiểm tra xem có cần reheapDown không
    int leftChild = 2 * foundIdx + 1;
    int rightChild = 2 * foundIdx + 2;

    // Nếu có con trái và phần tử lớn hơn con trái
    // hoặc có con phải và phần tử lớn hơn con phải
    if ((leftChild < count && elements[foundIdx] > elements[leftChild]) ||
        (rightChild < count && elements[foundIdx] > elements[rightChild]))
    {
      reheapDown(foundIdx);
    }
  }

  // Giải phóng bộ nhớ nếu có yêu cầu
  if (removeItemData != NULL)
    removeItemData(item);
}

/*
// Xóa một phần tử khỏi heap
template <class T>
void Heap<T>::remove(T item, void (*removeItemData)(T))
{
  int foundIdx = this->getItem(item); // Tìm vị trí của phần tử

  // TH 1: Không tìm thấy
  if (foundIdx == -1)
    return;

  // TH 2: Tìm thấy tại foundIdx
  elements[foundIdx] = elements[count - 1]; // Thay thế bằng phần tử cuối
  count -= 1;                               // Giảm số lượng phần tử
  reheapDown(foundIdx);                     // Sắp xếp lại heap
  if (removeItemData != NULL)
    removeItemData(item); // Giải phóng bộ nhớ phần tử
}*/

// Kiểm tra phần tử có tồn tại trong heap
template <class T>
bool Heap<T>::contains(T item)
{
  bool found = false;
  for (int idx = 0; idx < count; idx++) // Duyệt qua từng phần tử
  {
    if (compare(elements[idx], item) == 0) // So sánh bằng
    {
      found = true;
      break;
    }
  }
  return found;
}

// Trả về kích thước hiện tại của heap
template <class T>
int Heap<T>::size()
{
  return count;
}

// Chuyển đổi mảng thành heap
/*
 * heapify: convert an array into a heap
 * Example:
 *  Heap<int> heap;
 *  int array[] = {4, 2, 1, 7, 3, 9, 10, 6, 8, 5};
 *  heap.heapify(array, sizeof(array)/sizeof(int));
 * => heap: [1,2,3,4,5,6,7,8,9,10]
 */

// Chuyển đổi mảng thành heap bằng phương pháp bottom-up (Floyd's algorithm)
template <class T>
void Heap<T>::heapify(T array[], int size)
{
  // Xóa dữ liệu hiện tại nếu cần
  clear();

  // Đảm bảo có đủ dung lượng cho mảng mới
  ensureCapacity(size);

  // Sao chép dữ liệu từ mảng đầu vào
  for (int i = 0; i < size; i++)
  {
    elements[i] = array[i];
  }

  // Cập nhật số lượng phần tử
  count = size;

  // Thực hiện Floyd's algorithm: bottom-up heapify
  // Bắt đầu từ nút không phải lá cuối cùng (parent of last element)
  for (int i = (count - 2) / 2; i >= 0; i--)
  {
    reheapDown(i);
  }
}

// Xóa toàn bộ dữ liệu trong heap
template <class T>
void Heap<T>::clear()
{
  removeInternalData(); // Xóa dữ liệu nội bộ

  capacity = 10;              // Đặt lại dung lượng
  count = 0;                  // Đặt lại số lượng phần tử
  elements = new T[capacity]; // Tạo mảng mới
}

// Kiểm tra heap có rỗng hay không
template <class T>
bool Heap<T>::empty()
{
  return count == 0;
}

// Chuyển heap thành chuỗi
template <class T>
string Heap<T>::toString(string (*item2str)(T &))
{
  stringstream os;
  if (item2str != 0) // Nếu có hàm chuyển đổi
  {
    os << "[";
    for (int idx = 0; idx < count - 1; idx++)
      os << item2str(elements[idx]) << ",";
    if (count > 0)
      os << item2str(elements[count - 1]);
    os << "]";
  }
  else // Nếu không có hàm chuyển đổi
  {
    os << "[";
    for (int idx = 0; idx < count - 1; idx++)
      os << elements[idx] << ",";
    if (count > 0)
      os << elements[count - 1];
    os << "]";
  }
  return os.str();
}

// Định nghĩa phương thức heapsort theo yêu cầu
template <class T>
void Heap<T>::heapsort(XArrayList<T> &arrayList)
{
  // Xóa dữ liệu hiện tại trong heap nếu cần
  if (this->deleteUserData != 0)
    deleteUserData(this);
  count = 0; // Đặt lại số lượng phần tử

  // Lưu lại kích thước ban đầu của arrayList
  int size = arrayList.size();

  // Đảm bảo rằng heap có đủ dung lượng
  ensureCapacity(size);

  // Thêm tất cả phần tử từ arrayList vào heap
  for (int i = 0; i < size; i++)
  {
    push(arrayList.get(i));
  }

  // Tạo mảng kết quả để lưu các phần tử đã sắp xếp
  T *result = new T[size];
  for (int i = 0; i < size; i++)
  {
    result[i] = pop();
  }

  // Tạo một mảng trung gian để in từng bước và cuối cùng là kết quả
  T *intermediate = new T[size];
  for (int i = 0; i < size; i++)
  {
    intermediate[i] = arrayList.get(i);
  }

  // Hiển thị quá trình sắp xếp từng phần tử một từ trái sang phải
  for (int i = 0; i < size; i++)
  {
    // Cập nhật phần tử tại vị trí i
    intermediate[i] = result[i];

    // In ra trạng thái hiện tại sau khi cập nhật
    cout << "[";
    for (int j = 0; j < size - 1; j++)
    {
      cout << intermediate[j] << ", ";
    }
    cout << intermediate[size - 1] << "]" << endl;
  }

  // Cập nhật arrayList với kết quả đã sắp xếp
  arrayList.clear();
  for (int i = 0; i < size; i++)
  {
    arrayList.add(result[i]);
  }

  // Giải phóng bộ nhớ
  delete[] result;
  delete[] intermediate;
}

/*
//April 03, 2023
template<class T>
void Heap<T>::remove_bck(T item, void (*removeItemData)(T)){
    int foundIdx = this->getItem(item);

    //CASE 1: not found
    if(foundIdx == -1) return;

    //CASE 2: found at 0 => use pop
    if(foundIdx == 0){
        T removed = this->pop();
        if(removeItemData != 0) removeItemData(removed);
        return;
    }

    //CASE 3: found at anywhere else
    //Remove item's data
    if(removeItemData != 0) removeItemData(elements[foundIdx]);

    //Remove item: shift left
    int copyCount = (count - 1) - (foundIdx + 1) + 1;
    memcpy(&elements[foundIdx], &elements[foundIdx + 1], copyCount*sizeof(T));
    count -= 1;

    //Determine valid heap [0-> (new size -1)]
    int startOldData = foundIdx;
    int lastOldData = count - 1;
    count = foundIdx;

    //reheapify from startOldData
    for(int idx=startOldData; idx <= lastOldData; idx++)
        push(elements[idx]);
}
*/

//////////////////////////////////////////////////////////////////////
//////////////////////// CÁC PHƯƠNG THỨC PRIVATE /////////////////////
//////////////////////////////////////////////////////////////////////

// Đảm bảo mảng đủ dung lượng
template <class T>
void Heap<T>::ensureCapacity(int minCapacity)
{
  if (minCapacity >= capacity)
  {
    // Cấp phát lại
    int old_capacity = capacity;
    capacity = old_capacity + (old_capacity >> 2); // Tăng kích thước 1.25 lần
    try
    {
      T *new_data = new T[capacity]; // Tạo mảng mới
      // Sao chép dữ liệu cũ sang mảng mới
      memcpy(new_data, elements, old_capacity * sizeof(T));
      delete[] elements;   // Xóa mảng cũ
      elements = new_data; // Gán mảng mới
    }
    catch (std::bad_alloc e)
    {
      e.what();
    }
  }
}

// Hoán đổi 2 phần tử trong mảng
template <class T>
void Heap<T>::swap(int a, int b)
{
  T temp = this->elements[a];
  this->elements[a] = this->elements[b];
  this->elements[b] = temp;
}
// Sắp xếp lại heap từ dưới lên (khi thêm phần tử)
template <class T>
void Heap<T>::reheapUp(int position)
{
  if (position <= 0) // Nếu ở vị trí gốc thì dừng
    return;
  int parent = (position - 1) / 2;                            // Tính vị trí cha
  if (aLTb(this->elements[position], this->elements[parent])) // Nếu con nhỏ hơn cha
  {
    this->swap(position, parent); // Đổi chỗ con và cha
    reheapUp(parent);             // Đệ quy với vị trí cha
  }
}

// Sắp xếp lại heap từ trên xuống (khi xóa phần tử đỉnh)
template <class T>
void Heap<T>::reheapDown(int position)
{
  int leftChild = position * 2 + 1;   // Vị trí con trái
  int rightChild = position * 2 + 2;  // Vị trí con phải
  int lastPosition = this->count - 1; // Vị trí phần tử cuối

  if (leftChild <= lastPosition) // Nếu có con trái
  {
    int smallChild = leftChild;     // Mặc định con nhỏ nhất là con trái
    if (rightChild <= lastPosition) // Nếu có con phải
    {
      // Chọn con nhỏ hơn
      if (aLTb(this->elements[leftChild], this->elements[rightChild]))
        smallChild = leftChild;
      else
        smallChild = rightChild;
    }

    // Nếu con nhỏ hơn cha thì đổi chỗ
    if (aLTb(this->elements[smallChild], this->elements[position]))
    {
      this->swap(smallChild, position);
      reheapDown(smallChild); // Đệ quy với vị trí con
    }
  }
}

// Tìm vị trí của phần tử trong heap
template <class T>
int Heap<T>::getItem(T item)
{
  int foundIdx = -1;
  for (int idx = 0; idx < this->count; idx++) // Duyệt qua từng phần tử
  {
    if (compare(elements[idx], item) == 0) // So sánh bằng
    {
      foundIdx = idx;
      break;
    }
  }
  return foundIdx;
}

// Xóa dữ liệu nội bộ của heap
template <class T>
void Heap<T>::removeInternalData()
{
  if (this->deleteUserData != 0)
    deleteUserData(this); // Xóa dữ liệu người dùng nếu có
  delete[] elements;      // Xóa mảng động
}

// Sao chép dữ liệu từ heap khác
template <class T>
void Heap<T>::copyFrom(const Heap<T> &heap)
{
  capacity = heap.capacity;                   // Sao chép dung lượng
  count = heap.count;                         // Sao chép số lượng phần tử
  elements = new T[capacity];                 // Tạo mảng mới
  this->comparator = heap.comparator;         // Sao chép hàm so sánh
  this->deleteUserData = heap.deleteUserData; // Sao chép hàm xóa dữ liệu

  // Trước vòng lặp
  int heapSize = heap.count; // Nếu count có thể truy cập
  // Hoặc sử dụng một cách khác để lấy kích thước

  for (int idx = 0; idx < heapSize; idx++)
  // Copy items from heap:
  {
    this->elements[idx] = heap.elements[idx];
  }
}

/*
 * buildHeap: biến đổi một mảng bất kỳ thành heap
 * Đây là một phương thức phụ trợ (helper method) cho heapify
 * Phương thức này không bắt buộc nhưng giúp code dễ hiểu hơn
 */
template <class T>
void Heap<T>::buildHeap(int startIdx)
{
  if (startIdx < 0)
    return;

  // Sắp xếp lại heap từ vị trí hiện tại
  reheapDown(startIdx);

  // Đệ quy với vị trí trước đó
  buildHeap(startIdx - 1);
}
#endif /* HEAP_H */
