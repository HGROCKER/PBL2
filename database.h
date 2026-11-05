#pragma once
#ifndef DATABASE_H
#define DATABASE_H
#include "vector.h"
#include "csdl.h"
#include "search_features.h"
#include <algorithm>
#include <cstring>
#include <cctype>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <set>
#include <filesystem>
#include <ctime>
using namespace std;

template <typename K>
int findIndexPosition(MyVector<SecondaryIndex<K>>& idxVec, K key, long long entityId) {
    int low = 0;
    int high = idxVec.size();
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (idxVec[mid].key < key) low = mid + 1;
        else high = mid;
    }

    for (int i = low; i < idxVec.size() && idxVec[i].key == key; ++i)
        if (idxVec[i].entityId == entityId) return i;
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

template <typename K>
int findFirstIndexAtOrAfter(MyVector<SecondaryIndex<K>>& idxVec, K target) {
    int low = 0;
    int high = idxVec.size();
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (idxVec[mid].key < target) low = mid + 1;
        else high = mid;
    }
    return low < idxVec.size() ? low : -1;
}


class Database {
private:
    std::string dataDirectory = "data";
    inline static const std::string dataDirectoryCache = "cache";

    static std::string normalizeDocumentNumber(std::string value) {
        const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
            return std::isspace(ch) != 0;
        });
        const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
            return std::isspace(ch) != 0;
        }).base();
        if (first >= last) return {};
        value = std::string(first, last);
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
            return static_cast<char>(std::toupper(ch));
        });
        return value;
    }

