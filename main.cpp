#include <iostream>
#include "database.h"
using namespace std;

void printSeparator(const char* title) {
    cout << "\n" << string(60, '=') << endl;
    cout << "  " << title << endl;
    cout << string(60, '=') << endl;
}

int main() {
    Database db;

    // ==================== TEST 1: THEM dOI BoNG ====================
    printSeparator("TEST 1: THEM dOI BONG");
    
    doiBong db1(1, "Manchester United", "Erik ten Hag", "Manchester");
    doiBong db2(2, "Liverpool", "Jurgen Klopp", "Liverpool");
    doiBong db3(3, "Arsenal", "Mikel Arteta", "London");
    doiBong db4(4, "Chelsea", "Mauricio Pochettino", "London");
    
    db.insert_doiBong_single(db1);
    db.insert_doiBong_single(db2);
    db.insert_doiBong_single(db3);
    db.insert_doiBong_single(db4);
    
    cout << " da thEm " << db.getDoiBongCount() << " doi bong" << endl;

    // ==================== TEST 2: THEM CAU THU ====================
    printSeparator("TEST 2: THEM CAU THU");
    
    cauThu ct1(1, "Cristiano Ronaldo", "ST", 39, 7, 1);
    cauThu ct2(2, "Harry Maguire", "CB", 31, 6, 1);
    cauThu ct3(3, "Bruno Fernandes", "CM", 29, 8, 1);
    cauThu ct4(4, "Mohamed Salah", "RW", 32, 11, 2);
    cauThu ct5(5, "Virgil van Dijk", "CB", 32, 4, 2);
    cauThu ct6(6, "Bukayo Saka", "LW", 23, 7, 3);
    cauThu ct7(7, "Martin Odegaard", "CM", 25, 8, 3);
    cauThu ct8(8, "Cole Palmer", "RW", 22, 20, 4);
    
    db.insert_cauThu_single(ct1);
    db.insert_cauThu_single(ct2);
    db.insert_cauThu_single(ct3);
    db.insert_cauThu_single(ct4);
    db.insert_cauThu_single(ct5);
    db.insert_cauThu_single(ct6);
    db.insert_cauThu_single(ct7);
    db.insert_cauThu_single(ct8);
    
    cout << " da thEm " << db.getCauThuCount() << " cAu thU" << endl;

    // ==================== TEST 3: THEM TRAN dAU ====================
    printSeparator("TEST 3: THEM TRAN dAU");
    
    tranDau td1(1, 1, 2, 2, 2, 1704067200, 1, 0);  // 01/01/2024
    tranDau td2(2, 3, 4, 3, 1, 1704153600, 0, 1);  // 02/01/2024
    tranDau td3(3, 1, 3, 1, 1, 1704240000, 0, 0);  // 03/01/2024
    
    db.insert_tranDau_single(td1);
    db.insert_tranDau_single(td2);
    db.insert_tranDau_single(td3);
    
    cout << "✓ da thEm " << db.getTranDauCount() << " trAn dAu" << endl;

    // ==================== TEST 4: TIM KIEM THEO TUOI ====================
    printSeparator("TEST 4: TIM KIEM CAU THU THEO TUOI");
    db.filter_cauThu_by_tuoi(29);
    db.filter_cauThu_by_tuoi(32);
    db.filter_cauThu_by_tuoi(25);

    // ==================== TEST 5: TIM KIEM THEO dOI ====================
    printSeparator("TEST 5: TIM KIEM CAU THU THEO dOI");
    db.filter_cauThu_by_doiBong(1);
    db.filter_cauThu_by_doiBong(2);
    db.filter_cauThu_by_doiBong(3);

    // ==================== TEST 6: TIM KIEM dOI THEO diA PHuoNG ====================
    printSeparator("TEST 6: TIM KIEM dOI THEO diA PHuoNG");
    db.filter_doiBong_by_diaPhuong("Manchester");
    db.filter_doiBong_by_diaPhuong("London");
    db.filter_doiBong_by_diaPhuong("Liverpool");

    // ==================== TEST 7: UPDATE DỮ LIỆU ====================
    printSeparator("TEST 7: UPDATE TUOI CAU THU");
    
    cout << "Truoc update:\n";
    cauThu* ct = db.getCauThuById(1);
    if (ct) {
        cout << "  ID 1: " << ct->getTen() << ", TuOi: " << ct->getTuoi() << endl;
    }
    
    db.updateCauThuTuoi(1, 40);
    
    cout << "Sau update:\n";
    ct = db.getCauThuById(1);
    if (ct) {
        cout << "  ID 1: " << ct->getTen() << ", TuOi: " << ct->getTuoi() << endl;
    }

    // ==================== TEST 8: UPDATE BAN THẮNG ====================
    printSeparator("TEST 8: UPDATE BAN THANG");
    
    cout << "Truoc update:\n";
    ct = db.getCauThuById(4);
    if (ct) {
        cout << "  ID 4: " << ct->getTen() << ", Ban thang: " << ct->getSoBanThang() << endl;
    }
    
    db.updateCauThuBanThang(4, 25);
    
    cout << "Sau update:\n";
    ct = db.getCauThuById(4);
    if (ct) {
        cout << "  ID 4: " << ct->getTen() << ", Ban thang: " << ct->getSoBanThang() << endl;
    }

    // ==================== TEST 9: UPDATE dOI ====================
    printSeparator("TEST 9: UPDATE dIEM XEP HANG dOI");
    
    cout << "Truoc update:\n";
    doiBong* d = db.getDoiBongById(1);
    if (d) {
        cout << "  ID 1: " << d->getTen() << ", diểm XH: " << d->getDiemXepHang() << endl;
    }
    
    db.updateDoiBongDiemXepHang(1, 1500);
    
    cout << "Sau update:\n";
    d = db.getDoiBongById(1);
    if (d) {
        cout << "  ID 1: " << d->getTen() << ", diểm XH: " << d->getDiemXepHang() << endl;
    }

    // ==================== TEST 10: UPDATE TRAN dAU ====================
    printSeparator("TEST 10: UPDATE KET QUA TRAN dAU");
    
    cout << "Truoc update:\n";
    tranDau* t = db.getTranDauById(1);
    if (t) {
        cout << "  ID 1: " << t->getSoBanThangDoi1() << "-" << t->getSoBanThangDoi2() << endl;
    }
    
    db.updateTranDauBanThang(1, 3, 2);
    
    cout << "Sau update:\n";
    t = db.getTranDauById(1);
    if (t) {
        cout << "  ID 1: " << t->getSoBanThangDoi1() << "-" << t->getSoBanThangDoi2() << endl;
    }

    // ==================== TEST 11: TIM KIEM TRAN THEO NGAY ====================
    printSeparator("TEST 11: TIM KIEM TRAN dAU THEO NGAY");
    db.filter_tranDau_by_ngay(1704067200);
    db.filter_tranDau_by_ngay(1704153600);

    // ==================== TEST 12: THONG KE ====================
    printSeparator("TEST 12: THONG KE TONG HOP");
    cout << "TOng sO dOi bong: " << db.getDoiBongCount() << endl;
    cout << "TOng sO cAu thU: " << db.getCauThuCount() << endl;
    cout << "TOng sO trAn dAu: " << db.getTranDauCount() << endl;
    cout << "\nID tiEp theo:\n";
    cout << "  CAu thU: " << db.getNextCauThuId() << endl;
    cout << "  dOi bong: " << db.getNextDoiBongId() << endl;
    cout << "  TrAn dAu: " << db.getNextTranDauId() << endl;

    // ==================== TEST 13: PRINT TAT CA CAU THU ====================
    printSeparator("TEST 13: DANH SACH TAT CA CAU THU");
    for (int i = 0; i < db.getCauThuCount(); i++) {
        cauThu* player = db.getCauThuById(i + 1);
        if (player) {
            cout << "ID:" << player->getId() << " | " << player->getTen() 
                 << " | " << player->getViTri() << " | TuOi:" << player->getTuoi() 
                 << " | SO Ao:" << player->getSoAo() << " | BAn:" << player->getSoBanThang() << endl;
        }
    }

    printSeparator("KET THUC TEST");
    cout << "✓ TAt cA test da hoAn thAnh thAnh cong!\n" << endl;

    return 0;
}
