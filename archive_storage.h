#pragma once
#ifndef ARCHIVE_STORAGE_H
#define ARCHIVE_STORAGE_H

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <QSaveFile>
#include <limits>
#include <string>
#include "database.h"

class ArchiveStorage {
    static QString stringValue(const QJsonObject& object, const QString& key, bool& valid) {
        const QJsonValue value = object.value(key);
        if (!value.isString()) valid = false;
        return value.toString();
    }

    static long long integerValue(const QJsonObject& object, const QString& key, bool& valid) {
        bool ok = false;
        const long long result = object.value(key).toString().toLongLong(&ok);
        if (!ok) valid = false;
        return result;
    }

    static int intValue(const QJsonObject& object, const QString& key, bool& valid) {
        bool ok = false;
        const int result = object.value(key).toInt(std::numeric_limits<int>::min());
        ok = result != std::numeric_limits<int>::min();
        if (!ok) valid = false;
        return result;
    }

    static QJsonObject playerObject(const cauThu& player) {
        QJsonObject object;
        object["id"] = QString::number(player.getId());
        object["name"] = QString::fromStdString(player.getTen());
        object["position"] = QString::fromStdString(player.getViTri());
        object["age"] = player.getTuoi();
        object["shirt"] = player.getSoAo();
        object["goals"] = player.getSoBanThang();
        object["nationality"] = QString::fromStdString(player.getMaQuocGia());
        object["documentNumber"] = QString::fromStdString(player.getSoGiayTo());
        object["teamId"] = player.getMaDoiBong();
        object["appearances"] = player.getSoTranChoi();
        object["phone"] = player.getSdt();
        object["birthDate"] = QString::number(player.getNgaySinh());
        object["height"] = player.getChieuCao();
        object["weight"] = player.getCanNang();
        object["imagePath"] = QString::fromStdString(player.getImagePath());
        return object;
    }

    static QJsonObject teamObject(const doiBong& team) {
        QJsonObject object;
        object["id"] = QString::number(team.getId());
        object["name"] = QString::fromStdString(team.getTen());
        object["coach"] = QString::fromStdString(team.getHLV());
        object["location"] = QString::fromStdString(team.getDiaPhuong());
        object["points"] = team.getDiemXepHang();
        object["playerCount"] = team.getSoCauThu();
        object["wins"] = team.getSoTranThang();
        object["draws"] = team.getSoTranHoa();
        object["losses"] = team.getSoTranThua();
        object["goalsFor"] = team.getSoBanThang();
        object["goalsAgainst"] = team.getSoBanThua();
        object["nationality"] = QString::fromStdString(team.getMaQuocGia());
        object["rosterSize"] = team.getSoCauThuHienTai();
        object["foundationDate"] = QString::number(team.getNgayThanhLap());
        object["imagePath"] = QString::fromStdString(team.getImagePath());
        return object;
    }

    static QJsonObject matchObject(const tranDau& match) {
        QJsonObject object;
        object["id"] = QString::number(match.getId());
        object["team1"] = match.getMaDoiBong1();
        object["team2"] = match.getMaDoiBong2();
        object["goals1"] = match.getSoBanThangDoi1();
        object["goals2"] = match.getSoBanThangDoi2();
        object["date"] = QString::number(match.getNgayThiDau());
        object["cards1"] = match.getSoTheVangDoi1();
        object["cards2"] = match.getSoTheVangDoi2();
        object["stadium"] = QString::fromStdString(match.getSanVanDong());
        return object;
    }

    static QJsonObject toJson(const Database& database) {
        QJsonArray players, teams, matches;
        for (int i = 0; i < database.getCauThuCount(); ++i)
            players.append(playerObject(*database.getCauThuAt(i)));
        for (int i = 0; i < database.getDoiBongCount(); ++i)
            teams.append(teamObject(*database.getDoiBongAt(i)));
        for (int i = 0; i < database.getTranDauCount(); ++i)
            matches.append(matchObject(*database.getTranDauAt(i)));
        QJsonObject root;
        root["format"] = "PLB2_ARCHIVE";
        root["version"] = 1;
        root["players"] = players;
        root["teams"] = teams;
        root["matches"] = matches;
        return root;
    }

