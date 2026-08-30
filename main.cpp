#include <iostream>
#include <cstring>
#include <vector>
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QDateTime>
#include <QHeaderView>
#include <QIcon>

#include "vector.h"
#include "csdl.h"
#include "database.h"
#include "search_features.h"
#include "match_widget.h"
#include "tournament_widget.h"

using namespace std;

// ==================== HELPER PRINT ====================
void printSeparator(const std::string& title) {
    std::cout << "\n================================ " << title << " ================================\n" << std::endl;
}

// ==================== GUI - PLAYER WIDGET ====================
class PlayerWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    QTableWidget *playerTable;
    QLineEdit *nameInput, *positionInput;
    QSpinBox *ageInput, *numberInput, *goalsInput;
    QComboBox *teamCombo;

    QLineEdit *searchNameInput, *searchPositionInput;
    QComboBox *searchTeamCombo;
    QSpinBox *searchMinAge, *searchMaxAge, *searchMinNumber, *searchMaxNumber;
    QSpinBox *searchMinGoals, *searchMaxGoals;
    QPushButton *addBtn, *editBtn, *deleteBtn, *refreshBtn;
    QPushButton *searchBtn, *clearSearchBtn;
    QLabel *statusLabel;
    long long selectedPlayerId = -1;

    void renderPlayerRows(const std::vector<cauThu*>& players) {
        playerTable->setRowCount(0);
        for (cauThu* ct : players) {
            if (!ct) continue;
            int row = playerTable->rowCount();
            playerTable->insertRow(row);
            playerTable->setItem(row, 0, new QTableWidgetItem(QString::number(ct->getId())));
            playerTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(ct->getTen())));
            playerTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(ct->getViTri())));
            playerTable->setItem(row, 3, new QTableWidgetItem(QString::number(ct->getTuoi())));
            playerTable->setItem(row, 4, new QTableWidgetItem(QString::number(ct->getSoAo())));
            playerTable->setItem(row, 5, new QTableWidgetItem(QString::number(ct->getSoBanThang())));
            doiBong *team = db->getDoiBongById(ct->getMaDoiBong());
            if (team) {
                playerTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(team->getTen())));
            } else {
                playerTable->setItem(row, 6, new QTableWidgetItem("N/A"));
            }
            playerTable->setItem(row, 7, new QTableWidgetItem("🇻🇳"));
        }
    }

