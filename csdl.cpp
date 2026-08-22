#include "csdl.h"
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

const int MAX_RECORD = 1000;

static cauThu dsCauThu[MAX_RECORD];
static doiBong dsDoiBong[MAX_RECORD];
static tranDau dsTranDau[MAX_RECORD];

static int soLuongCauThu = 0;
static int soLuongDoiBong = 0;
static int soLuongTranDau = 0;

static long long idCauThu = 1;
static long long idDoiBong = 1;
static long long idTranDau = 1;

static void nhapChuoi(char* chuoi, int kichThuoc) {
    cin.ignore(10000, '\n');
    cin.getline(chuoi, kichThuoc);
}

static void nhapMaQuocGia(char* ma) {
    char temp[100];

    cin >> temp;
    strncpy(ma, temp, 3);
    ma[3] = '\0';
}

// ======================= CAU THU =======================

void cauThu::add() {
    cout << "Ten: ";
    nhapChuoi(ten, 50);

    cout << "Vi tri: ";
    cin.getline(viTri, 50);

    cout << "Tuoi: ";
    cin >> tuoi;

    cout << "So ao: ";
    cin >> soAo;

    cout << "So ban thang: ";
    cin >> soBanThang;

    cout << "Ma quoc gia: ";
    nhapMaQuocGia(maQuocGia);

    cout << "Ma doi bong: ";
    cin >> maDoiBong;

    cout << "So tran choi: ";
    cin >> soTranChoi;

    cout << "So dien thoai: ";
    cin >> sdt;

    doiBong = nullptr;
}

void cauThu::update() {
    cout << "Ten moi: ";
    nhapChuoi(ten, 50);

    cout << "Vi tri moi: ";
    cin.getline(viTri, 50);

    cout << "Tuoi moi: ";
    cin >> tuoi;

    cout << "So ao moi: ";
    cin >> soAo;

    cout << "So ban thang moi: ";
    cin >> soBanThang;

    cout << "Ma quoc gia moi: ";
    nhapMaQuocGia(maQuocGia);

    cout << "Ma doi bong moi: ";
    cin >> maDoiBong;

    cout << "So tran choi moi: ";
    cin >> soTranChoi;

    cout << "So dien thoai moi: ";
    cin >> sdt;
}

cauThu cauThu::get() {
    cout << "\nID: " << id
         << "\nTen: " << ten
         << "\nVi tri: " << viTri
         << "\nTuoi: " << tuoi
         << "\nSo ao: " << soAo
         << "\nSo ban thang: " << soBanThang
         << "\nMa quoc gia: " << maQuocGia
         << "\nMa doi bong: " << maDoiBong
         << "\nSo tran choi: " << soTranChoi
         << "\nSo dien thoai: " << sdt << '\n';

    return *this;
}

void cauThu::del() {
    for (int i = 0; i < soLuongCauThu; i++) {
        if (dsCauThu[i].id == id) {
            dsCauThu[i] = dsCauThu[soLuongCauThu - 1];
            soLuongCauThu--;
            return;
        }
    }
}

// ======================= DOI BONG =======================

void doiBong::add() {
    cout << "Ten doi bong: ";
    nhapChuoi(ten, 50);

    cout << "Huan luyen vien: ";
    cin.getline(huanLuyenVien, 50);

    cout << "Dia phuong: ";
    cin.getline(diaPhuong, 50);

    cout << "Ma quoc gia: ";
    nhapMaQuocGia(maQuocGia);

    cout << "So cau thu: ";
    cin >> soCauThu;

    cout << "So tran thang: ";
    cin >> soTranThang;

    cout << "So tran hoa: ";
    cin >> soTranHoa;

    cout << "So tran thua: ";
    cin >> soTranThua;

    cout << "So ban thang: ";
    cin >> soBanThang;

    cout << "So ban thua: ";
    cin >> soBanThua;

    cauThu = nullptr;
}

void doiBong::update() {
    add();
}

doiBong doiBong::get() {
    cout << "\nID: " << id
         << "\nTen doi bong: " << ten
         << "\nHuan luyen vien: " << huanLuyenVien
         << "\nDia phuong: " << diaPhuong
         << "\nMa quoc gia: " << maQuocGia
         << "\nSo cau thu: " << soCauThu
         << "\nSo tran thang: " << soTranThang
         << "\nSo tran hoa: " << soTranHoa
         << "\nSo tran thua: " << soTranThua
         << "\nSo ban thang: " << soBanThang
         << "\nSo ban thua: " << soBanThua << '\n';

    return *this;
}

void doiBong::del() {
    for (int i = 0; i < soLuongDoiBong; i++) {
        if (dsDoiBong[i].id == id) {
            dsDoiBong[i] = dsDoiBong[soLuongDoiBong - 1];
            soLuongDoiBong--;
            return;
        }
    }
}

// ======================= TRAN DAU =======================

