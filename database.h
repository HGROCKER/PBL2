#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include "vector.h"
#include "CSDL.h"
#include <iostream>
#include <cstring>
sdas
// ==================== TEMPLATE INDEX TỔNG QUÁT ====================
// ✅ FIX: Thay đổi 'id' → 'entityId' để nhất quán
template <typename K>
struct SecondaryIndex {
    K key;
    long long entityId;  // ✅ THAY ĐỔI: id → entityId
};

// ==================== TEMPLATE QUICKSORT ====================
template <typename K>
class IndexSorter {
public:
    static void sortNumeric(MyVector<SecondaryIndex<K>>& idxVec, int low, int high) {
        if (low >= high) return;
        K pivot = idxVec[high].key;
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (idxVec[j].key < pivot) {
                i++;
                std::swap(idxVec[i], idxVec[j]);
            }
        }
        std::swap(idxVec[i + 1], idxVec[high]);
        int pivotIndex = i + 1;
        sortNumeric(idxVec, low, pivotIndex - 1);
        sortNumeric(idxVec, pivotIndex + 1, high);
    }
};

// ✅ Specialization cho string (char*)
template <>
class IndexSorter<const char*> {
public:
    static void sortNumeric(MyVector<SecondaryIndex<const char*>>& idxVec, int low, int high) {
        if (low >= high) return;
        const char* pivot = idxVec[high].key;
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (std::strcmp(idxVec[j].key, pivot) < 0) {
                i++;
                std::swap(idxVec[i], idxVec[j]);
            }
        }
        std::swap(idxVec[i + 1], idxVec[high]);
        int pivotIndex = i + 1;
        sortNumeric(idxVec, low, pivotIndex - 1);
        sortNumeric(idxVec, pivotIndex + 1, high);
    }
};

// ==================== HELPER FUNCTIONS (NGOÀI CLASS) ====================

// ✅ Generic: Tìm vị trí index
template <typename K>
int findIndexPosition(MyVector<SecondaryIndex<K>>& idxVec, K key, long long entityId) {
    for (int i = 0; i < idxVec.size(); i++) {
        if (idxVec[i].key == key && idxVec[i].entityId == entityId) {  // ✅ entityId
            return i;
        }
    }
    return -1;
}

// ✅ Specialization: Tìm vị trí cho const char*
template <>
int findIndexPosition<const char*>(MyVector<SecondaryIndex<const char*>>& idxVec, 
                                   const char* key, long long entityId) {
    for (int i = 0; i < idxVec.size(); i++) {
        if (std::strcmp(idxVec[i].key, key) == 0 && idxVec[i].entityId == entityId) {  // ✅ entityId
            return i;
        }
    }
    return -1;
}

