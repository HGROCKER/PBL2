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

        int totalMatches = matches.size();
        for (int i = 0; i != totalMatches; ++i) {
            tranDau* match = matches[i];
            if (!match) continue;

            doiBong *team1 = db->getDoiBongById(match->getMaDoiBong1());
            doiBong *team2 = db->getDoiBongById(match->getMaDoiBong2());

            int row = matchTable->rowCount();
            matchTable->insertRow(row);

            // Cột 0: ID Trận đấu
            matchTable->setItem(row, 0, new QTableWidgetItem(QString::number(match->getId())));

            QString name1 = team1 ? QString::fromStdString(team1->getTen()).toUpper() : "N/A";
            QString name2 = team2 ? QString::fromStdString(team2->getTen()).toUpper() : "N/A";

            int score1 = match->getSoBanThangDoi1();
            int score2 = match->getSoBanThangDoi2();

            QTableWidgetItem *itemTeam1 = new QTableWidgetItem(name1);
            QTableWidgetItem *itemScore = new QTableWidgetItem(QString("  %1  -  %2  ").arg(score1).arg(score2));
            QTableWidgetItem *itemTeam2 = new QTableWidgetItem(name2);
            QTableWidgetItem *itemWinner = new QTableWidgetItem("");

            // Cấu hình font chữ TRẮNG ĐẬM
            QFont boldFont("Arial", 9, QFont::Bold);

            // ✅ FIX CHUẨN QT6: Thay thế setTextColor() bằng setForeground(QBrush(Qt::white))
            itemTeam1->setFont(boldFont); itemTeam1->setForeground(QBrush(Qt::white)); itemTeam1->setTextAlignment(Qt::AlignCenter);
            itemScore->setFont(boldFont); itemScore->setForeground(QBrush(Qt::white)); itemScore->setTextAlignment(Qt::AlignCenter);
            itemTeam2->setFont(boldFont); itemTeam2->setForeground(QBrush(Qt::white)); itemTeam2->setTextAlignment(Qt::AlignCenter);
            itemWinner->setFont(boldFont); itemWinner->setForeground(QBrush(Qt::white)); itemWinner->setTextAlignment(Qt::AlignCenter);

            // TỰ ĐỘNG PHÂN TÍCH KẾT QUẢ VÀ NHUỘM MÀU KHUNG CHIẾN THẮNG
            if (score1 > score2) {
                itemTeam1->setBackground(QBrush(QColor("#D32F2F"))); // Đội 1 thắng: Khung ĐỎ rực lửa
                itemTeam2->setBackground(QBrush(QColor("#000000"))); // Đội 2 thua: Khung ĐEN huyền bí
                itemWinner->setText(QString("👑 %1 THẮNG").arg(name1));
            } else if (score2 > score1) {
                itemTeam1->setBackground(QBrush(QColor("#000000"))); // Đội 1 thua: Khung ĐEN
                itemTeam2->setBackground(QBrush(QColor("#D32F2F"))); // Đội 2 thắng: Khung ĐỎ
                itemWinner->setText(QString("👑 %1 THẮNG").arg(name2));
            } else {
                itemTeam1->setBackground(QBrush(QColor("#000000"))); // Hòa: Cả 2 khung màu ĐEN
                itemTeam2->setBackground(QBrush(QColor("#000000")));
                itemWinner->setText("🤝 HÒA NHAU");
            }

            // Nạp các khung tương ứng vào các cột trên bảng dữ liệu
            matchTable->setItem(row, 1, itemTeam1);
            matchTable->setItem(row, 2, itemScore);
            matchTable->setItem(row, 3, itemTeam2);
            matchTable->setItem(row, 4, itemWinner);

            // Thống kê Thẻ vàng & Ngày thi đấu
            matchTable->setItem(row, 5, new QTableWidgetItem(QString::number(match->getSoTheVangDoi1())));
            matchTable->setItem(row, 6, new QTableWidgetItem(QString::number(match->getSoTheVangDoi2())));

            QDateTime dateTime;
            dateTime.setSecsSinceEpoch(match->getNgayThiDau());
            matchTable->setItem(row, 7, new QTableWidgetItem(dateTime.toString("dd/MM/yyyy HH:mm")));
        }

        // Cân chỉnh độ rộng cột tự động để không bao giờ dính lỗi che chữ
        matchTable->resizeColumnsToContents();
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
        if (!match) return;

        // 1. Cập nhật ngược dữ liệu lên các SpinBox của Form chỉnh sửa
        goals1Input->setValue(match->getSoBanThangDoi1());
        goals2Input->setValue(match->getSoBanThangDoi2());
        card1Input->setValue(match->getSoTheVangDoi1());
        card2Input->setValue(match->getSoTheVangDoi2());

        // 2. Lấy thông tin thực thể 2 đội bóng
        doiBong *team1 = db->getDoiBongById(match->getMaDoiBong1());
        doiBong *team2 = db->getDoiBongById(match->getMaDoiBong2());
        QString name1 = team1 ? QString::fromStdString(team1->getTen()).toUpper() : "N/A";
        QString name2 = team2 ? QString::fromStdString(team2->getTen()).toUpper() : "N/A";

        QDateTime dateTime;
        dateTime.setSecsSinceEpoch(match->getNgayThiDau());

        // 3. Xây dựng giao diện báo cáo chi tiết trận đấu (Tô đậm nhãn thông tin + Bo viền nghệ thuật)
        QString detail = QString(
                             "┌──────────────────────────────────────────────────┐\n"
                             " 🔥  CHI TIẾT DIỄN BIẾN TRẬN ĐẤU KINH ĐIỂN  🔥\n"
                             "└──────────────────────────────────────────────────┘\n\n"
                             "⏰  [THỜI GIAN]:\t%1\n"
                             "🏟️  [SÂN VẬN ĐỘNG]:\t%2\n\n"
                             "⚔️  [ĐỘI HÌNH ĐỐI ĐẦU]:\n"
                             "    ▶️  【%3】   VS   【%4】\n\n"
                             "📊  [KẾT QUẢ TỈ SỐ THỰC TẾ]:\n"
                             "    ⚽  %5   [ %6 ]  -  [ %7 ]   %8\n\n"
                             "🟨  [THỐNG KÊ THẺ VÀNG]:\n"
                             "    🔸 %9 (Thẻ) \t│\t 🔸 %10 (Thẻ)\n"
                             "────────────────────────────────────────────────────\n"
                             "💡 Mẹo: Bạn có muốn xem nhanh danh sách đội hình thi đấu của 2 đội này?"
                             ).arg(dateTime.toString("dd/MM/yyyy HH:mm"))
                             .arg(match->getSanVanDong().empty() ? "Sân vận động Quốc gia" : QString::fromStdString(match->getSanVanDong()))
                             .arg(name1).arg(name2)
                             .arg(name1).arg(match->getSoBanThangDoi1()).arg(match->getSoBanThangDoi2()).arg(name2)
                             .arg(match->getSoTheVangDoi1()).arg(match->getSoTheVangDoi2());

        // 4. Tạo Layer liên kết tương tác: Cho phép người dùng bấm nút để chuyển tiếp xem chi tiết Đội bóng
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("🎮 Chi Tiết Trận Đấu & Liên Kết Đội Hình");
        msgBox.setText(detail);
        msgBox.setIcon(QMessageBox::Information);

        QPushButton *viewTeam1Btn = msgBox.addButton(QString("👥 Xem CLB %1").arg(name1), QMessageBox::ActionRole);
        QPushButton *viewTeam2Btn = msgBox.addButton(QString("👥 Xem CLB %2").arg(name2), QMessageBox::ActionRole);
        QPushButton *closeBtn = msgBox.addButton("❌ Đóng lại", QMessageBox::RejectRole);

        msgBox.exec();

        // 5. Nếu người dùng nhấn chọn xem Đội 1 hoặc Đội 2 -> Tự động kích hoạt hiển thị Layer Profile Đội bóng
        doiBong* selectedTeam = nullptr;
        if (msgBox.clickedButton() == viewTeam1Btn) selectedTeam = team1;
        else if (msgBox.clickedButton() == viewTeam2Btn) selectedTeam = team2;

        if (selectedTeam != nullptr) {
            // Tái sử dụng mảng chỉ mục phụ (Secondary Index) để quét nhanh danh sách cầu thủ của đội đã chọn
            plb2::PlayerSearchFilter filter;
            filter.teamId = selectedTeam->getId();
            auto members = plb2::filterPlayers(*db, filter);

            QString memberListStr = "";
            int totalMembers = members.size();
            for (int i = 0; i != totalMembers; ++i) {
                cauThu* ct = members[i];
                if (ct) {
                    memberListStr += QString("   │   • [%1] %2 (%3)\n")
                                         .arg(ct->getSoAo())
                                         .arg(QString::fromStdString(ct->getTen()).toUpper())
                                         .arg(QString::fromStdString(ct->getViTri()).toUpper());
                }
            }

            QString teamDetail = QString(
                                     "┌──────────────────────────────────────────────────┐\n"
                                     " ⚽  HỒ SƠ CHI TIẾT ĐỘI BÓNG CHỦ QUẢN  ⚽\n"
                                     "└──────────────────────────────────────────────────┘\n\n"
                                     "🏢  [CÂU LẠC BỘ]:\t【%1】\n"
                                     "👨‍💼  [GEAR / HLV]:\t【%2】\n"
                                     "📍  [ĐỊA PHƯƠNG]:\t【%3】\n"
                                     "📈  [ĐIỂM XẾP HẠNG]:\t【%4 ĐIỂM】\n\n"
                                     "👥  [DANH SÁCH ĐỘI HÌNH ĐĂNG KÝ — %5 CHIẾN BINH]:\n"
                                     "┌──────────────────────────────────────────────────┐\n"
                                     "%6"
                                     "└──────────────────────────────────────────────────┘"
                                     ).arg(QString::fromStdString(selectedTeam->getTen()).toUpper())
                                     .arg(QString::fromStdString(selectedTeam->getHLV()).toUpper())
                                     .arg(QString::fromStdString(selectedTeam->getDiaPhuong()).toUpper())
                                     .arg(selectedTeam->getDiemXepHang())
                                     .arg(totalMembers)
                                     .arg(memberListStr.isEmpty() ? "   │   (Chưa đăng ký cầu thủ)\n" : memberListStr);

            QMessageBox::information(this, "⚽ Thông Tin Đội Hình Chi Tiết", teamDetail);
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
        // === CẤU TRÚC LẠI BẢNG TÁCH BIỆT KHUNG ĐỒ HỌA ===
        matchTable = new QTableWidget();
        matchTable->setColumnCount(8); // Tăng lên 8 cột để chia nhỏ khung
        matchTable->setHorizontalHeaderLabels({
            "ID", "⚔️ KHUNG ĐỘI 1", "📊 TỈ SỐ", "⚔️ KHUNG ĐỘI 2",
            "🏆 KẾT QUẢ", "🟨 THẺ V1", "🟨 THẺ V2", "📅 NGÀY THI ĐẤU"
        });
        matchTable->horizontalHeader()->setStretchLastSection(true);
        matchTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        matchTable->setStyleSheet(
            "QHeaderView::section { background-color: #F57C00; color: white; padding: 5px; font-weight: bold; }"
            "QTableWidget { gridline-color: #ccc; background-color: #222222; }" // Nền tối làm nổi bật khung Đỏ/Đen
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