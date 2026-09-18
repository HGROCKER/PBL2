#include "csdl.h"
#include <cstring>

//cau thu
cauThu::cauThu() : id(0), tuoi(0), soAo(0), soBanThang(0), maDoiBong(0) {
    ten[0] = '\0';
    viTri[0] = '\0';
}

cauThu::cauThu(long long _id, const char* _ten, const char* _viTri, int _tuoi, int _soAo, int _maDoiBong)
    : id(_id), tuoi(_tuoi), soAo(_soAo), soBanThang(0), maDoiBong(_maDoiBong) {
    std::strncpy(ten, _ten, 49);
    ten[49] = '\0';
    std::strncpy(viTri, _viTri, 49);
    viTri[49] = '\0';
}

// doi bong
doiBong::doiBong() : id(0), diemXepHang(0) {
    ten[0] = '\0';
    huanLuyenVien[0] = '\0';
    diaPhuong[0] = '\0';
}

doiBong::doiBong(long long _id, const char* _ten, const char* _hlv, const char* _diaPhuong)
    : id(_id), diemXepHang(0) {
    std::strncpy(ten, _ten, 99);
    ten[99] = '\0';
    std::strncpy(huanLuyenVien, _hlv, 49);
    huanLuyenVien[49] = '\0';
    std::strncpy(diaPhuong, _diaPhuong, 49);
    diaPhuong[49] = '\0';
}

// ==================== TRAN DAU ====================
tranDau::tranDau() : id(0), maDoiBong1(0), maDoiBong2(0), soBanThangDoi1(0), soBanThangDoi2(0), ngayThiDau(0), soTheVangDoi1(0), soTheVangDoi2(0) {
    sanVanDong[0] = '\0';
}

// FIX: Đồng bộ đầy đủ gán thuộc tính thẻ vàng trùng khớp hoàn toàn với file main.cpp
tranDau::tranDau(long long _id, int _maDB1, int _maDB2, int _b1, int _b2, long long _ngay, int _theVang1, int _theVang2)
    : id(_id), maDoiBong1(_maDB1), maDoiBong2(_maDB2), soBanThangDoi1(_b1), soBanThangDoi2(_b2), ngayThiDau(_ngay), soTheVangDoi1(_theVang1), soTheVangDoi2(_theVang2) {
    sanVanDong[0] = '\0';
}

const tranDau* tranDau::get() const { return this; }

void tranDau::update(const tranDau& thongTinMoi) {
    this->id = thongTinMoi.id;
    this->maDoiBong1 = thongTinMoi.maDoiBong1;
    this->maDoiBong2 = thongTinMoi.maDoiBong2;
    this->soBanThangDoi1 = thongTinMoi.soBanThangDoi1;
    this->soBanThangDoi2 = thongTinMoi.soBanThangDoi2;
    this->soTheVangDoi1 = thongTinMoi.soTheVangDoi1;
    this->soTheVangDoi2 = thongTinMoi.soTheVangDoi2;
    this->ngayThiDau = thongTinMoi.ngayThiDau;
    std::strncpy(this->sanVanDong, thongTinMoi.sanVanDong, 49);
    this->sanVanDong[49] = '\0';
}
