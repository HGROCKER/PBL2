#pragma once
#ifndef PLB2_APPLICATION_LOGIC_H
#define PLB2_APPLICATION_LOGIC_H

#include <QDate>
#include <QDateTime>
#include <QTime>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "../database.h"
#include "../search_features.h"

namespace plb2 {

enum class LogicError {
    None,
    InvalidName,
    NotFound,
    DuplicateId,
    InvalidSchedule,
    CannotDelete
};

struct OperationResult {
    LogicError error = LogicError::None;
    bool persisted = true;
    long long entityId = -1;
    std::string persistenceError;

    bool succeeded() const { return error == LogicError::None; }
};

struct PlayerStatistics {
    const cauThu* tallest = nullptr;
    const cauThu* shortest = nullptr;
    const cauThu* youngest = nullptr;
};

struct TournamentProgress {
    bool complete = false;
    int completedMatches = 0;
    long long expectedMatches = 0;
    std::vector<std::string> leaders;
};

enum class MatchOutcome {
    HomeWin,
    AwayWin,
    Draw
};

enum class MatchDateMode {
    All,
    Day,
    Month,
    Year,
    Range
};

inline int calculateAge(int birthYear, int birthMonth, int birthDay,
                        int currentYear, int currentMonth, int currentDay) {
    int age = currentYear - birthYear;
    if (currentMonth < birthMonth ||
        (currentMonth == birthMonth && currentDay < birthDay)) {
        --age;
    }
    return age;
}

class ApplicationLogic {
public:
    explicit ApplicationLogic(Database& database) : db(database) {}

    OperationResult savePlayer(cauThu player, bool edit, long long id = -1) {
        if (player.getTen().empty()) return {LogicError::InvalidName};
        const long long playerId = edit ? id : db.getNextCauThuId();
        if (edit) {
            if (!db.updateCauThu(playerId, player)) return {LogicError::NotFound};
        } else {
            player.id = playerId;
            if (!db.insert_cauThu_single(player)) return {LogicError::DuplicateId};
        }
        return persist(edit ? "UPDATE" : "INSERT", "cauThu", playerId);
    }

    OperationResult deletePlayer(long long id) {
        if (!db.delete_cauThu_by_id(id)) return {LogicError::NotFound};
        return persist("DELETE", "cauThu", id);
    }

    std::vector<const cauThu*> playersForTeam(long long teamId) const {
        std::vector<const cauThu*> players;
        for (int i = 0; i < db.getCauThuCount(); ++i) {
            const cauThu* player = db.getCauThuAt(i);
            if (player && player->getMaDoiBong() == teamId) players.push_back(player);
        }
        return players;
    }

    OperationResult saveTeam(doiBong team, bool edit, long long id = -1) {
        if (team.getTen().empty()) return {LogicError::InvalidName};
        const long long teamId = edit ? id : db.getNextDoiBongId();
        if (edit) {
            if (!db.updateDoiBong(teamId, team)) return {LogicError::NotFound};
        } else {
            team.id = teamId;
            if (!db.insert_doiBong_single(team)) return {LogicError::DuplicateId};
        }
        return persist(edit ? "UPDATE" : "INSERT", "doiBong", teamId);
    }

    OperationResult deleteTeam(long long id) {
        if (!db.delete_doiBong_by_id(id)) return {LogicError::CannotDelete};
        return persist("DELETE", "doiBong", id);
    }

    OperationResult saveMatch(const tranDau& match, bool edit, long long id = -1) {
        const long long matchId = edit ? id : db.getNextTranDauId();
        if (!db.canScheduleMatch(match.getMaDoiBong1(), match.getMaDoiBong2(),
                                 edit ? matchId : -1)) {
            return {LogicError::InvalidSchedule};
        }

        tranDau updated(matchId, match.getMaDoiBong1(), match.getMaDoiBong2(),
                        match.getSoBanThangDoi1(), match.getSoBanThangDoi2(),
                        match.getNgayThiDau(), match.getSoTheVangDoi1(),
                        match.getSoTheVangDoi2(), match.getSanVanDong());
        const bool changed = edit ? db.updateTranDau(matchId, updated)
                                  : db.insert_tranDau_single(updated);
        if (!changed) return {edit ? LogicError::NotFound : LogicError::DuplicateId};
        return persist(edit ? "UPDATE" : "INSERT", "tranDau", matchId);
    }

