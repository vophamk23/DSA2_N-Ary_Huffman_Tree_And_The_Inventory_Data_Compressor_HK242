# 🎓 DSA242 – Bài Tập Lớn 2

### *Cây Huffman Đa Nhánh & Bộ Nén Dữ Liệu Kho Hàng*

> 📘 Môn học: Cấu trúc Dữ liệu & Giải thuật
> 🏛️ Trường Đại học Bách Khoa – ĐHQG TP.HCM
> 🧑‍💻 Học kỳ 2 – Năm học 2024–2025

---

## 🌟 Giới thiệu

Bài tập lớn này không đơn thuần là một bài kiểm tra kỹ năng lập trình — nó là một **dự án mini** đưa sinh viên vào hành trình khám phá những ứng dụng thực tiễn của **cấu trúc dữ liệu** trong việc **nén dữ liệu** hiệu quả bằng **cây Huffman đa nhánh**.

Sinh viên không chỉ học cách hiện thực `Heap` hay `HashMap`, mà còn hiểu sâu về cách các cấu trúc này hoạt động cùng nhau để phục vụ một mục tiêu lớn hơn: **giảm kích thước lưu trữ dữ liệu kho hàng một cách tối ưu**.

---

## 🧭 Mục tiêu chính

### ✅ TASK 1 – Cài đặt Cấu trúc Dữ liệu (60% điểm)

* **Bảng băm (xMap)** – xử lý đụng độ bằng danh sách liên kết kép.
* **Heap tổng quát** – hỗ trợ min-heap và max-heap qua comparator.

### ✅ TASK 2 – Ứng dụng: Huffman & Nén Dữ liệu (40% điểm)

* **Xây dựng cây Huffman đa nhánh**
* **Nén và giải nén dữ liệu sản phẩm**

---

## 🗂️ Cấu trúc Dự án

```plaintext
.
├── include/                  # Chứa tất cả các file header
│   ├── hash/xMap.h          # Bảng băm
│   ├── heap/Heap.h          # Heap tổng quát
│   ├── app/                 # HuffmanTree & InventoryCompressor
│   └── list/                # Tái sử dụng từ BTL1: DLinkedList, XArrayList
│
├── src/                     # Source code chính
│   ├── main.cpp             # Hàm main chạy chương trình
│   ├── test/                # Test case (nếu có)
│
├── DSA242-A2-vi (v1.1).pdf  # Đề bài tiếng Việt
├── DSA242-A2-en (v1.1).pdf  # Đề bài tiếng Anh
├── main                    # File thực thi đã build
```

---

## 🏗️ Chi tiết triển khai

### 🔧 `xMap<K,V>` – Bảng Băm

* Tự thiết kế bảng băm sử dụng **template** và danh sách liên kết kép.
* Tự động **rehash** khi vượt `loadFactor`.
* Đảm bảo hỗ trợ đầy đủ API như `put`, `get`, `remove`, `clashes`, `keys`, v.v.

### 🔧 `Heap<T>` – Min/Max Heap

* Dạng cây nhị phân đầy đủ triển khai bằng mảng động.
* Hỗ trợ **comparator** để chuyển đổi giữa min-heap và max-heap.
* Có thể sử dụng cho `heapsort` và xây dựng cây Huffman.

### 🌲 `HuffmanTree<treeOrder>`

* Cây Huffman tổng quát với `treeOrder` nhánh.
* Tự động bổ sung ký tự giả để bảo toàn tính đầy đủ của cây.
* Cho phép sinh mã Huffman và decode chuỗi mã về nội dung ban đầu.

### 📦 `InventoryCompressor<treeOrder>`

* Giao tiếp với `InventoryManager`.
* Chuyển đổi dữ liệu sản phẩm thành chuỗi, nén bằng Huffman, và giải nén về dạng ban đầu.

---

## 🔁 Dòng đời dữ liệu

```plaintext
[InventoryManager] 
      ↓
productToString()
      ↓
[buildHuffman()] ─────→ [HuffmanTree] → Huffman mã hoá
      ↓                               ↓
encodeHuffman()                 decodeHuffman()
      ↓                               ↓
[Mã hoá]                        [Chuỗi gốc sản phẩm]
```

---

## 🧪 Cách chạy và biên dịch

```bash
g++ -g -I include -I src -std=c++17 src/main.cpp src/test/* -o main
./main
```

## 💡 Ví dụ cây Huffman 3-nhánh

Giả sử các ký tự và tần suất như sau:

```plaintext
A: 5, B: 9, C: 12, D: 13
```

Thêm 1 ký tự giả → xây cây → thu được mã Huffman:

```plaintext
A: "21"
B: "22"
C: "0"
D: "1"
```

---

## 🧾 Tham khảo

* [Huffman Coding – Chidokun Blog](https://chidokun.github.io/2021/07/huffman-coding-p1/)
* [Stanford CS106X - Huffman Supplement PDF](https://web.stanford.edu/class/archive/cs/cs106x/cs106x.1192/resources/minibrowser2/huffman-encoding-supplement.pdf)

---

## 👤 Tác giả

```markdown
👨‍💻 Họ tên      : Phạm Công Võ
🎓 MSSV         : 2313946 
📁 Lớp          : Computer Engineering - HCMUT
```

