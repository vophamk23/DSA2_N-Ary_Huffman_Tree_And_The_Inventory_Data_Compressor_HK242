#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include "list/XArrayList.h"  // our concrete IList implementation
#include "list/DLinkedList.h" // could also be used, but here we use XArrayList for simplicity
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

// -------------------- List1D --------------------
template <typename T>
class List1D
{
private:
    // Underlying list implementation (using XArrayList here)
    // Using concrete type for proper copy operations.
    XArrayList<T> *pList;

public:
    List1D();
    List1D(int num_elements);
    List1D(const T *array, int num_elements);
    List1D(const List1D<T> &other);
    virtual ~List1D();

    int size() const;
    T get(int index) const;
    void set(int index, T value);
    void add(const T &value);
    string toString() const;

    // NEW: Remove element at index.
    void removeAt(int index);

    /*
     * Overload toán tử << để hỗ trợ in danh sách
     * @param os Luồng xuất
     * @param list Danh sách cần in
     * @return Tham chiếu đến luồng xuất
     */
    friend ostream &operator<<(ostream &os, const List1D<T> &list)
    {
        os << list.toString();
        return os;
    }
};
// -------------------- List2D --------------------
template <typename T>
class List2D
{
private:
    // Underlying matrix: a list of pointers to List1D<T>
    IList<List1D<T> *> *pMatrix;

public:
    List2D();
    List2D(List1D<T> *array, int num_rows);
    List2D(const List2D<T> &other);
    virtual ~List2D();

    int rows() const;
    void setRow(int rowIndex, const List1D<T> &row);
    T get(int rowIndex, int colIndex) const;
    List1D<T> getRow(int rowIndex) const;
    string toString() const;

    // NEW: Add a new row.
    void addRow(List1D<T> *row);
    // NEW: Remove a row at rowIndex.
    void removeRow(int rowIndex);

    /*
     * Overload toán tử << để hỗ trợ in danh sách
     * @param os Luồng xuất
     * @param list Danh sách cần in
     * @return Tham chiếu đến luồng xuất
     */
    friend ostream &operator<<(ostream &os, const List2D<T> &matrix)
    {
        os << matrix.toString();
        return os;
    }
};
// -------------------- InventoryAttribute --------------------
struct InventoryAttribute
{
    string name;
    double value;
    InventoryAttribute() : name(""), value(0.0) {} // Constructor mặc định
    InventoryAttribute(const string &name, double value) : name(name), value(value) {}
    string toString() const { return name + ": " + to_string(value); }

    /////////////// Toán tử == cho InventoryAttribute//////////////////////
    bool operator==(const InventoryAttribute &other) const
    {
        return name == other.name && value == other.value;
    }
};

//////////////////// Toán tử << cho InventoryAttribute//////////////////////
inline ostream &operator<<(ostream &os, const InventoryAttribute &attr)
{
    os << attr.name << ": " << attr.value;
    return os;
}

// -------------------- InventoryManager --------------------
class InventoryManager
{
private:
    List2D<InventoryAttribute> attributesMatrix;
    List1D<string> productNames;
    List1D<int> quantities;

public:
    InventoryManager();
    InventoryManager(const List2D<InventoryAttribute> &matrix,
                     const List1D<string> &names,
                     const List1D<int> &quantities);
    InventoryManager(const InventoryManager &other);

    int size() const;
    List1D<InventoryAttribute> getProductAttributes(int index) const;
    string getProductName(int index) const;
    int getProductQuantity(int index) const;
    void updateQuantity(int index, int newQuantity);
    void addProduct(const List1D<InventoryAttribute> &attributes, const string &name, int quantity);
    void removeProduct(int index);

    // Query by attribute index (zero-based) within [minValue, maxValue] and with quantity at least minQuantity.
    // Result is sorted lexicographically if ascending is true.
    List1D<string> query(string attributeName, const double &minValue,
                         const double &maxValue, int minQuantity, bool ascending) const;

    void removeDuplicates();

    static InventoryManager merge(const InventoryManager &inv1,
                                  const InventoryManager &inv2);

    void split(InventoryManager &section1,
               InventoryManager &section2,
               double ratio) const;

