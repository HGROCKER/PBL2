#pragma once
#ifndef SEARCH_FEATURES_H
#define SEARCH_FEATURES_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <vector>
#include <cmath>
#include "csdl.h"

namespace plb2 {

inline QString normalizeText(const QString& value) {
    return value.trimmed().toCaseFolded();
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

inline bool isValidRange(long long minV, long long maxV) {
    return minV < 0 || maxV < 0 || minV <= maxV;
}

inline int calculateAge(int birthYear, int birthMonth, int birthDay,
                        int currentYear, int currentMonth, int currentDay) {
    int age = currentYear - birthYear;
    if (currentMonth < birthMonth ||
        (currentMonth == birthMonth && currentDay < birthDay)) {
        --age;
    }
    return age;
}

inline int playerAgeAt(const cauThu& player, const QDate& asOfDate = QDate::currentDate()) {
    if (player.getNgaySinh() <= 0) return player.getTuoi();

    const QDate birthDate = QDateTime::fromSecsSinceEpoch(player.getNgaySinh()).date();
    if (!birthDate.isValid() || birthDate > asOfDate) return -1;
    return calculateAge(birthDate.year(), birthDate.month(), birthDate.day(),
                        asOfDate.year(), asOfDate.month(), asOfDate.day());
}

struct PlayerSearchFilter {
    long long playerId = -1;
    QString name;
    QString position;
    QString documentNumber;
    int teamId = -1;
    int minAge = -1;
    int maxAge = -1;
    int minNumber = -1;
    int maxNumber = -1;
    long long minGoals = -1;
    long long maxGoals = -1;
    int minBirthYear = -1;
    int maxBirthYear = -1;
};

struct TeamSearchFilter {
    long long teamRecordId = -1;
    QString name;
    QString coach;
    QString location;
    int minPoints = -1;
    int maxPoints = -1;
};

struct MatchSearchFilter {
    long long matchId = -1;
    int teamId = -1;
    long long fromDate = -1;
    long long toDate = -1;
    int minGoalDiff = -1;
    int maxGoalDiff = -1;
    int minTotalGoals = -1;
    int maxTotalGoals = -1;
};

<<<<<<< HEAD
=======
inline std::vector<cauThu*> filterPlayers(Database& db, const PlayerSearchFilter& filter) {
    std::vector<cauThu*> result;

    const auto matchesFilter = [&](const cauThu& player) {
        if (filter.playerId >= 0 && player.getId() != filter.playerId) return false;
        if (!matchesText(QString::fromStdString(player.getTen()), filter.name)) return false;
        if (!matchesText(QString::fromStdString(player.getViTri()), filter.position)) return false;
        if (!matchesText(QString::fromStdString(player.getSoGiayTo()), filter.documentNumber)) return false;
        if (filter.minAge >= 0 || filter.maxAge >= 0) {
            const int age = playerAgeAt(player);
            if (age < 0 || !matchesRange(age, filter.minAge, filter.maxAge)) return false;
        }
        if (!matchesRange(player.getSoAo(), filter.minNumber, filter.maxNumber)) return false;
        if (!matchesRange(player.getSoBanThang(), filter.minGoals, filter.maxGoals)) return false;
        if (filter.minBirthYear >= 0 || filter.maxBirthYear >= 0) {
            if (player.getNgaySinh() <= 0) return false;
            const QDate birthDate = QDateTime::fromSecsSinceEpoch(player.getNgaySinh()).date();
            if (!birthDate.isValid()) return false;
            const int birthYear = birthDate.year();
            if (!matchesRange(birthYear, filter.minBirthYear, filter.maxBirthYear)) return false;
        }
        return true;
    };

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
                if (matchesFilter(*player)) result.push_back(player);
            }
        }
    }
    else {
        int totalCount = db.getCauThuCount();
        for (int i = 0; i < totalCount; ++i) {
            cauThu* player = db.getCauThuAt(i);
            if (!player) {
                continue;
            }
            if (matchesFilter(*player)) result.push_back(player);
        }
    }
    return result;
}

inline std::vector<doiBong*> filterTeams(Database& db, const plb2::TeamSearchFilter& filter) {
    std::vector<doiBong*> result;
    int totalCount = db.getDoiBongCount();
    for (int i = 0; i < totalCount; ++i) {
        doiBong* team = db.getDoiBongAt(i);
        if (!team) {
            continue;
        }

        if (filter.teamRecordId >= 0 && team->getId() != filter.teamRecordId) continue;
        if (!plb2::matchesText(QString::fromStdString(team->getTen()), filter.name)) continue;
        if (!plb2::matchesText(QString::fromStdString(team->getHLV()), filter.coach)) continue;
        if (!plb2::matchesText(QString::fromStdString(team->getDiaPhuong()), filter.location)) continue;
        if (!plb2::matchesRange(team->getDiemXepHang(), filter.minPoints, filter.maxPoints)) continue;

        result.push_back(team);
    }
    return result;
}

inline std::vector<tranDau*> filterMatches(Database& db, const MatchSearchFilter& filter) {
    std::vector<tranDau*> result;

    const auto matchesFilter = [&](const tranDau& match) {
        if (filter.matchId >= 0 && match.getId() != filter.matchId) return false;
        if (filter.teamId >= 0 &&
            match.getMaDoiBong1() != filter.teamId &&
            match.getMaDoiBong2() != filter.teamId) return false;
        if (filter.fromDate >= 0 && match.getNgayThiDau() < filter.fromDate) return false;
        if (filter.toDate >= 0 && match.getNgayThiDau() > filter.toDate) return false;
        const int goalDifference = std::abs(match.getSoBanThangDoi1() - match.getSoBanThangDoi2());
        if (!matchesRange(goalDifference, filter.minGoalDiff, filter.maxGoalDiff)) return false;
        const int totalGoals = match.getSoBanThangDoi1() + match.getSoBanThangDoi2();
        return matchesRange(totalGoals, filter.minTotalGoals, filter.maxTotalGoals);
    };

    if (filter.fromDate >= 0) {
        int startIndex = ::findFirstIndexAtOrAfter(db.indexTranDauNgay, filter.fromDate);
        if (startIndex >= 0) {
            int totalSize = db.indexTranDauNgay.size();
            for (int i = startIndex; i < totalSize; ++i) {
                auto& idxRecord = db.indexTranDauNgay[i];
                if (filter.toDate >= 0 && idxRecord.key > filter.toDate) {
                    break;
                }

                tranDau* match = db.getTranDauById(idxRecord.entityId);
                if (!match) {
                    continue;
                }
                if (matchesFilter(*match)) result.push_back(match);
            }
        }
    }
    else {
        int totalCount = db.getTranDauCount();
        for (int i = 0; i < totalCount; ++i) {
            tranDau* match = db.getTranDauAt(i);
            if (!match) {
                continue;
            }
            if (matchesFilter(*match)) result.push_back(match);
        }
    }
    return result;
}

>>>>>>> 9a8c9805284d4e8efaa4fcfd441d1230d4cc0a35
} // namespace plb2

#endif // SEARCH_FEATURES_H