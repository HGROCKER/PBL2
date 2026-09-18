#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include "vector.h"
#include "csdl.h"
#include <iostream>
#include <cstring>

// ==================== SECONDARY INDEX ====================
template <typename K>
struct SecondaryIndex {
    K key;
    long long entityId;
};

// ==================== QUICKSORT FOR INDEX ====================
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

// ✅ Specialization cho const char*
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

// ==================== HELPER FUNCTIONS ====================
template <typename K>
int findIndexPosition(MyVector<SecondaryIndex<K>>& idxVec, K key, long long entityId) {
    for (int i = 0; i < idxVec.size(); i++) {
        if (idxVec[i].key == key && idxVec[i].entityId == entityId) {
            return i;
        }
    }
    return -1;
}

// Thêm inline vào trước kiểu trả về int của hàm chuyên biệt hóa
template <>
inline int findIndexPosition<const char*>(MyVector<SecondaryIndex<const char*>>& idxVec,
                                           const char* key, long long entityId) {
    for (int i = 0; i < idxVec.size(); i++) {
        if (std::strcmp(idxVec[i].key, key) == 0 && idxVec[i].entityId == entityId) {
            return i;
        }
    }
    return -1;
}

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
// Thêm inline vào trước kiểu trả về int của hàm chuyên biệt hóa
template <>
inline int findFirstIndex<const char*>(MyVector<SecondaryIndex<const char*>>& idxVec,
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
    MyVector<cauThu> dbCauThu;
    MyVector<doiBong> dbDoiBong;
    MyVector<tranDau> dbTranDau;

    MyVector<SecondaryIndex<int>> indexCauThuTuoi;
    MyVector<SecondaryIndex<int>> indexCauThuMaDoiBong;
    MyVector<SecondaryIndex<const char*>> indexDoiBongDiaPhuong;
    MyVector<SecondaryIndex<long long>> indexTranDauNgay;

    Database() {}

    // ✅ INSERT CAU THU
    void insert_cauThu_single(const cauThu& ct) {
        dbCauThu.insert_ordered(ct);
        indexCauThuTuoi.push_back({ct.getTuoi(), ct.id});
        indexCauThuMaDoiBong.push_back({ct.getMaDoiBong(), ct.id});

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
        if (pos != -1) indexCauThuTuoi.delete_by_index(pos);
        ct->setTuoi(tuoiMoi);
        indexCauThuTuoi.push_back({tuoiMoi, id});
        if (indexCauThuTuoi.size() > 0) {
            IndexSorter<int>::sortNumeric(indexCauThuTuoi, 0, indexCauThuTuoi.size() - 1);
        }
    }

    void updateCauThuMaDoiBong(long long id, int maDoiBongMoi) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (!ct) return;
        int maDoiBongCu = ct->getMaDoiBong();
        int pos = findIndexPosition(indexCauThuMaDoiBong, maDoiBongCu, id);
        if (pos != -1) indexCauThuMaDoiBong.delete_by_index(pos);
        ct->setMaDoiBong(maDoiBongMoi);
        indexCauThuMaDoiBong.push_back({maDoiBongMoi, id});
        if (indexCauThuMaDoiBong.size() > 0) {
            IndexSorter<int>::sortNumeric(indexCauThuMaDoiBong, 0, indexCauThuMaDoiBong.size() - 1);
        }
    }

    void updateCauThuBanThang(long long id, int soBanThangMoi) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (ct) ct->setSoBanThang(soBanThangMoi);
    }
    void insert_doiBong_single(const doiBong& db) {
        dbDoiBong.insert_ordered(db);

        // Tạo một biến static/hoặc lấy vùng nhớ cố định từ mảng chính vừa insert
        doiBong* permanentTeam = dbDoiBong.find_by_id(db.id);
        if (permanentTeam) {
            SecondaryIndex<const char*> item;
            // SỬA Ở ĐÂY: Bỏ .c_str() đi vì bản thân getDiaPhuong() đã là const char* rồi
            item.key = permanentTeam->getDiaPhuong(); 
            item.entityId = permanentTeam->id;
            indexDoiBongDiaPhuong.push_back(item);
        }

        if (indexDoiBongDiaPhuong.size() > 0) {
            IndexSorter<const char*>::sortNumeric(indexDoiBongDiaPhuong, 0, indexDoiBongDiaPhuong.size() - 1);
        }
    }
    void updateDoiBongDiemXepHang(long long id, int diemMoi) {
        doiBong* db = dbDoiBong.find_by_id(id);
        if (db) db->setDiemXepHang(diemMoi);
    }

    // ✅ INSERT TRAN DAU
    void insert_tranDau_single(const tranDau& td) {
        dbTranDau.insert_ordered(td);
        indexTranDauNgay.push_back({td.getNgayThiDau(), td.id});
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

    // ✅ GETTERS
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