    static bool appendPlayers(Database& database, const QJsonArray& players, std::string& error) {
        for (const QJsonValue& value : players) {
            if (!value.isObject()) {
                error = "Tệp lưu trữ chứa bản ghi cầu thủ không hợp lệ.";
                return false;
            }
            const QJsonObject object = value.toObject();
            bool valid = true;
            const long long id = integerValue(object, "id", valid);
            const QString name = stringValue(object, "name", valid);
            const QString position = stringValue(object, "position", valid);
            const int age = intValue(object, "age", valid);
            const int shirt = intValue(object, "shirt", valid);
            const int goals = intValue(object, "goals", valid);
            const QString nationality = stringValue(object, "nationality", valid);
            const int teamId = intValue(object, "teamId", valid);
            const int appearances = intValue(object, "appearances", valid);
            const int phone = intValue(object, "phone", valid);
            const long long birthDate = integerValue(object, "birthDate", valid);
            const int height = intValue(object, "height", valid);
            const int weight = intValue(object, "weight", valid);
            const QString imagePath = stringValue(object, "imagePath", valid);
            QString documentNumber;
            if (object.contains("documentNumber")) {
                documentNumber = stringValue(object, "documentNumber", valid);
            }
            if (!valid || id <= 0) {
                error = "Tệp lưu trữ chứa thông tin cầu thủ không đầy đủ.";
                return false;
            }
            cauThu player(id, name.toStdString().c_str(), position.toStdString().c_str(), age, shirt, teamId);
            player.setSoBanThang(goals);
            player.setMaQuocGia(nationality.toStdString().c_str());
            player.setSoGiayTo(documentNumber.toStdString());
            player.setSoTranChoi(appearances);
            player.setSdt(phone);
            player.setNgaySinh(birthDate);
            player.setChieuCao(height);
            player.setCanNang(weight);
            player.setImagePath(imagePath.toStdString());
            if (!database.insert_cauThu_single(player)) {
                error = "ID hoặc số CMND/hộ chiếu cầu thủ bị trùng trong tệp lưu trữ.";
                return false;
            }
        }
        return true;
    }

    static bool appendTeams(Database& database, const QJsonArray& teams, std::string& error) {
        for (const QJsonValue& value : teams) {
            if (!value.isObject()) {
                error = "Tệp lưu trữ chứa bản ghi đội bóng không hợp lệ.";
                return false;
            }
            const QJsonObject object = value.toObject();
            bool valid = true;
            const long long id = integerValue(object, "id", valid);
            const QString name = stringValue(object, "name", valid);
            const QString coach = stringValue(object, "coach", valid);
            const QString location = stringValue(object, "location", valid);
            const int points = intValue(object, "points", valid);
            const int playerCount = intValue(object, "playerCount", valid);
            const int wins = intValue(object, "wins", valid);
            const int draws = intValue(object, "draws", valid);
            const int losses = intValue(object, "losses", valid);
            const int goalsFor = intValue(object, "goalsFor", valid);
            const int goalsAgainst = intValue(object, "goalsAgainst", valid);
            const QString nationality = stringValue(object, "nationality", valid);
            const int rosterSize = intValue(object, "rosterSize", valid);
            const long long foundationDate = integerValue(object, "foundationDate", valid);
            const QString imagePath = stringValue(object, "imagePath", valid);
            if (!valid || id <= 0) {
                error = "Tệp lưu trữ chứa thông tin đội bóng không đầy đủ.";
                return false;
            }
            doiBong team(id, name.toStdString().c_str(), coach.toStdString().c_str(), location.toStdString().c_str());
            team.setDiemXepHang(points);
            team.setSoCauThu(playerCount);
            team.setSoTranThang(wins);
            team.setSoTranHoa(draws);
            team.setSoTranThua(losses);
            team.setSoBanThang(goalsFor);
            team.setSoBanThua(goalsAgainst);
            team.setMaQuocGia(nationality.toStdString().c_str());
            team.setNgayThanhLap(foundationDate);
            team.setSoCauThuHienTai(rosterSize);
            team.setImagePath(imagePath.toStdString());
            if (!database.insert_doiBong_single(team)) {
                error = "ID đội bóng bị trùng trong tệp lưu trữ.";
                return false;
            }
        }
        return true;
    }