    List2D<InventoryAttribute> getAttributesMatrix() const;
    List1D<string> getProductNames() const;
    List1D<int> getQuantities() const;
    string toString() const;
};
//////////////// -- -- -- -- -- --List1D Method Definitions-- -- -- -- -- --////////////////////////////////
// List1D default constructor: creates an empty list.
template <typename T>
inline List1D<T>::List1D()
{
    pList = new XArrayList<T>();
}

/**
 * Hàm khởi tạo với số lượng phần tử
 * @param num_elements Số lượng phần tử cần khởi tạo
 * Tạo một danh sách với num_elements phần tử, mỗi phần tử có giá trị mặc định
 */
template <typename T>
inline List1D<T>::List1D(int num_elements)
{
    pList = new XArrayList<T>();
    for (int i = 0; i < num_elements; i++)
    {
        pList->add(T()); // Thêm giá trị mặc định của kiểu T
    }
}

/*
 * Hàm khởi tạo từ mảng
 * @param array Con trỏ đến mảng các phần tử kiểu T
 * @param num_elements Số lượng phần tử trong mảng
 * Tạo một danh sách chứa các phần tử từ mảng array
 */
template <typename T>
inline List1D<T>::List1D(const T *array, int num_elements)
{
    pList = new XArrayList<T>();
    for (int i = 0; i < num_elements; i++)
    {
        pList->add(array[i]); // Thêm từng phần tử từ mảng vào danh sách
    }
}

/*
 * Copy constructor: Tạo bản sao của danh sách other
 * @param other Danh sách cần sao chép
 * Thực hiện deep copy để đảm bảo danh sách mới độc lập với danh sách gốc
 */
template <typename T>
inline List1D<T>::List1D(const List1D<T> &other)
{
    pList = new XArrayList<T>();
    for (int i = 0; i < other.size(); i++)
    {
        pList->add(other.get(i)); // Sao chép từng phần tử từ danh sách other
    }
}
/*
 * Destructor: Giải phóng bộ nhớ đã cấp phát
 * Xóa đối tượng pList để tránh rò rỉ bộ nhớ
 */
template <typename T>
List1D<T>::~List1D()
{
    delete pList;
}

/*
 * Trả về số lượng phần tử trong danh sách
 * @return Số lượng phần tử
 */
template <typename T>
int List1D<T>::size() const
{
    return pList->size();
}

/*
 * Truy xuất phần tử tại vị trí index
 * @param index Vị trí cần truy xuất
 * @return Giá trị phần tử tại vị trí index
 * @throws out_of_range Nếu index nằm ngoài phạm vi hợp lệ
 */
template <typename T>
inline T List1D<T>::get(int index) const
{
    if (index < 0 || index >= pList->size())
    {
        throw out_of_range("Index out of range"); // Kiểm tra ràng buộc
    }
    return pList->get(index);
}

/*
 * Gán giá trị mới cho phần tử tại vị trí index
 * @param index Vị trí cần cập nhật
 * @param value Giá trị mới
 * @throws out_of_range Nếu index nằm ngoài phạm vi hợp lệ
 */
template <typename T>
inline void List1D<T>::set(int index, T value)
{
    if (index < 0 || index >= pList->size())
    {
        throw out_of_range("Index out of range");
    }

    // Tạo một danh sách mới
    IList<T> *newList = new XArrayList<T>();

    // Sao chép tất cả các phần tử, thay thế phần tử tại vị trí index
    for (int i = 0; i < pList->size(); i++)
    {
        if (i == index)
        {
            newList->add(value);
        }
        else
        {
            newList->add(pList->get(i));
        }
    }

    // Xóa nội dung của pList và thêm lại từ newList
    pList->clear();
    for (int i = 0; i < newList->size(); i++)
    {
        pList->add(newList->get(i));
    }

    // Giải phóng bộ nhớ
    delete newList;
}
/*
 * Thêm một phần tử vào cuối danh sách
 * @param value Giá trị cần thêm
 */
template <typename T>
inline void List1D<T>::add(const T &value)
{
    pList->add(value);
}

/*
 * Tạo chuỗi biểu diễn của danh sách
 * @return Chuỗi biểu diễn theo định dạng: [e1, e2, e3, ..., en]
 */
// Hàm phụ trợ để xử lý InventoryAttribute
template <typename T>
inline void appendToStream(std::ostringstream &oss, const T &value)
{
    oss << value;
}

