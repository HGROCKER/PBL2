#pragma once
#ifndef CSDL_H
#define CSDL_H
class doiBong;

class cauThu
{
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
        doiBong *doiBong;
    public:
        long long id;
        void add();
        void del();
        void update();
        cauThu get();
    };

class doiBong
{
    private:
        char ten[50];
        char huanLuyenVien[50];
        char diaPhuong[50];
        int soCauThu;
        int soTranThang;
        int soTranHoa;
        int soTranThua;
        int soBanThang;
        int soBanThua;
        char maQuocGia[4];
        cauThu **cauThu;
    public:
        long long id;
        void add();
        void del();
        void update();
        doiBong get();
};

class tranDau
{
    private:
        int maDoiBong1;
        int maDoiBong2;
        int soBanThang1;
        int soBanThang2;
        bool ketQua;
        char sanThiDau[50];
        char ngayThiDau[50];
        char thoiGian[50];
        char diaDiem[50];
    public:
        long long id;
        void add();
        void del();
        void update();
        tranDau get();
};
class CSDL
{
public:
    void add();
    void del();
    void update();
    void get();
};



#endif // CSDL_H