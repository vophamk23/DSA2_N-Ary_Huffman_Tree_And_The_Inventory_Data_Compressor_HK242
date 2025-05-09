
#ifndef INVENTORY_COMPRESSOR_H
#define INVENTORY_COMPRESSOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <utility>
#include "inventory.h"
#include "hash/xMap.h"
#include "heap/Heap.h"
#include "list/XArrayList.h"

template <int treeOrder>
class HuffmanTree
{
public:
    struct HuffmanNode
    {
        char symbol;
        int freq;
        long id; // Thêm ID làm tie-breaker
        XArrayList<HuffmanNode *> children;
        static long nextId; // Static counter để đảm bảo ID tăng dần

        HuffmanNode(char s, int f);                                  // Leaf node
        HuffmanNode(int f, const XArrayList<HuffmanNode *> &childs); // Internal node
    };

    HuffmanTree();
    ~HuffmanTree();

    void build(XArrayList<pair<char, int>> &symbolsFreqs);
    void generateCodes(xMap<char, std::string> &table);
    std::string decode(const std::string &huffmanCode);

private:
    HuffmanNode *root;
    void generateCodesRecursive(HuffmanNode *node, std::string code, xMap<char, std::string> &table);
    void destroyTree(HuffmanNode *node);
};

template <int treeOrder>
class InventoryCompressor
{
public:
    InventoryCompressor(InventoryManager *manager);
    ~InventoryCompressor();

    void buildHuffman();
    void printHuffmanTable();
    std::string productToString(const List1D<InventoryAttribute> &attributes, const std::string &name);
    std::string encodeHuffman(const List1D<InventoryAttribute> &attributes, const std::string &name);
    std::string decodeHuffman(const std::string &huffmanCode, List1D<InventoryAttribute> &attributesOutput, std::string &nameOutput);
    // Define these functions outside the constructor
private:
    xMap<char, std::string> *huffmanTable;
    InventoryManager *invManager;
    HuffmanTree<treeOrder> *tree;
};

#endif // INVENTORY_COMPRESSOR_H
template <int treeOrder>
HuffmanTree<treeOrder>::HuffmanNode::HuffmanNode(char s, int f) : symbol(s), freq(f)
{
    // Constructor cho nút lá
}

template <int treeOrder>
HuffmanTree<treeOrder>::HuffmanNode::HuffmanNode(int f, const XArrayList<HuffmanNode *> &childs) : symbol('\0'), freq(f)
{
    // Phương án 1: Sử dụng constructor sao chép để tạo bản sao tạm thời của childs
    XArrayList<HuffmanNode *> tempCopy = childs; // Gọi constructor sao chép

    // Bây giờ có thể sử dụng get() trên tempCopy vì nó không phải const
    for (int i = 0; i < tempCopy.size(); i++)
    {
        HuffmanNode *child = tempCopy.get(i);
        this->children.add(child);
    }

    // Phương án 2: Sử dụng iterator (nếu bạn thích cách tiếp cận này hơn)
    // XArrayList<HuffmanNode *> tempCopy = childs;  // Gọi constructor sao chép
    //
    // for (typename XArrayList<HuffmanNode *>::Iterator it = tempCopy.begin();
    //      it != tempCopy.end(); ++it)
    // {
    //     HuffmanNode *child = *it;
    //     this->children.add(child);
    // }
}

///////////////////////////// Triển khai HuffmanTree ////////////////////////
template <int treeOrder>
HuffmanTree<treeOrder>::HuffmanTree() : root(nullptr)
{
    // Khởi tạo cây Huffman với nút gốc là nullptr
}

template <int treeOrder>
HuffmanTree<treeOrder>::~HuffmanTree()
{
    // Giải phóng bộ nhớ cho toàn bộ cây
    if (root != nullptr)
    {
        destroyTree(root);
    }
}

// Khai báo các phương thức để hiện thực đầy đủ lớp HuffmanTree

