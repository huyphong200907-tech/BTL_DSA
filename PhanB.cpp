#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <windows.h> 

using namespace std;
using namespace std::chrono;

// ==========================================
// 1. NODE DÙNG CHUNG
// ==========================================
template <typename T>
struct Node {
    T data;
    Node* next;
    Node(T val) : data(val), next(nullptr) {}
};

// ==========================================
// 2. NGĂN XẾP TỰ CÀI ĐẶT
// ==========================================
template <typename T>
class MyStack {
private:
    Node<T>* topNode;
    size_t count;

public:
    MyStack() : topNode(nullptr), count(0) {}
    MyStack(const MyStack&) = delete;
    MyStack& operator=(const MyStack&) = delete;
    ~MyStack() { while (!empty()) pop(); }

    void push(T val) {
        Node<T>* newNode = new Node<T>(val);
        newNode->next = topNode;
        topNode = newNode;
        count++;
    }

    void pop() {
        if (empty()) return;
        Node<T>* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
    }

    T top() const {
        if (empty()) throw runtime_error("Stack is empty");
        return topNode->data;
    }

    bool empty() const { return topNode == nullptr; }
    size_t size() const { return count; }
};

// ==========================================
// 3. HÀNG ĐỢI TỰ CÀI ĐẶT
// ==========================================
template <typename T>
class MyQueue {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t count;

public:
    MyQueue() : head(nullptr), tail(nullptr), count(0) {}
    MyQueue(const MyQueue&) = delete;
    MyQueue& operator=(const MyQueue&) = delete;
    ~MyQueue() { while (!empty()) pop(); }

    void push(T val) {
        Node<T>* newNode = new Node<T>(val);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    void pop() {
        if (empty()) return;
        Node<T>* temp = head;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete temp;
        count--;
    }

    T front() const {
        if (empty()) throw runtime_error("Queue is empty");
        return head->data;
    }

    bool empty() const { return head == nullptr; }
    size_t size() const { return count; }

    // Hoán đổi toàn bộ nội dung hai hàng đợi trong O(1)
    void swap(MyQueue& o) {
        Node<T>* h = head; head = o.head; o.head = h;
        Node<T>* t = tail; tail = o.tail; o.tail = t;
        size_t c = count;  count = o.count; o.count = c;
    }

    // Cách làm trực tiếp cho getMax: duyệt toàn bộ hàng đợi, O(n)
    T maxElement() const {
        if (empty()) throw runtime_error("Queue is empty");
        T m = head->data;
        for (Node<T>* p = head->next; p != nullptr; p = p->next) {
            if (p->data > m) m = p->data;
        }
        return m;
    }
};

// ==========================================
// YÊU CẦU 2: HÀNG ĐỢI DÙNG HAI NGĂN XẾP
// ==========================================
template <typename T>
class QueueUsingTwoStacks {
private:
    MyStack<T> s1;
    MyStack<T> s2;

    void transfer() {
        while (!s1.empty()) {
            s2.push(s1.top());
            s1.pop();
        }
    }

public:
    void push(T val) { s1.push(val); }

    void pop() {
        if (s2.empty()) transfer();
        if (!s2.empty()) s2.pop();
    }

    T front() {
        if (s2.empty()) transfer();
        if (s2.empty()) throw runtime_error("Queue is empty");
        return s2.top();
    }

    bool empty() const { return s1.empty() && s2.empty(); }
    size_t size() const { return s1.size() + s2.size(); }
};

// ==========================================
// YÊU CẦU 3: NGĂN XẾP DÙNG HAI HÀNG ĐỢI
// push: O(n), pop/top: O(1)
// ==========================================
template <typename T>
class StackUsingTwoQueues {
private:
    MyQueue<T> q1;
    MyQueue<T> q2;

public:
    void push(T val) {
        q2.push(val);
        while (!q1.empty()) {
            q2.push(q1.front());
            q1.pop();
        }
        q1.swap(q2); // O(1), q2 trở thành rỗng
    }

    void pop() { if (!q1.empty()) q1.pop(); }

    T top() {
        if (q1.empty()) throw runtime_error("Stack is empty");
        return q1.front();
    }

