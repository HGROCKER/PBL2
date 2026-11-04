
#pragma once
#ifndef MATCH_WIDGET_H
#define MATCH_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDateEdit>
#include <QHeaderView>
#include <QStackedWidget>
#include <QFrame>
#include "database.h"
#include "core/application_logic.h"
#include "search_features.h"
#include "table_helpers.h"
#include "ui/logic_messages.h"

class MatchWidget : public QWidget
{
    Q_OBJECT

private:
    Database *db;
    plb2::ApplicationLogic logic;
    long long selectedMatchId = -1;

    // Quản lý chuyển đổi hai Layer mượt mà
    QStackedWidget *stackedWidget;
    QWidget *layerList;   // Layer 1: Tìm kiếm & Danh sách chính
    QWidget *layerDetail; // Layer 2: Xem chi tiết & Cập nhật dữ liệu

    // Thành phần Layer 1 (List & Search)
    QTableWidget *matchTable;
    QComboBox *searchTeamCombo;
    QComboBox *searchDateModeCombo;
    QDateEdit *searchFromDateInput, *searchToDateInput;
    QSpinBox *searchMinGoalDiff, *searchMaxGoalDiff;
    QSpinBox *searchMinTotalGoals, *searchMaxTotalGoals;
    QPushButton *searchBtn, *clearSearchBtn;
    QPushButton *openAddFormBtn;
    QLabel *statusLabel;

    // Thành phần Layer 2 (Form Edit & Profile view)
    QComboBox *team1Combo, *team2Combo;
    QSpinBox *goals1Input, *goals2Input;
    QSpinBox *card1Input, *card2Input;
    QLineEdit *stadiumInput;
    QDateTimeEdit *matchDateInput;
    QLabel *imagePlaceholder; // Ô 3*4 hiển thị hình ảnh đồ họa trận đấu
    QPushButton *saveBtn, *resetBtn, *backBtn, *deleteBtn;
    QLabel *detailTitleLabel;

    // Biến phụ trợ trạng thái Form (Dùng cho chức năng Add hoặc Edit)
    bool isEditMode = false;

