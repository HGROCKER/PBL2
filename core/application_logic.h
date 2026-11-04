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
    InvalidDocumentNumber,
    DuplicateDocumentNumber,
    NotFound,
    DuplicateId,
    InvalidSchedule,
    CannotDelete,
    InvalidSampleData
};

struct OperationResult {
    LogicError error = LogicError::None;
    bool logged = false;
    long long entityId = -1;
    std::string logError;

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

class ApplicationLogic {
public:
    explicit ApplicationLogic(Database& database) : db(database) {}

    OperationResult savePlayer(cauThu player, bool edit, long long id = -1) {
        if (player.getTen().empty()) return {LogicError::InvalidName};
        if (edit && !db.getCauThuById(id)) return {LogicError::NotFound};
        std::string documentNumber = player.getSoGiayTo();
        const auto first = documentNumber.find_first_not_of(" \t\r\n");
        const auto last = documentNumber.find_last_not_of(" \t\r\n");
        if (first == std::string::npos) return {LogicError::InvalidDocumentNumber};
        documentNumber = documentNumber.substr(first, last - first + 1);
        player.setSoGiayTo(documentNumber);
        if (db.hasPlayerDocumentNumber(documentNumber, edit ? id : -1))
            return {LogicError::DuplicateDocumentNumber};
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
            if (standings.empty()) return progress;
            const auto& champion = standings.front();
            const int championGoalDifference = champion.goalsFor - champion.goalsAgainst;
            for (const auto& row : standings) {
                if (row.points != champion.points ||
                    row.goalsFor - row.goalsAgainst != championGoalDifference ||
                    row.goalsFor != champion.goalsFor) {
                    break;
                }
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

    OperationResult loadSampleData() {
        db.dbCauThu.clear();
        db.dbDoiBong.clear();
        db.dbTranDau.clear();
        db.indexCauThuMaDoiBong.clear();
        db.indexTranDauNgay.clear();

        struct TeamSample {
            const char* name;
            const char* location;
        };
        const TeamSample teams[] = {
            {"Hà Nội T&T", "Hà Nội"},
            {"Hải Phòng", "Hải Phòng"},
            {"SHB Đà Nẵng", "Đà Nẵng"},
            {"Than Quảng Ninh", "Quảng Ninh"},
            {"QNK Quảng Nam", "Quảng Nam"},
            {"FLC Thanh Hóa", "Thanh Hóa"},
            {"Sài Gòn", "Thành phố Hồ Chí Minh"},
            {"Sanna Khánh Hòa", "Khánh Hòa"},
            {"Sông Lam Nghệ An", "Nghệ An"},
            {"Becamex Bình Dương", "Bình Dương"},
            {"XSKT Cần Thơ", "Cần Thơ"},
            {"Hoàng Anh Gia Lai", "Gia Lai"},
            {"Long An", "Long An"},
            {"Đồng Tháp", "Đồng Tháp"}
        };
        constexpr int teamCount = static_cast<int>(sizeof(teams) / sizeof(teams[0]));
        for (int i = 0; i < teamCount; ++i) {
            doiBong team(i + 1, teams[i].name, "Chưa xác minh", teams[i].location);
            team.setSoCauThu(18);
            team.setSoCauThuHienTai(18);
            if (!db.insert_doiBong_single(team)) return {LogicError::DuplicateId};
        }

        const std::vector<std::string> positions = {
            "GK", "GK", "CB", "CB", "LB", "RB", "CB", "LB", "RB",
            "DM", "CM", "CM", "AM", "LW", "RW", "ST", "ST", "FW"
        };
        long long playerId = 1;
        for (int teamId = 1; teamId <= teamCount; ++teamId) {
            for (size_t position = 0; position < positions.size(); ++position) {
                const std::string name = "Cầu thủ mẫu " + std::to_string(teamId) + "-" +
                                         std::to_string(position + 1);
                const int birthYear = 1985 + static_cast<int>(playerId % 15);
                cauThu player(playerId, name.c_str(), positions[position].c_str(),
                              2016 - birthYear, static_cast<int>(position + 1), teamId);
                player.setSoGiayTo("SAMPLE-" + std::to_string(playerId));
                player.setSoBanThang(static_cast<int>(playerId % 5));
                player.setNgaySinh(QDate(birthYear, 1 + static_cast<int>(playerId % 12),
                                         1 + static_cast<int>(playerId % 27))
                                       .startOfDay().toSecsSinceEpoch());
                player.setChieuCao(165 + static_cast<int>((playerId * 7) % 31));
                player.setCanNang(58 + static_cast<int>((playerId * 3) % 35));
                player.setSoTranChoi(static_cast<int>(playerId % 30));
                player.setMaQuocGia("VIE");
                if (!db.insert_cauThu_single(player)) return {LogicError::DuplicateId};
                ++playerId;
            }
        }

        static const char fixtureData[] = R"FIXTURES(
2016-02-20|10|2|0|11
2016-02-20|6|3|0|1
2016-02-20|3|2|1|14
2016-02-21|7|0|5|12
2016-02-21|13|1|0|5
2016-02-21|9|0|2|2
2016-02-21|4|1|2|8
2016-02-27|8|0|2|6
2016-02-27|1|0|1|4
2016-02-27|11|2|0|9
2016-02-28|14|0|1|7
2016-02-28|2|5|0|13
2016-02-28|12|1|2|3
2016-05-18|5|2|1|10
2016-03-05|2|1|0|11
2016-03-05|3|1|0|8
2016-03-05|9|3|3|5
2016-03-06|12|0|0|4
2016-03-06|13|1|1|10
2016-03-06|1|1|1|7
2016-05-18|6|4|0|14
2016-03-10|10|1|2|9
2016-03-12|5|0|1|2
2016-03-12|8|1|0|1
2016-03-12|4|2|4|6
2016-03-13|14|2|1|12
2016-03-13|7|3|0|3
2016-03-13|11|2|1|13
2016-04-10|1|1|2|10
2016-04-10|2|4|2|12
2016-04-10|5|2|0|6
2016-04-10|8|2|0|13
2016-04-10|3|1|0|4
2016-04-10|9|1|0|7
2016-04-10|11|4|1|14
2016-04-15|10|3|1|8
2016-04-16|14|0|1|2
2016-04-16|13|2|5|1
2016-04-16|4|3|1|9
2016-04-17|6|3|1|3
2016-04-17|12|2|1|11
2016-04-17|7|0|0|5
2016-04-23|2|2|1|4
2016-04-23|5|1|1|8
2016-04-23|11|5|2|6
2016-04-24|14|2|5|1
2016-04-24|12|0|0|9
2016-04-24|7|3|1|13
2016-04-25|3|1|1|10
2016-04-30|2|0|0|10
2016-04-30|8|0|2|7
2016-04-30|4|4|0|14
2016-05-01|6|2|1|12
2016-05-01|1|2|1|3
2016-05-01|9|1|1|13
2016-05-01|11|0|1|5
2016-05-07|13|2|1|14
2016-05-07|5|1|1|3
2016-05-07|8|2|0|11
2016-05-07|4|1|0|7
2016-05-08|10|5|0|12
2016-05-08|6|2|2|9
2016-05-08|1|2|1|2
2016-05-13|14|1|2|10
2016-05-13|5|1|1|4
2016-05-14|7|3|1|6
2016-05-14|3|1|1|9
2016-05-15|2|2|0|8
2016-05-15|12|3|2|13
2016-05-15|11|3|0|1
2016-05-21|13|1|3|3
2016-05-21|8|4|1|12
2016-05-21|4|2|1|11
2016-05-22|10|1|1|7
2016-05-22|6|0|1|2
2016-05-22|1|3|0|5
2016-05-22|9|3|0|14
2016-06-19|6|2|1|13
2016-06-19|1|3|0|12
2016-06-19|2|1|1|7
2016-06-19|5|2|0|14
2016-06-19|8|2|2|9
2016-06-19|4|2|1|10
2016-06-19|11|1|2|3
2016-06-25|10|0|3|6
2016-06-25|14|1|1|8
2016-06-25|12|1|4|5
2016-06-25|13|1|2|4
2016-06-25|7|0|1|11
2016-06-25|3|2|0|2
2016-06-25|9|1|3|1
2016-07-03|1|1|1|13
2016-07-03|2|3|2|14
2016-07-03|5|3|3|7
2016-07-03|8|1|1|10
2016-07-03|3|2|1|6
2016-07-03|9|4|0|4
2016-07-03|11|2|1|12
2016-07-09|10|4|5|1
2016-07-09|13|2|1|8
2016-07-09|4|3|2|3
2016-07-10|14|1|2|11
2016-07-10|6|4|4|5
2016-07-10|12|1|1|2
2016-07-10|7|1|0|9
2016-07-16|10|1|0|4
2016-07-16|14|1|1|5
2016-07-16|12|1|0|1
2016-07-16|13|0|1|6
2016-07-17|7|3|3|2
2016-07-17|3|2|2|11
2016-07-17|9|1|2|8
2016-07-23|8|1|1|14
2016-07-23|11|0|1|7
2016-07-24|6|2|2|10
2016-07-24|1|1|0|9
2016-07-24|2|0|3|3
2016-07-24|5|2|0|12
2016-07-24|4|2|1|13
2016-07-29|1|2|0|14
2016-07-29|9|1|0|12
2016-07-30|10|0|1|3
2016-07-30|6|3|2|11
2016-07-30|8|0|1|5
2016-07-31|13|1|0|7
2016-07-31|4|3|1|2
2016-08-06|13|1|2|9
2016-08-07|10|1|3|2
2016-08-07|14|1|1|4
2016-08-07|12|3|1|6
2016-08-07|5|0|0|11
2016-08-07|7|1|1|8
2016-08-07|3|1|2|1
2016-08-12|7|0|0|4
2016-08-13|14|1|3|13
2016-08-13|2|2|1|1
2016-08-13|12|3|1|10
2016-08-13|3|3|4|5
2016-08-13|9|1|0|6
2016-08-13|11|0|1|8
2016-08-17|10|3|0|14
2016-08-17|6|2|2|7
2016-08-17|1|0|1|11
2016-08-17|13|1|3|12
2016-08-17|8|3|0|2
2016-08-17|9|0|2|3
2016-08-17|4|2|0|5
2016-08-21|14|2|2|9
2016-08-21|2|1|1|6
2016-08-21|12|2|1|8
2016-08-21|5|0|1|1
2016-08-21|7|0|2|10
2016-08-21|3|4|1|13
2016-08-21|11|0|2|4
2016-08-27|14|2|4|6
2016-08-27|7|0|3|1
2016-08-27|8|3|1|3
2016-08-28|10|3|2|13
2016-08-28|5|2|0|9
2016-08-28|4|3|1|12
2016-08-28|11|3|2|2
2016-09-04|6|2|2|4
2016-09-04|1|1|0|8
2016-09-04|2|2|1|5
2016-09-04|12|5|2|14
2016-09-04|13|3|3|11
2016-09-04|3|3|1|7
2016-09-04|9|2|0|10
2016-09-11|10|1|3|5
2016-09-11|6|2|1|8
2016-09-11|13|2|5|2
2016-09-11|7|4|0|14
2016-09-11|3|3|1|12
2016-09-11|9|4|2|11
2016-09-11|4|0|1|1
2016-09-18|14|0|4|3
2016-09-18|1|2|0|6
2016-09-18|2|3|0|9
2016-09-18|12|1|3|7
2016-09-18|5|6|2|13
2016-09-18|8|3|1|4
2016-09-18|11|0|0|10
)FIXTURES";
        std::istringstream fixtureLines(fixtureData);
        std::string line;
        long long matchId = 1;
        while (std::getline(fixtureLines, line)) {
            if (line.empty()) continue;
            std::istringstream row(line);
            std::vector<std::string> fields;
            std::string field;
            while (std::getline(row, field, '|')) fields.push_back(field);
            if (fields.size() != 5) return {LogicError::InvalidSampleData};

            auto parseInt = [](const std::string& value, int& result) {
                std::istringstream input(value);
                char trailing;
                return static_cast<bool>(input >> result) && !(input >> trailing);
            };
            int homeTeamId, homeGoals, awayGoals, awayTeamId;
            if (!parseInt(fields[1], homeTeamId) || !parseInt(fields[2], homeGoals) ||
                !parseInt(fields[3], awayGoals) || !parseInt(fields[4], awayTeamId)) {
                return {LogicError::InvalidSampleData};
            }
            const QDate matchDate = QDate::fromString(QString::fromStdString(fields[0]),
                                                       Qt::ISODate);
            if (!matchDate.isValid() || homeTeamId < 1 || homeTeamId > teamCount ||
                awayTeamId < 1 || awayTeamId > teamCount || homeTeamId == awayTeamId ||
                homeGoals < 0 || awayGoals < 0) {
                return {LogicError::InvalidSampleData};
            }
            if (!db.insert_tranDau_single(tranDau(
                    matchId++, homeTeamId, awayTeamId, homeGoals, awayGoals,
                    matchDate.startOfDay().toSecsSinceEpoch(), 0, 0))) {
                return {LogicError::InvalidSampleData};
            }
        }

        if (matchId != 183) return {LogicError::InvalidSampleData};
        const auto standings = db.calculateStandingsAt(
            QDate(2016, 12, 31).startOfDay().toSecsSinceEpoch());
        if (standings.size() != teamCount) return {LogicError::InvalidSampleData};
        for (const auto& standing : standings) {
            doiBong* team = db.getDoiBongById(standing.teamId);
            if (!team) return {LogicError::InvalidSampleData};
            team->setDiemXepHang(standing.points);
            team->setSoTranThang(standing.wins);
            team->setSoTranHoa(standing.draws);
            team->setSoTranThua(standing.losses);
            team->setSoBanThang(standing.goalsFor);
            team->setSoBanThua(standing.goalsAgainst);
        }
        return {};
    }

private:
    Database& db;

    OperationResult persist(const std::string& action, const std::string& entity,
                            long long id) {
        OperationResult result;
        result.entityId = id;
        result.logged = db.persistAndLog(action, entity, id, result.logError);
        return result;
    }
};

} // namespace plb2

#endif