    bool empty() const { return q1.empty(); }
    size_t size() const { return q1.size(); }
};

// ==========================================
// YÊU CẦU 4: HÀNG ĐỢI CÓ getMax O(1) TRUNG BÌNH
// ==========================================
template <typename T>
class MaxStack {
private:
    struct MaxNode {
        T data;
        T maxVal;
        MaxNode* next;
        MaxNode(T d, T m, MaxNode* n) : data(d), maxVal(m), next(n) {}
    };
    MaxNode* topNode;
    size_t count;

public:
    MaxStack() : topNode(nullptr), count(0) {}
    MaxStack(const MaxStack&) = delete;
    MaxStack& operator=(const MaxStack&) = delete;
    ~MaxStack() { while (!empty()) pop(); }

    void push(T val) {
        T m = val;
        if (topNode != nullptr && topNode->maxVal > val) m = topNode->maxVal;
        topNode = new MaxNode(val, m, topNode);
        count++;
    }

    void pop() {
        if (empty()) return;
        MaxNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
    }

    T top() const {
        if (empty()) throw runtime_error("Stack is empty");
        return topNode->data;
    }

    T getMax() const {
        if (empty()) throw runtime_error("Stack is empty");
        return topNode->maxVal;
    }

    bool empty() const { return topNode == nullptr; }
    size_t size() const { return count; }
};

template <typename T>
class MaxQueue {
private:
    MaxStack<T> s1;
    MaxStack<T> s2;

    void transfer() {
        while (!s1.empty()) {
            s2.push(s1.top());
            s1.pop();
        }
    }

public:
    void push(T val) { s1.push(val); }

    void pop() {
        if (s2.empty()) transfer();
        if (!s2.empty()) s2.pop();
    }

    T front() {
        if (s2.empty()) transfer();
        if (s2.empty()) throw runtime_error("Queue is empty");
        return s2.top();
    }

    T getMax() {
        if (s1.empty() && s2.empty()) throw runtime_error("Queue is empty");
        if (s1.empty()) return s2.getMax();
        if (s2.empty()) return s1.getMax();
        T m1 = s1.getMax();
        T m2 = s2.getMax();
        return (m1 > m2) ? m1 : m2;
    }

    bool empty() const { return s1.empty() && s2.empty(); }
    size_t size() const { return s1.size() + s2.size(); }
};

// ==========================================
// YÊU CẦU 5: ĐỌC TRUY VẤN TỪ FILE, KIỂM TRA ĐÚNG, SO SÁNH THỜI GIAN
// Định dạng file: dòng 1 là n. Mỗi truy vấn tiếp theo:
//   1 x : push x
//   2   : pop
//   3   : lấy phần tử cần hỏi (front / top / getMax tùy phần test)
// ==========================================
struct Query {
    int type;
    int val;
};

// Các "thao tác hỏi" cho từng loại cấu trúc
struct FrontOp   { template <typename S> int operator()(S& s) const { return s.front(); } };
struct TopOp     { template <typename S> int operator()(S& s) const { return s.top(); } };
struct GetMaxOp  { template <typename S> int operator()(S& s) const { return s.getMax(); } };
struct SlowMaxOp { template <typename S> int operator()(S& s) const { return s.maxElement(); } };

// Sinh file test nếu chưa có
void generateFile(const string& filename, int n) {
    ofstream fout(filename);
    fout << n << "\n";
    srand(12345);
    for (int i = 0; i < n; ++i) {
        int r = rand() % 10;
        if (r < 5)      fout << "1 " << (rand() % 1000000) << "\n";
        else if (r < 8) fout << "2\n";
        else            fout << "3\n";
    }
}

// Chạy song song hai cấu trúc, trả về số lần hai bên cho kết quả khác nhau
template <typename A, typename B, typename OpA, typename OpB>
int checkEqual(A& a, B& b, const Query* qs, int n, OpA opA, OpB opB) {
    int mismatch = 0;
    for (int i = 0; i < n; ++i) {
        if (qs[i].type == 1) {
            a.push(qs[i].val);
            b.push(qs[i].val);
        } else if (qs[i].type == 2) {
            if (!a.empty()) a.pop();
            if (!b.empty()) b.pop();
        } else if (qs[i].type == 3) {
            if (!a.empty() && !b.empty()) {
                if (opA(a) != opB(b)) ++mismatch;
            }
        }
        if (a.empty() != b.empty() || a.size() != b.size()) ++mismatch;
    }
    return mismatch;
}

// Chạy một cấu trúc và đo thời gian (chỉ đo vòng lặp xử lý, không tính đọc file)
template <typename S, typename Op>
long long runTimed(S& s, const Query* qs, int n, Op op, long long& checksum) {
    checksum = 0;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        if (qs[i].type == 1) {
            s.push(qs[i].val);
        } else if (qs[i].type == 2) {
            if (!s.empty()) s.pop();
        } else if (qs[i].type == 3) {
            if (!s.empty()) checksum += op(s);
        }
    }
    auto stop = high_resolution_clock::now();
    return duration_cast<microseconds>(stop - start).count();
}

