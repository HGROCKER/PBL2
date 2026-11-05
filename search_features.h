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

} // namespace plb2

#endif // SEARCH_FEATURES_H