    void initLayerList()
    {
        layerList = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(layerList);
        QVBoxLayout *tableLayout = new QVBoxLayout();
        QVBoxLayout *sideLayout = new QVBoxLayout();

        // === BỘ LỌC TÌM KIẾM NÂNG CAO ===
        QGroupBox *searchGroup = new QGroupBox("🔎 Bộ Lọc Tìm Kiếm Trận Đấu Chuyên Sâu");
        searchGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #C62828; padding-top: 10px; }");
        QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
        QHBoxLayout *searchBtnLayout = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Tìm / Lọc");
        clearSearchBtn = new QPushButton("Xóa bộ lọc");
        searchBtnLayout->addWidget(searchBtn);
        searchBtnLayout->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchBtnLayout);
        QFormLayout *filterForm = new QFormLayout();
        searchTeamCombo = new QComboBox();
        searchTeamCombo->addItem("Tất cả các đội", -1);
        for (int i = 0; i < db->getDoiBongCount(); ++i)
        {
            doiBong *team = db->getDoiBongAt(i);
            if (team)
                searchTeamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        searchMinGoalDiff = new QSpinBox();
        searchMinGoalDiff->setRange(-1, 20);
        searchMinGoalDiff->setSpecialValueText("Bất kỳ");
        searchMinGoalDiff->setValue(-1);
        searchMaxGoalDiff = new QSpinBox();
        searchMaxGoalDiff->setRange(-1, 20);
        searchMaxGoalDiff->setSpecialValueText("Bất kỳ");
        searchMaxGoalDiff->setValue(-1);
        searchMinTotalGoals = new QSpinBox();
        searchMinTotalGoals->setRange(-1, 30);
        searchMinTotalGoals->setSpecialValueText("Bất kỳ");
        searchMinTotalGoals->setValue(-1);
        searchMaxTotalGoals = new QSpinBox();
        searchMaxTotalGoals->setRange(-1, 30);
        searchMaxTotalGoals->setSpecialValueText("Bất kỳ");
        searchMaxTotalGoals->setValue(-1);
        auto addRange = [&](const QString& label, QSpinBox *minimum, QSpinBox *maximum) {
            QHBoxLayout *rangeLayout = new QHBoxLayout();
            rangeLayout->addWidget(minimum);
            rangeLayout->addWidget(new QLabel("đến"));
            rangeLayout->addWidget(maximum);
            filterForm->addRow(label, rangeLayout);
        };
        filterForm->addRow("Đội tham gia:", searchTeamCombo);
        searchDateModeCombo = new QComboBox();
        searchDateModeCombo->addItem("Toàn giải", static_cast<int>(plb2::MatchDateMode::All));
        searchDateModeCombo->addItem("Ngày", static_cast<int>(plb2::MatchDateMode::Day));
        searchDateModeCombo->addItem("Tháng", static_cast<int>(plb2::MatchDateMode::Month));
        searchDateModeCombo->addItem("Năm", static_cast<int>(plb2::MatchDateMode::Year));
        searchDateModeCombo->addItem("Khoảng ngày", static_cast<int>(plb2::MatchDateMode::Range));
        searchFromDateInput = new QDateEdit(QDate::currentDate());
        searchFromDateInput->setCalendarPopup(true);
        searchFromDateInput->setDisplayFormat("dd/MM/yyyy");
        searchToDateInput = new QDateEdit(QDate::currentDate());
        searchToDateInput->setCalendarPopup(true);
        searchToDateInput->setDisplayFormat("dd/MM/yyyy");
        filterForm->addRow("Lọc theo:", searchDateModeCombo);
        filterForm->addRow("Ngày / từ:", searchFromDateInput);
        filterForm->addRow("Đến ngày:", searchToDateInput);
        connect(searchDateModeCombo, &QComboBox::currentIndexChanged, this, [this]() {
            const bool range = searchDateModeCombo->currentData().toInt() ==
                               static_cast<int>(plb2::MatchDateMode::Range);
            searchToDateInput->setEnabled(range);
        });
        searchToDateInput->setEnabled(false);
        addRange("Hiệu số bàn:", searchMinGoalDiff, searchMaxGoalDiff);
        addRange("Tổng số bàn:", searchMinTotalGoals, searchMaxTotalGoals);
        searchLayout->addLayout(filterForm);
        sideLayout->addWidget(searchGroup);

        // === THANH THAO TÁC NHANH ===
        openAddFormBtn = new QPushButton("➕ Tạo Biên Bản Trận Đấu Mới");
        openAddFormBtn->setStyleSheet("QPushButton { background-color: #2E7D32; color: white; padding: 10px; border-radius: 4px; font-weight: bold; }");
        sideLayout->addWidget(openAddFormBtn);
        sideLayout->addStretch();

        // === BẢNG HIỂN THỊ DANH SÁCH ===
        matchTable = new QTableWidget();
        matchTable->setColumnCount(13);
        matchTable->setHorizontalHeaderLabels({"ID", "Mã đội 1", "Đội nhà", "Bàn 1", "Mã đội 2",
                                               "Đội khách", "Bàn 2", "Điểm đội nhà", "Điểm đội khách",
                                               "Thẻ 1", "Thẻ 2", "Ngày giờ", "Sân vận động"});
        matchTable->horizontalHeader()->setStretchLastSection(true);
        matchTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        matchTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho sửa trực tiếp trên ô lưới bảng
        matchTable->setStyleSheet(
            "QHeaderView::section { background-color: #F57C00; color: white; padding: 5px; font-weight: bold; }"
            "QTableWidget { gridline-color: #ccc; background-color: #222222; }");
        matchTable->setMinimumHeight(400);
        tableLayout->addWidget(matchTable, 1);

        statusLabel = new QLabel("✓ Hệ thống sẵn sàng thao tác.");
        statusLabel->setStyleSheet("QLabel { color: #4CAF50; font-weight: bold; }");
        tableLayout->addWidget(statusLabel);
        layout->addLayout(tableLayout, 1);
        layout->addLayout(sideLayout);
    }

