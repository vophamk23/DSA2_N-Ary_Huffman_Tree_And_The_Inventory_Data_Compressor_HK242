#ifndef INVENTORY_COMPRESSOR_H
#define INVENTORY_COMPRESSOR_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility> // std::pair

#include "app/inventory.h"   // InventoryManager, InventoryAttribute
#include "hash/xMap.h"       // xMap<K,V>
#include "heap/Heap.h"       // Heap<T>
#include "list/XArrayList.h" // XArrayList<T>

namespace std
{
    template <typename U, typename V>
    ostream &operator<<(ostream &os, const pair<U, V> &p)
    {
        return os << "(" << p.first << "," << p.second << ")";
    }
}

/*─────────────────  N‑ARY HUFFMAN TREE  ─────────────────*/

template <int treeOrder>
class HuffmanTree
{
public:
    struct HuffmanNode
    {
        char symbol;
        int freq;
        long id; // insertion order – tie breaker
        XArrayList<HuffmanNode *> children;
        static long nextId;

        HuffmanNode(char s, int f)
            : symbol(s), freq(f), id(nextId++), children() {}
        HuffmanNode(int f, const XArrayList<HuffmanNode *> &c)
            : symbol('\0'), freq(f), id(nextId++), children(c) {}
    };

    HuffmanTree() : root(NULL) {}
    ~HuffmanTree() { destroyNode(root); }

    /* build ----------------------------------------------------------------*/
    void build(XArrayList<std::pair<char, int>> &syms)
    {
        /* custom comparator   */
        Heap<HuffmanNode *> heap(
            [](HuffmanNode *&a, HuffmanNode *&b) -> int
            {
                if (a->freq < b->freq)
                    return -1;
                if (a->freq > b->freq)
                    return 1;

                bool aLeaf = a->children.empty();
                bool bLeaf = b->children.empty();
                if (aLeaf != bLeaf)
                    return aLeaf ? -1 : 1;

                if (aLeaf)
                {
                    unsigned char ua = static_cast<unsigned char>(a->symbol);
                    unsigned char ub = static_cast<unsigned char>(b->symbol);
                    if (ua < ub)
                        return -1;
                    if (ua > ub)
                        return 1;
                }
                if (a->id < b->id)
                    return -1;
                if (a->id > b->id)
                    return 1;
                return 0;
            });

        int L = static_cast<int>(syms.size());
        int i = 0;
        while (i < L)
        {
            std::pair<char, int> &pr = syms.get(i);
            heap.push(new HuffmanNode(pr.first, pr.second));
            ++i;
        }

        int mod = (L - 1) % (treeOrder - 1);
        if (mod != 0)
        {
            int pad = (treeOrder - 1) - mod;
            while (pad-- > 0)
                heap.push(new HuffmanNode('\0', 0));
        }

        while (heap.size() > 1)
        {
            XArrayList<HuffmanNode *> group;
            int k = 0;
            while (k < treeOrder && heap.size() > 0)
            {
                group.add(heap.pop());
                ++k;
            }
            int sum = 0;
            int gi = 0;
            while (gi < group.size())
            {
                sum += group.get(gi)->freq;
                ++gi;
            }
            heap.push(new HuffmanNode(sum, group));
        }
        root = heap.empty() ? NULL : heap.pop();
    }

    /* generate code table ---------------------------------------------------*/
    void generateCodes(xMap<char, std::string> &table) { dfsGenerate(root, "", table); }

    /* decode ----------------------------------------------------------------*/
    std::string decode(const std::string &code)
    {
        std::string out;
        HuffmanNode *node = root;
        size_t idx = 0;
        while (idx < code.size())
        {
            char c = code[idx++];
            int branch;
            if (c >= '0' && c <= '9')
                branch = c - '0';
            else
                branch = (c - 'a') + 10;

            if (node == NULL || branch >= node->children.size())
                throw std::runtime_error("Invalid Huffman code");

            node = node->children.get(branch);
            if (node->children.empty())
            {
                out.push_back(node->symbol);
                node = root;
            }
        }
        return out;
    }

private:
    HuffmanNode *root;

    /* helpers ---------------------------------------------------------------*/
    static void destroyNode(HuffmanNode *n)
    {
        if (n == NULL)
            return;
        int ci = 0;
        while (ci < n->children.size())
        {
            destroyNode(n->children.get(ci));
            ++ci;
        }
        delete n;
    }

    static void dfsGenerate(HuffmanNode *n, const std::string &pref,
                            xMap<char, std::string> &tbl)
    {
        if (n == NULL)
            return;
        if (n->children.empty())
        {
            tbl.put(n->symbol, pref);
            return;
        }

        int idx = 0;
        while (idx < n->children.size())
        {
            HuffmanNode *child = n->children.get(idx);
            char edge = (idx < 10 ? '0' + idx : 'a' + (idx - 10));
            dfsGenerate(child, pref + edge, tbl);
            ++idx;
        }
    }
};