    OperationResult deleteMatch(long long id) {
        if (!db.delete_tranDau_by_id(id)) return {LogicError::NotFound};
        return persist("DELETE", "tranDau", id);
    }

    MatchSearchFilter matchSearchFilter(
        int teamId, int minGoalDiff, int maxGoalDiff, int minTotalGoals,
        int maxTotalGoals, MatchDateMode dateMode, QDate from, QDate to) const {
        MatchSearchFilter filter;
        filter.teamId = teamId;
        filter.minGoalDiff = minGoalDiff;
        filter.maxGoalDiff = maxGoalDiff;
        filter.minTotalGoals = minTotalGoals;
        filter.maxTotalGoals = maxTotalGoals;

        if (dateMode == MatchDateMode::All) return filter;
        if (dateMode == MatchDateMode::Day) {
            to = from;
        } else if (dateMode == MatchDateMode::Month) {
            from = QDate(from.year(), from.month(), 1);
            to = from.addMonths(1).addDays(-1);
        } else if (dateMode == MatchDateMode::Year) {
            from = QDate(from.year(), 1, 1);
            to = QDate(from.year(), 12, 31);
        } else if (from > to) {
            std::swap(from, to);
        }

        filter.fromDate = QDateTime(from, QTime(0, 0)).toSecsSinceEpoch();
        filter.toDate = QDateTime(to.addDays(1), QTime(0, 0)).toSecsSinceEpoch() - 1;
        return filter;
    }

    std::vector<tranDau*> findMatches(const MatchSearchFilter& filter) const {
        return filterMatches(db, filter);
    }

    PlayerStatistics playerStatistics() const {
        PlayerStatistics statistics;
        for (int i = 0; i < db.getCauThuCount(); ++i) {
            const cauThu* player = db.getCauThuAt(i);
            if (!player) continue;
            if (player->getChieuCao() > 0 &&
                (!statistics.tallest ||
                 player->getChieuCao() > statistics.tallest->getChieuCao())) {
                statistics.tallest = player;
            }
            if (player->getChieuCao() > 0 &&
                (!statistics.shortest ||
                 player->getChieuCao() < statistics.shortest->getChieuCao())) {
                statistics.shortest = player;
            }
            if (player->getNgaySinh() > 0 &&
                (!statistics.youngest ||
                 player->getNgaySinh() > statistics.youngest->getNgaySinh())) {
                statistics.youngest = player;
            }
        }
        return statistics;
    }

    std::vector<Database::TeamStanding> standingsAt(long long cutoffTimestamp) const {
        return db.calculateStandingsAt(cutoffTimestamp);
    }

    static int goalDifference(int goalsFor, int goalsAgainst) {
        return goalsFor - goalsAgainst;
    }

    static MatchOutcome outcomeOf(const tranDau& match) {
        if (match.getSoBanThangDoi1() > match.getSoBanThangDoi2())
            return MatchOutcome::HomeWin;
        if (match.getSoBanThangDoi1() < match.getSoBanThangDoi2())
            return MatchOutcome::AwayWin;
        return MatchOutcome::Draw;
    }

    TournamentProgress tournamentProgress(long long currentTimestamp) const {
        TournamentProgress progress;
        const int teamCount = db.getDoiBongCount();
        progress.expectedMatches = static_cast<long long>(teamCount) * (teamCount - 1);
        progress.complete = db.hasCompleteDoubleRoundRobinAt(currentTimestamp);
        if (progress.complete) {
            const auto standings = db.calculateStandingsAt(currentTimestamp);
            const int highestPoints = standings.empty() ? -1 : standings.front().points;
            for (const auto& row : standings) {
                if (row.points != highestPoints) break;
                progress.leaders.push_back(row.teamName);
            }
        } else {
            for (int i = 0; i < db.getTranDauCount(); ++i) {
                const tranDau* match = db.getTranDauAt(i);
                if (match && match->getNgayThiDau() <= currentTimestamp)
                    ++progress.completedMatches;
            }
        }
        return progress;
    }