    void initLayerDetail()
    {
        layerDetail = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(layerDetail);

        // Thanh tiêu đề điều hướng trên cùng của Layer 2
        QHBoxLayout *headerLayout = new QHBoxLayout();
        backBtn = new QPushButton("⬅ Trở về Danh sách");
        backBtn->setStyleSheet("QPushButton { background-color: #757575; color: white; padding: 6px 12px; font-weight: bold; border-radius: 3px; }");
        detailTitleLabel = new QLabel("🔥 CHI TIẾT DIỄN BIẾN TRẬN ĐẤU KINH ĐIỂN");
        detailTitleLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; color: #F57C00; }");
        headerLayout->addWidget(backBtn);
        headerLayout->addWidget(detailTitleLabel);
        headerLayout->addStretch();
        layout->addLayout(headerLayout);

        // Khung Workspace chia làm 2 phần: Trái (Ảnh Profile 3*4) - Phải (Form dữ liệu)
        QHBoxLayout *workspaceLayout = new QHBoxLayout();

        // 1. Bên Trái: Ô 3*4 mô phỏng hiển thị hình ảnh
        QGroupBox *imageGroup = new QGroupBox("Hồ Sơ Đồ Họa");
        QVBoxLayout *imageLayout = new QVBoxLayout(imageGroup);
        imagePlaceholder = new QLabel();
        imagePlaceholder->setFixedSize(180, 240); // Tỷ lệ chuẩn định dạng 3*4
        imagePlaceholder->setAlignment(Qt::AlignCenter);
        imagePlaceholder->setStyleSheet("QLabel { background-color: #111111; color: #888; border: 2px dashed #F57C00; border-radius: 4px; font-size: 11px; }");
        imageLayout->addWidget(imagePlaceholder);
        workspaceLayout->addWidget(imageGroup);

        // 2. Bên Phải: Form nhập liệu & tinh chỉnh số liệu
        QGroupBox *formGroup = new QGroupBox("Thông Tin Biên Bản Chi Tiết");
        formGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2196F3; }");
        QGridLayout *formLayout = new QGridLayout(formGroup);

        formLayout->addWidget(new QLabel("Đội Nhà (Đội 1):"), 0, 0);
        team1Combo = new QComboBox();
        team1Combo->addItem("Chọn đội tuyển", -1);
        for (int i = 0; i < db->getDoiBongCount(); ++i)
        {
            doiBong *team = db->getDoiBongAt(i);
            if (team)
                team1Combo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        formLayout->addWidget(team1Combo, 0, 1);

        formLayout->addWidget(new QLabel("Số Bàn Thắng:"), 0, 2);
        goals1Input = new QSpinBox();
        goals1Input->setRange(0, 20);
        formLayout->addWidget(goals1Input, 0, 3);

        formLayout->addWidget(new QLabel("Thẻ Vàng:"), 0, 4);
        card1Input = new QSpinBox();
        card1Input->setRange(0, 11);
        formLayout->addWidget(card1Input, 0, 5);

        formLayout->addWidget(new QLabel("Đội Khách (Đội 2):"), 1, 0);
        team2Combo = new QComboBox();
        team2Combo->addItem("Chọn đội tuyển", -1);
        for (int i = 0; i < db->getDoiBongCount(); ++i)
        {
            doiBong *team = db->getDoiBongAt(i);
            if (team)
                team2Combo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
        }
        formLayout->addWidget(team2Combo, 1, 1);

        formLayout->addWidget(new QLabel("Số Bàn Thắng:"), 1, 2);
        goals2Input = new QSpinBox();
        goals2Input->setRange(0, 20);
        formLayout->addWidget(goals2Input, 1, 3);

        formLayout->addWidget(new QLabel("Thẻ Vàng:"), 1, 4);
        card2Input = new QSpinBox();
        card2Input->setRange(0, 11);
        formLayout->addWidget(card2Input, 1, 5);

        formLayout->addWidget(new QLabel("Sân Vận Động:"), 2, 0);
        stadiumInput = new QLineEdit();
        formLayout->addWidget(stadiumInput, 2, 1, 1, 5);
        formLayout->addWidget(new QLabel("Ngày giờ thi đấu:"), 3, 0);
        matchDateInput = new QDateTimeEdit(QDateTime::currentDateTime());
        matchDateInput->setCalendarPopup(true);
        matchDateInput->setDisplayFormat("dd/MM/yyyy HH:mm");
        formLayout->addWidget(matchDateInput, 3, 1, 1, 2);

        workspaceLayout->addWidget(formGroup, 1); // Cấp giãn nở lớn hơn cho vùng form
        layout->addLayout(workspaceLayout);

        // Vùng nút bấm quản lý Control Panel dưới đáy Layer 2
        QHBoxLayout *controlLayout = new QHBoxLayout();
        saveBtn = new QPushButton("💾 Lưu Thay Đổi");
        saveBtn->setStyleSheet("QPushButton { background-color: #1E88E5; color: white; padding: 10px 20px; font-weight: bold; border-radius: 4px; }");

        resetBtn = new QPushButton("🔄 Reset / Khôi phục");
        resetBtn->setStyleSheet("QPushButton { background-color: #FFB300; color: black; padding: 10px 20px; font-weight: bold; border-radius: 4px; }");

        deleteBtn = new QPushButton("🗑️ Xóa Trận Đấu");
        deleteBtn->setStyleSheet("QPushButton { background-color: #E53935; color: white; padding: 10px 20px; font-weight: bold; border-radius: 4px; }");

        controlLayout->addWidget(saveBtn);
        controlLayout->addWidget(resetBtn);
        controlLayout->addWidget(deleteBtn);
        controlLayout->addStretch();
        layout->addLayout(controlLayout);
    }

