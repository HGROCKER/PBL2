#pragma once
#ifndef CSDL_H
#define CSDL_H

#include <cstring>

class Database;
template <typename T> class MyVector; 
class doiBong;

class cauThu {
    friend class Database;
    friend class MyVector<cauThu>;

private:
    char ten[50];
    char viTri[50];
    int tuoi;
    int soAo;
    int soBanThang;
    int maDoiBong;

public:
    cauThu();
    cauThu(long long _id, const char* _ten, const char* _viTri, int _tuoi, int _soAo, int _maDoiBong);
    
    long long id;
    long long getId() const { return id; }
    const char* getTen() const { return ten; }
    const char* getViTri() const { return viTri; }
    int getTuoi() const { return tuoi; }
    int getSoAo() const { return soAo; }
    int getSoBanThang() const { return soBanThang; }
    int getMaDoiBong() const { return maDoiBong; }
    
    void setTen(const char* t) { std::strncpy(ten, t, 49); ten[49] = '\0'; }
    void setViTri(const char* v) { std::strncpy(viTri, v, 49); viTri[49] = '\0'; }
    void setTuoi(int t) { tuoi = t; }
    void setSoAo(int s) { soAo = s; }
    void setSoBanThang(int b) { soBanThang = b; }
    void setMaDoiBong(int m) { maDoiBong = m; }
};

class doiBong {
    friend class Database;
    friend class MyVector<doiBong>;

private:
    char ten[100];
    char huanLuyenVien[50];
    char diaPhuong[50];
    int diemXepHang;

public:
    doiBong();
    doiBong(long long _id, const char* _ten, const char* _hlv, const char* _diaPhuong);
    
    long long id;
    long long getId() const { return id; }
    const char* getTen() const { return ten; }
    const char* getHLV() const { return huanLuyenVien; }
    const char* getDiaPhuong() const { return diaPhuong; }
    int getDiemXepHang() const { return diemXepHang; }
    
    void setTen(const char* t) { std::strncpy(ten, t, 99); ten[99] = '\0'; }
    void setHLV(const char* h) { std::strncpy(huanLuyenVien, h, 49); huanLuyenVien[49] = '\0'; }
    void setDiaPhuong(const char* d) { std::strncpy(diaPhuong, d, 49); diaPhuong[49] = '\0'; }
    void setDiemXepHang(int d) { diemXepHang = d; }
};

class tranDau {
    friend class Database;
    friend class MyVector<tranDau>;

private:
    char sanVanDong[50];
    int maDoiBong1;
    int maDoiBong2;
    int soBanThangDoi1;
    int soBanThangDoi2;
    int soTheVangDoi1;
    int soTheVangDoi2;
    long long ngayThiDau;

public:
    tranDau();
    tranDau(long long _id, int _maDB1, int _maDB2, int _b1, int _b2, long long _ngay, int _theVang1, int _theVang2);
    
    long long id;
    void update(const tranDau& thongTinMoi);
    const tranDau* get() const;
    
    long long getId() const { return id; }
    int getDoiBong1() const { return maDoiBong1; }
    int getDoiBong2() const { return maDoiBong2; }
    int getSoBanThangDoi1() const { return soBanThangDoi1; }
    int getSoBanThangDoi2() const { return soBanThangDoi2; }
    long long getNgayThiDau() const { return ngayThiDau; }
    
    void setSoBanThangDoi1(int b) { soBanThangDoi1 = b; }
    void setSoBanThangDoi2(int b) { soBanThangDoi2 = b; }
};

#endif