public slots:
    void onAddPlayer() {
        if (nameInput->text().isEmpty()) {
            QMessageBox::warning(this, "❌ Lỗi", "Nhập tên cầu thủ!");
            return;
        }
        long long newId = db->getNextCauThuId();
        cauThu ct(newId,
                  nameInput->text().toStdString().c_str(),
                  positionInput->text().toStdString().c_str(),
                  ageInput->value(),
                  numberInput->value(),
                  teamCombo->currentData().toInt());
        ct.setSoBanThang(goalsInput->value());
        db->insert_cauThu_single(ct);
        refreshTable();
        statusLabel->setText("✓ Thêm cầu thủ thành công!");
        clearInputs();
    }

    void onEditPlayer() {
        if (selectedPlayerId == -1) {
            QMessageBox::warning(this, "❌ Lỗi", "Chọn cầu thủ để sửa!");
            return;
        }
        cauThu *ct = db->getCauThuById(selectedPlayerId);
        if (ct) {
            ct->setTen(nameInput->text().toStdString().c_str());
            ct->setViTri(positionInput->text().toStdString().c_str());
            ct->setTuoi(ageInput->value());
            ct->setSoAo(numberInput->value());
            ct->setSoBanThang(goalsInput->value());
            ct->setMaDoiBong(teamCombo->currentData().toInt());
            db->updateCauThuTuoi(ct->getId(), ct->getTuoi());
            db->updateCauThuMaDoiBong(ct->getId(), ct->getMaDoiBong());
            db->updateCauThuBanThang(ct->getId(), ct->getSoBanThang());
            refreshTable();
            statusLabel->setText("✓ Cập nhật cầu thủ thành công!");
        }
    }

    void onDeletePlayer() {
        if (selectedPlayerId == -1) {
            QMessageBox::warning(this, "❌ Lỗi", "Chọn cầu thủ để xóa!");
            return;
        }
        if (QMessageBox::question(this, "⚠️ Xác nhận", 
            "Bạn chắc chắn muốn xóa?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            db->dbCauThu.delete_by_id(selectedPlayerId);
            refreshTable();
            statusLabel->setText("✓ Xóa thành công!");
            clearInputs();
        }
    }

    void onRefresh() {
        refreshTable();
        statusLabel->setText("✓ Đã làm mới!");
    }

    void onApplyPlayerSearch() {
        plb2::PlayerSearchFilter filter;
        filter.name = searchNameInput->text();
        filter.position = searchPositionInput->text();
        filter.teamId = searchTeamCombo->currentData().toInt();

        if (searchMinAge->value() > 0) filter.minAge = searchMinAge->value();
        if (searchMaxAge->value() > 0) filter.maxAge = searchMaxAge->value();

        if (searchMinNumber->value() > 0) filter.minNumber = searchMinNumber->value();
        if (searchMaxNumber->value() > 0) filter.maxNumber = searchMaxNumber->value();

        if (searchMinGoals->value() > 0) filter.minGoals = searchMinGoals->value();
        if (searchMaxGoals->value() > 0) filter.maxGoals = searchMaxGoals->value();

        auto players = plb2::filterPlayers(*db, filter);
        renderPlayerRows(players);
        statusLabel->setText(QString("✓ Tìm thấy %1 cầu thủ").arg(players.size()));
    }

    void onClearPlayerSearch() {
        searchNameInput->clear();
        searchPositionInput->clear();
        searchTeamCombo->setCurrentIndex(0);
        searchMinAge->setValue(0);
        searchMaxAge->setValue(0);
        searchMinNumber->setValue(0);
        searchMaxNumber->setValue(0);
        searchMinGoals->setValue(0);
        searchMaxGoals->setValue(0);
        refreshTable();
        statusLabel->setText("✓ Đã reset bộ lọc!");
    }

    void onTableClicked(int row, int column) {
        if (row < 0) return;
        selectedPlayerId = playerTable->item(row, 0)->text().toLongLong();
        cauThu *ct = db->getCauThuById(selectedPlayerId);
        if (ct) {
            nameInput->setText(QString::fromStdString(ct->getTen()));
            positionInput->setText(QString::fromStdString(ct->getViTri()));
            ageInput->setValue(ct->getTuoi());
            numberInput->setValue(ct->getSoAo());
            goalsInput->setValue(ct->getSoBanThang());
            for (int i = 0; i < teamCombo->count(); i++) {
                if (teamCombo->itemData(i).toInt() == ct->getMaDoiBong()) {
                    teamCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
    }

public:
    PlayerWidget(Database *db, QWidget *parent = nullptr) 
        : QWidget(parent), db(db), selectedPlayerId(-1) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        QGroupBox *formGroup = new QGroupBox("➕ Thêm / Sửa Cầu Thủ");
        formGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2196F3; padding-top: 10px; }");
        QGridLayout *formLayout = new QGridLayout(formGroup);

        formLayout->addWidget(new QLabel("Tên:"), 0, 0);
        nameInput = new QLineEdit();
        nameInput->setPlaceholderText("Nhập tên cầu thủ");
        formLayout->addWidget(nameInput, 0, 1);

        formLayout->addWidget(new QLabel("Vị Trí:"), 0, 2);
        positionInput = new QLineEdit();
        positionInput->setPlaceholderText("ST, CB, CM...");
        formLayout->addWidget(positionInput, 0, 3);

        formLayout->addWidget(new QLabel("Tuổi:"), 1, 0);
        ageInput = new QSpinBox();
        ageInput->setRange(15, 50);
        formLayout->addWidget(ageInput, 1, 1);

        formLayout->addWidget(new QLabel("Số Áo:"), 1, 2);
        numberInput = new QSpinBox();
        numberInput->setRange(1, 99);
        formLayout->addWidget(numberInput, 1, 3);

        formLayout->addWidget(new QLabel("Bàn Thắng:"), 2, 0);
        goalsInput = new QSpinBox();
        goalsInput->setRange(0, 500);
        formLayout->addWidget(goalsInput, 2, 1);

        formLayout->addWidget(new QLabel("Đội Bóng:"), 2, 2);
        teamCombo = new QComboBox();
        teamCombo->addItem("Tất cả", -1);
        for (int i = 1; i <= db->getDoiBongCount(); i++) {
            doiBong *team = db->getDoiBongById(i);
            if (team) {
                teamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
            }
        }
        formLayout->addWidget(teamCombo, 2, 3);

        QHBoxLayout *btnLayout = new QHBoxLayout();
        addBtn = new QPushButton("➕ Thêm");
        addBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }"
                             "QPushButton:hover { background-color: #45a049; }");
        editBtn = new QPushButton("✏️ Sửa");
        editBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }"
                              "QPushButton:hover { background-color: #0b7dda; }");
        deleteBtn = new QPushButton("🗑️ Xóa");
        deleteBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }"
                                "QPushButton:hover { background-color: #da190b; }");
        refreshBtn = new QPushButton("🔄 Làm Mới");
        refreshBtn->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }");

        btnLayout->addWidget(addBtn);
        btnLayout->addWidget(editBtn);
        btnLayout->addWidget(deleteBtn);
        btnLayout->addWidget(refreshBtn);
        formLayout->addLayout(btnLayout, 3, 0, 1, 4);

        mainLayout->addWidget(formGroup);

        QGroupBox *searchGroup = new QGroupBox("🔎 Tìm Kiếm Cầu Thủ");
        searchGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #009688; padding-top: 10px; }");
        QGridLayout *searchLayout = new QGridLayout(searchGroup);

        searchLayout->addWidget(new QLabel("Tên:"), 0, 0);
        searchNameInput = new QLineEdit();
        searchLayout->addWidget(searchNameInput, 0, 1);

        searchLayout->addWidget(new QLabel("Vị Trí:"), 0, 2);
        searchPositionInput = new QLineEdit();
        searchLayout->addWidget(searchPositionInput, 0, 3);

        searchLayout->addWidget(new QLabel("Đội:"), 1, 0);
        searchTeamCombo = new QComboBox();
        searchTeamCombo->addItem("Tất cả", -1);
        for (int i = 1; i <= db->getDoiBongCount(); i++) {
            doiBong *team = db->getDoiBongById(i);
            if (team) searchTeamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        searchLayout->addWidget(searchTeamCombo, 1, 1);

        searchLayout->addWidget(new QLabel("Tuổi từ:"), 1, 2);
        searchMinAge = new QSpinBox();
        searchMinAge->setRange(0, 100);
        searchLayout->addWidget(searchMinAge, 1, 3);

        searchLayout->addWidget(new QLabel("Tuổi đến:"), 2, 0);
        searchMaxAge = new QSpinBox();
        searchMaxAge->setRange(0, 100);
        searchLayout->addWidget(searchMaxAge, 2, 1);

        searchLayout->addWidget(new QLabel("Số áo từ:"), 2, 2);
        searchMinNumber = new QSpinBox();
        searchMinNumber->setRange(0, 99);
        searchLayout->addWidget(searchMinNumber, 2, 3);

        searchLayout->addWidget(new QLabel("Số áo đến:"), 3, 0);
        searchMaxNumber = new QSpinBox();
        searchMaxNumber->setRange(0, 99);
        searchLayout->addWidget(searchMaxNumber, 3, 1);

        searchLayout->addWidget(new QLabel("Bàn từ:"), 3, 2);
        searchMinGoals = new QSpinBox();
        searchMinGoals->setRange(0, 500);
        searchLayout->addWidget(searchMinGoals, 3, 3);

        searchLayout->addWidget(new QLabel("Bàn đến:"), 4, 0);
        searchMaxGoals = new QSpinBox();
        searchMaxGoals->setRange(0, 500);
        searchLayout->addWidget(searchMaxGoals, 4, 1);

        QHBoxLayout *searchBtnLayout = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Lọc");
        clearSearchBtn = new QPushButton("❌ Xóa lọc");
        searchBtnLayout->addWidget(searchBtn);
        searchBtnLayout->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchBtnLayout, 4, 2, 1, 2);

        mainLayout->addWidget(searchGroup);

        playerTable = new QTableWidget();
        playerTable->setColumnCount(8);
        playerTable->setHorizontalHeaderLabels({"ID", "Tên", "Vị Trí", "Tuổi", "Số Áo", "Bàn", "Đội", "QG"});
        playerTable->horizontalHeader()->setStretchLastSection(true);
        playerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        playerTable->setSelectionMode(QAbstractItemView::SingleSelection);
        playerTable->setStyleSheet(
            "QHeaderView::section { background-color: #2196F3; color: white; padding: 5px; font-weight: bold; }"
            "QTableWidget { gridline-color: #ccc; }"
            "QTableWidget::item:selected { background-color: #90CAF9; }"
        );
        playerTable->setAlternatingRowColors(true);
        mainLayout->addWidget(playerTable);

        statusLabel = new QLabel("✓ Sẵn sàng");
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; padding: 5px; }");
        mainLayout->addWidget(statusLabel);

        connect(addBtn, &QPushButton::clicked, this, &PlayerWidget::onAddPlayer);
        connect(editBtn, &QPushButton::clicked, this, &PlayerWidget::onEditPlayer);
        connect(deleteBtn, &QPushButton::clicked, this, &PlayerWidget::onDeletePlayer);
        connect(refreshBtn, &QPushButton::clicked, this, &PlayerWidget::onRefresh);
        connect(playerTable, &QTableWidget::cellClicked, this, &PlayerWidget::onTableClicked);
        connect(searchBtn, &QPushButton::clicked, this, &PlayerWidget::onApplyPlayerSearch);
        connect(clearSearchBtn, &QPushButton::clicked, this, &PlayerWidget::onClearPlayerSearch);

        refreshTable();
    }

    void refreshTable() {
        std::vector<cauThu*> allPlayers;
        for (int i = 1; i <= db->getCauThuCount(); ++i) {
            cauThu* ct = db->getCauThuById(i);
            if (ct) allPlayers.push_back(ct);
        }
        renderPlayerRows(allPlayers);
    }

    void clearInputs() {
        nameInput->clear();
        positionInput->clear();
        ageInput->setValue(20);
        numberInput->setValue(1);
        goalsInput->setValue(0);
        selectedPlayerId = -1;
    }
};