// Phương thức xây dựng cây Huffman từ danh sách ký tự và tần số
// Sử dụng XArrayList để chứa danh sách ký tự và tần số
// Sử dụng pair<char, int> để lưu ký tự và tần số tương ứng
// Sử dụng std::string để lưu mã Huffman cho từng ký tự
// Sử dụng xMap để lưu bảng mã Huffman
// Sử dụng Heap để xây dựng cây Huffman từ danh sách ký tự và tần số (*******)
template <int treeOrder>
void HuffmanTree<treeOrder>::build(XArrayList<pair<char, int>> &symbolsFreqs)
{
    // Tạo hàng đợi ưu tiên (min heap) với bộ so sánh tùy chỉnh
    // Hàng đợi này sẽ lưu các nút của cây Huffman, sắp xếp theo tần suất và các tiêu chí khác
    Heap<HuffmanNode *> heap(
        [](HuffmanNode *&a, HuffmanNode *&b) -> int
        {
            // So sánh theo tần suất trước tiên (tần suất nhỏ hơn được ưu tiên)
            if (a->freq < b->freq)
                return -1;
            if (a->freq > b->freq)
                return 1;

            // Nếu tần suất bằng nhau, ưu tiên nút lá trước nút trong
            bool aLeaf = a->children.empty();
            bool bLeaf = b->children.empty();
            if (aLeaf != bLeaf)
                return aLeaf ? -1 : 1;

            // Nếu cả hai là nút lá, so sánh theo mã ASCII của ký tự
            if (aLeaf)
            {
                unsigned char ua = static_cast<unsigned char>(a->symbol);
                unsigned char ub = static_cast<unsigned char>(b->symbol);
                if (ua < ub)
                    return -1;
                if (ua > ub)
                    return 1;
            }

            // Nếu vẫn bằng nhau, so sánh theo ID
            if (a->id < b->id)
                return -1;
            if (a->id > b->id)
                return 1;

            // Hai nút hoàn toàn giống nhau
            return 0;
        });

    // Thêm tất cả các ký tự và tần suất vào hàng đợi ưu tiên
    int L = static_cast<int>(symbolsFreqs.size());
    for (int i = 0; i < L; i++)
    {
        std::pair<char, int> &pr = symbolsFreqs.get(i);
        heap.push(new HuffmanNode(pr.first, pr.second));
    }

    // Thêm các nút giả để đảm bảo thuật toán hoạt động đúng với cây đa nhánh (treeOrder > 2)
    // Khi tạo cây đa nhánh, cần (treeOrder - 1) nút con cùng lúc để tạo 1 nút cha
    // Nếu số nút không chia hết cho (treeOrder - 1), cần thêm các nút giả
    int mod = (L - 1) % (treeOrder - 1);
    if (mod != 0)
    {
        int pad = (treeOrder - 1) - mod;
        while (pad-- > 0)
            heap.push(new HuffmanNode('\0', 0)); // Thêm nút giả với ký tự '\0' và tần suất 0
    }

    // Xây dựng cây Huffman theo thuật toán:
    // 1. Lấy treeOrder nút có tần suất thấp nhất từ hàng đợi
    // 2. Tạo nút cha mới có tần suất là tổng tần suất của các nút con
    // 3. Thêm nút cha mới vào hàng đợi
    // 4. Lặp lại cho đến khi hàng đợi chỉ còn 1 nút (nút gốc)
    while (heap.size() > 1)
    {
        // Lấy tối đa treeOrder nút từ hàng đợi để tạo một nhóm
        XArrayList<HuffmanNode *> group;
        for (int k = 0; k < treeOrder && heap.size() > 0; k++)
        {
            group.add(heap.pop());
        }

        // Tính tổng tần suất của các nút trong nhóm
        int sum = 0;
        for (int gi = 0; gi < group.size(); gi++)
        {
            sum += group.get(gi)->freq;
        }

        // Tạo nút cha mới với tổng tần suất và danh sách các nút con
        heap.push(new HuffmanNode(sum, group));
    }

    // Lấy nút gốc từ hàng đợi (hoặc NULL nếu hàng đợi trống)
    root = heap.empty() ? NULL : heap.pop();
}

