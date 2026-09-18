#include "CSDL.h"
#include <cstring>
#include <iostream>
//test
// ==================== CAU THU ====================
cauThu::cauThu() {
    id = 0; tuoi = 0; soAo = 0; soBanThang = 0; maDoiBong = 0; soTranChoi = 0; sdt = 0;
    ptrDoiBong = nullptr; ngaySinh = 0;
    ten[0] = '\0'; viTri[0] = '\0'; maQuocGia[0] = '\0';
}

cauThu::cauThu(long long _id, const char* _ten, const char* _viTri, int _tuoi, int _soAo, int _maDoiBong) {
    id = _id; tuoi = _tuoi; soAo = _soAo; maDoiBong = _maDoiBong;
    soBanThang = 0; soTranChoi = 0; sdt = 0; ptrDoiBong = nullptr; ngaySinh = 0;
    std::strncpy(ten, _ten, 49); ten[49] = '\0';
    std::strncpy(viTri, _viTri, 49); viTri[49] = '\0';
    maQuocGia[0] = '\0';
}

const cauThu* cauThu::get() const { return this; }

void cauThu::update(const cauThu& thongTinMoi) {
    std::strncpy(ten, thongTinMoi.ten, 49); ten[49] = '\0';
    std::strncpy(viTri, thongTinMoi.viTri, 49); viTri[49] = '\0';
    tuoi = thongTinMoi.tuoi;
    soAo = thongTinMoi.soAo;
    soBanThang = thongTinMoi.soBanThang;
    maDoiBong = thongTinMoi.maDoiBong;
    soTranChoi = thongTinMoi.soTranChoi;
    sdt = thongTinMoi.sdt;
    ngaySinh = thongTinMoi.ngaySinh;
    std::strncpy(maQuocGia, thongTinMoi.maQuocGia, 3); maQuocGia[3] = '\0';
}

// ==================== DOI BONG ====================
doiBong::doiBong() {
    id = 0; soCauThu = 0; soTranThang = 0; soTranHoa = 0; soTranThua = 0; soBanThang = 0; soBanThua = 0;
    diemXepHang = 0; ngayThanhLap = 0; soCauThuHienTai = 0;
    ten[0] = '\0'; huanLuyenVien[0] = '\0'; diaPhuong[0] = '\0'; maQuocGia[0] = '\0';
    listCauThu = nullptr; cauThuList = nullptr;
}

doiBong::doiBong(long long _id, const char* _ten, const char* _hlv, const char* _diaPhuong) {
    id = _id; soCauThu = 0; soTranThang = 0; soTranHoa = 0; soTranThua = 0; soBanThang = 0; soBanThua = 0;
    diemXepHang = 0; ngayThanhLap = 0; soCauThuHienTai = 0;
    std::strncpy(ten, _ten, 49); ten[49] = '\0';
    std::strncpy(huanLuyenVien, _hlv, 49); huanLuyenVien[49] = '\0';
    std::strncpy(diaPhuong, _diaPhuong, 49); diaPhuong[49] = '\0';
    maQuocGia[0] = '\0';
    listCauThu = nullptr; cauThuList = nullptr;
}

doiBong::~doiBong() {
    if (listCauThu != nullptr) delete[] listCauThu;
}

const doiBong* doiBong::get() const { return this; }

void doiBong::update(const doiBong& thongTinMoi) {
    std::strncpy(ten, thongTinMoi.ten, 49); ten[49] = '\0';
    std::strncpy(huanLuyenVien, thongTinMoi.huanLuyenVien, 49); huanLuyenVien[49] = '\0';
    std::strncpy(diaPhuong, thongTinMoi.diaPhuong, 49); diaPhuong[49] = '\0';
    std::strncpy(maQuocGia, thongTinMoi.maQuocGia, 3); maQuocGia[3] = '\0';
    soCauThu = thongTinMoi.soCauThu;
    soTranThang = thongTinMoi.soTranThang;
    soTranHoa = thongTinMoi.soTranHoa;
    soTranThua = thongTinMoi.soTranThua;
    soBanThang = thongTinMoi.soBanThang;
    soBanThua = thongTinMoi.soBanThua;
    diemXepHang = thongTinMoi.diemXepHang;
    ngayThanhLap = thongTinMoi.ngayThanhLap;
}

// ==================== TRAN DAU ====================
tranDau::tranDau() {
    id = 0; maDoiBong1 = 0; maDoiBong2 = 0; soBanThangDoi1 = 0; soBanThangDoi2 = 0;
    soTheVangDoi1 = 0; soTheVangDoi2 = 0; ngayThiDau = 0;
    sanVanDong[0] = '\0';
}

tranDau::tranDau(long long _id, int _maDB1, int _maDB2, int _b1, int _b2,  long long _ngay, int _theVang1, int _theVang2) {
    id = _id; maDoiBong1 = _maDB1; maDoiBong2 = _maDB2;
    soBanThangDoi1 = _b1; soBanThangDoi2 = _b2;
    ngayThiDau = _ngay;
    soTheVangDoi1 = _theVang1;
    soTheVangDoi2 = _theVang2;
    sanVanDong[0] = '\0';
}

const tranDau* tranDau::get() const { return this; }

void tranDau::update(const tranDau& thongTinMoi) {
    id = thongTinMoi.id;
    maDoiBong1 = thongTinMoi.maDoiBong1;
    maDoiBong2 = thongTinMoi.maDoiBong2;
    soBanThangDoi1 = thongTinMoi.soBanThangDoi1;
    soBanThangDoi2 = thongTinMoi.soBanThangDoi2;
    soTheVangDoi1 = thongTinMoi.soTheVangDoi1;
    soTheVangDoi2 = thongTinMoi.soTheVangDoi2;
    ngayThiDau = thongTinMoi.ngayThiDau;
    std::strncpy(sanVanDong, thongTinMoi.sanVanDong, 49); sanVanDong[49] = '\0';
}