// ==================== GUI - TEAM WIDGET ====================
class TeamWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    QTableWidget *teamTable;
    QLineEdit *nameInput, *coachInput, *locationInput;
    QSpinBox *rankInput;
    QLineEdit *searchNameInput, *searchCoachInput, *searchLocationInput;
    QSpinBox *searchMinRank, *searchMaxRank;
    QPushButton *addBtn, *editBtn, *deleteBtn;
    QPushButton *searchBtn, *clearSearchBtn;
    QLabel *statusLabel;
    long long selectedTeamId = -1;

    void renderTeamRows(const std::vector<doiBong*>& teams) {
        teamTable->setRowCount(0);
        for (doiBong* team : teams) {
            if (!team) continue;
            int row = teamTable->rowCount();
            teamTable->insertRow(row);
            teamTable->setItem(row, 0, new QTableWidgetItem(QString::number(team->getId())));
            teamTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(team->getTen())));
            teamTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(team->getHLV())));
            teamTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(team->getDiaPhuong())));
            teamTable->setItem(row, 4, new QTableWidgetItem(QString::number(team->getDiemXepHang())));
        }
    }

public slots:
    void onAddTeam() {
        if (nameInput->text().isEmpty()) {
            QMessageBox::warning(this, "❌ Lỗi", "Nhập tên đội!");
            return;
        }
        long long newId = db->getNextDoiBongId();
        doiBong team(newId, nameInput->text().toStdString().c_str(),
                     coachInput->text().toStdString().c_str(),
                     locationInput->text().toStdString().c_str());
        team.setDiemXepHang(rankInput->value());
        db->insert_doiBong_single(team);
        refreshTable();
        statusLabel->setText("✓ Thêm đội thành công!");
        clearInputs();
    }

    void onEditTeam() {
        if (selectedTeamId == -1) {
            QMessageBox::warning(this, "❌ Lỗi", "Chọn đội để sửa!");
            return;
        }
        doiBong *team = db->getDoiBongById(selectedTeamId);
        if (team) {
            team->setTen(nameInput->text().toStdString().c_str());
            team->setHLV(coachInput->text().toStdString().c_str());
            team->setDiaPhuong(locationInput->text().toStdString().c_str());
            team->setDiemXepHang(rankInput->value());
            db->updateDoiBongDiemXepHang(team->getId(), team->getDiemXepHang());
            refreshTable();
            statusLabel->setText("✓ Cập nhật đội thành công!");
        }
    }

    void onDeleteTeam() {
        if (selectedTeamId == -1) {
            QMessageBox::warning(this, "❌ Lỗi", "Chọn đội để xóa!");
            return;
        }
        if (QMessageBox::question(this, "⚠️ Xác nhận", 
            "Xóa đội này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            db->dbDoiBong.delete_by_id(selectedTeamId);
            refreshTable();
            statusLabel->setText("✓ Xóa thành công!");
            clearInputs();
        }
    }

    void onApplyTeamSearch() {
        plb2::TeamSearchFilter filter;
        filter.name = searchNameInput->text();
        filter.coach = searchCoachInput->text();
        filter.location = searchLocationInput->text();

        if (searchMinRank->value() > 0) filter.minPoints = searchMinRank->value();
        if (searchMaxRank->value() > 0) filter.maxPoints = searchMaxRank->value();

        auto teams = plb2::filterTeams(*db, filter);
        renderTeamRows(teams);
        statusLabel->setText(QString("✓ Tìm thấy %1 đội").arg(teams.size()));
    }

    void onClearTeamSearch() {
        searchNameInput->clear();
        searchCoachInput->clear();
        searchLocationInput->clear();
        searchMinRank->setValue(0);
        searchMaxRank->setValue(0);
        refreshTable();
        statusLabel->setText("✓ Đã reset bộ lọc!");
    }

    void onTableClicked(int row, int column) {
        if (row < 0) return;
        selectedTeamId = teamTable->item(row, 0)->text().toLongLong();
        doiBong *team = db->getDoiBongById(selectedTeamId);
        if (team) {
            nameInput->setText(QString::fromStdString(team->getTen()));
            coachInput->setText(QString::fromStdString(team->getHLV()));
            locationInput->setText(QString::fromStdString(team->getDiaPhuong()));
            rankInput->setValue(team->getDiemXepHang());
        }
    }

public:
    TeamWidget(Database *db, QWidget *parent = nullptr) 
        : QWidget(parent), db(db), selectedTeamId(-1) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        QGroupBox *formGroup = new QGroupBox("➕ Thêm / Sửa Đội Bóng");
        formGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #FF9800; }");
        QGridLayout *formLayout = new QGridLayout(formGroup);

        formLayout->addWidget(new QLabel("Tên Đội:"), 0, 0);
        nameInput = new QLineEdit();
        formLayout->addWidget(nameInput, 0, 1);

        formLayout->addWidget(new QLabel("HLV:"), 0, 2);
        coachInput = new QLineEdit();
        formLayout->addWidget(coachInput, 0, 3);

        formLayout->addWidget(new QLabel("Địa Phương:"), 1, 0);
        locationInput = new QLineEdit();
        formLayout->addWidget(locationInput, 1, 1);

        formLayout->addWidget(new QLabel("Điểm XH:"), 1, 2);
        rankInput = new QSpinBox();
        rankInput->setRange(0, 5000);
        formLayout->addWidget(rankInput, 1, 3);

        QHBoxLayout *btnLayout = new QHBoxLayout();
        addBtn = new QPushButton("➕ Thêm");
        addBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; border-radius: 4px; }");
        editBtn = new QPushButton("✏️ Sửa");
        editBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; border-radius: 4px; }");
        deleteBtn = new QPushButton("🗑️ Xóa");
        deleteBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");

        btnLayout->addWidget(addBtn);
        btnLayout->addWidget(editBtn);
        btnLayout->addWidget(deleteBtn);
        formLayout->addLayout(btnLayout, 2, 0, 1, 4);

        mainLayout->addWidget(formGroup);

        QGroupBox *searchGroup = new QGroupBox("🔎 Tìm Kiếm Đội Bóng");
        searchGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #795548; padding-top: 10px; }");
        QGridLayout *searchLayout = new QGridLayout(searchGroup);

        searchLayout->addWidget(new QLabel("Tên đội:"), 0, 0);
        searchNameInput = new QLineEdit();
        searchLayout->addWidget(searchNameInput, 0, 1);

        searchLayout->addWidget(new QLabel("HLV:"), 0, 2);
        searchCoachInput = new QLineEdit();
        searchLayout->addWidget(searchCoachInput, 0, 3);

        searchLayout->addWidget(new QLabel("Địa phương:"), 1, 0);
        searchLocationInput = new QLineEdit();
        searchLayout->addWidget(searchLocationInput, 1, 1);

        searchLayout->addWidget(new QLabel("Điểm từ:"), 1, 2);
        searchMinRank = new QSpinBox();
        searchMinRank->setRange(0, 5000);
        searchLayout->addWidget(searchMinRank, 1, 3);

        searchLayout->addWidget(new QLabel("Điểm đến:"), 2, 0);
        searchMaxRank = new QSpinBox();
        searchMaxRank->setRange(0, 5000);
        searchLayout->addWidget(searchMaxRank, 2, 1);

        QHBoxLayout *searchBtnLayout = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Lọc");
        clearSearchBtn = new QPushButton("❌ Xóa lọc");
        searchBtnLayout->addWidget(searchBtn);
        searchBtnLayout->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchBtnLayout, 2, 2, 1, 2);

        mainLayout->addWidget(searchGroup);

        teamTable = new QTableWidget();
        teamTable->setColumnCount(5);
        teamTable->setHorizontalHeaderLabels({"ID", "Tên Đội", "HLV", "Địa Phương", "Điểm XH"});
        teamTable->horizontalHeader()->setStretchLastSection(true);
        teamTable->setStyleSheet(
            "QHeaderView::section { background-color: #FF9800; color: white; padding: 5px; font-weight: bold; }"
        );
        mainLayout->addWidget(teamTable);

        statusLabel = new QLabel("✓ Sẵn sàng");
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        mainLayout->addWidget(statusLabel);

        connect(addBtn, &QPushButton::clicked, this, &TeamWidget::onAddTeam);
        connect(editBtn, &QPushButton::clicked, this, &TeamWidget::onEditTeam);
        connect(deleteBtn, &QPushButton::clicked, this, &TeamWidget::onDeleteTeam);
        connect(teamTable, &QTableWidget::cellClicked, this, &TeamWidget::onTableClicked);
        connect(searchBtn, &QPushButton::clicked, this, &TeamWidget::onApplyTeamSearch);
        connect(clearSearchBtn, &QPushButton::clicked, this, &TeamWidget::onClearTeamSearch);

        refreshTable();
    }

    void refreshTable() {
        std::vector<doiBong*> allTeams;
        for (int i = 1; i <= db->getDoiBongCount(); ++i) {
            doiBong* team = db->getDoiBongById(i);
            if (team) allTeams.push_back(team);
        }
        renderTeamRows(allTeams);
    }

    void clearInputs() {
        nameInput->clear();
        coachInput->clear();
        locationInput->clear();
        rankInput->setValue(0);
        selectedTeamId = -1;
    }
};