// Phương thức tạo mã Huffman cho từng ký tự
// Sử dụng xMap để lưu bảng mã Huffman
// Sử dụng std::string để lưu mã Huffman cho từng ký tự
template <int treeOrder>
void HuffmanTree<treeOrder>::generateCodes(xMap<char, std::string> &table)
{
    // Xóa bảng cũ
    table.clear();

    // Kiểm tra nếu cây rỗng
    if (root == nullptr)
    {
        return;
    }

    // Trường hợp đặc biệt: chỉ có một ký tự (một nút lá)
    if (root->children.size() == 0)
    {
        // Khi có một ký tự duy nhất trong cây có bậc treeOrder
        // Gán mã là ký tự cuối cùng của hệ cơ số tương ứng với treeOrder
        // Ví dụ: treeOrder = 10 => mã là '9', treeOrder = 16 => mã là 'f'
        char code = (treeOrder <= 10) ? ('0' + treeOrder - 1) : ('a' + treeOrder - 11);
        table.put(root->symbol, std::string(1, code));
        return;
    }

    // Tạo mã Huffman cho mỗi ký tự trong cây
    generateCodesRecursive(root, "", table);
}
template <int treeOrder>
std::string HuffmanTree<treeOrder>::decode(const std::string &huffmanCode)
{
    // Kiểm tra nếu cây rỗng hoặc mã rỗng
    if (root == nullptr || huffmanCode.empty())
    {
        return std::string(1, '\0'); // Trả về null character
    }

    // Trường hợp đặc biệt: nếu chỉ có một nút (một ký tự)
    if (root->children.size() == 0)
    {
        // Kiểm tra mã cho cây một nút dựa vào treeOrder
        char expectedCode = (treeOrder <= 10) ? ('0' + treeOrder - 1) : ('a' + treeOrder - 11);
        if (huffmanCode == std::string(1, expectedCode))
        {
            return std::string(1, root->symbol);
        }
        return std::string(1, '\0'); // Mã không hợp lệ cho cây một nút
    }

    std::string decodedText;     // Chuỗi kết quả giải mã
    HuffmanNode *current = root; // Bắt đầu từ nút gốc

    // Duyệt qua từng ký tự trong mã Huffman
    for (size_t i = 0; i < huffmanCode.length(); ++i)
    {
        // Chuyển đổi ký tự mã sang chỉ số con
        int childIndex;
        char c = huffmanCode[i];

        // Xử lý ký tự số (0-9)
        if (c >= '0' && c <= '9')
        {
            childIndex = c - '0';
        }
        // Xử lý ký tự chữ (a-f)
        else if (c >= 'a' && c <= 'f')
        {
            childIndex = 10 + (c - 'a');
        }
        else
        {
            // Ký tự không hợp lệ trong mã Huffman
            return std::string(1, '\0');
        }

        // Kiểm tra chỉ số con có hợp lệ không
        if (childIndex < 0 || childIndex >= current->children.size())
        {
            // Chỉ số con không hợp lệ
            return std::string(1, '\0');
        }

        // Di chuyển tới nút con tương ứng với mã
        current = current->children.get(childIndex);

        // Nếu đến nút lá và không phải ký tự giả, thêm ký tự vào kết quả và quay về nút gốc
        if (current->children.size() == 0)
        {
            if (current->symbol == '\0')
            {
                return std::string(1, '\0'); // Gặp nút lá nhưng là ký tự giả → mã không hợp lệ
            }
            decodedText += current->symbol;
            current = root;
        }
    }

    // Kiểm tra xem đã giải mã hoàn toàn hay chưa
    if (current != root)
    {
        // Mã không kết thúc tại nút lá, có thể không hợp lệ
        return std::string(1, '\0');
    }

    // Nếu không giải mã được gì, trả về null character
    return decodedText.empty() ? std::string(1, '\0') : decodedText;
}

