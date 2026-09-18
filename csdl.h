#pragma once
#ifndef CSDL_H
#define CSDL_H

#include <cstring>  // ✅ THÊM

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
    char maQuocGia[4];
    int maDoiBong;
    int soTranChoi;
    int sdt;
    long long ngaySinh;
    doiBong *ptrDoiBong;

public:
    long long id;
    cauThu();
    cauThu(long long _id, const char* _ten, const char* _viTri, int _tuoi, int _soAo, int _maDoiBong);
    const cauThu* get() const;
    void update(const cauThu& thongTinMoi);
    
    // ✅ THÊM: Getters
    const char* getTen() const { return ten; }
    const char* getViTri() const { return viTri; }
    int getTuoi() const { return tuoi; }
    int getSoAo() const { return soAo; }
    int getSoBanThang() const { return soBanThang; }
    const char* getMaQuocGia() const { return maQuocGia; }
    int getMaDoiBong() const { return maDoiBong; }
    int getSoTranChoi() const { return soTranChoi; }
    long long getId() const { return id; }
    
    // ✅ THÊM: Setters
    void setTuoi(int tuoiMoi) { this->tuoi = tuoiMoi; }
    void setSoAo(int soAoMoi) { this->soAo = soAoMoi; }
    void setSoBanThang(int soThangMoi) { this->soBanThang = soThangMoi; }
    void setSoTranChoi(int soTranMoi) { this->soTranChoi = soTranMoi; }
    void setMaDoiBong(int maDoiBongMoi) { this->maDoiBong = maDoiBongMoi; }
};

class doiBong {
    friend class Database;
    friend class MyVector<doiBong>;

private:
    char ten[50];
    char huanLuyenVien[50];
    char diaPhuong[50];
    int diemXepHang;
    int soCauThu;
    int soTranThang;
    int soTranHoa;
    int soTranThua;
    int soBanThang;
    int soBanThua;
    char maQuocGia[4];
    long long *listCauThu;
    MyVector<cauThu*> *cauThuList;
    int soCauThuHienTai;
    long long ngayThanhLap;

public:
    long long id;
    doiBong();
    doiBong(long long _id, const char* _ten, const char* _hlv, const char* _diaPhuong);
    ~doiBong();
    const doiBong* get() const;
    void update(const doiBong& thongTinMoi);
    
    // ✅ THÊM: Getters
    const char* getTen() const { return ten; }
    const char* getDiaPhuong() const { return diaPhuong; }
    int getDiemXepHang() const { return diemXepHang; }
    long long getId() const { return id; }
    
    // ✅ THÊM: Setters
    void setDiemXepHang(int diemMoi) { this->diemXepHang = diemMoi; }
    void setSoTranThang(int soThangMoi) { this->soTranThang = soThangMoi; }
    void setSoTranHoa(int soHoaMoi) { this->soTranHoa = soHoaMoi; }
    void setSoTranThua(int soThuaMoi) { this->soTranThua = soThuaMoi; }
    void setSoBanThang(int bThangMoi) { this->soBanThang = bThangMoi; }
    void setSoBanThua(int bThuaMoi) { this->soBanThua = bThuaMoi; }
    void setTen(const char* tenMoi) {
        std::strncpy(this->ten, tenMoi, 49);
        this->ten[49] = '\0';
    }
    void setDiaPhuong(const char* diaPhuongMoi) {
        std::strncpy(this->diaPhuong, diaPhuongMoi, 49);
        this->diaPhuong[49] = '\0';
    }
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
    long long id;
    tranDau();
    tranDau(long long _id, int _maDB1, int _maDB2, int _b1, int _b2, long long _ngay, int _theVang1, int _theVang2);
    const tranDau* get() const;
    void update(const tranDau& thongTinMoi);
    
    // ✅ THÊM: Getters
    long long getNgayThiDau() const { return ngayThiDau; }
    int getSoBanThangDoi1() const { return soBanThangDoi1; }
    int getSoBanThangDoi2() const { return soBanThangDoi2; }
    long long getId() const { return id; }
    
    // ✅ THÊM: Setters
    void setSoBanThangDoi1(int banMoi) { this->soBanThangDoi1 = banMoi; }
    void setSoBanThangDoi2(int banMoi) { this->soBanThangDoi2 = banMoi; }
};

class QuocGia {
    long long id;
    char maQuocGia[4];
    char tenQuocGia[50];
    int diemXepHang;
};

#endif