// Chuyên biệt hóa cho InventoryAttribute
template <>
inline void appendToStream<InventoryAttribute>(std::ostringstream &oss, const InventoryAttribute &attr)
{
    oss << attr.name << ": " << attr.value;
}

template <typename T>
inline string List1D<T>::toString() const
{
    ostringstream oss;
    oss << "[";
    for (int i = 0; i < size(); i++)
    {
        appendToStream(oss, get(i));

        if (i < size() - 1)
        {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}
// NEW: Removes the element at index by calling the underlying list method.
template <typename T>
inline void List1D<T>::removeAt(int index)
{
    pList->removeAt(index);
}
// -------------------- Định nghĩa Phương thức List2D --------------------
/*
 * Hàm khởi tạo mặc định: Tạo một ma trận rỗng
 * Khởi tạo pMatrix là một XArrayList mới không chứa phần tử nào
 */
template <typename T>
inline List2D<T>::List2D()
{
    pMatrix = new XArrayList<List1D<T> *>();
}

/*
 * Hàm khởi tạo từ một mảng các List1D
 * @param array Con trỏ đến mảng các đối tượng List1D<T>
 * @param num_rows Số lượng hàng cần khởi tạo
 * Tạo một ma trận từ mảng các List1D, mỗi List1D sẽ trở thành một hàng trong ma trận
 */
template <typename T>
List2D<T>::List2D(List1D<T> *array, int num_rows)
{
    // Tạo một danh sách mới để lưu các hàng
    pMatrix = new XArrayList<List1D<T> *>();

    // Kiểm tra xem mảng đầu vào có hợp lệ không
    if (array == nullptr || num_rows <= 0)
    {
        // Nếu mảng rỗng hoặc số hàng không hợp lệ, báo lỗi
        throw std::invalid_argument("Không thể tạo ma trận với mảng rỗng");
    }

    // Duyệt qua từng hàng của mảng
    for (int i = 0; i < num_rows; i++)
    {
        // Tạo một bản sao của hàng hiện tại
        // Điều này đảm bảo mỗi hàng trong ma trận đều là một bản sao độc lập
        List1D<T> *newRow = new List1D<T>(array[i]);

        // Thêm hàng mới vào ma trận
        pMatrix->add(newRow);
    }
}

/*
 * Copy constructor: Tạo bản sao của ma trận other
 * @param other Ma trận cần sao chép
 * Thực hiện deep copy để đảm bảo ma trận mới độc lập với ma trận gốc
 */
template <typename T>
List2D<T>::List2D(const List2D<T> &other)
{
    // Tạo một danh sách mới để lưu các hàng
    pMatrix = new XArrayList<List1D<T> *>();

    // Kiểm tra xem ma trận nguồn có hàng không
    int soHang = other.rows();

    // Sao chép từng hàng của ma trận
    for (int i = 0; i < soHang; i++)
    {
        // Lấy hàng từ ma trận nguồn
        List1D<T> hangHienTai = other.getRow(i);

        // Tạo bản sao của hàng
        List1D<T> *hangSaoChep = new List1D<T>(hangHienTai);

        // Thêm hàng sao chép vào ma trận mới
        pMatrix->add(hangSaoChep);
    }
}
/*
 * Destructor: Giải phóng bộ nhớ đã cấp phát
 * Xóa từng danh sách hàng và sau đó xóa ma trận chính để tránh rò rỉ bộ nhớ
 */
template <typename T>
List2D<T>::~List2D()
{
    // Duyệt qua từng hàng trong ma trận và giải phóng bộ nhớ
    for (int i = 0; i < pMatrix->size(); i++)
    {
        delete pMatrix->get(i); // Giải phóng bộ nhớ của từng hàng
    }

    // Giải phóng bộ nhớ của ma trận chính
    delete pMatrix;
}

/*
 * Trả về số lượng hàng của ma trận
 * @return Số lượng hàng
 */
template <typename T>
int List2D<T>::rows() const
{
    return pMatrix->size();
}
// Thay thế 1 hàng tại vị trí rowIndex.
// Thay thế 1 hàng tại vị trí rowIndex.
template <typename T>
void List2D<T>::setRow(int rowIndex, const List1D<T> &row)
{
    // Kiểm tra xem rowIndex có hợp lệ không
    if (rowIndex < 0 || rowIndex >= pMatrix->size())
    {
        throw out_of_range("Row index out of range");
    }

    // Lấy hàng cũ để giải phóng bộ nhớ sau khi thay thế
    List1D<T> *oldRow = pMatrix->get(rowIndex);

    // Tạo một danh sách mới để lưu trữ các phần tử của hàng mới
    List1D<T> *newRow = new List1D<T>(row);

    // Thêm từng phần tử từ row vào danh sách mới
    for (int i = 0; i < row.size(); i++)
    {
        T element = row.get(i);
        newRow->add(element);
    }

    // Sử dụng tham chiếu từ get để thay đổi trực tiếp
    pMatrix->get(rowIndex) = newRow;

    // Giải phóng bộ nhớ của hàng cũ
    delete oldRow;
}

/*
 * Truy xuất giá trị tại vị trí xác định bởi rowIndex và colIndex
 * @param rowIndex Chỉ số hàng
 * @param colIndex Chỉ số cột
 * @return Giá trị tại vị trí (rowIndex, colIndex)
 * @throws out_of_range Nếu rowIndex hoặc colIndex nằm ngoài phạm vi hợp lệ
 */
template <typename T>
T List2D<T>::get(int rowIndex, int colIndex) const
{
    // Kiểm tra xem rowIndex có hợp lệ không
    if (rowIndex < 0 || rowIndex >= pMatrix->size())
    {
        throw out_of_range("Row index out of range");
    }

    // Lấy hàng tại vị trí rowIndex
    List1D<T> *row = pMatrix->get(rowIndex);

    // Kiểm tra xem colIndex có hợp lệ không
    if (colIndex < 0 || colIndex >= row->size())
    {
        throw out_of_range("Column index out of range");
    }

    // Trả về giá trị tại vị trí (rowIndex, colIndex)
    return row->get(colIndex);
}

/*
 * Trả về hàng của ma trận dưới dạng đối tượng List1D<T>
 * @param rowIndex Chỉ số hàng cần truy xuất
 * @return Đối tượng List1D<T> chứa dữ liệu của hàng
 * @throws out_of_range Nếu rowIndex nằm ngoài phạm vi hợp lệ
 */
template <typename T>
List1D<T> List2D<T>::getRow(int rowIndex) const
{
    // Kiểm tra xem rowIndex có hợp lệ không
    if (rowIndex < 0 || rowIndex >= pMatrix->size())
    {
        throw out_of_range("Row index out of range");
    }

    // Tạo một đối tượng List1D mới
    List1D<T> result;

    // Lấy hàng tại vị trí rowIndex
    List1D<T> *row = pMatrix->get(rowIndex);

    // Thêm từng phần tử của hàng vào đối tượng List1D mới
    for (int i = 0; i < row->size(); i++)
    {
        result.add(row->get(i));
    }

    // Trả về đối tượng List1D đã tạo
    return result;
}

/*
 * Tạo chuỗi biểu diễn của ma trận
 * @return Chuỗi biểu diễn theo định dạng: [[e11, e12, ...], [e21, e22, ...], ..., [em1, em2, ...]]
 */
template <typename T>
string List2D<T>::toString() const
{
    ostringstream oss;
    oss << "[";

    // Duyệt qua từng hàng trong ma trận
    for (int i = 0; i < pMatrix->size(); i++)
    {
        // Lấy hàng tại vị trí i
        List1D<T> *row = pMatrix->get(i);

        oss << "[";

        // Duyệt qua từng phần tử trong hàng
        for (int j = 0; j < row->size(); j++)
        {
            oss << row->get(j);

            // Thêm dấu phẩy nếu không phải phần tử cuối cùng trong hàng
            if (j < row->size() - 1)
            {
                oss << ", ";
            }
        }

        oss << "]";

        // Thêm dấu phẩy nếu không phải hàng cuối cùng
        if (i < pMatrix->size() - 1)
        {
            oss << ", ";
        }
    }

    oss << "]";
    return oss.str();
}

// NEW: Adds a new row to the matrix.
template <typename T>
void List2D<T>::addRow(List1D<T> *row)
{
    pMatrix->add(row);
}

// NEW: Removes and deletes the row at rowIndex.
template <typename T>
void List2D<T>::removeRow(int rowIndex)
{
    if (rowIndex < 0 || rowIndex >= pMatrix->size())
        throw out_of_range("Row index is out of range!");
    delete pMatrix->removeAt(rowIndex);
}
//////////////// -- -- -- -- -- --InventoryManager Method Definitions-- -- -- -- -- --////////////////////////////////
// Constructor mặc định: Khởi tạo các thành viên
inline InventoryManager::InventoryManager()
{
    // Không cần làm gì đặc biệt vì các thành viên attributesMatrix, productNames, quantities
    // đã được khởi tạo mặc định bởi constructor của List2D và List1D
}
// Constructor with parameters.
inline InventoryManager::InventoryManager(const List2D<InventoryAttribute> &matrix,
                                          const List1D<string> &names,
                                          const List1D<int> &quantities)
    : attributesMatrix(matrix), productNames(names), quantities(quantities) {}

inline InventoryManager::InventoryManager(const InventoryManager &other)
{
    // Sao chép từng thành viên từ đối tượng other
    this->attributesMatrix = other.attributesMatrix;
    this->productNames = other.productNames;
    this->quantities = other.quantities;
}

inline int InventoryManager::size() const
{
    // Trả về kích thước của productNames (có thể dùng attributesMatrix.rows() hoặc quantities.size())
    return productNames.size();
}
inline List1D<InventoryAttribute> InventoryManager::getProductAttributes(int index) const
{
    // Kiểm tra chỉ số hợp lệ: từ 0 đến size() - 1
    if (index < 0 || index >= attributesMatrix.rows())
    {
        throw out_of_range("Index is invalid!");
    }
    // Trả về hàng tại chỉ số index từ attributesMatrix
    return attributesMatrix.getRow(index);
}
inline string InventoryManager::getProductName(int index) const
{
    // Kiểm tra chỉ số hợp lệ
    if (index < 0 || index >= size())
    {
        throw out_of_range("Index is invalid!");
    }
    // Trả về tên sản phẩm tại index
    return productNames.get(index);
}

inline int InventoryManager::getProductQuantity(int index) const
{
    // Kiểm tra chỉ số hợp lệ
    if (index < 0 || index >= quantities.size())
    {
        throw out_of_range("Index is invalid!");
    }
    // Trả về số lượng tại index
    return quantities.get(index);
}

inline void InventoryManager::updateQuantity(int index, int newQuantity)
{
    // Kiểm tra chỉ số hợp lệ
    if (index < 0 || index >= quantities.size())
    {
        throw out_of_range("Index is invalid!");
    }
    // Cập nhật số lượng mới
    quantities.set(index, newQuantity);
}

// Adds a new product.
inline void InventoryManager::addProduct(const List1D<InventoryAttribute> &attributes, const string &name, int quantity)
{
    // Create a new List1D for attributes (copy).
    List1D<InventoryAttribute> *newRow = new List1D<InventoryAttribute>(attributes);
    // Use the public addRow method.
    attributesMatrix.addRow(newRow);
    productNames.add(name);
    quantities.add(quantity);
}

// Removes a product at the given index.
inline void InventoryManager::removeProduct(int index)
{
    if (index < 0 || index >= size())
        throw out_of_range("Index is invalid!");
    // Remove the corresponding row using the public removeRow method.
    attributesMatrix.removeRow(index);
    productNames.removeAt(index);
    quantities.removeAt(index);
}

inline List1D<string> InventoryManager::query(
    string attributeName,
    const double &minValue,
    const double &maxValue,
    int minQuantity,
    bool ascending) const
{
    List1D<string> matchedProducts;

    // Duyệt qua từng sản phẩm trong kho
    for (int i = 0; i < size(); i++)
    {
        // Kiểm tra số lượng tồn kho
        if (quantities.get(i) < minQuantity)
            continue;

        // Lấy danh sách thuộc tính của sản phẩm
        List1D<InventoryAttribute> productAttributes =
            attributesMatrix.getRow(i);
        bool foundMatchingAttribute = false;

        // Duyệt qua từng thuộc tính để tìm tên trùng khớp
        for (int j = 0; j < productAttributes.size(); j++)
        {
            InventoryAttribute attribute = productAttributes.get(j);

            // Kiểm tra tên thuộc tính và giá trị
            if (attribute.name == attributeName &&
                attribute.value >= minValue &&
                attribute.value <= maxValue)
            {
                matchedProducts.add(productNames.get(i));
                break; // Tìm thấy thuộc tính phù hợp, chuyển sang sản phẩm tiếp theo
            }
        }
    }

    // Sắp xếp kết quả
    for (int i = 0; i < matchedProducts.size() - 1; i++)
    {
        for (int j = 0; j < matchedProducts.size() - i - 1; j++)
        {
            bool shouldSwap = ascending
                                  ? (matchedProducts.get(j) > matchedProducts.get(j + 1))
                                  : (matchedProducts.get(j) < matchedProducts.get(j + 1));

            if (shouldSwap)
            {
                string temp = matchedProducts.get(j);
                matchedProducts.set(j, matchedProducts.get(j + 1));
                matchedProducts.set(j + 1, temp);
            }
        }
    }

    return matchedProducts;
}
inline void InventoryManager::removeDuplicates()
{
    int totalCount = size();
    for (int current = 0; current < size() - 1; current++)
    {
        for (int compare = current + 1; compare < size();)
        {
            // So sánh thuộc tính và tên sản phẩm
            if (attributesMatrix
                        .getRow(current)
                        .toString() == attributesMatrix
                                           .getRow(compare)
                                           .toString() &&
                productNames.get(current) == productNames.get(compare))
            {

                // Cộng số lượng sản phẩm
                int combinedQuantity = quantities.get(current) + quantities.get(compare);
                quantities.set(current, combinedQuantity);

                // Loại bỏ sản phẩm trùng
                removeProduct(compare);
                totalCount--;
            }
            else
            {
                compare++;
            }
        }
    }
}
inline InventoryManager InventoryManager::merge(const InventoryManager &inv1, const InventoryManager &inv2)
{
    InventoryManager result = inv1;
    for (int i = 0; i < inv2.size(); i++)
    {
        result.addProduct(inv2.getProductAttributes(i), inv2.getProductName(i), inv2.getProductQuantity(i));
    }
    result.removeDuplicates(); // Loại bỏ các sản phẩm trùng lặp
    return result;
}

inline void InventoryManager::split(InventoryManager &section1, InventoryManager &section2, double ratio) const
{
    int total = size();
    int size1 = (total * ratio + (total * ratio > 0)); // Làm tròn lên
    int size2 = total - size1;
    section1 = InventoryManager();
    section2 = InventoryManager();
    for (int i = 0; i < size1; i++)
    {
        section1.addProduct(getProductAttributes(i), getProductName(i), getProductQuantity(i));
    }
    for (int i = size1; i < total; i++)
    {
        section2.addProduct(getProductAttributes(i), getProductName(i), getProductQuantity(i));
    }
}

inline List2D<InventoryAttribute> InventoryManager::getAttributesMatrix() const
{
    return attributesMatrix;
}

inline List1D<string> InventoryManager::getProductNames() const
{
    return productNames;
}

inline List1D<int> InventoryManager::getQuantities() const
{
    return quantities;
}
inline string InventoryManager::toString() const
{
    stringstream ss;
    ss << "InventoryManager[" << endl;

    // AttributesMatrix
    ss << "  AttributesMatrix: [";
    for (int i = 0; i < attributesMatrix.rows(); i++)
    {
        ss << "[";
        List1D<InventoryAttribute> attributeRow = attributesMatrix.getRow(i);
        bool firstAttr = true;

        for (int j = 0; j < attributeRow.size(); j++)
        {
            if (!firstAttr)
                ss << ", ";
            InventoryAttribute attr = attributeRow.get(j);
            ss << attr.name << ": ";
            // Định dạng số với 6 chữ số thập phân
            ss << fixed << setprecision(6) << attr.value;
            firstAttr = false;
        }

        ss << "]";
        if (i < attributesMatrix.rows() - 1)
            ss << ", ";
    }
    ss << "]," << endl;

    // ProductNames
    ss << "  ProductNames: [";
    for (int i = 0; i < productNames.size(); i++)
    {
        ss << productNames.get(i);
        if (i < productNames.size() - 1)
            ss << ", ";
    }
    ss << "]," << endl;

    // Quantities
    ss << "  Quantities: [";
    for (int i = 0; i < quantities.size(); i++)
    {
        ss << quantities.get(i);
        if (i < quantities.size() - 1)
            ss << ", ";
    }
    ss << "]" << endl;

    ss << "]";
    return ss.str();
}

#endif /* INVENTORY_MANAGER_H */
