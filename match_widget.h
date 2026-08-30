#pragma once
#ifndef MATCH_WIDGET_H
#define MATCH_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView> // ✅ ĐÃ BỔ SUNG: Sửa lỗi incomplete type 'class QHeaderView'
#include "database.h"
#include "search_features.h"

// Giữ nguyên toàn bộ phần mã nguồn class MatchWidget bên dưới...


class MatchWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    QTableWidget *matchTable;
    QComboBox *team1Combo, *team2Combo;
    QSpinBox *goals1Input, *goals2Input;
    QSpinBox *card1Input, *card2Input;
    QPushButton *addBtn, *editBtn, *deleteBtn;
    
    QComboBox *searchTeamCombo;
    QSpinBox *searchMinGoalDiff, *searchMaxGoalDiff;
    QSpinBox *searchMinTotalGoals, *searchMaxTotalGoals;
    QPushButton *searchBtn, *clearSearchBtn;
    QLabel *statusLabel;
    long long selectedMatchId = -1;

    void renderMatchRows(const std::vector<tranDau*>& matches) {
        matchTable->setRowCount(0);
        for (tranDau* match : matches) {
            if (!match) continue;
            
            doiBong *team1 = db->getDoiBongById(match->getMaDoiBong1());
            doiBong *team2 = db->getDoiBongById(match->getMaDoiBong2());
            
            int row = matchTable->rowCount();
            matchTable->insertRow(row);
            matchTable->setItem(row, 0, new QTableWidgetItem(QString::number(match->getId())));
            
            QString tenDoi1 = team1 ? QString::fromStdString(team1->getTen()) : "N/A";
            QString tenDoi2 = team2 ? QString::fromStdString(team2->getTen()) : "N/A";
            
            QString ketQua = QString("%1 %2 - %3 %4")
                .arg(tenDoi1)
                .arg(match->getSoBanThangDoi1())
                .arg(match->getSoBanThangDoi2())
                .arg(tenDoi2);
            
            matchTable->setItem(row, 1, new QTableWidgetItem(ketQua));
            matchTable->setItem(row, 2, new QTableWidgetItem(QString::number(match->getSoTheVangDoi1())));
            matchTable->setItem(row, 3, new QTableWidgetItem(QString::number(match->getSoTheVangDoi2())));
            
            QDateTime dateTime;
            dateTime.setSecsSinceEpoch(match->getNgayThiDau());
            matchTable->setItem(row, 4, new QTableWidgetItem(dateTime.toString("dd/MM/yyyy HH:mm")));
        }
    }

