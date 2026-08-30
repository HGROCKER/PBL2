#pragma once
#ifndef SEARCH_FEATURES_H
#define SEARCH_FEATURES_H

#include <QString>
#include <vector>
#include <algorithm>
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

inline bool matchesRange(int value, int minV, int maxV) {
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
    int minGoals = -1;
    int maxGoals = -1;
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

inline std::vector<cauThu*> filterPlayers(Database& db, const PlayerSearchFilter& filter) {
    std::vector<cauThu*> result;
    for (int i = 1; i <= db.getCauThuCount(); ++i) {
        cauThu* player = db.getCauThuById(i);
        if (!player) continue;

        if (!matchesText(QString::fromStdString(player->getTen()), filter.name)) continue;
        if (!matchesText(QString::fromStdString(player->getViTri()), filter.position)) continue;
        if (filter.teamId != -1 && player->getMaDoiBong() != filter.teamId) continue;
        if (!matchesRange(player->getTuoi(), filter.minAge, filter.maxAge)) continue;
        if (!matchesRange(player->getSoAo(), filter.minNumber, filter.maxNumber)) continue;
        if (!matchesRange(player->getSoBanThang(), filter.minGoals, filter.maxGoals)) continue;

        result.push_back(player);
    }
    return result;
}

inline std::vector<doiBong*> filterTeams(Database& db, const TeamSearchFilter& filter) {
    std::vector<doiBong*> result;
    for (int i = 1; i <= db.getDoiBongCount(); ++i) {
        doiBong* team = db.getDoiBongById(i);
        if (!team) continue;

        if (!matchesText(QString::fromStdString(team->getTen()), filter.name)) continue;
        if (!matchesText(QString::fromStdString(team->getHLV()), filter.coach)) continue;
        if (!matchesText(QString::fromStdString(team->getDiaPhuong()), filter.location)) continue;
        if (!matchesRange(team->getDiemXepHang(), filter.minPoints, filter.maxPoints)) continue;

        result.push_back(team);
    }
    return result;
}

inline std::vector<tranDau*> filterMatches(Database& db, const MatchSearchFilter& filter) {
    std::vector<tranDau*> result;
    for (int i = 1; i <= db.getTranDauCount(); ++i) {
        tranDau* match = db.getTranDauById(i);
        if (!match) continue;

        if (filter.teamId != -1 &&
            match->getDoiBong1() != filter.teamId &&
            match->getDoiBong2() != filter.teamId) {
            continue;
        }

        if (filter.fromDate >= 0 && match->getNgayThiDau() < filter.fromDate) continue;
        if (filter.toDate >= 0 && match->getNgayThiDau() > filter.toDate) continue;

        int goalDiff = std::abs(match->getSoBanThangDoi1() - match->getSoBanThangDoi2());
        if (!matchesRange(goalDiff, filter.minGoalDiff, filter.maxGoalDiff)) continue;

        int totalGoals = match->getSoBanThangDoi1() + match->getSoBanThangDoi2();
        if (!matchesRange(totalGoals, filter.minTotalGoals, filter.maxTotalGoals)) continue;

        result.push_back(match);
    }
    return result;
}

} // namespace plb2

#endif