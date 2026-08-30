#pragma once
#ifndef CSDL_H
#define CSDL_H

#include <cstring>
#include <string>

// ==================== CAU THU ====================
class cauThu {
public:
    long long id;
    char ten[50];
    char viTri[50];
    int tuoi;
    int soAo;
    long long soBanThang;
    int maDoiBong;

    cauThu();
    cauThu(long long _id, const char* _ten, const char* _viTri, int _tuoi, int _soAo, int _maDoiBong);

    long long getId() const { return id; }
    std::string getTen() const { return std::string(ten); }       // ✅ FIX: return std::string
    std::string getViTri() const { return std::string(viTri); }   // ✅ FIX: return std::string
    int getTuoi() const { return tuoi; }
    int getSoAo() const { return soAo; }
    long long getSoBanThang() const { return soBanThang; }
    int getMaDoiBong() const { return maDoiBong; }

    void setTen(const char* _ten) { std::strncpy(ten, _ten, 49); ten[49] = '\0'; }
    void setViTri(const char* _viTri) { std::strncpy(viTri, _viTri, 49); viTri[49] = '\0'; }
    void setTuoi(int _tuoi) { tuoi = _tuoi; }
    void setSoAo(int _soAo) { soAo = _soAo; }
    void setSoBanThang(long long _soBanThang) { soBanThang = _soBanThang; }
    void setMaDoiBong(int _maDoiBong) { maDoiBong = _maDoiBong; }
};

// ==================== DOI BONG ====================
class doiBong {
public:
    long long id;
    char ten[100];
    char huanLuyenVien[50];
    char diaPhuong[50];
    int diemXepHang;

    doiBong();
    doiBong(long long _id, const char* _ten, const char* _hlv, const char* _diaPhuong);

    long long getId() const { return id; }
    std::string getTen() const { return std::string(ten); }               // ✅ FIX
    std::string getHLV() const { return std::string(huanLuyenVien); }     // ✅ FIX
    std::string getDiaPhuong() const { return std::string(diaPhuong); }   // ✅ FIX
    int getDiemXepHang() const { return diemXepHang; }

    void setTen(const char* _ten) { std::strncpy(ten, _ten, 99); ten[99] = '\0'; }
    void setHLV(const char* _hlv) { std::strncpy(huanLuyenVien, _hlv, 49); huanLuyenVien[49] = '\0'; }
    void setDiaPhuong(const char* _diaPhuong) { std::strncpy(diaPhuong, _diaPhuong, 49); diaPhuong[49] = '\0'; }
    void setDiemXepHang(int _diem) { diemXepHang = _diem; }
};

// ==================== TRAN DAU ====================
class tranDau {
public:
    long long id;
    int maDoiBong1, maDoiBong2;
    int soBanThangDoi1, soBanThangDoi2;
    long long ngayThiDau;
    int soTheVangDoi1, soTheVangDoi2;
    char sanVanDong[50];

    tranDau();
    tranDau(long long _id, int _maDB1, int _maDB2, int _b1, int _b2, long long _ngay, int _theVang1, int _theVang2);

    long long getId() const { return id; }
    int getMaDoiBong1() const { return maDoiBong1; }
    int getMaDoiBong2() const { return maDoiBong2; }
    int getSoBanThangDoi1() const { return soBanThangDoi1; }
    int getSoBanThangDoi2() const { return soBanThangDoi2; }
    long long getNgayThiDau() const { return ngayThiDau; }
    int getSoTheVangDoi1() const { return soTheVangDoi1; }
    int getSoTheVangDoi2() const { return soTheVangDoi2; }
    std::string getSanVanDong() const { return std::string(sanVanDong); }  // ✅ FIX

    void setSoBanThangDoi1(int _b) { soBanThangDoi1 = _b; }
    void setSoBanThangDoi2(int _b) { soBanThangDoi2 = _b; }

    const tranDau* get() const;
    void update(const tranDau& thongTinMoi);
};

// ==================== VONG BANG (TOURNAMENT BRACKET) ====================
struct VongBang {
    long long id;
    char ten[100];  // "Vòng 1/16", "Vòng 1/8", "Bán kết", "Chung kết"
    int capDo;      // 4=16 đội, 3=8 đội, 2=4 đội, 1=2 đội, 0=1 đội (chế độ)
    int soTranDauTrongVong;
    long long trangThaiId;  // Liên kết đến match

    VongBang() : id(0), capDo(0), soTranDauTrongVong(0), trangThaiId(-1) {
        ten[0] = '\0';
    }
    VongBang(long long _id, const char* _ten, int _capDo, int _soTran)
        : id(_id), capDo(_capDo), soTranDauTrongVong(_soTran), trangThaiId(-1) {
        std::strncpy(ten, _ten, 99);
        ten[99] = '\0';
    }

    long long getId() const { return id; }
    const char* getTen() const { return ten; }
    int getCapDo() const { return capDo; }
    int getSoTranDau() const { return soTranDauTrongVong; }
};

// ==================== BRACKET NODE (NÚT TRONG GIẢI ĐẤU) ====================
struct BracketNode {
    long long id;
    int doiBongId;      // ID đội bóng tham dự
    long long vongBangId;
    int viTriTronRound;  // 1=trái, 2=phải
    bool daHoanThanh;

    BracketNode() : id(0), doiBongId(-1), vongBangId(-1), viTriTronRound(0), daHoanThanh(false) {}
};

#endif