/*
// Giải mã mã Huffman
// Trả về chuỗi ký tự đã giải mã hoặc null character nếu không hợp lệ
// Nếu mã không hợp lệ, trả về chuỗi chứa ký tự null (char(0))
// Nếu mã hợp lệ nhưng không giải mã được gì, trả về chuỗi rỗng
// Nếu mã hợp lệ và giải mã thành công, trả về chuỗi đã giải mã
template <int treeOrder>
std::string HuffmanTree<treeOrder>::decode(const std::string &huffmanCode)
{
    // Kiểm tra nếu cây rỗng hoặc mã rỗng
    if (root == nullptr || huffmanCode.empty())
    {
        return std::string(1, '\0'); // Trả về null character
    }

    // Trường hợp đặc biệt: nếu chỉ có một nút (một ký tự)
    if (root->children.size() == 0)
    {
        // Theo đề bài, với trường hợp cây chỉ có một nút, mã "2" sẽ giải mã thành ký tự đó
        if (huffmanCode == "2")
        {
            return std::string(1, root->symbol);
        }
        return std::string(1, '\0'); // Mã không hợp lệ cho cây một nút
    }

    std::string decodedText;     // Chuỗi kết quả giải mã
    HuffmanNode *current = root; // Bắt đầu từ nút gốc

    // Duyệt qua từng ký tự trong mã Huffman
    for (size_t i = 0; i < huffmanCode.length(); ++i)
    {
        // Chuyển đổi ký tự mã sang chỉ số con
        int childIndex;
        char c = huffmanCode[i];

        // Xử lý ký tự số (0-9)
        if (c >= '0' && c <= '9')
        {
            childIndex = c - '0';
        }
        // Xử lý ký tự chữ (a-f)
        else if (c >= 'a' && c <= 'f')
        {
            childIndex = 10 + (c - 'a');
        }
        else
        {
            // Ký tự không hợp lệ trong mã Huffman
            return std::string(1, '\0');
        }

        // Kiểm tra chỉ số con có hợp lệ không
        if (childIndex < 0 || childIndex >= current->children.size())
        {
            // Chỉ số con không hợp lệ
            return std::string(1, '\0');
        }

        // Di chuyển tới nút con tương ứng với mã
        current = current->children.get(childIndex);

        // Nếu đến nút lá và không phải ký tự giả, thêm ký tự vào kết quả và quay về nút gốc
        if (current->children.size() == 0)
        {
            if (current->symbol == '\0')
            {
                return std::string(1, '\0'); // Gặp nút lá nhưng là ký tự giả → mã không hợp lệ
            }
            decodedText += current->symbol;
            current = root;
        }
    }

    // Kiểm tra xem đã giải mã hoàn toàn hay chưa
    if (current != root)
    {
        // Mã không kết thúc tại nút lá, có thể không hợp lệ
        return std::string(1, '\0');
    }

    // Nếu không giải mã được gì, trả về null character
    return decodedText.empty() ? std::string(1, '\0') : decodedText;
}*/