public:
    struct TeamStanding {
        long long teamId;
        std::string teamName;
        int played = 0;
        int points = 0;
        int wins = 0;
        int draws = 0;
        int losses = 0;
        int goalsFor = 0;
        int goalsAgainst = 0;
    };

    MyVector<cauThu> dbCauThu;
    MyVector<doiBong> dbDoiBong;
    MyVector<tranDau> dbTranDau;
    MyVector<SecondaryIndex<int>> indexCauThuMaDoiBong;
    MyVector<SecondaryIndex<long long>> indexTranDauNgay;

    Database() {
        std::error_code ec;
        std::filesystem::create_directories(dataDirectoryCache, ec);
        std::ofstream touch(std::filesystem::path(dataDirectoryCache) / "data_log.txt", std::ios::app);
    }

    void setDataDirectory(const std::string& directory) {
        if (!directory.empty()) dataDirectory = directory;
    }
    std::string getDataDirectory() const { return dataDirectory; }

    bool loadFromFiles(std::string& error, bool* foundDataFiles = nullptr,
                       bool requireDataFiles = false) {
        error.clear();
        if (foundDataFiles) *foundDataFiles = false;
        std::error_code directoryError;
        std::filesystem::create_directories(dataDirectory, directoryError);
        if (directoryError) {
            error = "Không thể tạo thư mục dữ liệu " + dataDirectory + ": " + directoryError.message();
            return false;
        }
        const auto dataPath = [&](const char* fileName) {
            return (std::filesystem::path(dataDirectory) / fileName).string();
        };
        std::vector<cauThu> players;
        std::vector<doiBong> teams;
        std::vector<tranDau> matches;
        std::set<long long> playerIds, teamIds, matchIds;
        int openedDataFiles = 0;

        auto openInput = [&](const std::string& path, std::ifstream& input) {
            input.open(path);
            if (input.is_open()) {
                ++openedDataFiles;
                if (foundDataFiles) *foundDataFiles = true;
                return true;
            }
            std::error_code ec;
            if (std::filesystem::exists(path, ec) || ec) {
                error = "Không thể mở tệp " + path;
                return false;
            }
            return true;
        };
        auto hasOnlyWhitespace = [](std::istringstream& line) {
            line >> std::ws;
            return line.eof();
        };
        auto isCommentOrBlank = [](const std::string& value) {
            const auto first = value.find_first_not_of(" \t\r\n");
            return first == std::string::npos || value[first] == '#';
        };
        auto isSchemaHeader = [](const std::string& value, const char* header) {
            return value.compare(0, std::char_traits<char>::length(header), header) == 0;
        };

        std::ifstream playerFile;
        if (!openInput(dataPath("data_cauthu.txt"), playerFile)) return false;
        std::string text;
        int lineNumber = 0;
        while (std::getline(playerFile, text)) {
            ++lineNumber;
            if (isCommentOrBlank(text) ||
                isSchemaHeader(text, "id name position age ")) continue;
            std::istringstream line(text);
            long long id;
            long long birthDate = 0;
            int age, shirt, goals, teamId;
            int appearances = 0, phone = 0, height = 0, weight = 0;
            std::string name, position, nationality, imagePath, documentNumber;
            if (!(line >> id >> std::quoted(name) >> std::quoted(position) >> age >> shirt >> goals
                       >> std::quoted(nationality) >> teamId)) {
                error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                return false;
            }
            line >> std::ws;
            if (!line.eof()) {
                if (line.peek() == '"') {
                    if (!(line >> std::quoted(imagePath))) {
                        error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                        return false;
                    }
                } else if (line.peek() == 'V') {
                    std::string marker;
                    line >> marker;
                    if (marker == "V3") {
                        if (!(line >> std::quoted(documentNumber) >> appearances >> phone >> birthDate
                                  >> height >> weight >> std::quoted(imagePath))) {
                            error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                            return false;
                        }
                    } else if (marker != "V2" ||
                               !(line >> appearances >> phone >> birthDate >> height >> weight >> std::quoted(imagePath))) {
                        error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                        return false;
                    }
                } else if (line.peek() >= '0' && line.peek() <= '9') {
                    if (!(line >> appearances >> phone >> birthDate >> std::quoted(imagePath))) {
                        error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                        return false;
                    }
                } else {
                    error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                    return false;
                }
            }
            if (!hasOnlyWhitespace(line) || id <= 0 || !playerIds.insert(id).second) {
                error = "Dữ liệu cầu thủ không hợp lệ tại data_cauthu.txt, dòng " + std::to_string(lineNumber);
                return false;
            }
            cauThu player(id, name.c_str(), position.c_str(), age, shirt, teamId);
            player.setSoBanThang(goals);
            player.setMaQuocGia(nationality.c_str());
            player.setSoGiayTo(documentNumber);
            player.setImagePath(imagePath);
            player.soTranChoi = appearances;
            player.sdt = phone;
            player.ngaySinh = birthDate;
            player.chieuCao = height;
            player.canNang = weight;
            players.push_back(player);
        }
        if (playerFile.bad()) {
            error = "Lỗi khi đọc data_cauthu.txt";
            return false;
        }

        std::ifstream teamFile;
        if (!openInput(dataPath("data_doibong.txt"), teamFile)) return false;
        lineNumber = 0;
        while (std::getline(teamFile, text)) {
            ++lineNumber;
            if (isCommentOrBlank(text) ||
                isSchemaHeader(text, "id name coach location ")) continue;
            std::istringstream line(text);
            long long id, foundationDate = 0;
            int points, playerCount = 0, wins = 0, draws = 0, losses = 0;
            int goalsFor = 0, goalsAgainst = 0, currentRosterSize = 0;
            std::string name, coach, location, nationality, imagePath;
            if (!(line >> id >> std::quoted(name) >> std::quoted(coach) >> std::quoted(location)
                       >> points)) {
                error = "Dữ liệu đội bóng không hợp lệ tại data_doibong.txt, dòng " + std::to_string(lineNumber);
                return false;
            }
            line >> std::ws;
            if (!line.eof()) {
                if (line.peek() == '"') {
                    if (!(line >> std::quoted(imagePath))) {
                        error = "Dữ liệu đội bóng không hợp lệ tại data_doibong.txt, dòng " + std::to_string(lineNumber);
                        return false;
                    }
                } else if (line.peek() == 'V') {
                    std::string marker;
                    line >> marker;
                    if (marker != "V2" ||
                        !(line >> playerCount >> wins >> draws >> losses >> goalsFor >> goalsAgainst
                               >> std::quoted(nationality) >> currentRosterSize >> foundationDate
                               >> std::quoted(imagePath))) {
                        error = "Dữ liệu đội bóng không hợp lệ tại data_doibong.txt, dòng " + std::to_string(lineNumber);
                        return false;
                    }
                } else if (line.peek() >= '0' && line.peek() <= '9') {
                    if (!(line >> playerCount >> wins >> draws >> losses >> goalsFor >> goalsAgainst
                               >> std::quoted(nationality) >> currentRosterSize >> foundationDate
                               >> std::quoted(imagePath))) {
                        error = "Dữ liệu đội bóng không hợp lệ tại data_doibong.txt, dòng " + std::to_string(lineNumber);
                        return false;
                    }
                } else {
                    error = "Dữ liệu đội bóng không hợp lệ tại data_doibong.txt, dòng " + std::to_string(lineNumber);
                    return false;
                }
            }
            if (!hasOnlyWhitespace(line) || id <= 0 || !teamIds.insert(id).second) {
                error = "Dữ liệu đội bóng không hợp lệ tại data_doibong.txt, dòng " + std::to_string(lineNumber);
                return false;
            }
            doiBong team(id, name.c_str(), coach.c_str(), location.c_str());
            team.setDiemXepHang(points);
            team.setImagePath(imagePath);
            team.soCauThu = playerCount;
            team.soTranThang = wins;
            team.soTranHoa = draws;
            team.soTranThua = losses;
            team.soBanThang = goalsFor;
            team.soBanThua = goalsAgainst;
            std::strncpy(team.maQuocGia, nationality.c_str(), sizeof(team.maQuocGia) - 1);
            team.maQuocGia[sizeof(team.maQuocGia) - 1] = '\0';
            team.soCauThuHienTai = currentRosterSize;
            team.ngayThanhLap = foundationDate;
            teams.push_back(team);
        }
        if (teamFile.bad()) {
            error = "Lỗi khi đọc data_doibong.txt";
            return false;
        }

        std::ifstream matchFile;
        if (!openInput(dataPath("data_trandau.txt"), matchFile)) return false;
        lineNumber = 0;
        while (std::getline(matchFile, text)) {
            ++lineNumber;
            if (isCommentOrBlank(text) ||
                isSchemaHeader(text, "id homeTeamId awayTeamId ")) continue;
            std::istringstream line(text);
            long long id, date;
            int team1, team2, goals1, goals2, cards1, cards2;
            std::string stadium;
            if (!(line >> id >> team1 >> team2 >> goals1 >> goals2 >> date >> cards1 >> cards2
                       >> std::quoted(stadium)) ||
                !hasOnlyWhitespace(line) || id <= 0 || !matchIds.insert(id).second) {
                error = "Dữ liệu trận đấu không hợp lệ tại data_trandau.txt, dòng " + std::to_string(lineNumber);
                return false;
            }
            matches.emplace_back(id, team1, team2, goals1, goals2, date, cards1, cards2, stadium);
        }
        if (matchFile.bad()) {
            error = "Lỗi khi đọc data_trandau.txt";
            return false;
        }
        if (requireDataFiles && openedDataFiles != 3) {
            error = "Không tìm thấy data_cauthu.txt, data_doibong.txt hoặc data_trandau.txt.";
            return false;
        }

        dbCauThu.clear();
        dbDoiBong.clear();
        dbTranDau.clear();
        indexCauThuMaDoiBong.clear();
        indexTranDauNgay.clear();
        for (const cauThu& player : players) {
            if (!insert_cauThu_single(player)) {
                error = "Không thể nạp dữ liệu cầu thủ do ID hoặc số CMND/hộ chiếu bị trùng.";
                return false;
            }
        }
        for (const doiBong& team : teams) {
            if (!insert_doiBong_single(team)) {
                error = "Không thể nạp dữ liệu đội bóng do ID bị trùng.";
                return false;
            }
        }
        for (const tranDau& match : matches) {
            if (!insert_tranDau_single(match)) {
                error = "Không thể nạp dữ liệu trận đấu do ID bị trùng.";
                return false;
            }
        }
        return true;
    }

    bool saveToFiles(std::string& error) const {
        error.clear();
        std::error_code directoryError;
        std::filesystem::create_directories(dataDirectory, directoryError);
        if (directoryError) {
            error = "Không thể tạo thư mục dữ liệu " + dataDirectory + ": " + directoryError.message();
            return false;
        }
        const std::filesystem::path dataPath(dataDirectory);
        std::ofstream playerFile(dataPath / "data_cauthu.txt", std::ios::trunc);
        if (!playerFile) {
            error = "Không thể ghi data_cauthu.txt";
            return false;
        }
        playerFile << "# id name position age shirt goals nationality teamId "
                      "documentNumber appearances phone birthDateUnix heightCm weightKg imagePath\n";
        for (int i = 0; i < dbCauThu.size(); ++i) {
            const cauThu& player = dbCauThu[i];
            playerFile << player.getId() << ' ' << std::quoted(player.getTen()) << ' '
                       << std::quoted(player.getViTri()) << ' ' << player.getTuoi() << ' '
                       << player.getSoAo() << ' ' << player.getSoBanThang() << ' '
                       << std::quoted(player.getMaQuocGia()) << ' ' << player.getMaDoiBong()
                       << " V3 " << std::quoted(player.getSoGiayTo()) << ' '
                       << player.soTranChoi << ' ' << player.sdt << ' ' << player.ngaySinh
                       << ' ' << player.chieuCao << ' ' << player.canNang
                       << ' ' << std::quoted(player.getImagePath()) << '\n';
        }
        playerFile.close();
        if (!playerFile) {
            error = "Lỗi khi ghi data_cauthu.txt";
            return false;
        }

        std::ofstream teamFile(dataPath / "data_doibong.txt", std::ios::trunc);
        if (!teamFile) {
            error = "Không thể ghi data_doibong.txt";
            return false;
        }
        teamFile << "# id name coach location savedRankingPoints playerCount "
                    "wins draws losses goalsFor goalsAgainst nationality rosterSize "
                    "foundationDateUnix imagePath\n";
        for (int i = 0; i < dbDoiBong.size(); ++i) {
            const doiBong& team = dbDoiBong[i];
            teamFile << team.getId() << ' ' << std::quoted(team.getTen()) << ' '
                     << std::quoted(team.getHLV()) << ' ' << std::quoted(team.getDiaPhuong())
                     << ' ' << team.getDiemXepHang() << " V2 " << team.soCauThu << ' '
                     << team.soTranThang << ' ' << team.soTranHoa << ' ' << team.soTranThua
                     << ' ' << team.soBanThang << ' ' << team.soBanThua << ' '
                     << std::quoted(std::string(team.maQuocGia)) << ' ' << team.soCauThuHienTai
                     << ' ' << team.ngayThanhLap << ' ' << std::quoted(team.getImagePath()) << '\n';
        }
        teamFile.close();
        if (!teamFile) {
            error = "Lỗi khi ghi data_doibong.txt";
            return false;
        }

        std::ofstream matchFile(dataPath / "data_trandau.txt", std::ios::trunc);
        if (!matchFile) {
            error = "Không thể ghi data_trandau.txt";
            return false;
        }
        matchFile << "# id homeTeamId awayTeamId homeGoals awayGoals "
                     "matchDateUnix homeYellowCards awayYellowCards stadium\n";
        for (int i = 0; i < dbTranDau.size(); ++i) {
            const tranDau& match = dbTranDau[i];
            matchFile << match.getId() << ' ' << match.getMaDoiBong1() << ' '
                      << match.getMaDoiBong2() << ' ' << match.getSoBanThangDoi1() << ' '
                      << match.getSoBanThangDoi2() << ' ' << match.getNgayThiDau() << ' '
                      << match.getSoTheVangDoi1() << ' ' << match.getSoTheVangDoi2()
                      << ' ' << std::quoted(match.getSanVanDong()) << '\n';
        }
        matchFile.close();
        if (!matchFile) {
            error = "Lỗi khi ghi data_trandau.txt";
            return false;
        }

        try {
            std::filesystem::path cacheLogPath = std::filesystem::path(dataDirectoryCache) / "data_log.txt";
            std::filesystem::path currentLogPath = dataPath / "data_log.txt";
            
            if (std::filesystem::exists(cacheLogPath)) {
                std::error_code delete_ec;
                std::filesystem::remove(currentLogPath, delete_ec);
                std::filesystem::copy_file(cacheLogPath, currentLogPath, std::filesystem::copy_options::overwrite_existing);
            } else {
                std::error_code ec;
                std::filesystem::create_directories(dataDirectoryCache, ec);
                
                std::ofstream createCacheLog(cacheLogPath, std::ios::trunc);
                std::ofstream createMainLog(currentLogPath, std::ios::trunc);
            }
        } 
        catch (const std::filesystem::filesystem_error& e) {
            error = "Lỗi sao chép dữ liệu nhật ký sang thư mục hiện tại: " + std::string(e.what());
            return false;
        }
        return true;
    }

        bool persistAndLog(const std::string& action, const std::string& entity, long long id, std::string& error) const {
        error.clear();
        std::error_code ec;
        std::filesystem::create_directories(dataDirectoryCache, ec);
        std::ofstream logFile(std::filesystem::path(dataDirectoryCache) / "data_log.txt", std::ios::app);
        if (!logFile) {
            error = "Không thể mở data_log.txt trong cache.";
            return false;
        }
        const std::time_t now = std::time(nullptr);
        const std::tm* localTime = std::localtime(&now);
        if (localTime) logFile << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        else logFile << now;
        logFile << ' ' << std::quoted(action) << ' ' << std::quoted(entity) << ' ' << id << '\n';
        if (!logFile) {
            error = "Xảy ra lỗi khi cập nhật cache nhật ký.";
            return false;
        }
        return true;
    }


    bool replaceWith(Database& source) {
        dbCauThu.clear();
        dbDoiBong.clear();
        dbTranDau.clear();
        indexCauThuMaDoiBong.clear();
        indexTranDauNgay.clear();
        for (int i = 0; i < source.getCauThuCount(); ++i)
            if (!insert_cauThu_single(*source.getCauThuAt(i))) return false;
        for (int i = 0; i < source.getDoiBongCount(); ++i)
            if (!insert_doiBong_single(*source.getDoiBongAt(i))) return false;
        for (int i = 0; i < source.getTranDauCount(); ++i)
            if (!insert_tranDau_single(*source.getTranDauAt(i))) return false;
        return true;
    }

    std::vector<TeamStanding> calculateStandingsAt(long long cutoffTimestamp) const {
        std::vector<TeamStanding> standings;
        standings.reserve(static_cast<size_t>(dbDoiBong.size()));
        for (int i = 0; i < dbDoiBong.size(); ++i)
            standings.push_back({dbDoiBong[i].getId(), dbDoiBong[i].getTen()});
        for (int i = 0; i < dbTranDau.size(); ++i) {
            const tranDau& match = dbTranDau[i];
            if (match.getNgayThiDau() > cutoffTimestamp) continue;
            auto home = std::find_if(standings.begin(), standings.end(), [&](const TeamStanding& row) {
                return row.teamId == match.getMaDoiBong1();
            });
            auto away = std::find_if(standings.begin(), standings.end(), [&](const TeamStanding& row) {
                return row.teamId == match.getMaDoiBong2();
            });
            if (home == standings.end() || away == standings.end()) continue;
            ++home->played;
            ++away->played;
            home->goalsFor += match.getSoBanThangDoi1();
            home->goalsAgainst += match.getSoBanThangDoi2();
            away->goalsFor += match.getSoBanThangDoi2();
            away->goalsAgainst += match.getSoBanThangDoi1();
            if (match.getSoBanThangDoi1() > match.getSoBanThangDoi2()) {
                home->points += match.getDiemDoiBong1();
                ++home->wins;
                ++away->losses;
            } else if (match.getSoBanThangDoi1() < match.getSoBanThangDoi2()) {
                away->points += match.getDiemDoiBong2();
                ++away->wins;
                ++home->losses;
            } else {
                home->points += match.getDiemDoiBong1();
                away->points += match.getDiemDoiBong2();
                ++home->draws;
                ++away->draws;
            }
        }
        std::stable_sort(standings.begin(), standings.end(), [](const TeamStanding& left, const TeamStanding& right) {
            if (left.points != right.points) return left.points > right.points;
            const int leftDifference = left.goalsFor - left.goalsAgainst;
            const int rightDifference = right.goalsFor - right.goalsAgainst;
            if (leftDifference != rightDifference) return leftDifference > rightDifference;
            if (left.goalsFor != right.goalsFor) return left.goalsFor > right.goalsFor;
            return left.teamName < right.teamName;
        });
        return standings;
    }

    bool hasCompleteDoubleRoundRobinAt(long long cutoffTimestamp) const {
        const int teamCount = dbDoiBong.size();
        if (teamCount < 2) return false;

        std::set<std::pair<long long, long long>> fixtures;
        std::set<long long> teamIds;
        for (int i = 0; i < dbDoiBong.size(); ++i)
            teamIds.insert(dbDoiBong[i].getId());
        int playedMatchCount = 0;
        for (int i = 0; i < dbTranDau.size(); ++i) {
            const tranDau& match = dbTranDau[i];
            if (match.getNgayThiDau() > cutoffTimestamp) continue;
            const long long homeId = match.getMaDoiBong1();
            const long long awayId = match.getMaDoiBong2();
            if (homeId == awayId || teamIds.find(homeId) == teamIds.end() ||
                teamIds.find(awayId) == teamIds.end() ||
                !fixtures.insert({homeId, awayId}).second) {
                return false;
            }
            ++playedMatchCount;
        }

        const long long expectedMatchCount = static_cast<long long>(teamCount) * (teamCount - 1);
        if (playedMatchCount != expectedMatchCount) return false;

        for (int i = 0; i < dbDoiBong.size(); ++i) {
            for (int j = 0; j < dbDoiBong.size(); ++j) {
                if (i == j) continue;
                const auto fixture = std::make_pair(dbDoiBong[i].getId(), dbDoiBong[j].getId());
                if (fixtures.find(fixture) == fixtures.end()) return false;
            }
        }
        return true;
    }

    bool canScheduleMatch(long long homeTeamId, long long awayTeamId, long long excludedMatchId = -1) const {
        bool hasHomeTeam = false;
        bool hasAwayTeam = false;
        for (int i = 0; i < dbDoiBong.size(); ++i) {
            hasHomeTeam = hasHomeTeam || dbDoiBong[i].getId() == homeTeamId;
            hasAwayTeam = hasAwayTeam || dbDoiBong[i].getId() == awayTeamId;
        }
        if (homeTeamId == awayTeamId || !hasHomeTeam || !hasAwayTeam) return false;
        for (int i = 0; i < dbTranDau.size(); ++i) {
            const tranDau& match = dbTranDau[i];
            if (match.getId() == excludedMatchId) continue;
            if (match.getMaDoiBong1() == homeTeamId && match.getMaDoiBong2() == awayTeamId)
                return false;
        }
        return true;
    }

    bool hasPlayerDocumentNumber(const std::string& documentNumber,
                                 long long excludedPlayerId = -1) const {
        const std::string normalized = normalizeDocumentNumber(documentNumber);
        if (normalized.empty()) return false;
        for (int i = 0; i < dbCauThu.size(); ++i) {
            const cauThu& player = dbCauThu[i];
            if (player.getId() != excludedPlayerId &&
                normalizeDocumentNumber(player.getSoGiayTo()) == normalized) {
                return true;
            }
        }
        return false;
    }

    bool insert_cauThu_single(const cauThu& ct) {
        cauThu value = ct;
        if (value.id <= 0) value.id = dbCauThu.getNextId();
        if (dbCauThu.find_by_id(value.id) ||
            hasPlayerDocumentNumber(value.getSoGiayTo())) return false;

        dbCauThu.insert_ordered(value);
        indexCauThuMaDoiBong.insert_ordered({ value.getMaDoiBong(), value.id });
        return true;
    }

    bool updateCauThu(long long id, const cauThu& thongTinMoi) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (!ct || hasPlayerDocumentNumber(thongTinMoi.getSoGiayTo(), id)) return false;

        const int doiBongCu = ct->getMaDoiBong();
        ct->update(thongTinMoi);

        if (doiBongCu != ct->getMaDoiBong()) {
            int pos = findIndexPosition(indexCauThuMaDoiBong, doiBongCu, id);
            if (pos != -1) indexCauThuMaDoiBong.delete_by_index(pos);
            indexCauThuMaDoiBong.insert_ordered({ ct->getMaDoiBong(), id });
        }
        return true;
    }

    bool delete_cauThu_by_id(long long id) {
        cauThu* ct = dbCauThu.find_by_id(id);
        if (!ct) return false;

        int pos = findIndexPosition(indexCauThuMaDoiBong, ct->getMaDoiBong(), id);
        if (pos != -1) indexCauThuMaDoiBong.delete_by_index(pos);
        return dbCauThu.delete_by_id(id);
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
        indexCauThuMaDoiBong.insert_ordered({ maDoiBongMoi, id });
    }

    bool insert_doiBong_single(const doiBong& db) {
        doiBong value = db;
        if (value.id <= 0) value.id = dbDoiBong.getNextId();
        if (dbDoiBong.find_by_id(value.id)) return false;

        dbDoiBong.insert_ordered(value);
        return true;
    }

    bool updateDoiBong(long long id, const doiBong& thongTinMoi) {
        doiBong* team = dbDoiBong.find_by_id(id);
        if (!team) return false;

        team->update(thongTinMoi);
        team->id = id;
        return true;
    }

    bool delete_doiBong_by_id(long long id) {
        if (!dbDoiBong.find_by_id(id)) return false;

        for (int i = 0; i < dbTranDau.size(); ++i) {
            if (dbTranDau[i].getMaDoiBong1() == id || dbTranDau[i].getMaDoiBong2() == id)
                return false;
        }

        for (int i = 0; i < dbCauThu.size(); ++i) {
            if (dbCauThu[i].getMaDoiBong() == id)
                updateCauThuMaDoiBong(dbCauThu[i].getId(), -1);
        }

        return dbDoiBong.delete_by_id(id);
    }

    bool insert_tranDau_single(const tranDau& td) {
        tranDau value = td;
        if (value.id <= 0) value.id = dbTranDau.getNextId();
        if (dbTranDau.find_by_id(value.id)) return false;

        dbTranDau.insert_ordered(value);
        indexTranDauNgay.insert_ordered({ value.getNgayThiDau(), value.id });
        return true;
    }

    bool updateTranDau(long long id, const tranDau& thongTinMoi) {
        tranDau* match = dbTranDau.find_by_id(id);
        if (!match) return false;

        const long long ngayCu = match->getNgayThiDau();
        match->update(thongTinMoi);
        match->id = id;
        if (ngayCu != match->getNgayThiDau()) {
            int pos = findIndexPosition(indexTranDauNgay, ngayCu, id);
            if (pos != -1) indexTranDauNgay.delete_by_index(pos);
            indexTranDauNgay.insert_ordered({ match->getNgayThiDau(), id });
        }
        return true;
    }

    bool delete_tranDau_by_id(long long id) {
        tranDau* match = dbTranDau.find_by_id(id);
        if (!match) return false;

        int pos = findIndexPosition(indexTranDauNgay, match->getNgayThiDau(), id);
        if (pos != -1) indexTranDauNgay.delete_by_index(pos);
        return dbTranDau.delete_by_id(id);
    }

    std::vector<cauThu*> searchPlayers(const plb2::PlayerSearchFilter& filter) {
        std::vector<cauThu*> result;
        const auto matchesFilter = [&](const cauThu& player) {
            if (filter.playerId >= 0 && player.id != filter.playerId) return false;
            if (!plb2::matchesText(QString::fromUtf8(player.ten), filter.name)) return false;
            if (!plb2::matchesText(QString::fromUtf8(player.viTri), filter.position)) return false;
            if (!plb2::matchesText(QString::fromStdString(player.soGiayTo), filter.documentNumber)) return false;
            if (filter.minAge >= 0 || filter.maxAge >= 0) {
                const int age = plb2::playerAgeAt(player);
                if (age < 0 || !plb2::matchesRange(age, filter.minAge, filter.maxAge)) return false;
            }
            if (!plb2::matchesRange(player.soAo, filter.minNumber, filter.maxNumber)) return false;
            if (!plb2::matchesRange(player.soBanThang, filter.minGoals, filter.maxGoals)) return false;
            if (filter.minBirthYear >= 0 || filter.maxBirthYear >= 0) {
                if (player.ngaySinh <= 0) return false;
                const QDate birthDate = QDateTime::fromSecsSinceEpoch(player.ngaySinh).date();
                if (!birthDate.isValid()) return false;
                if (!plb2::matchesRange(birthDate.year(), filter.minBirthYear, filter.maxBirthYear))
                    return false;
            }
            return true;
        };

        if (filter.teamId >= 0) {
            const int startIndex = findFirstIndex(indexCauThuMaDoiBong, filter.teamId);
            for (int i = startIndex; i >= 0 && i < indexCauThuMaDoiBong.size(); ++i) {
                const auto& indexRecord = indexCauThuMaDoiBong[i];
                if (indexRecord.key != filter.teamId) break;
                cauThu* player = dbCauThu.find_by_id(indexRecord.entityId);
                if (player && matchesFilter(*player)) result.push_back(player);
            }
            return result;
        }

        for (int i = 0; i < dbCauThu.size(); ++i) {
            cauThu& player = dbCauThu[i];
            if (matchesFilter(player)) result.push_back(&player);
        }
        return result;
    }

    std::vector<doiBong*> searchTeams(const plb2::TeamSearchFilter& filter) {
        std::vector<doiBong*> result;
        for (int i = 0; i < dbDoiBong.size(); ++i) {
            doiBong& team = dbDoiBong[i];
            if (filter.teamRecordId >= 0 && team.id != filter.teamRecordId) continue;
            if (!plb2::matchesText(QString::fromUtf8(team.ten), filter.name)) continue;
            if (!plb2::matchesText(QString::fromUtf8(team.huanLuyenVien), filter.coach)) continue;
            if (!plb2::matchesText(QString::fromUtf8(team.diaPhuong), filter.location)) continue;
            if (!plb2::matchesRange(team.diemXepHang, filter.minPoints, filter.maxPoints)) continue;
            result.push_back(&team);
        }
        return result;
    }

    std::vector<tranDau*> searchMatches(const plb2::MatchSearchFilter& filter) {
        std::vector<tranDau*> result;
        const auto matchesFilter = [&](const tranDau& match) {
            if (filter.matchId >= 0 && match.id != filter.matchId) return false;
            if (filter.teamId >= 0 &&
                match.maDoiBong1 != filter.teamId &&
                match.maDoiBong2 != filter.teamId) return false;
            if (filter.fromDate >= 0 && match.ngayThiDau < filter.fromDate) return false;
            if (filter.toDate >= 0 && match.ngayThiDau > filter.toDate) return false;
            const int goalDifference = std::abs(match.soBanThangDoi1 - match.soBanThangDoi2);
            if (!plb2::matchesRange(goalDifference, filter.minGoalDiff, filter.maxGoalDiff)) return false;
            const int totalGoals = match.soBanThangDoi1 + match.soBanThangDoi2;
            return plb2::matchesRange(totalGoals, filter.minTotalGoals, filter.maxTotalGoals);
        };

        if (filter.fromDate >= 0) {
            const int startIndex = findFirstIndexAtOrAfter(indexTranDauNgay, filter.fromDate);
            for (int i = startIndex; i >= 0 && i < indexTranDauNgay.size(); ++i) {
                const auto& indexRecord = indexTranDauNgay[i];
                if (filter.toDate >= 0 && indexRecord.key > filter.toDate) break;
                tranDau* match = dbTranDau.find_by_id(indexRecord.entityId);
                if (match && matchesFilter(*match)) result.push_back(match);
            }
            return result;
        }

        for (int i = 0; i < dbTranDau.size(); ++i) {
            tranDau& match = dbTranDau[i];
            if (matchesFilter(match)) result.push_back(&match);
        }
        return result;
    }

    cauThu* getCauThuById(long long id) { return dbCauThu.find_by_id(id); }
    doiBong* getDoiBongById(long long id) { return dbDoiBong.find_by_id(id); }
    tranDau* getTranDauById(long long id) { return dbTranDau.find_by_id(id); }

    cauThu* getCauThuAt(int index) {
        return index >= 0 && index < dbCauThu.size() ? &dbCauThu[index] : nullptr;
    }
    const cauThu* getCauThuAt(int index) const {
        return index >= 0 && index < dbCauThu.size() ? &dbCauThu[index] : nullptr;
    }
    doiBong* getDoiBongAt(int index) {
        return index >= 0 && index < dbDoiBong.size() ? &dbDoiBong[index] : nullptr;
    }
    const doiBong* getDoiBongAt(int index) const {
        return index >= 0 && index < dbDoiBong.size() ? &dbDoiBong[index] : nullptr;
    }
    tranDau* getTranDauAt(int index) {
        return index >= 0 && index < dbTranDau.size() ? &dbTranDau[index] : nullptr;
    }
    const tranDau* getTranDauAt(int index) const {
        return index >= 0 && index < dbTranDau.size() ? &dbTranDau[index] : nullptr;
    }

    long long getNextCauThuId() { return dbCauThu.getNextId(); }
    long long getNextDoiBongId() { return dbDoiBong.getNextId(); }
    long long getNextTranDauId() { return dbTranDau.getNextId(); }

    int getCauThuCount() const { return dbCauThu.size(); }
    int getDoiBongCount() const { return dbDoiBong.size(); }
    int getTranDauCount() const { return dbTranDau.size(); }
};


#endif