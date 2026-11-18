#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <windows.h>
using namespace std;

// ==========================================
// 0. HÀM TIỆN ÍCH 
// ==========================================

// Đếm số ký tự UTF-8 (không đếm byte) để căn cột đúng với tiếng Việt có dấu
size_t doDaiUTF8(const string& s) {
    size_t n = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) ++n;
    }
    return n;
}

// Căn trái chuỗi theo độ rộng w (tính theo ký tự, không phải byte)
string canTrai(const string& s, size_t w) {
    size_t len = doDaiUTF8(s);
    return (len >= w) ? s : s + string(w - len, ' ');
}

void xoaBoDem(istream& is) {
    is.ignore(numeric_limits<streamsize>::max(), '\n');
}

void thoatNeuHetDuLieu(istream& is) {
    if (is.eof()) {
        cout << "\nKhông còn dữ liệu đầu vào. Kết thúc chương trình.\n";
        exit(0);
    }
}

// Đọc một chuỗi không rỗng (đã cắt khoảng trắng hai đầu)
string docChuoiKhongRong(istream& is, const string& loiNhac) {
    string s;
    while (true) {
        cout << loiNhac;
        if (!getline(is, s)) thoatNeuHetDuLieu(is);
        size_t a = s.find_first_not_of(" \t\r");
        if (a != string::npos) {
            size_t b = s.find_last_not_of(" \t\r");
            return s.substr(a, b - a + 1);
        }
        cout << "  ! Không được để trống, vui lòng nhập lại.\n";
    }
}

// Đọc số nguyên trong khoảng [tu, den]
int docSoNguyen(istream& is, const string& loiNhac, int tu, int den) {
    int x;
    while (true) {
        cout << loiNhac;
        if (is >> x && x >= tu && x <= den) {
            xoaBoDem(is);
            return x;
        }
        thoatNeuHetDuLieu(is);
        is.clear();
        xoaBoDem(is);
        cout << "  ! Giá trị không hợp lệ, cần nhập số nguyên từ " << tu << " đến " << den << ".\n";
    }
}

// Đọc số thực trong khoảng [tu, den]
double docSoThuc(istream& is, const string& loiNhac, double tu, double den) {
    double x;
    while (true) {
        cout << loiNhac;
        if (is >> x && x >= tu && x <= den) {
            xoaBoDem(is);
            return x;
        }
        thoatNeuHetDuLieu(is);
        is.clear();
        xoaBoDem(is);
        cout << "  ! Giá trị không hợp lệ, cần nhập số từ " << tu << " đến " << den << ".\n";
    }
}

// ==========================================
// 1. LỚP ĐỐI TƯỢNG QUẢN LÝ: SINH VIÊN
// ==========================================
class SinhVien {
private:
    string maSV;
    string ten;
    int tuoi;
    double diem;

public:
    SinhVien() : maSV(""), ten(""), tuoi(0), diem(0.0) {}
    SinhVien(string ma, string t, int tu, double d) : maSV(ma), ten(t), tuoi(tu), diem(d) {}

    string getMaSV() const { return maSV; }
    double getDiem() const { return diem; }

    // Toán tử so sánh '<' theo điểm (bằng điểm thì so theo mã SV)
    bool operator<(const SinhVien& other) const {
        if (this->diem != other.diem) {
            return this->diem < other.diem;
        }
        return this->maSV < other.maSV;
    }

    // Toán tử nhập >> (có kiểm tra dữ liệu hợp lệ)
    friend istream& operator>>(istream& is, SinhVien& sv) {
        sv.maSV = docChuoiKhongRong(is, "Nhập mã sinh viên: ");
        sv.ten = docChuoiKhongRong(is, "Nhập tên sinh viên: ");
        sv.tuoi = docSoNguyen(is, "Nhập tuổi (1-120): ", 1, 120);
        sv.diem = docSoThuc(is, "Nhập điểm (0-10): ", 0.0, 10.0);
        return is;
    }

    // Toán tử xuất <<
    friend ostream& operator<<(ostream& os, const SinhVien& sv) {
        os << canTrai(sv.maSV, 12)
           << canTrai(sv.ten, 25)
           << left << setw(8) << sv.tuoi
           << fixed << setprecision(2) << sv.diem;
        return os;
    }
};

// ==========================================
// 2. LỚP DANH SÁCH ĐỐI TƯỢNG QUẢN LÝ
// ==========================================
class DanhSachSinhVien {
private:
    vector<SinhVien> ds; // vector có sẵn trong STL

    static void inTieuDe() {
        cout << canTrai("Mã SV", 12)
             << canTrai("Họ và Tên", 25)
             << canTrai("Tuổi", 8)
             << "Điểm" << "\n";
    }

public:
    bool tonTai(const string& ma) const {
        return find_if(ds.begin(), ds.end(), [&ma](const SinhVien& sv) {
            return sv.getMaSV() == ma;
        }) != ds.end();
    }

    // Thao tác 1: Thêm sinh viên (không cho trùng mã)
    bool themSinhVien(const SinhVien& sv) {
        if (tonTai(sv.getMaSV())) {
            cout << "=> Mã sinh viên " << sv.getMaSV() << " đã tồn tại, không thêm!\n";
            return false;
        }
        ds.push_back(sv);
        cout << "=> Thêm sinh viên thành công!\n";
        return true;
    }

    // Nhập danh sách từ bàn phím
    void nhapDanhSach() {
        int n = docSoNguyen(cin, "Nhập số lượng sinh viên muốn thêm (1-1000): ", 1, 1000);
        for (int i = 0; i < n; ++i) {
            cout << "\n--- Nhập thông tin sinh viên thứ " << i + 1 << " ---\n";
            SinhVien sv;
            cin >> sv;
            themSinhVien(sv);
        }
    }

