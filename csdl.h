#pragma once
#ifndef CSDL_H
#define CSDL_H
#include <cstring>
#include <string>
using namespace std;
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
    int chieuCao;
    int canNang;
    long long ngaySinh;
    doiBong *ptrDoiBong;
    string imagePath;

public:
    long long id;
    cauThu();
    cauThu(long long _id, const char* _ten, const char* _viTri, int _tuoi, int _soAo, int _maDoiBong);
    const cauThu* get() const;
    void update(const cauThu& thongTinMoi);
    
    string getTen() const { return ten; }
    string getViTri() const { return viTri; }
    int getTuoi() const { return tuoi; }
    int getSoAo() const { return soAo; }
    int getSoBanThang() const { return soBanThang; }
    string getMaQuocGia() const { return maQuocGia; }
    string getImagePath() const { return imagePath; }
    int getMaDoiBong() const { return maDoiBong; }
    int getSoTranChoi() const { return soTranChoi; }
    int getSdt() const { return sdt; }
    int getChieuCao() const { return chieuCao; }
    int getCanNang() const { return canNang; }
    long long getNgaySinh() const { return ngaySinh; }
    long long getId() const { return id; }
    

    void setTuoi(int tuoiMoi) { this->tuoi = tuoiMoi; }
    void setSoAo(int soAoMoi) { this->soAo = soAoMoi; }
    void setSoBanThang(int soThangMoi) { this->soBanThang = soThangMoi; }
    void setSoTranChoi(int soTranMoi) { this->soTranChoi = soTranMoi; }
    void setSdt(int soDienThoai) { sdt = soDienThoai; }
    void setChieuCao(int chieuCaoMoi) { chieuCao = chieuCaoMoi; }
    void setCanNang(int canNangMoi) { canNang = canNangMoi; }
    void setNgaySinh(long long ngaySinhMoi) { ngaySinh = ngaySinhMoi; }
    void setMaDoiBong(int maDoiBongMoi) { this->maDoiBong = maDoiBongMoi; }
    void setTen(const char* tenMoi) {
        std::strncpy(ten, tenMoi, sizeof(ten) - 1);
        ten[sizeof(ten) - 1] = '\0';
    }
    void setViTri(const char* viTriMoi) {
        std::strncpy(viTri, viTriMoi, sizeof(viTri) - 1);
        viTri[sizeof(viTri) - 1] = '\0';
    }
    void setMaQuocGia(const char* maMoi) {
        std::strncpy(maQuocGia, maMoi, sizeof(maQuocGia) - 1);
        maQuocGia[sizeof(maQuocGia) - 1] = '\0';
    }
    void setImagePath(const string& path) { imagePath = path; }
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
    string imagePath;
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
    
    string getTen() const { return ten; }
    string getDiaPhuong() const { return diaPhuong; }
    int getDiemXepHang() const { return diemXepHang; }
    int getSoCauThu() const { return soCauThu; }
    int getSoTranThang() const { return soTranThang; }
    int getSoTranHoa() const { return soTranHoa; }
    int getSoTranThua() const { return soTranThua; }
    int getSoBanThang() const { return soBanThang; }
    int getSoBanThua() const { return soBanThua; }
    string getMaQuocGia() const { return maQuocGia; }
    int getSoCauThuHienTai() const { return soCauThuHienTai; }
    long long getNgayThanhLap() const { return ngayThanhLap; }
    long long getId() const { return id; }
    string getHLV() const { return huanLuyenVien; }
    string getImagePath() const { return imagePath; }
    
    void setDiemXepHang(int diemMoi) { this->diemXepHang = diemMoi; }
    void setHLV(const char* hlvMoi) {
        std::strncpy(huanLuyenVien, hlvMoi, sizeof(huanLuyenVien) - 1);
        huanLuyenVien[sizeof(huanLuyenVien) - 1] = '\0';
    }
    void setSoTranThang(int soThangMoi) { this->soTranThang = soThangMoi; }
    void setSoTranHoa(int soHoaMoi) { this->soTranHoa = soHoaMoi; }
    void setSoTranThua(int soThuaMoi) { this->soTranThua = soThuaMoi; }
    void setSoBanThang(int bThangMoi) { this->soBanThang = bThangMoi; }
    void setSoBanThua(int bThuaMoi) { this->soBanThua = bThuaMoi; }
    void setTen(const char* tenMoi) {
        strncpy(this->ten, tenMoi, 49);
        this->ten[49] = '\0';
    }
    void setDiaPhuong(const char* diaPhuongMoi) {
        strncpy(this->diaPhuong, diaPhuongMoi, 49);
        this->diaPhuong[49] = '\0';
    }
    void setImagePath(const string& path) { imagePath = path; }
    void setMaQuocGia(const char* maMoi) {
        std::strncpy(maQuocGia, maMoi, sizeof(maQuocGia) - 1);
        maQuocGia[sizeof(maQuocGia) - 1] = '\0';
    }
    void setNgayThanhLap(long long ngayMoi) { ngayThanhLap = ngayMoi; }
    void setSoCauThu(int soLuong) { soCauThu = soLuong; }
    void setSoCauThuHienTai(int soLuong) { soCauThuHienTai = soLuong; }
};

class tranDau {
    friend class Database;
    friend class MyVector<tranDau>;

private:
    string sanVanDong;
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
    tranDau(long long _id, int _maDB1, int _maDB2, int _b1, int _b2, long long _ngay, int _theVang1, int _theVang2, string svd = "");
    const tranDau* get() const;
    void update(const tranDau& thongTinMoi);
    
    long long getNgayThiDau() const { return ngayThiDau; }
    int getSoBanThangDoi1() const { return soBanThangDoi1; }
    int getSoBanThangDoi2() const { return soBanThangDoi2; }
    long long getId() const { return id; }
    int getMaDoiBong1() const { return maDoiBong1; }
    int getMaDoiBong2() const { return maDoiBong2; }
    int getSoTheVangDoi1() const {return soTheVangDoi1;}
    int getSoTheVangDoi2() const {return soTheVangDoi2;}
    string getSanVanDong() const {return sanVanDong;}
    int getDiemDoiBong1() const {
        return soBanThangDoi1 > soBanThangDoi2 ? 3 : (soBanThangDoi1 == soBanThangDoi2 ? 1 : 0);
    }
    int getDiemDoiBong2() const {
        return soBanThangDoi2 > soBanThangDoi1 ? 3 : (soBanThangDoi1 == soBanThangDoi2 ? 1 : 0);
    }
    void setNgayThiDau(long long ngayMoi) { ngayThiDau = ngayMoi; }
    
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