// Hàm đệ quy để tạo mã Huffman cho từng ký tự
// Sử dụng std::string để lưu mã Huffman cho từng ký tự
template <int treeOrder>
void HuffmanTree<treeOrder>::generateCodesRecursive(HuffmanNode *node, std::string code, xMap<char, std::string> &table)
{

    if (node == nullptr)
    {
        return;
    }
    // Nếu là nút lá và không phải ký tự giả, lưu mã vào bảng
    if (node->children.empty())
    {
        table.put(node->symbol, code); // Lưu mã vào bảng
        return;
    }

    // Duyệt qua tất cả các nút con và gán mã tương ứng
    for (int i = 0; i < node->children.size(); i++)
    {
        // Chuyển đổi chỉ số thành ký tự mã (0-9 và a-f trong hệ 16)
        HuffmanNode *childNode = node->children.get(i);       // Lấy nút con
        char childCode = (i < 10 ? '0' + i : 'a' + (i - 10)); // Chuyển đổi chỉ số thành ký tự mã

        generateCodesRecursive(node->children.get(i), code + childCode, table);
    }
}
// Hàm giải phóng bộ nhớ cho cây Huffman
template <int treeOrder>
void HuffmanTree<treeOrder>::destroyTree(HuffmanNode *node)
{
    // Hàm đệ quy để giải phóng bộ nhớ cho cây
    if (node == nullptr)
    {
        return;
    }

    // Giải phóng bộ nhớ cho các nút con trước
    for (int i = 0; i < node->children.size(); i++)
    {
        destroyTree(node->children.get(i));
    }

    // Giải phóng bộ nhớ cho nút hiện tại
    delete node;
}
///////////////////// Triển khai InventoryCompressor ////////////////////////
template <int treeOrder>
InventoryCompressor<treeOrder>::InventoryCompressor(InventoryManager *invManager)
{
    this->invManager = invManager;
    this->tree = nullptr; // Đảm bảo tree được khởi tạo là nullptr

    // Hash function cho xMap
    auto hashFunc = [](char &key, int tableSize) -> int
    {
        return (int)key % tableSize;
    };

    // Hàm so sánh giá trị
    auto compareValues = [](std::string &v1, std::string &v2) -> bool
    {
        return v1 == v2;
    };

    // Rehash callback
    auto rehashCallback = [](xMap<char, std::string> *map)
    {
        // Không làm gì
    };

    // Hàm so sánh khóa
    auto compareKeys = [](char &k1, char &k2) -> bool
    {
        return k1 == k2;
    };

    // Cleanup callback
    auto cleanupCallback = [](xMap<char, std::string> *map)
    {
        // Không làm gì
    };

    // Khởi tạo xMap với 6 tham số
    huffmanTable = new xMap<char, std::string>(
        hashFunc,       // hash function
        0.75f,          // load factor
        compareValues,  // value equality function
        rehashCallback, // rehash callback
        compareKeys,    // key equality function
        cleanupCallback // cleanup callback
    );
}

template <int treeOrder>
InventoryCompressor<treeOrder>::~InventoryCompressor()
{
    // Giải phóng bộ nhớ đã cấp phát
    if (huffmanTable != nullptr)
    {
        delete huffmanTable;
    }

    if (tree != nullptr)
    {
        delete tree;
    }
}