// ==================== GUI - MAIN WINDOW ====================
class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Database db;
    PlayerWidget *playerWidget;
    TeamWidget *teamWidget;
    MatchWidget *matchWidget;
    TournamentWidget *tournamentWidget;
    QLabel *statusLabel;

public:
    MainWindow() {
        setWindowTitle("🏆 Quản Lý Bóng Đá PLB2 - v1.0");
        setGeometry(100, 100, 1200, 700);

        loadSampleData();

        QWidget *centralWidget = new QWidget();
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        statusLabel = new QLabel("✓ Ứng dụng sẵn sàng | Cầu thủ: " + 
                                QString::number(db.getCauThuCount()) + 
                                " | Đội: " + QString::number(db.getDoiBongCount()) +
                                " | Trận: " + QString::number(db.getTranDauCount()));
        statusLabel->setStyleSheet("QLabel { background-color: #E8F5E9; color: #2E7D32; padding: 8px; font-weight: bold; }");
        mainLayout->addWidget(statusLabel);

        QTabWidget *tabWidget = new QTabWidget();
        playerWidget = new PlayerWidget(&db);
        teamWidget = new TeamWidget(&db);
        matchWidget = new MatchWidget(&db);
        tournamentWidget = new TournamentWidget(&db);

        tabWidget->addTab(playerWidget, "👥 Cầu Thủ");
        tabWidget->addTab(teamWidget, "⚽ Đội Bóng");
        tabWidget->addTab(matchWidget, "🎮 Trận Đấu");
        tabWidget->addTab(tournamentWidget, "🏆 Vòng Bảng");

        mainLayout->addWidget(tabWidget);
        setCentralWidget(centralWidget);

        createMenuBar();
    }