void printResult(const string& name, long long us, long long checksum) {
    cout << "   " << name << ": " << us << " us (checksum = " << checksum << ")\n";
}

void printVerdict(int mismatch) {
    if (mismatch == 0) cout << "   Kiểm tra tính đúng: ĐẠT\n";
    else               cout << "   Kiểm tra tính đúng: SAI (" << mismatch << " chỗ khác nhau)\n";
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
    const string filename = "input.txt";
    const int STACK_LIMIT = 10000;   // push của StackUsingTwoQueues là O(n)
    const int MAX_LIMIT   = 100000;  // getMax duyệt O(n) rất chậm với n lớn

    {
        ifstream test(filename);
        if (!test) {
            cout << "Không thấy " << filename << ", tự sinh file test 1000000 truy vấn.\n";
            test.close();
            generateFile(filename, 1000000);
        }
    }

    // Đọc toàn bộ truy vấn vào bộ nhớ trước để tách thời gian I/O khỏi thời gian đo
    ifstream fin(filename);
    if (!fin) {
        cout << "Không thể mở file " << filename << "\n";
        return 1;
    }
    int n;
    fin >> n;
    Query* qs = new Query[n];
    for (int i = 0; i < n; ++i) {
        fin >> qs[i].type;
        if (qs[i].type == 1) fin >> qs[i].val;
        else qs[i].val = 0;
    }
    fin.close();

    cout << "=== SO SÁNH HIỆU NĂNG CÁC CẤU TRÚC TỰ CÀI ĐẶT (n = " << n << ") ===\n";

    // ---------- Cặp 1: MyQueue (trực tiếp) và QueueUsingTwoStacks ----------
    cout << "\n[1] Hàng đợi: cài trực tiếp và dùng hai ngăn xếp\n";
    {
        MyQueue<int> a;
        QueueUsingTwoStacks<int> b;
        printVerdict(checkEqual(a, b, qs, n, FrontOp(), FrontOp()));
    }
    {
        MyQueue<int> a;
        long long c;
        long long t = runTimed(a, qs, n, FrontOp(), c);
        printResult("MyQueue (trực tiếp)    ", t, c);
    }
    {
        QueueUsingTwoStacks<int> b;
        long long c;
        long long t = runTimed(b, qs, n, FrontOp(), c);
        printResult("QueueUsingTwoStacks    ", t, c);
    }

    // ---------- Cặp 2: MyStack (trực tiếp) và StackUsingTwoQueues ----------
    int nStack = (n < STACK_LIMIT) ? n : STACK_LIMIT;
    cout << "\n[2] Ngăn xếp: cài trực tiếp và dùng hai hàng đợi (chỉ chạy " << nStack
         << " truy vấn đầu vì push là O(n))\n";
    {
        MyStack<int> a;
        StackUsingTwoQueues<int> b;
        printVerdict(checkEqual(a, b, qs, nStack, TopOp(), TopOp()));
    }
    {
        MyStack<int> a;
        long long c;
        long long t = runTimed(a, qs, nStack, TopOp(), c);
        printResult("MyStack (trực tiếp)    ", t, c);
    }
    {
        StackUsingTwoQueues<int> b;
        long long c;
        long long t = runTimed(b, qs, nStack, TopOp(), c);
        printResult("StackUsingTwoQueues    ", t, c);
    }

    // ---------- Cặp 3: MaxQueue (getMax O(1)) và duyệt O(n) ----------
    int nMax = (n < MAX_LIMIT) ? n : MAX_LIMIT;
    cout << "\n[3] getMax: MaxQueue O(1) và duyệt toàn bộ O(n) (chạy " << nMax << " truy vấn đầu)\n";
    {
        MaxQueue<int> a;
        MyQueue<int> b;
        printVerdict(checkEqual(a, b, qs, nMax, GetMaxOp(), SlowMaxOp()));
    }
    {
        MaxQueue<int> a;
        long long c;
        long long t = runTimed(a, qs, nMax, GetMaxOp(), c);
        printResult("MaxQueue (O(1))        ", t, c);
    }
    {
        MyQueue<int> b;
        long long c;
        long long t = runTimed(b, qs, nMax, SlowMaxOp(), c);
        printResult("MyQueue + duyệt O(n)   ", t, c);
    }

    cout << "\n===============================================================\n";
    delete[] qs;
    return 0;
}