    // Xuất danh sách ra màn hình
    void xuatDanhSach() const {
        if (ds.empty()) {
            cout << "\nDanh sách sinh viên trống!\n";
            return;
        }
        cout << "\n" << string(53, '=') << "\n";
        inTieuDe();
        cout << string(53, '=') << "\n";
        for (size_t i = 0; i < ds.size(); ++i) {
            cout << ds[i] << "\n";
        }
        cout << string(53, '=') << "\n";
    }

    // Sắp xếp bằng std::sort và toán tử < của SinhVien
    void sapXepTheoDiem(bool tangDan) {
        if (tangDan) {
            sort(ds.begin(), ds.end());
            cout << "=> Đã sắp xếp tăng dần theo điểm!\n";
        } else {
            sort(ds.begin(), ds.end(), [](const SinhVien& a, const SinhVien& b) {
                return b < a;
            });
            cout << "=> Đã sắp xếp giảm dần theo điểm!\n";
        }
    }

    // Thao tác 2: Tìm kiếm theo mã sinh viên
    void timKiemTheoMa(const string& ma) const {
        cout << "\nKết quả tìm kiếm cho mã: " << ma << "\n";
        auto it = find_if(ds.begin(), ds.end(), [&ma](const SinhVien& sv) {
            return sv.getMaSV() == ma;
        });
        if (it == ds.end()) {
            cout << "Không tìm thấy sinh viên có mã: " << ma << "\n";
            return;
        }
        inTieuDe();
        cout << *it << "\n";
    }

    // Thao tác 3: Xóa theo mã sinh viên
    void xoaTheoMa(const string& ma) {
        auto it = find_if(ds.begin(), ds.end(), [&ma](const SinhVien& sv) {
            return sv.getMaSV() == ma;
        });
        if (it != ds.end()) {
            ds.erase(it);
            cout << "=> Xóa sinh viên có mã " << ma << " thành công!\n";
        } else {
            cout << "=> Không tìm thấy sinh viên có mã " << ma << " để xóa!\n";
        }
    }

    // Thao tác 4: Tìm sinh viên điểm cao nhất và thấp nhất
    void timMinMaxDiem() const {
        if (ds.empty()) {
            cout << "\nDanh sách trống!\n";
            return;
        }
        auto minIt = min_element(ds.begin(), ds.end());
        auto maxIt = max_element(ds.begin(), ds.end());

        cout << "\n--- THÔNG TIN ĐIỂM SỐ ---\n";
        inTieuDe();
        cout << "Điểm thấp nhất:\n" << *minIt << "\n";
        cout << "Điểm cao nhất:\n" << *maxIt << "\n";
    }
};

// ==========================================
// 3. LỚP APP ĐỂ QUẢN LÝ (MENU & ĐIỀU PHỐI)
// ==========================================
class AppQuanLy {
private:
    DanhSachSinhVien quanLyDS;

    void hienThiMenu() const {
        cout << "\n================ CHƯƠNG TRÌNH QUẢN LÝ SINH VIÊN ================\n";
        cout << "1. Nhập danh sách sinh viên\n";
        cout << "2. Hiển thị danh sách sinh viên\n";
        cout << "3. Thêm một sinh viên mới\n";
        cout << "4. Tìm kiếm sinh viên theo mã SV\n";
        cout << "5. Xóa sinh viên theo mã SV\n";
        cout << "6. Sắp xếp danh sách sinh viên theo điểm\n";
        cout << "7. Tìm sinh viên có điểm cao nhất và thấp nhất\n";
        cout << "0. Thoát chương trình\n";
        cout << "===============================================================\n";
    }

public:
    void chay() {
        int luaChon;
        do {
            hienThiMenu();
            luaChon = docSoNguyen(cin, "Nhập lựa chọn của bạn (0-7): ", 0, 7);
            switch (luaChon) {
                case 1:
                    quanLyDS.nhapDanhSach();
                    break;
                case 2:
                    quanLyDS.xuatDanhSach();
                    break;
                case 3: {
                    cout << "\n--- Nhập thông tin sinh viên mới ---\n";
                    SinhVien sv;
                    cin >> sv;
                    quanLyDS.themSinhVien(sv);
                    break;
                }
                case 4: {
                    string ma = docChuoiKhongRong(cin, "Nhập mã sinh viên cần tìm: ");
                    quanLyDS.timKiemTheoMa(ma);
                    break;
                }
                case 5: {
                    string ma = docChuoiKhongRong(cin, "Nhập mã sinh viên cần xóa: ");
                    quanLyDS.xoaTheoMa(ma);
                    break;
                }
                case 6: {
                    int c = docSoNguyen(cin, "Chọn thứ tự (1: tăng dần, 2: giảm dần): ", 1, 2);
                    quanLyDS.sapXepTheoDiem(c == 1);
                    break;
                }
                case 7:
                    quanLyDS.timMinMaxDiem();
                    break;
                case 0:
                    cout << "\nCảm ơn bạn đã sử dụng chương trình! Tạm biệt.\n";
                    break;
            }
        } while (luaChon != 0);
    }
};

// ==========================================
// 4. HÀM MAIN CHẠY CHƯƠNG TRÌNH
// ==========================================
int main() {
	SetConsoleOutputCP(CP_UTF8);
#ifdef _WIN32
    system("chcp 65001 > nul"); // 65001 = UTF-8
#endif

    AppQuanLy app;
    app.chay();

    return 0;
}