private:
    void createMenuBar() {
        QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
        QAction *loadAction = fileMenu->addAction(tr("📥 Load Sample Data"));
        fileMenu->addSeparator();
        QAction *exitAction = fileMenu->addAction(tr("❌ Exit"));

        QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
        QAction *aboutAction = helpMenu->addAction(tr("ℹ️ About"));

        connect(loadAction, &QAction::triggered, this, &MainWindow::onLoadSampleData);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    }

    void loadSampleData() {
        doiBong db1(1, "Manchester United", "Erik ten Hag", "Manchester");
        doiBong db2(2, "Liverpool", "Jurgen Klopp", "Liverpool");
        doiBong db3(3, "Arsenal", "Mikel Arteta", "London");
        doiBong db4(4, "Chelsea", "Mauricio Pochettino", "London");

        db.insert_doiBong_single(db1);
        db.insert_doiBong_single(db2);
        db.insert_doiBong_single(db3);
        db.insert_doiBong_single(db4);

        cauThu ct1(1, "Cristiano Ronaldo", "ST", 39, 7, 1);
        cauThu ct2(2, "Harry Maguire", "CB", 31, 6, 1);
        cauThu ct3(3, "Bruno Fernandes", "CM", 29, 8, 1);
        cauThu ct4(4, "Mohamed Salah", "RW", 32, 11, 2);
        cauThu ct5(5, "Virgil van Dijk", "CB", 32, 4, 2);
        cauThu ct6(6, "Bukayo Saka", "LW", 23, 7, 3);
        cauThu ct7(7, "Martin Odegaard", "CM", 25, 8, 3);
        cauThu ct8(8, "Cole Palmer", "RW", 22, 20, 4);

        db.insert_cauThu_single(ct1);
        db.insert_cauThu_single(ct2);
        db.insert_cauThu_single(ct3);
        db.insert_cauThu_single(ct4);
        db.insert_cauThu_single(ct5);
        db.insert_cauThu_single(ct6);
        db.insert_cauThu_single(ct7);
        db.insert_cauThu_single(ct8);

        tranDau td1(1, 1, 2, 2, 2, 1704067200, 1, 0);
        tranDau td2(2, 3, 4, 3, 1, 1704153600, 0, 1);
        tranDau td3(3, 1, 3, 1, 1, 1704240000, 0, 0);

        db.insert_tranDau_single(td1);
        db.insert_tranDau_single(td2);
        db.insert_tranDau_single(td3);
    }