// ✅ Generic: Binary search
template <typename K>
int findFirstIndex(MyVector<SecondaryIndex<K>>& idxVec, K target) {
    int low = 0, high = idxVec.size() - 1;
    int result = -1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (idxVec[mid].key == target) {
            result = mid;
            high = mid - 1;
        } else if (idxVec[mid].key < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

// ✅ Specialization: Binary search cho const char*
template <>
int findFirstIndex<const char*>(MyVector<SecondaryIndex<const char*>>& idxVec, 
                                const char* target) {
    int low = 0, high = idxVec.size() - 1;
    int result = -1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = std::strcmp(idxVec[mid].key, target);
        if (cmp == 0) {
            result = mid;
            high = mid - 1;
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

// ==================== DATABASE CLASS ====================
class Database {
public:
    // --- KHO DỮ LIỆU CHÍNH ---
    MyVector<cauThu> dbCauThu;
    MyVector<doiBong> dbDoiBong;
    MyVector<tranDau> dbTranDau;

    // --- CÁC MẢNG CHỈ MỤC PHỤ ---
    MyVector<SecondaryIndex<int>> indexCauThuTuoi;
    MyVector<SecondaryIndex<int>> indexCauThuMaDoiBong;
    MyVector<SecondaryIndex<const char*>> indexDoiBongDiaPhuong;
    MyVector<SecondaryIndex<long long>> indexTranDauNgay;

    Database() {}

    // ==================== XỬ LÝ CẦU THỦ ====================
    void insert_bulk_cauThu(cauThu* rawArray, int count) {
        if (count <= 0) return;
        
        for (int i = 0; i < count; i++) {
            indexCauThuTuoi.push_back({ rawArray[i].getTuoi(), rawArray[i].id });
            indexCauThuMaDoiBong.push_back({ rawArray[i].getMaDoiBong(), rawArray[i].id });
        }
        
        dbCauThu.insert_bulk(rawArray, count);

        if (indexCauThuTuoi.size() > 0) {
            IndexSorter<int>::sortNumeric(indexCauThuTuoi, 0, indexCauThuTuoi.size() - 1);
            IndexSorter<int>::sortNumeric(indexCauThuMaDoiBong, 0, indexCauThuMaDoiBong.size() - 1);
        }
    }

    void insert_cauThu_single(const cauThu& ct) {
        dbCauThu.insert_ordered(ct);
        indexCauThuTuoi.push_back({ ct.getTuoi(), ct.id });
        indexCauThuMaDoiBong.push_back({ ct.getMaDoiBong(), ct.id });
        
        if (indexCauThuTuoi.size() > 0) {
            IndexSorter<int>::sortNumeric(indexCauThuTuoi, 0, indexCauThuTuoi.size() - 1);
            IndexSorter<int>::sortNumeric(indexCauThuMaDoiBong, 0, indexCauThuMaDoiBong.size() - 1);
        }
    }

    void updateCauThuTuoi(long long id, int tuoiMoi) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (!ct) return;

        int tuoCu = ct->getTuoi();
        
        int pos = findIndexPosition(indexCauThuTuoi, tuoCu, id);
        if (pos != -1) {
            indexCauThuTuoi.delete_by_index(pos);
        }

        ct->setTuoi(tuoiMoi);
        indexCauThuTuoi.push_back({ tuoiMoi, id });
        
        if (indexCauThuTuoi.size() > 0) {
            IndexSorter<int>::sortNumeric(indexCauThuTuoi, 0, indexCauThuTuoi.size() - 1);
        }
    }

    void updateCauThuMaDoiBong(long long id, int maDoiBongMoi) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (!ct) return;

        int maDoiBongCu = ct->getMaDoiBong();
        
        int pos = findIndexPosition(indexCauThuMaDoiBong, maDoiBongCu, id);
        if (pos != -1) {
            indexCauThuMaDoiBong.delete_by_index(pos);
        }

        ct->setMaDoiBong(maDoiBongMoi);
        indexCauThuMaDoiBong.push_back({ maDoiBongMoi, id });
        
        if (indexCauThuMaDoiBong.size() > 0) {
            IndexSorter<int>::sortNumeric(indexCauThuMaDoiBong, 0, indexCauThuMaDoiBong.size() - 1);
        }
    }

    void updateCauThuBanThang(long long id, int soBanThangMoi) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (ct) {
            ct->setSoBanThang(soBanThangMoi);
        }
    }

    void filter_cauThu_by_tuoi(int targetTuoi) {
        int firstIdx = findFirstIndex(indexCauThuTuoi, targetTuoi);
        if (firstIdx == -1) {
            std::cout << "Khong tim thay cau thu tuoi " << targetTuoi << std::endl;
            return;
        }

        std::cout << "=== Cau thu tuoi " << targetTuoi << " ===\n";
        for (int i = firstIdx; i < indexCauThuTuoi.size(); i++) {
            if (indexCauThuTuoi[i].key != targetTuoi) break;
            cauThu* ct = dbCauThu.find_by_id(indexCauThuTuoi[i].entityId);
            if (ct) {
                std::cout << "  ID: " << ct->getId() << " | Ten: " << ct->getTen() 
                         << " | vi tri: " << ct->getViTri() << " | So ao: " << ct->getSoAo() << std::endl;
            }
        }
    }

    void filter_cauThu_by_doiBong(int maDoiBong) {
        int firstIdx = findFirstIndex(indexCauThuMaDoiBong, maDoiBong);
        if (firstIdx == -1) {
            std::cout << "Khong tim thay cau thu doi " << maDoiBong << std::endl;
            return;
        }

        std::cout << "=== Cau thu doi " << maDoiBong << " ===\n";
        for (int i = firstIdx; i < indexCauThuMaDoiBong.size(); i++) {
            if (indexCauThuMaDoiBong[i].key != maDoiBong) break;
            cauThu* ct = dbCauThu.find_by_id(indexCauThuMaDoiBong[i].entityId);
            if (ct) {
                std::cout << "  ID: " << ct->getId() << " | Ten: " << ct->getTen() 
                         << " | Tuoi: " << ct->getTuoi() << " | BBan thang: " << ct->getSoBanThang() << std::endl;
            }
        }
    }

    // ==================== XỬ LÝ ĐỘI BÓNG ====================
    void insert_bulk_doiBong(doiBong* rawArray, int count) {
        if (count <= 0) return;

        for (int i = 0; i < count; i++) {
            indexDoiBongDiaPhuong.push_back({ rawArray[i].getDiaPhuong(), rawArray[i].id });
        }

        dbDoiBong.insert_bulk(rawArray, count);

        if (indexDoiBongDiaPhuong.size() > 0) {
            IndexSorter<const char*>::sortNumeric(indexDoiBongDiaPhuong, 0, indexDoiBongDiaPhuong.size() - 1);
        }
    }

    void insert_doiBong_single(const doiBong& db) {
        dbDoiBong.insert_ordered(db);
        indexDoiBongDiaPhuong.push_back({ db.getDiaPhuong(), db.id });
        
        if (indexDoiBongDiaPhuong.size() > 0) {
            IndexSorter<const char*>::sortNumeric(indexDoiBongDiaPhuong, 0, indexDoiBongDiaPhuong.size() - 1);
        }
    }

    void updateDoiBongDiemXepHang(long long id, int diemMoi) {
        doiBong* db = dbDoiBong.find_by_id(id);
        if (db) {
            db->setDiemXepHang(diemMoi);
        }
    }

    void filter_doiBong_by_diaPhuong(const char* targetDiaPhuong) {
        int firstIdx = findFirstIndex(indexDoiBongDiaPhuong, targetDiaPhuong);
        if (firstIdx == -1) {
            std::cout << "Khong tim thay doi bong o " << targetDiaPhuong << std::endl;
            return;
        }

        std::cout << "=== Doi bong o " << targetDiaPhuong << " ===\n";
        for (int i = firstIdx; i < indexDoiBongDiaPhuong.size(); i++) {
            if (std::strcmp(indexDoiBongDiaPhuong[i].key, targetDiaPhuong) != 0) break;
            doiBong* db = dbDoiBong.find_by_id(indexDoiBongDiaPhuong[i].entityId);
            if (db) {
                std::cout << "  ID: " << db->getId() << " | Ten: " << db->getTen() 
                         << " | Diem XH: " << db->getDiemXepHang() << std::endl;
            }
        }
    }

    // ==================== XỬ LÝ TRẬN ĐẤU ====================
    void insert_bulk_tranDau(tranDau* rawArray, int count) {
        if (count <= 0) return;

        for (int i = 0; i < count; i++) {
            indexTranDauNgay.push_back({ rawArray[i].getNgayThiDau(), rawArray[i].id });
        }

        dbTranDau.insert_bulk(rawArray, count);

        if (indexTranDauNgay.size() > 0) {
            IndexSorter<long long>::sortNumeric(indexTranDauNgay, 0, indexTranDauNgay.size() - 1);
        }
    }

    void insert_tranDau_single(const tranDau& td) {
        dbTranDau.insert_ordered(td);
        indexTranDauNgay.push_back({ td.getNgayThiDau(), td.id });
        
        if (indexTranDauNgay.size() > 0) {
            IndexSorter<long long>::sortNumeric(indexTranDauNgay, 0, indexTranDauNgay.size() - 1);
        }
    }

    void updateTranDauBanThang(long long id, int banDoi1Moi, int banDoi2Moi) {
        tranDau* td = dbTranDau.find_by_id(id);
        if (td) {
            td->setSoBanThangDoi1(banDoi1Moi);
            td->setSoBanThangDoi2(banDoi2Moi);
        }
    }

    void filter_tranDau_by_ngay(long long targetNgay) {
        int firstIdx = findFirstIndex(indexTranDauNgay, targetNgay);
        if (firstIdx == -1) {
            std::cout << "Khong tim thay tran dau ngay " << targetNgay << std::endl;
            return;
        }

        std::cout << "=== Tran dau ngay " << targetNgay << " ===\n";
        for (int i = firstIdx; i < indexTranDauNgay.size(); i++) {
            if (indexTranDauNgay[i].key != targetNgay) break;
            tranDau* td = dbTranDau.find_by_id(indexTranDauNgay[i].entityId);
            if (td) {
                std::cout << "  ID: " << td->getId() << " | Doi 1: " << td->getSoBanThangDoi1()
                         << " - Doi 2: " << td->getSoBanThangDoi2() << std::endl;
            }
        }
    }

    // ==================== UTILITY ====================
    cauThu* getCauThuById(long long id) { return dbCauThu.find_by_id(id); }
    doiBong* getDoiBongById(long long id) { return dbDoiBong.find_by_id(id); }
    tranDau* getTranDauById(long long id) { return dbTranDau.find_by_id(id); }

    long long getNextCauThuId() { return dbCauThu.getNextId(); }
    long long getNextDoiBongId() { return dbDoiBong.getNextId(); }
    long long getNextTranDauId() { return dbTranDau.getNextId(); }

    int getCauThuCount() const { return dbCauThu.size(); }
    int getDoiBongCount() const { return dbDoiBong.size(); }
    int getTranDauCount() const { return dbTranDau.size(); }
};

#endif