template <int N>
long HuffmanTree<N>::HuffmanNode::nextId = 0;

/*─────────────────  INVENTORY COMPRESSOR  ─────────────────*/

template <int treeOrder>
class InventoryCompressor
{
public:
    explicit InventoryCompressor(InventoryManager *m)
        : invManager(m),
          huffmanTable(xMap<char, std::string>::simpleHash, 0.75f, NULL, NULL, NULL, NULL),
          tree(new HuffmanTree<treeOrder>()) {}

    ~InventoryCompressor() { delete tree; }

    void printHuffmanTable()
    {
        DLinkedList<char> ks = huffmanTable.keys();
        typename DLinkedList<char>::Iterator it = ks.begin();
        while (it != ks.end())
        {
            char c = *it;
            if (c == 0)
                std::cout << "'\\x00' : ";
            else
                std::cout << "'" << c << "' : ";
            std::cout << huffmanTable.get(c) << '\n';
            ++it;
        }
    }

    /* build Huffman --------------------------------------------------------*/
    void buildHuffman()
    {
        xMap<char, int> freq(xMap<char, int>::simpleHash, 0.75f, NULL, NULL, NULL, NULL);
        XArrayList<char> seen;

        int idx = 0;
        int total = invManager->size();
        while (idx < total)
        {
            List1D<InventoryAttribute> attrs = invManager->getProductAttributes(idx);
            std::string name = invManager->getProductName(idx);
            std::string s = productToString(attrs, name);
            ++idx;

            size_t j = 0;
            while (j < s.size())
            {
                char c = s[j++];
                if (freq.containsKey(c))
                    freq.put(c, freq.get(c) + 1);
                else
                {
                    freq.put(c, 1);
                    seen.add(c);
                }
            }
        }

        XArrayList<std::pair<char, int>> syms;
        int k = 0;
        while (k < seen.size())
        {
            char c = seen.get(k++);
            syms.add(std::make_pair(c, freq.get(c)));
        }
        tree->build(syms);
        tree->generateCodes(huffmanTable);
    }

    /* stringify a product --------------------------------------------------*/
    std::string productToString(const List1D<InventoryAttribute> &attrs,
                                const std::string &name)
    {
        std::ostringstream oss;
        oss << name << ":";
        int i = 0;
        int n = attrs.size();
        while (i < n)
        {
            InventoryAttribute a = attrs.get(i);
            oss << "(" << a.name << ": " << std::fixed
                << std::setprecision(6) << a.value << ")";
            ++i;
            if (i < n)
                oss << ", ";
        }
        return oss.str();
    }

    /* encode ---------------------------------------------------------------*/
    std::string encodeHuffman(const List1D<InventoryAttribute> &attrs,
                              const std::string &name)
    {
        std::string plain = productToString(attrs, name);
        std::string coded;
        coded.reserve(plain.size() * 2);
        size_t i = 0;
        while (i < plain.size())
        {
            coded += huffmanTable.get(plain[i]);
            ++i;
        }
        return coded;
    }

    /* decode ---------------------------------------------------------------*/
    std::string decodeHuffman(const std::string &code,
                              List1D<InventoryAttribute> &attrsOut,
                              std::string &nameOut)
    {
        std::string dec = tree->decode(code);
        size_t pos = dec.find(':');
        if (pos == std::string::npos)
        {
            nameOut = dec;
            attrsOut = List1D<InventoryAttribute>();
            return dec;
        }

        nameOut = dec.substr(0, pos);
        std::string rest = dec.substr(pos + 1);
        parseAttributes(rest, attrsOut);
        return dec;
    }

private:
    InventoryManager *invManager;
    HuffmanTree<treeOrder> *tree;
    xMap<char, std::string> huffmanTable;

    static void parseAttributes(const std::string &rest,
                                List1D<InventoryAttribute> &out)
    {
        std::istringstream iss(rest);
        std::string tok;
        out = List1D<InventoryAttribute>();

        while (std::getline(iss, tok, ','))
        {
            size_t f = tok.find('(');
            size_t l = tok.find(')');
            if (f == std::string::npos || l <= f)
                continue;

            std::string inner = tok.substr(f + 1, l - f - 1);
            size_t col = inner.find(':');
            if (col == std::string::npos)
                continue;

            std::string key = inner.substr(0, col);
            std::string valStr = inner.substr(col + 1);
            double val = std::stod(valStr);
            out.add(InventoryAttribute(key, val));
        }
    }
};

#endif /* INVENTORY_COMPRESSOR_H */