private slots:
    void onLoadSampleData() {
        db.dbCauThu.clear();
        db.dbDoiBong.clear();
        db.dbTranDau.clear();
        db.indexCauThuTuoi.clear();
        db.indexCauThuMaDoiBong.clear();
        db.indexDoiBongDiaPhuong.clear();
        db.indexTranDauNgay.clear();

        loadSampleData();
        playerWidget->refreshTable();
        teamWidget->refreshTable();
        statusLabel->setText("✓ Đã tải lại dữ liệu | Cầu thủ: " + 
                            QString::number(db.getCauThuCount()) + 
                            " | Đội: " + QString::number(db.getDoiBongCount()));
        QMessageBox::information(this, "✓", "Đã tải dữ liệu mẫu!");
    }

    void onAbout() {
        QMessageBox::about(this, tr("About PLB2"),
            tr("🏆 <b>Ứng Dụng Quản Lý Bóng Đá PLB2</b><br><br>"
               "<b>Phiên bản:</b> 1.0<br>"
               "<b>Công nghệ:</b> Qt5 + C++17<br>"
               "<b>Tính năng:</b><br>"
               "• Quản lý cầu thủ (CRUD)<br>"
               "• Quản lý đội bóng (CRUD)<br>"
               "• Tìm kiếm nâng cao bằng nhiều thuộc tính<br>"
               "• Secondary Indexing<br>"
               "• Giao diện thân thiện<br><br>"
               "© 2024 - All Rights Reserved"));
    }
};

