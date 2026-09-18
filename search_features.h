#pragma once
#ifndef SEARCH_FEATURES_H
#define SEARCH_FEATURES_H

#include <QString>
#include <vector>
#include <cmath>
#include "csdl.h"
#include "database.h"

namespace plb2 {

inline QString normalizeText(const QString& value) {
    QString out = value.trimmed();
    return out.toLower();
}

inline bool matchesText(const QString& value, const QString& filter) {
    if (filter.trimmed().isEmpty()) return true;
    return normalizeText(value).contains(normalizeText(filter));
}

inline bool matchesRange(long long value, long long minV, long long maxV) {
    if (minV >= 0 && value < minV) return false;
    if (maxV >= 0 && value > maxV) return false;
    return true;
}

struct PlayerSearchFilter {
    QString name;
    QString position;
    int teamId = -1;
    int minAge = -1;
    int maxAge = -1;
    int minNumber = -1;
    int maxNumber = -1;
    long long minGoals = -1;
    long long maxGoals = -1;
};

struct TeamSearchFilter {
    QString name;
    QString coach;
    QString location;
    int minPoints = -1;
    int maxPoints = -1;
};

struct MatchSearchFilter {
    int teamId = -1;
    long long fromDate = -1;
    long long toDate = -1;
    int minGoalDiff = -1;
    int maxGoalDiff = -1;
    int minTotalGoals = -1;
    int maxTotalGoals = -1;
};

// ==================== TỐI ƯU HÓA LỌC CẦU THỦ ====================
inline std::vector<cauThu*> filterPlayers(Database& db, const PlayerSearchFilter& filter) {
    std::vector<cauThu*> result;

    if (filter.teamId >= 0) {
        int startIndex = ::findFirstIndex(db.indexCauThuMaDoiBong, filter.teamId);
        if (startIndex != -1) {
            int totalSize = db.indexCauThuMaDoiBong.size();
            for (int i = startIndex; i != totalSize; ++i) {
                auto& idxRecord = db.indexCauThuMaDoiBong[i];
                if (idxRecord.key != filter.teamId) {
                    break;
                }

                cauThu* player = db.getCauThuById(idxRecord.entityId);
                if (!player) {
                    continue;
                }

                if (!matchesText(QString::fromStdString(player->getTen()), filter.name)) continue;
                if (!matchesText(QString::fromStdString(player->getViTri()), filter.position)) continue;
                if (!matchesRange(player->getTuoi(), filter.minAge, filter.maxAge)) continue;
                if (!matchesRange(player->getSoAo(), filter.minNumber, filter.maxNumber)) continue;
                if (!matchesRange(player->getSoBanThang(), filter.minGoals, filter.maxGoals)) continue;

                result.push_back(player);
            }
        }
    }
    else if (filter.minAge >= 0) {
        int startIndex = ::findFirstIndex(db.indexCauThuTuoi, filter.minAge);
        if (startIndex != -1) {
            int totalSize = db.indexCauThuTuoi.size();
            for (int i = startIndex; i != totalSize; ++i) {
                auto& idxRecord = db.indexCauThuTuoi[i];
                if (filter.maxAge >= 0 && idxRecord.key > filter.maxAge) {
                    break;
                }

                cauThu* player = db.getCauThuById(idxRecord.entityId);
                if (!player) {
                    continue;
                }

                if (!matchesText(QString::fromStdString(player->getTen()), filter.name)) continue;
                if (!matchesText(QString::fromStdString(player->getViTri()), filter.position)) continue;
                if (!matchesRange(player->getSoAo(), filter.minNumber, filter.maxNumber)) continue;
                if (!matchesRange(player->getSoBanThang(), filter.minGoals, filter.maxGoals)) continue;

                result.push_back(player);
            }
        }
    }
    else {
        int totalCount = db.getCauThuCount();
        for (int i = 1; i != (totalCount + 1); ++i) {
            cauThu* player = db.getCauThuById(i);
            if (!player) {
                continue;
            }

            if (!matchesText(QString::fromStdString(player->getTen()), filter.name)) continue;
            if (!matchesText(QString::fromStdString(player->getViTri()), filter.position)) continue;
            if (!matchesRange(player->getTuoi(), filter.minAge, filter.maxAge)) continue;
            if (!matchesRange(player->getSoAo(), filter.minNumber, filter.maxNumber)) continue;
            if (!matchesRange(player->getSoBanThang(), filter.minGoals, filter.maxGoals)) continue;

            result.push_back(player);
        }
    }
    return result;
}

// ==================== TỐI ƯU HÓA LỌC ĐỘI BÓNG ====================
inline std::vector<doiBong*> filterTeams(Database& db, const plb2::TeamSearchFilter& filter) {
    std::vector<doiBong*> result;
    int totalCount = db.getDoiBongCount();
    for (int i = 1; i != (totalCount + 1); ++i) {
        doiBong* team = db.getDoiBongById(i);
        if (!team) {
            continue;
        }

        if (!plb2::matchesText(QString::fromStdString(team->getTen()), filter.name)) continue;
        if (!plb2::matchesText(QString::fromStdString(team->getHLV()), filter.coach)) continue;
        if (!plb2::matchesText(QString::fromStdString(team->getDiaPhuong()), filter.location)) continue;
        if (!plb2::matchesRange(team->getDiemXepHang(), filter.minPoints, filter.maxPoints)) continue;

        result.push_back(team);
    }
    return result;
}

// ==================== TỐI ƯU HÓA LỌC TRẬN ĐẤU ====================
inline std::vector<tranDau*> filterMatches(Database& db, const MatchSearchFilter& filter) {
    std::vector<tranDau*> result;

    if (filter.fromDate >= 0) {
        int startIndex = ::findFirstIndex(db.indexTranDauNgay, filter.fromDate);
        if (startIndex != -1) {
            int totalSize = db.indexTranDauNgay.size();
            for (int i = startIndex; i != totalSize; ++i) {
                auto& idxRecord = db.indexTranDauNgay[i];
                if (filter.toDate >= 0 && idxRecord.key > filter.toDate) {
                    break;
                }

                tranDau* match = db.getTranDauById(idxRecord.entityId);
                if (!match) {
                    continue;
                }

                if (filter.teamId >= 0 && match->getMaDoiBong1() != filter.teamId && match->getMaDoiBong2() != filter.teamId) continue;

                int hieuSo = std::abs(match->getSoBanThangDoi1() - match->getSoBanThangDoi2());
                if (!plb2::matchesRange(hieuSo, filter.minGoalDiff, filter.maxGoalDiff)) continue;

                int tongBan = match->getSoBanThangDoi1() + match->getSoBanThangDoi2();
                if (!plb2::matchesRange(tongBan, filter.minTotalGoals, filter.maxTotalGoals)) continue;

                result.push_back(match);
            }
        }
    }
    else {
        int totalCount = db.getTranDauCount();
        for (int i = 1; i != (totalCount + 1); ++i) {
            tranDau* match = db.getTranDauById(i);
            if (!match) {
                continue;
            }

            if (filter.teamId >= 0 && match->getMaDoiBong1() != filter.teamId && match->getMaDoiBong2() != filter.teamId) continue;
            if (filter.toDate >= 0 && match->getNgayThiDau() > filter.toDate) continue;

            int hieuSo = std::abs(match->getSoBanThangDoi1() - match->getSoBanThangDoi2());
            if (!plb2::matchesRange(hieuSo, filter.minGoalDiff, filter.maxGoalDiff)) continue;

            int tongBan = match->getSoBanThangDoi1() + match->getSoBanThangDoi2();
            if (!plb2::matchesRange(tongBan, filter.minTotalGoals, filter.maxTotalGoals)) continue;

            result.push_back(match);
        }
    }
    return result;
}

} // namespace plb2

#endif // SEARCH_FEATURES_H