void tranDau::add() {
    cout << "Ma doi bong 1: ";
    cin >> maDoiBong1;

    cout << "Ma doi bong 2: ";
    cin >> maDoiBong2;

    cout << "So ban thang doi 1: ";
    cin >> soBanThang1;

    cout << "So ban thang doi 2: ";
    cin >> soBanThang2;

    ketQua = soBanThang1 > soBanThang2;

    cout << "San thi dau: ";
    nhapChuoi(sanThiDau, 50);

    cout << "Ngay thi dau: ";
    cin.getline(ngayThiDau, 50);

    cout << "Thoi gian: ";
    cin.getline(thoiGian, 50);

    cout << "Dia diem: ";
    cin.getline(diaDiem, 50);
}

void tranDau::update() {
    add();
}

tranDau tranDau::get() {
    cout << "\nID: " << id
         << "\nMa doi bong 1: " << maDoiBong1
         << "\nMa doi bong 2: " << maDoiBong2
         << "\nTi so: " << soBanThang1
         << " - " << soBanThang2
         << "\nSan thi dau: " << sanThiDau
         << "\nNgay thi dau: " << ngayThiDau
         << "\nThoi gian: " << thoiGian
         << "\nDia diem: " << diaDiem << '\n';

    return *this;
}

void tranDau::del() {
    for (int i = 0; i < soLuongTranDau; i++) {
        if (dsTranDau[i].id == id) {
            dsTranDau[i] = dsTranDau[soLuongTranDau - 1];
            soLuongTranDau--;
            return;
        }
    }
}

// ======================= CSDL =======================

void CSDL::add() {
    int loai;

    cout << "\n1. Them cau thu";
    cout << "\n2. Them doi bong";
    cout << "\n3. Them tran dau";
    cout << "\nChon: ";
    cin >> loai;

    if (loai == 1 && soLuongCauThu < MAX_RECORD) {
        cauThu item;
        item.id = idCauThu++;
        item.add();
        dsCauThu[soLuongCauThu++] = item;
    }
    else if (loai == 2 && soLuongDoiBong < MAX_RECORD) {
        doiBong item;
        item.id = idDoiBong++;
        item.add();
        dsDoiBong[soLuongDoiBong++] = item;
    }
    else if (loai == 3 && soLuongTranDau < MAX_RECORD) {
        tranDau item;
        item.id = idTranDau++;
        item.add();
        dsTranDau[soLuongTranDau++] = item;
    }
    else {
        cout << "Lua chon khong hop le hoac mang da day!\n";
    }
}

void CSDL::del() {
    int loai;
    long long id;

    cout << "Loai bang can xoa: ";
    cin >> loai;

    cout << "ID can xoa: ";
    cin >> id;

    if (loai == 1) {
        for (int i = 0; i < soLuongCauThu; i++) {
            if (dsCauThu[i].id == id) {
                dsCauThu[i] = dsCauThu[soLuongCauThu - 1];
                soLuongCauThu--;
                cout << "Xoa thanh cong!\n";
                return;
            }
        }
    }
    else if (loai == 2) {
        for (int i = 0; i < soLuongDoiBong; i++) {
            if (dsDoiBong[i].id == id) {
                dsDoiBong[i] = dsDoiBong[soLuongDoiBong - 1];
                soLuongDoiBong--;
                cout << "Xoa thanh cong!\n";
                return;
            }
        }
    }
    else if (loai == 3) {
        for (int i = 0; i < soLuongTranDau; i++) {
            if (dsTranDau[i].id == id) {
                dsTranDau[i] = dsTranDau[soLuongTranDau - 1];
                soLuongTranDau--;
                cout << "Xoa thanh cong!\n";
                return;
            }
        }
    }

    cout << "Khong tim thay ID!\n";
}

void CSDL::update() {
    int loai;
    long long id;

    cout << "Loai bang can sua: ";
    cin >> loai;

    cout << "ID can sua: ";
    cin >> id;

    if (loai == 1) {
        for (int i = 0; i < soLuongCauThu; i++) {
            if (dsCauThu[i].id == id) {
                dsCauThu[i].update();
                dsCauThu[i].id = id;
                return;
            }
        }
    }
    else if (loai == 2) {
        for (int i = 0; i < soLuongDoiBong; i++) {
            if (dsDoiBong[i].id == id) {
                dsDoiBong[i].update();
                dsDoiBong[i].id = id;
                return;
            }
        }
    }
    else if (loai == 3) {
        for (int i = 0; i < soLuongTranDau; i++) {
            if (dsTranDau[i].id == id) {
                dsTranDau[i].update();
                dsTranDau[i].id = id;
                return;
            }
        }
    }

    cout << "Khong tim thay ID!\n";
}

void CSDL::get() {
    int loai;

    cout << "Loai bang can xem: ";
    cin >> loai;

    if (loai == 1) {
        for (int i = 0; i < soLuongCauThu; i++)
            dsCauThu[i].get();
    }
    else if (loai == 2) {
        for (int i = 0; i < soLuongDoiBong; i++)
            dsDoiBong[i].get();
    }
    else if (loai == 3) {
        for (int i = 0; i < soLuongTranDau; i++)
            dsTranDau[i].get();
    }
    else {
        cout << "Loai bang khong hop le!\n";
    }
}