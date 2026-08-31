#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include "vector.h"
#include "csdl.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <algorithm> // ✅ Bổ sung thư viện để dùng std::sort chuẩn C++
#include <QDateTime>
#include <QString>

// ==================== SECONDARY INDEX ====================
template <typename K>
struct SecondaryIndex
{
    K key;
    long long entityId;
};

// ==================== HELPER FUNCTIONS ====================
template <typename K>
int findIndexPosition(MyVector<SecondaryIndex<K>> &idxVec, K key, long long entityId)
{
    int totalSize = idxVec.size();
    for (int i = 0; totalSize > i; i++)
    {
        if (idxVec[i].key == key && idxVec[i].entityId == entityId)
        {
            return i;
        }
    }
    return -1;
}

// Chuyên biệt hóa hàm tìm vị trí cho chuỗi ký tự cố định C-style string
template <>
inline int findIndexPosition<const char *>(MyVector<SecondaryIndex<const char *>> &idxVec,
                                           const char *key, long long entityId)
{
    int totalSize = idxVec.size();
    for (int i = 0; totalSize > i; i++)
    {
        if (std::strcmp(idxVec[i].key, key) == 0 && idxVec[i].entityId == entityId)
        {
            return i;
        }
    }
    return -1;
}

template <typename K>
int findFirstIndex(MyVector<SecondaryIndex<K>> &idxVec, K target)
{
    int low = 0;
    int high = idxVec.size() - 1;
    int result = -1;
    while (high >= low)
    {
        int mid = low + (high - low) / 2;
        if (idxVec[mid].key == target)
        {
            result = mid;
            high = mid - 1;
        }
        else if (target > idxVec[mid].key)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return result;
}

// Chuyên biệt hóa hàm tìm vị trí đầu tiên nhị phân cho chuỗi ký tự
template <>
inline int findFirstIndex<const char *>(MyVector<SecondaryIndex<const char *>> &idxVec,
                                        const char *target)
{
    int low = 0;
    int high = idxVec.size() - 1;
    int result = -1;
    while (high >= low)
    {
        int mid = low + (high - low) / 2;
        int cmp = std::strcmp(idxVec[mid].key, target);
        if (cmp == 0)
        {
            result = mid;
            high = mid - 1;
        }
        else if (0 > cmp)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return result;
}

// ==================== DATABASE CLASS ====================
class Database
{
private:
    std::vector<std::string> splitString(const std::string &s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    // ✅ Hàm Helper tự tạo để sort MyVector sử dụng con trỏ vùng đệm nhằm tránh lỗi Template
    template <typename K>
    void sortIndexVector(MyVector<SecondaryIndex<K>> &idxVec)
    {
        if (idxVec.size() <= 1)
            return;
        std::sort(&idxVec[0], &idxVec[0] + idxVec.size(), [](const SecondaryIndex<K> &a, const SecondaryIndex<K> &b)
                  { return a.key < b.key; });
    }

    // ✅ Chuyên biệt hóa hàm sort cho kiểu chuỗi const char* C-style string
    void sortStringIndexVector(MyVector<SecondaryIndex<const char *>> &idxVec)
    {
        if (idxVec.size() <= 1)
            return;
        std::sort(&idxVec[0], &idxVec[0] + idxVec.size(), [](const SecondaryIndex<const char *> &a, const SecondaryIndex<const char *> &b)
                  { return std::strcmp(a.key, b.key) < 0; });
    }

public:
    MyVector<cauThu> dbCauThu;
    MyVector<doiBong> dbDoiBong;
    MyVector<tranDau> dbTranDau;

    MyVector<SecondaryIndex<int>> indexCauThuTuoi;
    MyVector<SecondaryIndex<int>> indexCauThuMaDoiBong;
    MyVector<SecondaryIndex<const char *>> indexDoiBongDiaPhuong;
    MyVector<SecondaryIndex<long long>> indexTranDauNgay;

    Database()
    {
        loadAllDataFromFiles();
    }

    void writeLog(const std::string &action, const std::string &details)
    {
        std::ofstream logFile("system_log.txt", std::ios::app);
        if (logFile.is_open())
        {
            std::string timeStr = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss").toStdString();
            logFile << "[" << timeStr << "] [" << action << "] " << details << "\n";
            logFile.close();
        }
    }

    bool isIdExists(const std::string &table, long long id)
    {
        if (table == "cauthu")
            return dbCauThu.find_by_id(id) != nullptr;
        if (table == "doibong")
            return dbDoiBong.find_by_id(id) != nullptr;
        if (table == "trandau")
            return dbTranDau.find_by_id(id) != nullptr;
        return false;
    }

    void saveAllDataToFiles()
    {
        std::ofstream fPlayer("cauthu.txt");
        if (fPlayer.is_open())
        {
            fPlayer << "id|ten|viTri|tuoi|soAo|soBanThang|maDoiBong\n";
            int total = dbCauThu.size();
            for (int i = 0; total > i; ++i)
            {
                cauThu &c = dbCauThu[i];
                fPlayer << c.getId() << "|" << c.getTen() << "|" << c.getViTri() << "|"
                        << c.getTuoi() << "|" << c.getSoAo() << "|" << c.getSoBanThang() << "|" << c.getMaDoiBong() << "\n";
            }
            fPlayer.close();
        }

        std::ofstream fTeam("doibong.txt");
        if (fTeam.is_open())
        {
            fTeam << "id|ten|huanLuyenVien|diaPhuong|diemXepHang\n";
            int total = dbDoiBong.size();
            for (int i = 0; total > i; ++i)
            {
                doiBong &d = dbDoiBong[i];
                fTeam << d.getId() << "|" << d.getTen() << "|" << d.getHLV() << "|"
                      << d.getDiaPhuong() << "|" << d.getDiemXepHang() << "\n";
            }
            fTeam.close();
        }

        std::ofstream fMatch("trandau.txt");
        if (fMatch.is_open())
        {
            fMatch << "id|maDoiBong1|maDoiBong2|soBanThangDoi1|soBanThangDoi2|ngayThiDau|soTheVangDoi1|soTheVangDoi2\n";
            int total = dbTranDau.size();
            for (int i = 0; total > i; ++i)
            {
                tranDau &t = dbTranDau[i];
                fMatch << t.getId() << "|" << t.getMaDoiBong1() << "|" << t.getMaDoiBong2() << "|"
                       << t.getSoBanThangDoi1() << "|" << t.getSoBanThangDoi2() << "|" << t.getNgayThiDau() << "|"
                       << t.getSoTheVangDoi1() << "|" << t.getSoTheVangDoi2() << "\n";
            }
            fMatch.close();
        }
    }

    void loadAllDataFromFiles()
    {
        dbCauThu.clear();
        dbDoiBong.clear();
        dbTranDau.clear();
        indexCauThuTuoi.clear();
        indexCauThuMaDoiBong.clear();
        indexDoiBongDiaPhuong.clear();
        indexTranDauNgay.clear();

        std::string line;

        std::ifstream fTeam("doibong.txt");
        if (fTeam.is_open())
        {
            std::getline(fTeam, line);
            while (std::getline(fTeam, line))
            {
                if (line.empty())
                    continue;
                auto fields = splitString(line, '|');
                if (fields.size() >= 5)
                {
                    long long id = std::stoll(fields[0]);
                    doiBong dbong(id, fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
                    dbong.setDiemXepHang(std::stoi(fields[4]));
                    dbDoiBong.insert_ordered(dbong);

                    doiBong *pTeam = dbDoiBong.find_by_id(id);
                    if (pTeam)
                    {
                        indexDoiBongDiaPhuong.push_back({pTeam->getDiaPhuong().c_str(), id});
                    }
                }
            }
            fTeam.close();
            sortStringIndexVector(indexDoiBongDiaPhuong);
        }

        std::ifstream fPlayer("cauthu.txt");
        if (fPlayer.is_open())
        {
            std::getline(fPlayer, line);
            while (std::getline(fPlayer, line))
            {
                if (line.empty())
                    continue;
                auto fields = splitString(line, '|');
                if (fields.size() >= 7)
                {
                    long long id = std::stoll(fields[0]);
                    cauThu ct(id, fields[1].c_str(), fields[2].c_str(), std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[6]));
                    ct.setSoBanThang(std::stoll(fields[5]));
                    dbCauThu.insert_ordered(ct);

                    indexCauThuTuoi.push_back({ct.getTuoi(), id});
                    indexCauThuMaDoiBong.push_back({ct.getMaDoiBong(), id});
                }
            }
            fPlayer.close();
            sortIndexVector(indexCauThuTuoi);
            sortIndexVector(indexCauThuMaDoiBong);
        }

        std::ifstream fMatch("trandau.txt");
        if (fMatch.is_open())
        {
            std::getline(fMatch, line);
            while (std::getline(fMatch, line))
            {
                if (line.empty())
                    continue;

                auto fields = splitString(line, '|');
                if (fields.size() >= 8)
                {
                    long long id = std::stoll(fields[0]);
                    tranDau td(id, std::stoi(fields[1]), std::stoi(fields[2]), std::stoi(fields[3]), std::stoi(fields[4]), std::stoll(fields[5]), std::stoi(fields[6]), std::stoi(fields[7]));
                    dbTranDau.insert_ordered(td);
                    indexTranDauNgay.push_back({td.getNgayThiDau(), id});
                }
            }
            fMatch.close();
            sortIndexVector(indexTranDauNgay);
        }
    }

    bool insert_cauThu_single(const cauThu &ct)
    {
        if (isIdExists("cauthu", ct.id)) return false;
        dbCauThu.insert_ordered(ct);
        indexCauThuTuoi.push_back({ct.getTuoi(), ct.id});
        indexCauThuMaDoiBong.push_back({ct.getMaDoiBong(), ct.id});
        sortIndexVector(indexCauThuTuoi);
        sortIndexVector(indexCauThuMaDoiBong);
        writeLog("INSERT", "Them cau thu moi ID: " + std::to_string(ct.id) + ", Ten: " + ct.getTen());
        saveAllDataToFiles();
        return true;
    }
    bool updateCauThu(long long id, const cauThu &newData)
    {
        cauThu *ct = dbCauThu.find_by_id(id);
        if (!ct)
            return false;
        if (id != newData.id && isIdExists("cauthu", newData.id))
            return false;
        writeLog("UPDATE", "Cap nhat cau thu ID: " + std::to_string(id) + " - Ten: " + newData.getTen());
        if (ct->getTuoi() != newData.getTuoi())
        {
            int pos = findIndexPosition(indexCauThuTuoi, ct->getTuoi(), id);
            if (pos != -1)
                indexCauThuTuoi.delete_by_index(pos);
            indexCauThuTuoi.push_back({newData.getTuoi(), id});
            sortIndexVector(indexCauThuTuoi);
        }
        if (ct->getMaDoiBong() != newData.getMaDoiBong())
        {
            int pos = findIndexPosition(indexCauThuMaDoiBong, ct->getMaDoiBong(), id);
            if (pos != -1)
                indexCauThuMaDoiBong.delete_by_index(pos);
            indexCauThuMaDoiBong.push_back({newData.getMaDoiBong(), id});
            sortIndexVector(indexCauThuMaDoiBong);
        }
        ct->setTen(newData.getTen().c_str());
        ct->setViTri(newData.getViTri().c_str());
        ct->setTuoi(newData.getTuoi());
        ct->setSoAo(newData.getSoAo());
        ct->setSoBanThang(newData.getSoBanThang());
        ct->setMaDoiBong(newData.getMaDoiBong());
        saveAllDataToFiles();
        return true;
    }
    void updateCauThuTuoi(long long id, int tuoiMoi)
    {
        cauThu *ct = dbCauThu.find_by_id(id);
        if (!ct)
            return;
        int tuoCu = ct->getTuoi();
        int pos = findIndexPosition(indexCauThuTuoi, tuoCu, id);
        if (pos != -1)
            indexCauThuTuoi.delete_by_index(pos);
        writeLog("UPDATE", "Cap nhat tuoi cau thu ID: " + std::to_string(id) + " -> Tuoi moi: " + std::to_string(tuoiMoi));
        ct->setTuoi(tuoiMoi);
        indexCauThuTuoi.push_back({tuoiMoi, id});
        sortIndexVector(indexCauThuTuoi);
        saveAllDataToFiles();
    }
    void updateCauThuMaDoiBong(long long id, int maDoiBongMoi)
    {
        cauThu *ct = dbCauThu.find_by_id(id);
        if (!ct)
            return;
        int maDoiBongCu = ct->getMaDoiBong();
        int pos = findIndexPosition(indexCauThuMaDoiBong, maDoiBongCu, id);
        if (pos != -1)
            indexCauThuMaDoiBong.delete_by_index(pos);
        writeLog("UPDATE", "Thay doi ma CLB cau thu ID: " + std::to_string(id) + " -> CLB moi: " + std::to_string(maDoiBongMoi));
        ct->setMaDoiBong(maDoiBongMoi);
        indexCauThuMaDoiBong.push_back({maDoiBongMoi, id});
        sortIndexVector(indexCauThuMaDoiBong);
        saveAllDataToFiles();
    }
    void updateCauThuBanThang(long long id, int soBanThangMoi)
    {
        cauThu *ct = dbCauThu.find_by_id(id);
        if (ct)
        {
            writeLog("UPDATE", "Cap nhat ban thang cau thu ID: " + std::to_string(id) + " -> So ban mới: " + std::to_string(soBanThangMoi));
            ct->setSoBanThang(soBanThangMoi);
            saveAllDataToFiles();
        }
    }
    bool delete_cauThu_by_id(long long id)
    {
        cauThu *ct = dbCauThu.find_by_id(id);
        if (!ct)
            return false;
        writeLog("DELETE", "Xoa cau thu ID: " + std::to_string(id) + ", Ten: " + ct->getTen());
        int posTuoi = findIndexPosition(indexCauThuTuoi, ct->getTuoi(), id);
        if (posTuoi != -1)
            indexCauThuTuoi.delete_by_index(posTuoi);
        int posDoi = findIndexPosition(indexCauThuMaDoiBong, ct->getMaDoiBong(), id);
        if (posDoi != -1)
            indexCauThuMaDoiBong.delete_by_index(posDoi);
        dbCauThu.delete_by_id(id);
        saveAllDataToFiles();
        return true;
    }
    bool insert_doiBong_single(const doiBong &db)
    {
        if (isIdExists("doibong", db.id))
            return false;
        dbDoiBong.insert_ordered(db);
        doiBong *permanentTeam = dbDoiBong.find_by_id(db.id);
        if (permanentTeam)
        {
            SecondaryIndex<const char *> item;
            item.key = permanentTeam->getDiaPhuong().c_str();
            item.entityId = permanentTeam->id;
            indexDoiBongDiaPhuong.push_back(item);
        }
        sortStringIndexVector(indexDoiBongDiaPhuong);
        writeLog("INSERT", "Them doi bong moi ID: " + std::to_string(db.id) + ", Ten: " + db.getTen());
        saveAllDataToFiles();
        return true;
    }
    bool updateDoiBong(long long id, const doiBong &newData)
    {
        doiBong *db = dbDoiBong.find_by_id(id);
        if (!db)
            return false;
        if (id != newData.id && isIdExists("doibong", newData.id))
            return false;
        writeLog("UPDATE", "Cap nhat CLB ID: " + std::to_string(id) + " - Ten: " + newData.getTen());
        db->setTen(newData.getTen().c_str());
        db->setHLV(newData.getHLV().c_str());
        db->setDiaPhuong(newData.getDiaPhuong().c_str());
        db->setDiemXepHang(newData.getDiemXepHang());
        saveAllDataToFiles();
        return true;
    }
    void updateDoiBongDiemXepHang(long long id, int diemMoi)
    {
        doiBong *db = dbDoiBong.find_by_id(id);
        if (db)
        {
            writeLog("UPDATE", "Cap nhat diem xep hang CLB ID: " + std::to_string(id) + " -> Diem moi: " + std::to_string(diemMoi));
            db->setDiemXepHang(diemMoi);
            saveAllDataToFiles();
        }
    }
    bool delete_doiBong_by_id(long long id)
    {
        doiBong *db = dbDoiBong.find_by_id(id);
        if (!db)
            return false;
        writeLog("DELETE", "Xoa clb ID: " + std::to_string(id) + ", Ten: " + db->getTen());
        dbDoiBong.delete_by_id(id);
        saveAllDataToFiles();
        return true;
    }
    bool insert_tranDau_single(const tranDau &td)
    {
        if (isIdExists("trandau", td.id))
            return false;
        dbTranDau.insert_ordered(td);
        indexTranDauNgay.push_back({td.getNgayThiDau(), td.id});
        sortIndexVector(indexTranDauNgay);
        writeLog("INSERT", "Them tran dau moi ID: " + std::to_string(td.id));
        saveAllDataToFiles();
        return true;
    }
    bool updateTranDau(long long id, const tranDau &newData)
    {
        tranDau *td = dbTranDau.find_by_id(id);
        if (!td)
            return false;
        if (id != newData.id && isIdExists("trandau", newData.id))
            return false;
        writeLog("UPDATE", "Cap nhat ti so tran dau ID: " + std::to_string(id));
        td->setSoBanThangDoi1(newData.getSoBanThangDoi1());
        td->setSoBanThangDoi2(newData.getSoBanThangDoi2());
        saveAllDataToFiles();
        return true;
    }
    void updateTranDauBanThang(long long id, int banDoi1Moi, int banDoi2Moi)
    {
        tranDau *td = dbTranDau.find_by_id(id);
        if (td)
        {
            writeLog("UPDATE", "Thay doi nhanh ban thang tran dau ID: " + std::to_string(id));
            td->setSoBanThangDoi1(banDoi1Moi);
            td->setSoBanThangDoi2(banDoi2Moi);
            saveAllDataToFiles();
        }
    }
    bool delete_tranDau_by_id(long long id)
    {
        if (dbTranDau.find_by_id(id) == nullptr)
            return false;
        writeLog("DELETE", "Xoa tran dau ID: " + std::to_string(id));
        dbTranDau.delete_by_id(id);
        saveAllDataToFiles();
        return true;
    }
    cauThu *getCauThuById(long long id) { return dbCauThu.find_by_id(id); }
    doiBong *getDoiBongById(long long id) { return dbDoiBong.find_by_id(id); }
    tranDau *getTranDauById(long long id) { return dbTranDau.find_by_id(id); }
    long long getNextCauThuId() { return dbCauThu.getNextId(); }
    long long getNextDoiBongId() { return dbDoiBong.getNextId(); }
    long long getNextTranDauId() { return dbTranDau.getNextId(); }
    int getCauThuCount() const { return dbCauThu.size(); }
    int getDoiBongCount() const { return dbDoiBong.size(); }
    int getTranDauCount() const { return dbTranDau.size(); }
};
#endif