// ==================== GUI ENTRY POINT ====================
int run_gui(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

// ==================== CONSOLE TESTS ====================
int main(int argc, char *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--gui") {
        return run_gui(argc, argv);
    }

    Database db;

    printSeparator("TEST 1: THEM DOI BONG");
    doiBong db1(1, "Manchester United", "Erik ten Hag", "Manchester");
    doiBong db2(2, "Liverpool", "Jurgen Klopp", "Liverpool");
    doiBong db3(3, "Arsenal", "Mikel Arteta", "London");
    doiBong db4(4, "Chelsea", "Mauricio Pochettino", "London");
    db.insert_doiBong_single(db1);
    db.insert_doiBong_single(db2);
    db.insert_doiBong_single(db3);
    db.insert_doiBong_single(db4);
    std::cout << "✓ Da them " << db.getDoiBongCount() << " doi bong\n" << std::endl;

    printSeparator("TEST 2: THEM CAU THU");
    cauThu ct1(1, "Cristiano Ronaldo", "ST", 39, 7, 1);
    cauThu ct2(2, "Harry Maguire", "CB", 31, 6, 1);
    cauThu ct3(3, "Bruno Fernandes", "CM", 29, 8, 1);
    cauThu ct4(4, "Mohamed Salah", "RW", 32, 11, 2);
    db.insert_cauThu_single(ct1);
    db.insert_cauThu_single(ct2);
    db.insert_cauThu_single(ct3);
    db.insert_cauThu_single(ct4);
    std::cout << "✓ Da them " << db.getCauThuCount() << " cau thu\n" << std::endl;

    printSeparator("TEST 3: TIM KIEM THEO ID");
    cauThu *ct = db.getCauThuById(1);
    if (ct) {
        std::cout << "✓ Tim thay: " << ct->getTen() << " - " << ct->getViTri() << std::endl;
    }

    printSeparator("TEST 4: THEM TRAN DAU");
    tranDau td1(1, 1, 2, 2, 2, 1704067200, 1, 0);
    tranDau td2(2, 3, 4, 3, 1, 1704153600, 0, 1);
    db.insert_tranDau_single(td1);
    db.insert_tranDau_single(td2);
    std::cout << "✓ Da them " << db.getTranDauCount() << " tran dau\n" << std::endl;

    printSeparator("TEST 5: CAP NHAT CAU THU");
    cauThu *ctUpdate = db.getCauThuById(1);
    if (ctUpdate) {
        ctUpdate->setTuoi(40);
        ctUpdate->setSoBanThang(850);
        std::cout << "✓ Cap nhat " << ctUpdate->getTen() << " - Tuoi: " << ctUpdate->getTuoi() 
                  << ", Ban thang: " << ctUpdate->getSoBanThang() << std::endl;
    }

    printSeparator("TEST 6: XOA CAU THU");
    bool deleted = db.dbCauThu.delete_by_id(4);
    std::cout << "✓ Xoa cau thu ID=4: " << (deleted ? "Thanh cong" : "That bai") 
              << " | So luong con lai: " << db.getCauThuCount() << std::endl;

    printSeparator("TEST 7: TIM THEO TUOI");
    std::cout << "Cac cau thu tuoi 31:\n";
    for (int i = 0; i < db.indexCauThuTuoi.size(); i++) {
        if (db.indexCauThuTuoi[i].key == 31) {
            cauThu *player = db.getCauThuById(db.indexCauThuTuoi[i].entityId);
            if (player) {
                std::cout << "  - " << player->getTen() << " (" << player->getTuoi() << " tuoi)\n";
            }
        }
    }

    printSeparator("TEST 8: TIM CAU THU THEO DOI");
    std::cout << "Cac cau thu thuoc doi 1:\n";
    for (int i = 0; i < db.indexCauThuMaDoiBong.size(); i++) {
        if (db.indexCauThuMaDoiBong[i].key == 1) {
            cauThu *player = db.getCauThuById(db.indexCauThuMaDoiBong[i].entityId);
            if (player) {
                std::cout << "  - " << player->getTen() << " (" << player->getViTri() << ")\n";
            }
        }
    }

    printSeparator("KET THUC TEST");
    std::cout << "✓ Tat ca test da hoan thanh thanh cong!\n" << std::endl;

    return 0;
}

#include "main.moc"
