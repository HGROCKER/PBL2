#ifndef CSDL_H
#define CSDL_H
struct cauThu{
    int maCauThu;
    int maDoiBong;
    int maQuocGia;

    char *tenCauThu;
    char *ngaySinh;
    char *queQuan;
    int soAo;
};

struct doiBong{
    int maDoiBong;
    int maQuocGia;

    char *tenDoiBong;
    char *ngayThanhLap;
    char *sanNha;
    int soCauThu;
    int soCauThuDangChoi;
    int danhSachCauThu[100];
};

struct quocGia{
    int maQuocGia;
    char *tenQuocGia;
    char *ngayThanhLap;
    char *thuDo;
};

struct tranDau{
    int maTranDau;
    int maDoiBong1;
    int maDoiBong2;
    char *ngayThiDau;
    char *sanThiDau;
    int soBanThang1;
    int soBanThang2;
    int ketQua;
};

struct cauThu *createCauThu(int maCauThu, char *tenCauThu, char *ngaySinh, char *queQuan, int soAo, int maDoiBong, int maQuocGia, int maVung);
struct doiBong *createDoiBong(int maDoiBong, char *tenDoiBong, char *ngayThanhLap, char *sanNha, int maQuocGia);
struct quocGia *createQuocGia(int maQuocGia, char *tenQuocGia, char *ngayThanhLap, char *thuDo);
struct tranDau *createTranDau(int maTranDau, int maDoiBong1, int maDoiBong2, char *ngayThiDau, char *sanThiDau, int soBanThang1, int soBanThang2, int ketQua);

int saveDataCauThu(struct cauThu *ct, char *filename = "data/cauThu.txt");
int saveDataDoiBong(struct doiBong *db, char *filename = "data/doiBong.txt");
int saveDataQuocGia(struct quocGia *qg, char *filename = "data/quocGia.txt");
int saveDataTranDau(struct tranDau *td, char *fileName = "data/tranDau.txt");

int deleteDataCauThu(int maCauThu);
int deleteDataDoiBong(int maDoiBong);
int deleteDataQuocGia(int maQuocGia);
int deleteDataTranDau(int maTranDau);

struct cauThu *loadDataDanhSachCauThu(char *dataText);
struct doiBong *loadDataDanhSachDoiBong(char *dataText);
struct quocGia *loadDataDanhSachQuocGia(char *dataText);
struct tranDau *loadDataDanhSachTranDau(char *dataText);

int updateDataCauThu(struct cauThu *ct);
int updateDataDoiBong(struct doiBong *db);
int updateDataQuocGia(struct quocGia *qg);
int updateDataTranDau(struct tranDau *td);

struct CSDL {
    struct cauThu* dsCauThu; 
    int soLuongCauThu;

    struct doiBong* dsDoiBong;
    int soLuongDoiBong;

    struct quocGia* dsQuocGia;
    int soLuongQuocGia;

    struct tranDau* dsTranDau;
    int soLuongTranDau;
};

struct CSDL* createCSDL();
int saveAllDatabase(struct CSDL* db, char* folderPath);
int loadAllDatabase(struct CSDL* db, char* folderPath);

void freeCSDL(struct CSDL* db);
#endif // CSDL_H