    void renderMatchRows(const std::vector<tranDau *> &matches)
    {
        matchTable->setRowCount(0);

        int totalMatches = matches.size();
        for (int i = 0; i != totalMatches; ++i)
        {
            tranDau *match = matches[i];
            if (!match)
                continue;
            doiBong *team1 = db->getDoiBongById(match->getMaDoiBong1());
            doiBong *team2 = db->getDoiBongById(match->getMaDoiBong2());
            int row = matchTable->rowCount();
            matchTable->insertRow(row);
            matchTable->setItem(row, 0, sortableItem(QString::number(match->getId()),
                                                      QVariant::fromValue<qlonglong>(match->getId())));
            QString name1 = team1 ? QString::fromStdString(team1->getTen()).toUpper() : "N/A";
            QString name2 = team2 ? QString::fromStdString(team2->getTen()).toUpper() : "N/A";
            const int score1 = match->getSoBanThangDoi1();
            const int score2 = match->getSoBanThangDoi2();
            const auto outcome = plb2::ApplicationLogic::outcomeOf(*match);
            QTableWidgetItem *itemTeam1 = new QTableWidgetItem(name1);
            QTableWidgetItem *itemScore = new QTableWidgetItem(QString(" %1 - %2 ").arg(score1).arg(score2));
            QTableWidgetItem *itemTeam2 = new QTableWidgetItem(name2);
            QTableWidgetItem *itemWinner = new QTableWidgetItem("");
            QFont boldFont("Arial", 9, QFont::Bold);
            itemTeam1->setFont(boldFont);
            itemTeam1->setForeground(QBrush(Qt::white));
            itemTeam1->setTextAlignment(Qt::AlignCenter);
            itemScore->setFont(boldFont);
            itemScore->setForeground(QBrush(Qt::white));
            itemScore->setTextAlignment(Qt::AlignCenter);
            itemTeam2->setFont(boldFont);
            itemTeam2->setForeground(QBrush(Qt::white));
            itemTeam2->setTextAlignment(Qt::AlignCenter);
            itemWinner->setFont(boldFont);
            itemWinner->setForeground(QBrush(Qt::white));
            itemWinner->setTextAlignment(Qt::AlignCenter);
            if (outcome == plb2::MatchOutcome::HomeWin)
            {
                itemTeam1->setBackground(QBrush(QColor("#D32F2F")));
                itemTeam2->setBackground(QBrush(QColor("#000000")));
                itemWinner->setText(QString("👑 %1 THẮNG").arg(name1));
            }
            else if (outcome == plb2::MatchOutcome::AwayWin)
            {
                itemTeam1->setBackground(QBrush(QColor("#000000")));
                itemTeam2->setBackground(QBrush(QColor("#D32F2F")));
                itemWinner->setText(QString("👑 %1 THẮNG").arg(name2));
            }
            else
            {
                itemTeam1->setBackground(QBrush(QColor("#000000")));
                itemTeam2->setBackground(QBrush(QColor("#000000")));
                itemWinner->setText("🤝 HÒA NHAU");
            }
            matchTable->setItem(row, 1, sortableItem(QString::number(match->getMaDoiBong1()), match->getMaDoiBong1()));
            matchTable->setItem(row, 2, itemTeam1);
            matchTable->setItem(row, 3, sortableItem(QString::number(score1), score1));
            matchTable->setItem(row, 4, sortableItem(QString::number(match->getMaDoiBong2()), match->getMaDoiBong2()));
            matchTable->setItem(row, 5, itemTeam2);
            matchTable->setItem(row, 6, sortableItem(QString::number(score2), score2));
            matchTable->setItem(row, 7, sortableItem(QString::number(match->getDiemDoiBong1()), match->getDiemDoiBong1()));
            matchTable->setItem(row, 8, sortableItem(QString::number(match->getDiemDoiBong2()), match->getDiemDoiBong2()));
            matchTable->setItem(row, 9, sortableItem(QString::number(match->getSoTheVangDoi1()), match->getSoTheVangDoi1()));
            matchTable->setItem(row, 10, sortableItem(QString::number(match->getSoTheVangDoi2()), match->getSoTheVangDoi2()));
            QDateTime dateTime;
            dateTime.setSecsSinceEpoch(match->getNgayThiDau());
            matchTable->setItem(row, 11, sortableItem(dateTime.toString("dd/MM/yyyy HH:mm"),
                                                      QVariant::fromValue<qlonglong>(match->getNgayThiDau())));
            matchTable->setItem(row, 12, sortableItem(QString::fromStdString(match->getSanVanDong())));
        }
        const int columnWidths[] = {55, 75, 130, 55, 75, 130, 55, 70, 75, 55, 55, 130, 150};
        for (int column = 0; column < matchTable->columnCount(); ++column)
            matchTable->setColumnWidth(column, columnWidths[column]);
    }
public slots:
    void onTableClicked(int row, int column)
    {
        if (row < 0)
            return;
        selectedMatchId = matchTable->item(row, 0)->text().toLongLong();
        isEditMode = true;
        deleteBtn->setVisible(true);
        detailTitleLabel->setText("🔥 CHẾ ĐỘ: CẬP NHẬT CHI TIẾT BIÊN BẢN TRẬN ĐẤU");
        loadMatchToForm(selectedMatchId);
        // Chuyển dịch toàn bộ giao diện sang Layer 2 (giữ nguyên Layer 1 chạy ngầm)
        stackedWidget->setCurrentIndex(1);
    }
    void onOpenAddForm()
    {
        selectedMatchId = -1;
        isEditMode = false;
        deleteBtn->setVisible(false);
        detailTitleLabel->setText("➕ CHẾ ĐỘ: THÊM BIÊN BẢN TRẬN ĐẤU MỚI THÀNH LẬP");
        // Clear trắng form
        team1Combo->setCurrentIndex(0);
        team2Combo->setCurrentIndex(0);
        goals1Input->setValue(0);
        goals2Input->setValue(0);
        card1Input->setValue(0);
        card2Input->setValue(0);
        stadiumInput->setText("Sân vận động Quốc gia");
        matchDateInput->setDateTime(QDateTime::currentDateTime());
        // Tạo ảnh đồ họa giả lập 3*4 cho trận đấu mới
        imagePlaceholder->setText("⚽ MATCH IMAGE\n[ 3 * 4 PREVIEW ]\nChưa có dữ liệu");
        stackedWidget->setCurrentIndex(1);
    }
    void loadMatchToForm(long long id)
    {
        tranDau *match = db->getTranDauById(id);
        if (!match)
            return;
        goals1Input->setValue(match->getSoBanThangDoi1());
        goals2Input->setValue(match->getSoBanThangDoi2());
        card1Input->setValue(match->getSoTheVangDoi1());
        card2Input->setValue(match->getSoTheVangDoi2());
        stadiumInput->setText(match->getSanVanDong().empty() ? "Sân vận động Quốc gia" : QString::fromStdString(match->getSanVanDong()));
        matchDateInput->setDateTime(QDateTime::fromSecsSinceEpoch(match->getNgayThiDau()));
        int t1Idx = team1Combo->findData(QVariant((int)match->getMaDoiBong1()));
        if (t1Idx != -1)
            team1Combo->setCurrentIndex(t1Idx);
        int t2Idx = team2Combo->findData(QVariant((int)match->getMaDoiBong2()));
        if (t2Idx != -1)
            team2Combo->setCurrentIndex(t2Idx);
        // Vẽ text giả lập ảnh 3*4 trực quan
        doiBong *d1 = db->getDoiBongById(match->getMaDoiBong1());
        doiBong *d2 = db->getDoiBongById(match->getMaDoiBong2());
        QString n1 = d1 ? QString::fromStdString(d1->getTen()).toUpper() : "N/A";
        QString n2 = d2 ? QString::fromStdString(d2->getTen()).toUpper() : "N/A";
        imagePlaceholder->setText(QString("🏟️ VERSUS VISUAL\n\n%1\nVS\n%2\n\n⚽ Tỉ số: %3 - %4")
                                      .arg(n1)
                                      .arg(n2)
                                      .arg(match->getSoBanThangDoi1())
                                      .arg(match->getSoBanThangDoi2()));
    }
    void onResetForm()
    {
        if (isEditMode && selectedMatchId != -1)
        {
            // Đọc lại chính xác thông tin đối tượng từ CSDL gốc (Xóa bỏ các ký tự ghi bậy)
            loadMatchToForm(selectedMatchId);
            statusLabel->setText("✓ Đã hủy thay đổi, khôi phục dữ liệu gốc lần lưu gần nhất!");
        }
        else
        {
            team1Combo->setCurrentIndex(0);
            team2Combo->setCurrentIndex(0);
            goals1Input->setValue(0);
            goals2Input->setValue(0);
            card1Input->setValue(0);
            card2Input->setValue(0);
            stadiumInput->setText("Sân vận động Quốc gia");
            matchDateInput->setDateTime(QDateTime::currentDateTime());
            statusLabel->setText("✓ Đã reset trắng form nhập liệu!");
        }
    }
    void onSaveData()
    {
        int id1 = team1Combo->currentData().toInt();
        int id2 = team2Combo->currentData().toInt();
        if (isEditMode)
        {
            tranDau *match = db->getTranDauById(selectedMatchId);
            if (!match)
            {
                QMessageBox::warning(this, "Lỗi", "Không tìm thấy trận đấu cần cập nhật.");
                return;
            }
            if (QMessageBox::question(this, "Xác nhận cập nhật",
                                      "Bạn có chắc chắn muốn cập nhật biên bản trận đấu này?",
                                      QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
        }
        const tranDau match(0, id1, id2, goals1Input->value(), goals2Input->value(),
                            matchDateInput->dateTime().toSecsSinceEpoch(), card1Input->value(),
                            card2Input->value(), stadiumInput->text().toStdString());
        const auto result = logic.saveMatch(match, isEditMode, selectedMatchId);
        if (!result.succeeded()) {
            QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
            return;
        }
        statusLabel->setText(!result.persisted
                                 ? "Dữ liệu đã đổi trong bộ nhớ nhưng chưa ghi đầy đủ ra tệp."
                                 : (isEditMode ? "✓ Cập nhật trận đấu thành công!"
                                               : "✓ Ghi nhận biên bản trận đấu mới thành công!"));
        if (!result.persisted)
            QMessageBox::warning(this, "Lỗi lưu dữ liệu", QString::fromStdString(result.persistenceError));
        refreshTable();
        stackedWidget->setCurrentIndex(0); // Tự động thu hồi về layer danh sách
    }
    void onDeleteMatch()
    {
        if (selectedMatchId == -1)
            return;
        if (QMessageBox::question(this, "⚠️ Xác nhận xóa", "Bạn có chắc chắn muốn xóa vĩnh viễn trận đấu này?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            const auto result = logic.deleteMatch(selectedMatchId);
            if (!result.succeeded())
            {
                QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
                return;
            }
            if (!result.persisted)
                QMessageBox::warning(this, "Lỗi lưu dữ liệu", QString::fromStdString(result.persistenceError));
            refreshTable();
            statusLabel->setText(result.persisted ? "✓ Đã xóa biên bản trận đấu thành công!" :
                                 "Đã xóa trong bộ nhớ nhưng chưa ghi đầy đủ ra tệp.");
            stackedWidget->setCurrentIndex(0);
        }
    }
    void onBack()
    {
        stackedWidget->setCurrentIndex(0); // Trở về layer trước, vị trí cuộn grid và filter giữ nguyên vẹn
    }
    void onApplySearch()
    {
        const auto filter = logic.matchSearchFilter(
            searchTeamCombo->currentData().toInt(), searchMinGoalDiff->value(),
            searchMaxGoalDiff->value(), searchMinTotalGoals->value(),
            searchMaxTotalGoals->value(),
            static_cast<plb2::MatchDateMode>(searchDateModeCombo->currentData().toInt()),
            searchFromDateInput->date(), searchToDateInput->date());
        const auto matches = logic.findMatches(filter);
        renderMatchRows(matches);
        statusLabel->setText(QString("✓ Bộ lọc tìm thấy %1 trận thích hợp").arg(matches.size()));
    }
    void onClearSearch()
    {
        searchTeamCombo->setCurrentIndex(0);
        searchDateModeCombo->setCurrentIndex(0);
        searchFromDateInput->setDate(QDate::currentDate());
        searchToDateInput->setDate(QDate::currentDate());
        searchToDateInput->setEnabled(false);
        searchMinGoalDiff->setValue(-1);
        searchMaxGoalDiff->setValue(-1);
        searchMinTotalGoals->setValue(-1);
        searchMaxTotalGoals->setValue(-1);
        refreshTable();
        statusLabel->setText("✓ Đã reset bộ lọc về mặc định!");
    }

public:
    MatchWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), db(db), logic(*db)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        stackedWidget = new QStackedWidget(this);
        // Khởi tạo kiến trúc phân tầng 2 layer độc lập chạy song song
        initLayerList();
        initLayerDetail();
        stackedWidget->addWidget(layerList);   // Index 0
        stackedWidget->addWidget(layerDetail); // Index 1
        mainLayout->addWidget(stackedWidget);
        // Hệ thống liên kết tín hiệu (Signals & Slots)
        connect(matchTable, &QTableWidget::cellClicked, this, &MatchWidget::onTableClicked);
        connect(openAddFormBtn, &QPushButton::clicked, this, &MatchWidget::onOpenAddForm);
        connect(backBtn, &QPushButton::clicked, this, &MatchWidget::onBack);
        connect(resetBtn, &QPushButton::clicked, this, &MatchWidget::onResetForm);
        connect(saveBtn, &QPushButton::clicked, this, &MatchWidget::onSaveData);
        connect(deleteBtn, &QPushButton::clicked, this, &MatchWidget::onDeleteMatch);
        connect(searchBtn, &QPushButton::clicked, this, &MatchWidget::onApplySearch);
        connect(clearSearchBtn, &QPushButton::clicked, this, &MatchWidget::onClearSearch);
        enableThreeStateSorting(matchTable, [this]() { onApplySearch(); },
                                [this](const QString& status) { statusLabel->setText(status); });
        refreshTable();
    }
    void refreshTable()
    {
        std::vector<tranDau *> allMatches;
        for (int i = 0; i < db->getTranDauCount(); ++i)
        {
            tranDau *match = db->getTranDauAt(i);
            if (match)
                allMatches.push_back(match);
        }
        renderMatchRows(allMatches);
    }

    void refreshTeams()
    {
        const QVariant selectedSearchTeam = searchTeamCombo->currentData();
        const QVariant selectedTeam1 = team1Combo->currentData();
        const QVariant selectedTeam2 = team2Combo->currentData();
        searchTeamCombo->clear();
        team1Combo->clear();
        team2Combo->clear();
        searchTeamCombo->addItem("Tất cả các đội", -1);
        team1Combo->addItem("Chọn đội tuyển", -1);
        team2Combo->addItem("Chọn đội tuyển", -1);
        for (int i = 0; i < db->getDoiBongCount(); ++i)
        {
            doiBong *team = db->getDoiBongAt(i);
            if (!team) continue;
            const QString name = QString::fromStdString(team->getTen());
            const int id = static_cast<int>(team->getId());
            searchTeamCombo->addItem(name, id);
            team1Combo->addItem(name, id);
            team2Combo->addItem(name, id);
        }
        int index = searchTeamCombo->findData(selectedSearchTeam);
        if (index >= 0) searchTeamCombo->setCurrentIndex(index);
        index = team1Combo->findData(selectedTeam1);
        if (index >= 0) team1Combo->setCurrentIndex(index);
        index = team2Combo->findData(selectedTeam2);
        if (index >= 0) team2Combo->setCurrentIndex(index);
    }
};
#endif // MATCH_WIDGET_H