public slots:
    void onAddMatch() {
        if (team1Combo->currentData().toInt() == team2Combo->currentData().toInt()) {
            QMessageBox::warning(this, "❌ Lỗi", "Hai đội phải khác nhau!");
            return;
        }
        
        long long newId = db->getNextTranDauId();
        QDateTime now = QDateTime::currentDateTime();
        
        tranDau match(newId,
                     team1Combo->currentData().toInt(),
                     team2Combo->currentData().toInt(),
                     goals1Input->value(),
                     goals2Input->value(),
                     now.toSecsSinceEpoch(),
                     card1Input->value(),
                     card2Input->value());
        
        db->insert_tranDau_single(match);
        refreshTable();
        statusLabel->setText("✓ Thêm trận đấu thành công!");
        clearInputs();
    }

    void onEditMatch() {
        if (selectedMatchId == -1) {
            QMessageBox::warning(this, "❌ Lỗi", "Chọn trận để sửa!");
            return;
        }
        tranDau *match = db->getTranDauById(selectedMatchId);
        if (match) {
            match->setSoBanThangDoi1(goals1Input->value());
            match->setSoBanThangDoi2(goals2Input->value());
            refreshTable();
            statusLabel->setText("✓ Cập nhật trận đấu thành công!");
        }
    }

    void onDeleteMatch() {
        if (selectedMatchId == -1) {
            QMessageBox::warning(this, "❌ Lỗi", "Chọn trận để xóa!");
            return;
        }
        if (QMessageBox::question(this, "⚠️", "Xóa trận đấu này?", 
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            db->dbTranDau.delete_by_id(selectedMatchId);
            refreshTable();
            statusLabel->setText("✓ Xóa thành công!");
            clearInputs();
        }
    }

    void onApplySearch() {
        plb2::MatchSearchFilter filter;
        if (searchTeamCombo->currentData().toInt() >= 0) {
            filter.teamId = searchTeamCombo->currentData().toInt();
        }
        if (searchMinGoalDiff->value() > 0) filter.minGoalDiff = searchMinGoalDiff->value();
        if (searchMaxGoalDiff->value() > 0) filter.maxGoalDiff = searchMaxGoalDiff->value();
        if (searchMinTotalGoals->value() > 0) filter.minTotalGoals = searchMinTotalGoals->value();
        if (searchMaxTotalGoals->value() > 0) filter.maxTotalGoals = searchMaxTotalGoals->value();

        auto matches = plb2::filterMatches(*db, filter);
        renderMatchRows(matches);
        statusLabel->setText(QString("✓ Tìm thấy %1 trận").arg(matches.size()));
    }

    void onClearSearch() {
        searchTeamCombo->setCurrentIndex(0);
        searchMinGoalDiff->setValue(0);
        searchMaxGoalDiff->setValue(0);
        searchMinTotalGoals->setValue(0);
        searchMaxTotalGoals->setValue(0);
        refreshTable();
        statusLabel->setText("✓ Đã reset bộ lọc!");
    }

    void onTableClicked(int row, int column) {
        if (row < 0) return;
        selectedMatchId = matchTable->item(row, 0)->text().toLongLong();
        tranDau *match = db->getTranDauById(selectedMatchId);
        if (match) {
            for (int i = 0; i < team1Combo->count(); i++) {
                if (team1Combo->itemData(i).toInt() == match->getMaDoiBong1()) {
                    team1Combo->setCurrentIndex(i);
                    break;
                }
            }
            for (int i = 0; i < team2Combo->count(); i++) {
                if (team2Combo->itemData(i).toInt() == match->getMaDoiBong2()) {
                    team2Combo->setCurrentIndex(i);
                    break;
                }
            }
            goals1Input->setValue(match->getSoBanThangDoi1());
            goals2Input->setValue(match->getSoBanThangDoi2());
            card1Input->setValue(match->getSoTheVangDoi1());
            card2Input->setValue(match->getSoTheVangDoi2());
        }
    }

public:
    MatchWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), db(db), selectedMatchId(-1) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // === FORM NHẬP DỮ LIỆU ===
        QGroupBox *formGroup = new QGroupBox("➕ Thêm / Sửa Trận Đấu");
        formGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #F57C00; padding-top: 10px; }");
        QGridLayout *formLayout = new QGridLayout(formGroup);

        formLayout->addWidget(new QLabel("Đội 1:"), 0, 0);
        team1Combo = new QComboBox();
        team1Combo->addItem("Chọn đội", -1);
        for (int i = 1; i <= db->getDoiBongCount(); i++) {
            doiBong *team = db->getDoiBongById(i);
            if (team) team1Combo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        formLayout->addWidget(team1Combo, 0, 1);

        formLayout->addWidget(new QLabel("Bàn:"), 0, 2);
        goals1Input = new QSpinBox();
        goals1Input->setRange(0, 20);
        formLayout->addWidget(goals1Input, 0, 3);

        formLayout->addWidget(new QLabel("Thẻ vàng:"), 0, 4);
        card1Input = new QSpinBox();
        card1Input->setRange(0, 11);
        formLayout->addWidget(card1Input, 0, 5);

        formLayout->addWidget(new QLabel("Đội 2:"), 1, 0);
        team2Combo = new QComboBox();
        team2Combo->addItem("Chọn đội", -1);
        for (int i = 1; i <= db->getDoiBongCount(); i++) {
            doiBong *team = db->getDoiBongById(i);
            if (team) team2Combo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        formLayout->addWidget(team2Combo, 1, 1);

        formLayout->addWidget(new QLabel("Bàn:"), 1, 2);
        goals2Input = new QSpinBox();
        goals2Input->setRange(0, 20);
        formLayout->addWidget(goals2Input, 1, 3);

        formLayout->addWidget(new QLabel("Thẻ vàng:"), 1, 4);
        card2Input = new QSpinBox();
        card2Input->setRange(0, 11);
        formLayout->addWidget(card2Input, 1, 5);

        QHBoxLayout *btnLayout = new QHBoxLayout();
        addBtn = new QPushButton("➕ Thêm Trận");
        addBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }");
        editBtn = new QPushButton("✏️ Cập Nhật");
        editBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }");
        deleteBtn = new QPushButton("🗑️ Xóa");
        deleteBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }");

        btnLayout->addWidget(addBtn);
        btnLayout->addWidget(editBtn);
        btnLayout->addWidget(deleteBtn);
        formLayout->addLayout(btnLayout, 2, 0, 1, 6);

        mainLayout->addWidget(formGroup);

        // === SEARCH PANEL ===
        QGroupBox *searchGroup = new QGroupBox("🔎 Tìm Kiếm Trận Đấu");
        searchGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #C62828; padding-top: 10px; }");
        QGridLayout *searchLayout = new QGridLayout(searchGroup);

        searchLayout->addWidget(new QLabel("Đội:"), 0, 0);
        searchTeamCombo = new QComboBox();
        searchTeamCombo->addItem("Tất cả", -1);
        for (int i = 1; i <= db->getDoiBongCount(); i++) {
            doiBong *team = db->getDoiBongById(i);
            if (team) searchTeamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        searchLayout->addWidget(searchTeamCombo, 0, 1);

        searchLayout->addWidget(new QLabel("Hiệu số bàn từ:"), 0, 2);
        searchMinGoalDiff = new QSpinBox();
        searchMinGoalDiff->setRange(0, 20);
        searchLayout->addWidget(searchMinGoalDiff, 0, 3);

        searchLayout->addWidget(new QLabel("đến:"), 0, 4);
        searchMaxGoalDiff = new QSpinBox();
        searchMaxGoalDiff->setRange(0, 20);
        searchLayout->addWidget(searchMaxGoalDiff, 0, 5);

        searchLayout->addWidget(new QLabel("Tổng bàn từ:"), 1, 0);
        searchMinTotalGoals = new QSpinBox();
        searchMinTotalGoals->setRange(0, 30);
        searchLayout->addWidget(searchMinTotalGoals, 1, 1);

        searchLayout->addWidget(new QLabel("đến:"), 1, 2);
        searchMaxTotalGoals = new QSpinBox();
        searchMaxTotalGoals->setRange(0, 30);
        searchLayout->addWidget(searchMaxTotalGoals, 1, 3);

        QHBoxLayout *searchBtnLayout = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Lọc");
        clearSearchBtn = new QPushButton("❌ Xóa lọc");
        searchBtnLayout->addWidget(searchBtn);
        searchBtnLayout->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchBtnLayout, 1, 4, 1, 2);

        mainLayout->addWidget(searchGroup);

        // === TABLE ===
        matchTable = new QTableWidget();
        matchTable->setColumnCount(5);
        matchTable->setHorizontalHeaderLabels({"ID", "Kết Quả", "Thẻ Vàng Đội 1", "Thẻ Vàng Đội 2", "Ngày Thi Đấu"});
        matchTable->horizontalHeader()->setStretchLastSection(true);
        matchTable->setStyleSheet(
            "QHeaderView::section { background-color: #F57C00; color: white; padding: 5px; font-weight: bold; }"
        );
        mainLayout->addWidget(matchTable);

        statusLabel = new QLabel("✓ Sẵn sàng");
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        mainLayout->addWidget(statusLabel);

        connect(addBtn, &QPushButton::clicked, this, &MatchWidget::onAddMatch);
        connect(editBtn, &QPushButton::clicked, this, &MatchWidget::onEditMatch);
        connect(deleteBtn, &QPushButton::clicked, this, &MatchWidget::onDeleteMatch);
        connect(matchTable, &QTableWidget::cellClicked, this, &MatchWidget::onTableClicked);
        connect(searchBtn, &QPushButton::clicked, this, &MatchWidget::onApplySearch);
        connect(clearSearchBtn, &QPushButton::clicked, this, &MatchWidget::onClearSearch);

        refreshTable();
    }

    void refreshTable() {
        std::vector<tranDau*> allMatches;
        for (int i = 1; i <= db->getTranDauCount(); ++i) {
            tranDau* match = db->getTranDauById(i);
            if (match) allMatches.push_back(match);
        }
        renderMatchRows(allMatches);
    }

    void clearInputs() {
        team1Combo->setCurrentIndex(0);
        team2Combo->setCurrentIndex(0);
        goals1Input->setValue(0);
        goals2Input->setValue(0);
        card1Input->setValue(0);
        card2Input->setValue(0);
        selectedMatchId = -1;
    }
};

#endif