// Chuyển đổi thông tin sản phẩm thành chuỗi theo định dạng: "tên sản phẩm, thuộc tính1, thuộc tính2, ..."
// Sử dụng std::string để lưu chuỗi kết quả
// Sử dụng List1D để lưu danh sách thuộc tính của sản phẩm
template <int treeOrder>
void InventoryCompressor<treeOrder>::buildHuffman()
{
    // Xóa cây cũ nếu có
    if (this->tree != nullptr)
    {
        delete this->tree;
    }
    this->tree = new HuffmanTree<treeOrder>();

    // Bảng tần số ký tự chung
    XArrayList<std::pair<char, int>> frequencies;

    // Duyệt tất cả sản phẩm trong InventoryManager
    List1D<std::string> itemNames = invManager->getProductNames();
    for (int i = 0; i < itemNames.size(); i++)
    {
        std::string name = itemNames.get(i);
        List1D<InventoryAttribute> attributes = invManager->getProductAttributes(i);

        std::string productStr = productToString(attributes, name);

        for (char c : productStr)
        {
            bool found = false;
            for (int j = 0; j < frequencies.size(); j++)
            {
                if (frequencies.get(j).first == c)
                {
                    auto pair = frequencies.get(j);
                    frequencies.removeAt(j);
                    frequencies.add(std::make_pair(c, pair.second + 1));
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                frequencies.add(std::make_pair(c, 1));
            }
        }
    }

    // Sắp xếp bảng tần số bằng Insertion Sort đơn giản
    // Tạo một danh sách mới đã sắp xếp
    XArrayList<std::pair<char, int>> sortedFreq;

    // Duyệt qua từng phần tử trong frequencies
    for (int i = 0; i < frequencies.size(); i++)
    {
        auto current = frequencies.get(i);
        bool inserted = false;

        // Tìm vị trí thích hợp để chèn trong sortedFreq
        for (int j = 0; j < sortedFreq.size(); j++)
        {
            // So sánh theo tiêu chí: tần suất tăng dần, nếu bằng nhau thì so sánh ký tự
            if (current.second < sortedFreq.get(j).second ||
                (current.second == sortedFreq.get(j).second && current.first < sortedFreq.get(j).first))
            {

                // Tạo một danh sách tạm để thêm phần tử vào đúng vị trí
                XArrayList<std::pair<char, int>> tempList;

                // Copy các phần tử từ đầu đến j-1
                for (int k = 0; k < j; k++)
                {
                    tempList.add(sortedFreq.get(k));
                }

                // Thêm phần tử hiện tại
                tempList.add(current);

                // Copy các phần tử từ j đến cuối
                for (int k = j; k < sortedFreq.size(); k++)
                {
                    tempList.add(sortedFreq.get(k));
                }

                // Gán lại sortedFreq = tempList
                sortedFreq = tempList;
                inserted = true;
                break;
            }
        }

        // Nếu chưa chèn (tức là phần tử lớn nhất), thêm vào cuối
        if (!inserted)
        {
            sortedFreq.add(current);
        }
    }

    // Xây dựng cây Huffman từ bảng tần số đã sắp xếp
    this->tree->build(sortedFreq);

    // Tạo bảng mã Huffman từ cây
    this->huffmanTable->clear(); // Xóa bảng cũ nếu có
    this->tree->generateCodes(*this->huffmanTable);
}
// Sử dụng hàm này để in bảng mã Huffman cho từng ký tự (*)
template <int treeOrder>
void InventoryCompressor<treeOrder>::printHuffmanTable()
{
    // Lấy danh sách các ký tự có trong bảng mã Huffman
    DLinkedList<char> keyList = huffmanTable->keys();

    // Duyệt qua từng ký tự trong danh sách
    typename DLinkedList<char>::Iterator keyIterator = keyList.begin();
    while (keyIterator != keyList.end())
    {
        // Lấy ký tự hiện tại
        char currentChar = *keyIterator;

        // In ký tự: nếu là ký tự null thì in dạng đặc biệt
        if (currentChar == 0)
        {
            std::cout << "'\\x00' : ";
        }
        else
        {
            std::cout << "'" << currentChar << "' : ";
        }

        // Lấy mã Huffman tương ứng với ký tự và in ra
        std::string code = huffmanTable->get(currentChar);
        std::cout << code << std::endl;

        // Di chuyển đến phần tử tiếp theo
        ++keyIterator;
    }
}
// Chuyển đổi thông tin sản phẩm thành chuỗi theo định dạng: "tên sản phẩm:(thuộc tính1:giá trị1), (thuộc tính2:giá trị2), ..."
template <int treeOrder>
std::string InventoryCompressor<treeOrder>::productToString(const List1D<InventoryAttribute> &attributes, const std::string &name)
{
    // TODO: Chuyển đổi dữ liệu sản phẩm thành chuỗi theo định dạng: "tên sản phẩm:(thuộc tính1:giá trị1), (thuộc tính2:giá trị2), ..."
    std::stringstream ss;

    // Thêm tên sản phẩm vào chuỗi kết quả
    ss << name << ":";

    // Duyệt qua từng thuộc tính và thêm vào chuỗi kết quả
    for (int i = 0; i < attributes.size(); i++)
    {
        InventoryAttribute attr = attributes.get(i);
        ss << "(" << attr.name << ": " << std::fixed << std::setprecision(6) << attr.value << ")";

        // Thêm dấu phẩy và khoảng trắng giữa các thuộc tính nếu không phải thuộc tính cuối cùng
        if (i < attributes.size() - 1)
        {
            ss << ", ";
        }
    }

    return ss.str();
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::encodeHuffman(const List1D<InventoryAttribute> &attributes, const std::string &name)

{
    // TODO: Mã hóa thông tin sản phẩm thành chuỗi mã Huffman
    // Chuyển đổi sản phẩm thành chuỗi
    std::string productStr = productToString(attributes, name);

    if (productStr.empty())
    {
        return std::string();
    }

    // Chuỗi kết quả mã hóa
    std::string encodedString;

    // Mã hóa từng ký tự trong chuỗi sản phẩm
    for (size_t i = 0; i < productStr.length(); i++)
    {
        char c = productStr[i];

        // Kiểm tra xem ký tự có trong bảng mã Huffman không
        if (huffmanTable->containsKey(c))
        {
            // Thêm mã Huffman tương ứng vào chuỗi kết quả
            encodedString += huffmanTable->get(c);
        }
        else
        {
            // Nếu ký tự không có trong bảng mã Huffman, bỏ qua ký tự này
            // Thay vì throw exception có thể dẫn đến segmentation fault
            std::cerr << "Warning: Character '" << c << "' not found in Huffman table" << std::endl;
        }
    }

    return encodedString;
}

// Giải mã chuỗi Huffman thành tên sản phẩm và danh sách thuộc tính
// Nếu giải mã không thành công, trả về chuỗi rỗng
template <int treeOrder>
std::string InventoryCompressor<treeOrder>::decodeHuffman(const std::string &huffmanCode,
                                                          List1D<InventoryAttribute> &attributesOutput,
                                                          std::string &nameOutput)
{
    // Kiểm tra nếu cây Huffman chưa được xây dựng
    if (tree == nullptr)
    {
        return std::string();
    }

    // Giải mã chuỗi Huffman
    std::string decodedText = tree->decode(huffmanCode);

    // Nếu giải mã không thành công (trả về null character)
    if (decodedText.length() == 1 && decodedText[0] == '\0')
    {
        return std::string();
    }

    // Phân tích chuỗi giải mã
    std::string productName;
    List1D<InventoryAttribute> attributes;

    // Tìm vị trí dấu ':'
    size_t colonPos = decodedText.find(':');
    if (colonPos != std::string::npos)
    {
        // Lấy tên sản phẩm
        productName = decodedText.substr(0, colonPos);

        // Phân tích thuộc tính
        std::string attrStr = decodedText.substr(colonPos + 1);
        size_t pos = 0;

        while (pos < attrStr.length())
        {
            // Tìm bắt đầu thuộc tính
            size_t openParenPos = attrStr.find('(', pos);
            if (openParenPos == std::string::npos)
                break;

            // Tìm kết thúc thuộc tính
            size_t closeParenPos = attrStr.find(')', openParenPos);
            if (closeParenPos == std::string::npos)
                break;

            // Lấy chuỗi thuộc tính
            std::string attributeStr = attrStr.substr(openParenPos + 1, closeParenPos - openParenPos - 1);

            // Tìm dấu ':' trong thuộc tính
            size_t attrColonPos = attributeStr.find(':');
            if (attrColonPos != std::string::npos)
            {
                std::string attrName = attributeStr.substr(0, attrColonPos);
                std::string attrValStr = attributeStr.substr(attrColonPos + 1);

                // Loại bỏ khoảng trắng
                attrName.erase(0, attrName.find_first_not_of(" "));
                attrName.erase(attrName.find_last_not_of(" ") + 1);
                attrValStr.erase(0, attrValStr.find_first_not_of(" "));
                attrValStr.erase(attrValStr.find_last_not_of(" ") + 1);

                // Chuyển đổi giá trị thành số
                double attrVal = std::stod(attrValStr);

                // Thêm thuộc tính vào danh sách
                attributes.add(InventoryAttribute(attrName, attrVal));
            }

            // Cập nhật vị trí
            pos = closeParenPos + 1;
        }
    }

    // Gán kết quả ra tham số đầu ra
    nameOutput = productName;
    attributesOutput = attributes;

    return decodedText;
}