    OperationResult loadSampleData(int currentYear, int currentMonth, int currentDay) {
        db.dbCauThu.clear();
        db.dbDoiBong.clear();
        db.dbTranDau.clear();
        db.indexCauThuTuoi.clear();
        db.indexCauThuMaDoiBong.clear();
        db.indexDoiBongDiaPhuong.clear();
        db.indexTranDauNgay.clear();

        const std::vector<std::pair<std::string, std::string>> teams = {
            {"Manchester United", "Manchester"}, {"Liverpool", "Liverpool"},
            {"Arsenal", "London"}, {"Chelsea", "London"},
            {"Manchester City", "Manchester"}, {"Real Madrid", "Madrid"},
            {"Barcelona", "Barcelona"}, {"Bayern Munich", "Munich"}
        };
        for (size_t i = 0; i < teams.size(); ++i) {
            doiBong team(static_cast<long long>(i + 1), teams[i].first.c_str(),
                         "HLV Truong", teams[i].second.c_str());
            team.setDiemXepHang(0);
            team.setNgayThanhLap(QDate(1900 + static_cast<int>(i) * 10, 1, 1)
                                     .startOfDay().toSecsSinceEpoch());
            if (!db.insert_doiBong_single(team)) return {LogicError::DuplicateId};
        }

        const std::vector<std::string> positions = {
            "GK", "LB", "CB", "CB", "RB", "CM", "CM", "AM", "LW", "RW", "ST"
        };
        long long playerId = 1;
        const QDate today(currentYear, currentMonth, currentDay);
        for (int teamId = 1; teamId <= static_cast<int>(teams.size()); ++teamId) {
            const doiBong* team = db.getDoiBongById(teamId);
            const std::string teamName = team ? team->getTen() : "Team";
            for (size_t position = 0; position < positions.size(); ++position) {
                const std::string name = teamName + " Player " + std::to_string(position + 1);
                const int age = 18 + static_cast<int>(playerId % 15);
                cauThu player(playerId, name.c_str(), positions[position].c_str(), age,
                              position == 0 ? 1 : static_cast<int>(position + 1), teamId);
                player.setSoBanThang(static_cast<int>(playerId % 5));
                player.setNgaySinh(today.addYears(-age)
                                       .addDays(-static_cast<int>(playerId % 365))
                                       .startOfDay().toSecsSinceEpoch());
                player.setChieuCao(165 + static_cast<int>((playerId * 7) % 31));
                player.setCanNang(58 + static_cast<int>((playerId * 3) % 35));
                player.setSoTranChoi(static_cast<int>(playerId % 30));
                player.setMaQuocGia("VIE");
                if (!db.insert_cauThu_single(player)) return {LogicError::DuplicateId};
                ++playerId;
            }
        }
        if (!db.insert_tranDau_single(tranDau(
                1, 1, 2, 2, 1, QDate(2024, 1, 1).startOfDay().toSecsSinceEpoch(), 1, 2)) ||
            !db.insert_tranDau_single(tranDau(
                2, 3, 4, 0, 3, QDate(2024, 1, 2).startOfDay().toSecsSinceEpoch(), 2, 1))) {
            return {LogicError::DuplicateId};
        }
        return {};
    }

private:
    Database& db;

    OperationResult persist(const std::string& action, const std::string& entity,
                            long long id) {
        OperationResult result;
        result.entityId = id;
        result.persisted = db.persistAndLog(action, entity, id, result.persistenceError);
        return result;
    }
};

} // namespace plb2

#endif