    static bool appendMatches(Database& database, const QJsonArray& matches, std::string& error) {
        for (const QJsonValue& value : matches) {
            if (!value.isObject()) {
                error = "Tệp lưu trữ chứa bản ghi trận đấu không hợp lệ.";
                return false;
            }
            const QJsonObject object = value.toObject();
            bool valid = true;
            const long long id = integerValue(object, "id", valid);
            const int team1 = intValue(object, "team1", valid);
            const int team2 = intValue(object, "team2", valid);
            const int goals1 = intValue(object, "goals1", valid);
            const int goals2 = intValue(object, "goals2", valid);
            const long long date = integerValue(object, "date", valid);
            const int cards1 = intValue(object, "cards1", valid);
            const int cards2 = intValue(object, "cards2", valid);
            const QString stadium = stringValue(object, "stadium", valid);
            if (!valid || id <= 0 || team1 == team2) {
                error = "Tệp lưu trữ chứa thông tin trận đấu không đầy đủ hoặc không hợp lệ.";
                return false;
            }
            tranDau match(id, team1, team2, goals1, goals2, date, cards1, cards2, stadium.toStdString());
            if (!database.insert_tranDau_single(match)) {
                error = "ID trận đấu bị trùng trong tệp lưu trữ.";
                return false;
            }
        }
        return true;
    }

public:
    static bool save(const Database& database, const QString& path, std::string& error) {
        error.clear();
        const QJsonObject root = toJson(database);
        const QByteArray bytes = QJsonDocument(root).toJson(QJsonDocument::Indented);

        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
            error = "Không thể ghi tệp lưu trữ: " + file.errorString().toStdString();
            return false;
        }
        return true;
    }

    static bool load(Database& database, const QString& path, std::string& error) {
        error.clear();
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            error = "Không thể mở tệp lưu trữ: " + file.errorString().toStdString();
            return false;
        }
        const QByteArray bytes = file.readAll();
        if (file.error() != QFile::NoError) {
            error = "Lỗi khi đọc tệp lưu trữ: " + file.errorString().toStdString();
            return false;
        }
        QJsonArray players, teams, matches;
        QJsonParseError jsonError;
        const QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            if (!jsonDocument.isObject()) {
                error = "Tệp JSON phải có đối tượng gốc chứa players, teams và matches.";
                return false;
            }
            const QJsonObject root = jsonDocument.object();
            if (root.value("format").toString() != "PLB2_ARCHIVE" ||
                root.value("version").toInt() != 1 ||
                !root.value("players").isArray() ||
                !root.value("teams").isArray() ||
                !root.value("matches").isArray()) {
                error = "Tệp JSON không đúng cấu trúc PLB2_ARCHIVE phiên bản 1.";
                return false;
            }
            players = root.value("players").toArray();
            teams = root.value("teams").toArray();
            matches = root.value("matches").toArray();
        } else {
            const QList<QByteArray> lines = bytes.split('\n');
            if (lines.isEmpty()) {
                error = "Tệp rỗng.";
                return false;
            }
            for (int i = 1; i < lines.size(); ++i) {
                const QByteArray row = lines[i].trimmed();
                if (row.isEmpty()) continue;
                const int separator = row.indexOf('\t');
                if (separator <= 0) {
                    error = "Bản ghi văn bản không hợp lệ ở dòng " + std::to_string(i + 1);
                    return false;
                }
                QJsonParseError recordError;
                const QJsonDocument record = QJsonDocument::fromJson(row.mid(separator + 1), &recordError);
                if (recordError.error != QJsonParseError::NoError || !record.isObject()) {
                    error = "Bản ghi văn bản không hợp lệ ở dòng " + std::to_string(i + 1);
                    return false;
                }
                const QByteArray type = row.left(separator);
                if (type == "PLAYER") players.append(record.object());
                else if (type == "TEAM") teams.append(record.object());
                else if (type == "MATCH") matches.append(record.object());
                else {
                    error = "Loại đối tượng không được hỗ trợ ở dòng " + std::to_string(i + 1);
                    return false;
                }
            }
        }

        Database parsed;
        if (!appendPlayers(parsed, players, error) ||
            !appendTeams(parsed, teams, error) ||
            !appendMatches(parsed, matches, error)) {
            return false;
        }
        if (!database.replaceWith(parsed)) {
            error = "Không thể thay thế dữ liệu hiện tại bằng tệp lưu trữ.";
            return false;
        }
        return true;
    }
};

#endif
