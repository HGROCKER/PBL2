#pragma once
#ifndef PLAYER_WIDGET_H
#define PLAYER_WIDGET_H

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
#include <QStackedWidget>
#include <QHeaderView>
#include <QFileDialog>
#include <QPixmap>
#include <QDateEdit>
#include <QCheckBox>
#include <QIntValidator>
#include "database.h"
#include "core/application_logic.h"
#include "search_features.h"
#include "table_helpers.h"
#include "ui/logic_messages.h"

class PlayerWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    plb2::ApplicationLogic logic;
    long long selectedPlayerId = -1;
    bool isEditMode = false;

    QStackedWidget *stackedWidget;
    QWidget *layerList;
    QWidget *layerDetail;

    // Layer 1: List & Search
    QTableWidget *playerTable;
    QLineEdit *searchIdInput, *searchNameInput, *searchPositionInput, *searchDocumentNumberInput;
    QComboBox *searchTeamCombo;
    QSpinBox *searchMinAge, *searchMaxAge, *searchMinNumber, *searchMaxNumber;
    QSpinBox *searchMinGoals, *searchMaxGoals;
    QSpinBox *searchMinBirthYear, *searchMaxBirthYear;
    QPushButton *searchBtn, *clearSearchBtn, *openAddFormBtn;
    QPushButton *statisticsBtn;
    QLabel *statusLabelList;

    // Layer 2: Detail & Edit Form
    QLineEdit *nameInput, *positionInput, *nationalityInput, *documentNumberInput;
    QLineEdit *imagePathInput;
    QLineEdit *phoneInput;
    QDateEdit *birthDateInput;
    QCheckBox *birthDateUnknownInput;
    QSpinBox *appearancesInput;
    QSpinBox *ageInput, *numberInput, *goalsInput, *heightInput, *weightInput;
    QComboBox *teamCombo;
    QLabel *imagePlaceholder;
    QPushButton *saveBtn, *resetBtn, *backBtn, *deleteBtn;
    QLabel *detailTitleLabel;
    QLabel *statusLabelDetail;

    void initLayerList() {
        layerList = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(layerList);
        QVBoxLayout *tableLayout = new QVBoxLayout();
        QVBoxLayout *sideLayout = new QVBoxLayout();
        QGroupBox *searchGroup = new QGroupBox("🔎 Bộ Lọc Tìm Kiếm Cầu Thủ");
        searchGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #1976D2; padding-top: 10px; }");
        QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
        QHBoxLayout *searchBtnLayout = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Tìm / Lọc");
        clearSearchBtn = new QPushButton("Xóa bộ lọc");
        searchBtnLayout->addWidget(searchBtn);
        searchBtnLayout->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchBtnLayout);
        QFormLayout *filterForm = new QFormLayout();
        searchIdInput = new QLineEdit();
        searchIdInput->setPlaceholderText("Nhập ID chính xác");
        filterForm->addRow("ID:", searchIdInput);
        searchNameInput = new QLineEdit();
        filterForm->addRow("Tên:", searchNameInput);
        searchPositionInput = new QLineEdit();
        filterForm->addRow("Vị trí:", searchPositionInput);
        searchDocumentNumberInput = new QLineEdit();
        filterForm->addRow("CCCD/Hộ chiếu:", searchDocumentNumberInput);
        searchTeamCombo = new QComboBox();
        searchTeamCombo->addItem("Tất cả", -1);
        filterForm->addRow("Đội bóng:", searchTeamCombo);
        auto addRange = [&](const QString& label, QSpinBox*& minimum, QSpinBox*& maximum,
                            int maxValue) {
            minimum = new QSpinBox();
            maximum = new QSpinBox();
            minimum->setRange(-1, maxValue);
            maximum->setRange(-1, maxValue);
            minimum->setSpecialValueText("Bất kỳ");
            maximum->setSpecialValueText("Bất kỳ");
            minimum->setValue(-1);
            maximum->setValue(-1);
            QHBoxLayout *rangeLayout = new QHBoxLayout();
            rangeLayout->addWidget(minimum);
            rangeLayout->addWidget(new QLabel("đến"));
            rangeLayout->addWidget(maximum);
            filterForm->addRow(label, rangeLayout);
        };
        addRange("Tuổi:", searchMinAge, searchMaxAge, 100);
        addRange("Số áo:", searchMinNumber, searchMaxNumber, 99);
        addRange("Bàn thắng:", searchMinGoals, searchMaxGoals, 1000);
        addRange("Năm sinh:", searchMinBirthYear, searchMaxBirthYear, QDate::currentDate().year());
        searchLayout->addLayout(filterForm);
        sideLayout->addWidget(searchGroup);
        openAddFormBtn = new QPushButton("➕ Thêm Cầu Thủ Mới");
        openAddFormBtn->setStyleSheet("QPushButton { background-color: #2E7D32; color: white; padding: 10px; border-radius: 4px; font-weight: bold; }");
        sideLayout->addWidget(openAddFormBtn);
        statisticsBtn = new QPushButton("Thống kê cầu thủ");
        sideLayout->addWidget(statisticsBtn);
        sideLayout->addStretch();

        playerTable = new QTableWidget();
        playerTable->setColumnCount(14);
        playerTable->setHorizontalHeaderLabels({"ID", "Tên", "Vị Trí", "Tuổi", "Ngày sinh", "Số áo",
                                                "Bàn thắng", "Đội bóng", "Quốc tịch", "Số trận", "Điện thoại",
                                                "Cao (cm)", "Nặng (kg)", "CMND/Hộ chiếu"});
        playerTable->horizontalHeader()->setStretchLastSection(true);
        playerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        playerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        playerTable->setMinimumHeight(400);
        tableLayout->addWidget(playerTable, 1);

        statusLabelList = new QLabel("✓ Hệ thống sẵn sàng.");
        statusLabelList->setStyleSheet("color: #4CAF50; font-weight: bold;");
        tableLayout->addWidget(statusLabelList);
        layout->addLayout(tableLayout, 1);
        layout->addLayout(sideLayout);
    }

    void initLayerDetail() {
        layerDetail = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(layerDetail);

        QHBoxLayout *headerLayout = new QHBoxLayout();
        backBtn = new QPushButton("⬅ Trở về Danh sách");
        backBtn->setStyleSheet("background-color: #757575; color: white; padding: 6px 12px; font-weight: bold; border-radius: 3px;");
        detailTitleLabel = new QLabel("HỒ SƠ CẦU THỦ CHI TIẾT");
        detailTitleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #1976D2; text-transform: uppercase;");
        headerLayout->addWidget(backBtn);
        headerLayout->addStretch();
        headerLayout->addWidget(detailTitleLabel);
        layout->addLayout(headerLayout);

        QHBoxLayout *workspaceLayout = new QHBoxLayout();
        
        QGroupBox *imageGroup = new QGroupBox("Ảnh Đại Diện");
        QVBoxLayout *imageLayout = new QVBoxLayout(imageGroup);
        imagePlaceholder = new QLabel("👤\n3 x 4");
        imagePlaceholder->setFixedSize(200, 260);
        imagePlaceholder->setAlignment(Qt::AlignCenter);
        imagePlaceholder->setStyleSheet("background-color: #f5f5f5; color: #888; border: 2px dashed #1976D2; border-radius: 8px; font-size: 24px; font-weight: bold;");
        imageLayout->addWidget(imagePlaceholder);
        imageLayout->addStretch();
        workspaceLayout->addWidget(imageGroup);

        QGroupBox *formGroup = new QGroupBox("Thông Tin Cá Nhân & Chuyên Môn");
        formGroup->setStyleSheet("QGroupBox { font-size: 13px; font-weight: bold; }");
        QGridLayout *formLayout = new QGridLayout(formGroup);
        formLayout->setSpacing(15);

        // Hàng 0
        formLayout->addWidget(new QLabel("Tên Cầu Thủ:"), 0, 0);
        nameInput = new QLineEdit(); formLayout->addWidget(nameInput, 0, 1);
        formLayout->addWidget(new QLabel("Quốc Tịch:"), 0, 2);
        nationalityInput = new QLineEdit();
        nationalityInput->setMaxLength(3);
        nationalityInput->setPlaceholderText("Mã quốc gia");
        formLayout->addWidget(nationalityInput, 0, 3);

        // Hàng 1
        formLayout->addWidget(new QLabel("Vị Trí Thi Đấu:"), 1, 0);
        positionInput = new QLineEdit(); formLayout->addWidget(positionInput, 1, 1);
        formLayout->addWidget(new QLabel("Số Áo:"), 1, 2);
        numberInput = new QSpinBox(); numberInput->setRange(1, 99); formLayout->addWidget(numberInput, 1, 3);

        // Hàng 2
        formLayout->addWidget(new QLabel("Tuổi:"), 2, 0);
        ageInput = new QSpinBox(); ageInput->setRange(15, 50); formLayout->addWidget(ageInput, 2, 1);
        formLayout->addWidget(new QLabel("Bàn Thắng:"), 2, 2);
        goalsInput = new QSpinBox(); goalsInput->setRange(0, 1000); formLayout->addWidget(goalsInput, 2, 3);

        // Hàng 3 (Các trường mở rộng - Có thể map với backend nếu cần)
        formLayout->addWidget(new QLabel("Chiều Cao (cm):"), 3, 0);
        heightInput = new QSpinBox(); heightInput->setRange(0, 250); heightInput->setSpecialValueText("Chưa rõ"); heightInput->setValue(175); formLayout->addWidget(heightInput, 3, 1);
        formLayout->addWidget(new QLabel("Cân Nặng (kg):"), 3, 2);
        weightInput = new QSpinBox(); weightInput->setRange(0, 250); weightInput->setSpecialValueText("Chưa rõ"); weightInput->setValue(70); formLayout->addWidget(weightInput, 3, 3);
        formLayout->addWidget(new QLabel("Ngày sinh:"), 4, 0);
        birthDateInput = new QDateEdit();
        birthDateInput->setCalendarPopup(true);
        birthDateInput->setDisplayFormat("dd/MM/yyyy");
        birthDateInput->setDate(QDate::currentDate().addYears(-20));
        formLayout->addWidget(birthDateInput, 4, 1);
        birthDateUnknownInput = new QCheckBox("Chưa rõ");
        formLayout->addWidget(birthDateUnknownInput, 4, 2);
        formLayout->addWidget(new QLabel("Số trận:"), 4, 3);
        appearancesInput = new QSpinBox();
        appearancesInput->setRange(0, 10000);
        formLayout->addWidget(appearancesInput, 4, 4);
        formLayout->addWidget(new QLabel("Điện thoại:"), 5, 0);
        phoneInput = new QLineEdit();
        phoneInput->setValidator(new QIntValidator(0, 2147483647, phoneInput));
        formLayout->addWidget(phoneInput, 5, 1);
        formLayout->addWidget(new QLabel("CMND/Hộ chiếu:"), 5, 2);
        documentNumberInput = new QLineEdit();
        documentNumberInput->setMaxLength(40);
        documentNumberInput->setPlaceholderText("Số giấy tờ định danh");
        formLayout->addWidget(documentNumberInput, 5, 3, 1, 2);

        // Hàng 6
        formLayout->addWidget(new QLabel("Trực Thuộc CLB:"), 6, 0);
        teamCombo = new QComboBox();
        teamCombo->addItem("Cầu thủ tự do", -1);
        formLayout->addWidget(teamCombo, 6, 1, 1, 3); // Kéo dài qua 3 cột
        formLayout->addWidget(new QLabel("Đường dẫn ảnh:"), 7, 0);
        imagePathInput = new QLineEdit();
        imagePathInput->setPlaceholderText("Đường dẫn tệp ảnh (để trống nếu không có)");
        formLayout->addWidget(imagePathInput, 7, 1, 1, 2);
        QPushButton *browseImageBtn = new QPushButton("Chọn ảnh...");
        formLayout->addWidget(browseImageBtn, 7, 3);
        connect(browseImageBtn, &QPushButton::clicked, this, &PlayerWidget::onBrowseImage);
        connect(imagePathInput, &QLineEdit::textChanged, this, [this]() {
            updateImagePreview(QString("👤\n%1").arg(nameInput->text()));
        });
        connect(nameInput, &QLineEdit::textChanged, this, [this]() {
            updateImagePreview(QString("👤\n%1").arg(nameInput->text()));
        });
        connect(birthDateUnknownInput, &QCheckBox::toggled, birthDateInput, &QDateEdit::setDisabled);
        connect(birthDateInput, &QDateEdit::dateChanged, this, [this](const QDate& date) {
            const QDate today = QDate::currentDate();
            ageInput->setValue(plb2::calculateAge(date.year(), date.month(), date.day(),
                                                  today.year(), today.month(), today.day()));
        });
        
        workspaceLayout->addWidget(formGroup, 1);
        layout->addLayout(workspaceLayout);

        QHBoxLayout *controlLayout = new QHBoxLayout();
        saveBtn = new QPushButton("💾 LƯU HỒ SƠ");
        saveBtn->setStyleSheet("background-color: #1E88E5; color: white; padding: 12px 24px; font-weight: bold; border-radius: 4px; font-size: 13px;");
        resetBtn = new QPushButton("🔄 TẢI LẠI GỐC");
        resetBtn->setStyleSheet("background-color: #FFB300; color: black; padding: 12px 24px; font-weight: bold; border-radius: 4px; font-size: 13px;");
        deleteBtn = new QPushButton("🗑️ XÓA CẦU THỦ");
        deleteBtn->setStyleSheet("background-color: #E53935; color: white; padding: 12px 24px; font-weight: bold; border-radius: 4px; font-size: 13px;");
        
        controlLayout->addWidget(saveBtn);
        controlLayout->addWidget(resetBtn);
        controlLayout->addWidget(deleteBtn);
        controlLayout->addStretch();
        layout->addLayout(controlLayout);

        statusLabelDetail = new QLabel("");
        layout->addWidget(statusLabelDetail);
    }

    void renderPlayerRows(const std::vector<cauThu*>& players) {
        playerTable->setRowCount(0);
        for (cauThu* ct : players) {
            if (!ct) continue;
            int row = playerTable->rowCount();
            playerTable->insertRow(row);
            playerTable->setItem(row, 0, sortableItem(QString::number(ct->getId()), QVariant::fromValue<qlonglong>(ct->getId())));
            playerTable->setItem(row, 1, sortableItem(QString::fromStdString(ct->getTen())));
            playerTable->setItem(row, 2, sortableItem(QString::fromStdString(ct->getViTri())));
            const int age = plb2::playerAgeAt(*ct);
            playerTable->setItem(row, 3, sortableItem(age >= 0 ? QString::number(age) : "—", age));
            const QDate birthDate = ct->getNgaySinh() > 0
                                        ? QDateTime::fromSecsSinceEpoch(ct->getNgaySinh()).date()
                                        : QDate();
            playerTable->setItem(row, 4, sortableItem(birthDate.isValid() ? birthDate.toString("dd/MM/yyyy") : "—",
                                                       ct->getNgaySinh()));
            playerTable->setItem(row, 5, sortableItem(QString::number(ct->getSoAo()), ct->getSoAo()));
            playerTable->setItem(row, 6, sortableItem(QString::number(ct->getSoBanThang()), ct->getSoBanThang()));
            doiBong *team = db->getDoiBongById(ct->getMaDoiBong());
            playerTable->setItem(row, 7, sortableItem(team ? QString::fromStdString(team->getTen()) : "Tự do"));
            playerTable->setItem(row, 8, sortableItem(ct->getMaQuocGia().empty() ? "-" :
                                                       QString::fromStdString(ct->getMaQuocGia())));
            playerTable->setItem(row, 9, sortableItem(QString::number(ct->getSoTranChoi()), ct->getSoTranChoi()));
            playerTable->setItem(row, 10, sortableItem(QString::number(ct->getSdt()), ct->getSdt()));
            playerTable->setItem(row, 11, sortableItem(QString::number(ct->getChieuCao()), ct->getChieuCao()));
            playerTable->setItem(row, 12, sortableItem(QString::number(ct->getCanNang()), ct->getCanNang()));
            playerTable->setItem(row, 13, sortableItem(QString::fromStdString(ct->getSoGiayTo())));
        }
        const int columnWidths[] = {48, 115, 70, 45, 90, 45, 62, 115, 55, 58, 78, 55, 55, 125};
        for (int column = 0; column < playerTable->columnCount(); ++column)
            playerTable->setColumnWidth(column, columnWidths[column]);
    }

    void updateImagePreview(const QString& fallbackText) {
        const QPixmap image(imagePathInput->text().trimmed());
        if (!image.isNull()) {
            imagePlaceholder->setText(QString());
            imagePlaceholder->setPixmap(image.scaled(imagePlaceholder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imagePlaceholder->setPixmap(QPixmap());
            imagePlaceholder->setText(fallbackText);
        }
    }

    void applySearch() {
        long long playerId = -1;
        const QString idText = searchIdInput->text().trimmed();
        if (!idText.isEmpty()) {
            bool validId = false;
            playerId = idText.toLongLong(&validId);
            if (!validId || playerId <= 0) {
                QMessageBox::warning(this, "ID không hợp lệ",
                                     "ID cầu thủ phải là số nguyên dương.");
                return;
            }
        }

        const auto validRange = [](const QSpinBox* minimum, const QSpinBox* maximum) {
            return plb2::isValidRange(minimum->value(), maximum->value());
        };
        if (!validRange(searchMinAge, searchMaxAge) ||
            !validRange(searchMinNumber, searchMaxNumber) ||
            !validRange(searchMinGoals, searchMaxGoals) ||
            !validRange(searchMinBirthYear, searchMaxBirthYear)) {
            QMessageBox::warning(this, "Khoảng lọc không hợp lệ",
                                 "Giá trị Từ không được lớn hơn giá trị Đến.");
            return;
        }

        plb2::PlayerSearchFilter filter;
        filter.playerId = playerId;
        filter.name = searchNameInput->text();
        filter.position = searchPositionInput->text();
        filter.documentNumber = searchDocumentNumberInput->text();
        filter.teamId = searchTeamCombo->currentData().toInt();
        filter.minAge = searchMinAge->value();
        filter.maxAge = searchMaxAge->value();
        filter.minNumber = searchMinNumber->value();
        filter.maxNumber = searchMaxNumber->value();
        filter.minGoals = searchMinGoals->value();
        filter.maxGoals = searchMaxGoals->value();
        filter.minBirthYear = searchMinBirthYear->value();
        filter.maxBirthYear = searchMaxBirthYear->value();

        const auto players = plb2::filterPlayers(*db, filter);
        renderPlayerRows(players);
        statusLabelList->setText(QString("Tìm thấy %1 cầu thủ").arg(players.size()));
    }

    void clearSearch() {
        searchIdInput->clear();
        searchNameInput->clear();
        searchPositionInput->clear();
        searchDocumentNumberInput->clear();
        searchTeamCombo->setCurrentIndex(0);
        searchMinAge->setValue(-1);
        searchMaxAge->setValue(-1);
        searchMinNumber->setValue(-1);
        searchMaxNumber->setValue(-1);
        searchMinGoals->setValue(-1);
        searchMaxGoals->setValue(-1);
        searchMinBirthYear->setValue(-1);
        searchMaxBirthYear->setValue(-1);
        refreshTable();
        statusLabelList->setText("Đã xóa bộ lọc.");
    }

    void loadTeamsToCombo() {
        const QVariant selectedSearchTeam = searchTeamCombo->currentData();
        const QVariant selectedTeam = teamCombo->currentData();
        searchTeamCombo->clear(); searchTeamCombo->addItem("Tất cả", -1);
        teamCombo->clear(); teamCombo->addItem("Cầu thủ tự do", -1);
        for (int i = 0; i < db->getDoiBongCount(); ++i) {
            doiBong *team = db->getDoiBongAt(i);
            if (team) {
                searchTeamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
                teamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
            }
        }
        int searchIndex = searchTeamCombo->findData(selectedSearchTeam);
        if (searchIndex >= 0) searchTeamCombo->setCurrentIndex(searchIndex);
        int teamIndex = teamCombo->findData(selectedTeam);
        if (teamIndex >= 0) teamCombo->setCurrentIndex(teamIndex);
    }

public slots:
    void onTableClicked(int row, int column) {
        if (row < 0) return;
        selectedPlayerId = playerTable->item(row, 0)->text().toLongLong();
        isEditMode = true; deleteBtn->setVisible(true);
        detailTitleLabel->setText("✏️ CẬP NHẬT HỒ SƠ: " + playerTable->item(row, 1)->text().toUpper());
        statusLabelDetail->clear();
        loadPlayerToForm(selectedPlayerId);
        stackedWidget->setCurrentIndex(1);
    }

    void onOpenAddForm() {
        selectedPlayerId = -1; isEditMode = false; deleteBtn->setVisible(false);
        detailTitleLabel->setText("➕ THÊM HỒ SƠ CẦU THỦ MỚI");
        statusLabelDetail->clear(); clearInputs(); loadTeamsToCombo();
        stackedWidget->setCurrentIndex(1);
    }

    void onBrowseImage() {
        const QString path = QFileDialog::getOpenFileName(
            this, "Chọn ảnh cầu thủ", QString(), "Tệp ảnh (*.png *.jpg *.jpeg *.bmp *.gif)");
        if (!path.isEmpty()) imagePathInput->setText(path);
    }

    void loadPlayerToForm(long long id) {
        cauThu *ct = db->getCauThuById(id);
        if (!ct) return;
        nameInput->setText(QString::fromStdString(ct->getTen()));
        positionInput->setText(QString::fromStdString(ct->getViTri()));
        numberInput->setValue(ct->getSoAo());
        goalsInput->setValue(ct->getSoBanThang());
        int tIdx = teamCombo->findData(QVariant((int)ct->getMaDoiBong()));
        if (tIdx != -1) teamCombo->setCurrentIndex(tIdx);
        nationalityInput->setText(QString::fromStdString(ct->getMaQuocGia()));
        documentNumberInput->setText(QString::fromStdString(ct->getSoGiayTo()));
        imagePathInput->setText(QString::fromStdString(ct->getImagePath()));
        appearancesInput->setValue(ct->getSoTranChoi());
        phoneInput->setText(ct->getSdt() > 0 ? QString::number(ct->getSdt()) : QString());
        heightInput->setValue(ct->getChieuCao());
        weightInput->setValue(ct->getCanNang());
        const bool unknownBirthDate = ct->getNgaySinh() <= 0;
        birthDateUnknownInput->setChecked(unknownBirthDate);
        birthDateInput->setDate(unknownBirthDate
                                    ? QDate::currentDate().addYears(-ct->getTuoi())
                                    : QDateTime::fromSecsSinceEpoch(ct->getNgaySinh()).date());
        const int age = plb2::playerAgeAt(*ct);
        if (age >= 0) ageInput->setValue(age);
        updateImagePreview(QString("👤\n%1").arg(QString::fromStdString(ct->getTen())));
    }

    void onResetForm() {
        if (isEditMode && selectedPlayerId != -1) loadPlayerToForm(selectedPlayerId);
        else clearInputs();
        statusLabelDetail->setText("✓ Đã khôi phục dữ liệu gốc!");
    }

    void onSaveData() {
        cauThu ct;
        if (isEditMode) {
            cauThu *existing = db->getCauThuById(selectedPlayerId);
            if (!existing) {
                QMessageBox::warning(this, "Lỗi", "Không tìm thấy cầu thủ cần cập nhật.");
                return;
            }
            if (QMessageBox::question(this, "Xác nhận cập nhật",
                                      "Bạn có chắc chắn muốn cập nhật hồ sơ cầu thủ này?",
                                      QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
            ct = *existing;
            ct.setTen(nameInput->text().toStdString().c_str());
            ct.setViTri(positionInput->text().toStdString().c_str());
            ct.setTuoi(ageInput->value());
            ct.setSoAo(numberInput->value());
            ct.setSoBanThang(goalsInput->value());
            ct.setMaDoiBong(teamCombo->currentData().toInt());
            ct.setMaQuocGia(nationalityInput->text().trimmed().toUpper().toStdString().c_str());
            ct.setSoGiayTo(documentNumberInput->text().trimmed().toStdString());
            ct.setImagePath(imagePathInput->text().trimmed().toStdString());
            ct.setSoTranChoi(appearancesInput->value());
            ct.setSdt(phoneInput->text().toInt());
            ct.setChieuCao(heightInput->value());
            ct.setCanNang(weightInput->value());
            ct.setNgaySinh(birthDateUnknownInput->isChecked() ? 0 :
                           QDateTime(birthDateInput->date(), QTime(0, 0)).toSecsSinceEpoch());
        } else {
            ct = cauThu(0,
                        nameInput->text().toStdString().c_str(),
                        positionInput->text().toStdString().c_str(),
                        ageInput->value(), numberInput->value(),
                        teamCombo->currentData().toInt());
            ct.setSoBanThang(goalsInput->value());
            ct.setMaQuocGia(nationalityInput->text().trimmed().toUpper().toStdString().c_str());
            ct.setSoGiayTo(documentNumberInput->text().trimmed().toStdString());
            ct.setImagePath(imagePathInput->text().trimmed().toStdString());
            ct.setSoTranChoi(appearancesInput->value());
            ct.setSdt(phoneInput->text().toInt());
            ct.setChieuCao(heightInput->value());
            ct.setCanNang(weightInput->value());
            ct.setNgaySinh(birthDateUnknownInput->isChecked() ? 0 :
                           QDateTime(birthDateInput->date(), QTime(0, 0)).toSecsSinceEpoch());
        }
        const auto result = logic.savePlayer(ct, isEditMode, selectedPlayerId);
        if (!result.succeeded()) {
            QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
            return;
        }
        if (!result.logged)
            QMessageBox::warning(this, "Lỗi ghi nhật ký",
                                 "Dữ liệu đã đổi trong bộ nhớ nhưng không ghi được nhật ký: " +
                                     QString::fromStdString(result.logError));
        statusLabelList->setText(isEditMode ? "Đã cập nhật trong bộ nhớ. Chọn File > Lưu tệp dữ liệu để lưu bền vững." :
                                               "Đã thêm trong bộ nhớ. Chọn File > Lưu tệp dữ liệu để lưu bền vững.");
        refreshTable();
        stackedWidget->setCurrentIndex(0);
    }

    void onDeletePlayer() {
        if (selectedPlayerId == -1) return;
        if (QMessageBox::question(this, "Xác nhận", "Xóa vĩnh viễn cầu thủ này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            const auto result = logic.deletePlayer(selectedPlayerId);
            if (!result.succeeded()) {
                QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
                return;
            }
            if (!result.logged)
                QMessageBox::warning(this, "Lỗi ghi nhật ký",
                                     "Dữ liệu đã đổi trong bộ nhớ nhưng không ghi được nhật ký: " +
                                         QString::fromStdString(result.logError));
            refreshTable();
            statusLabelList->setText("Đã xóa trong bộ nhớ. Chọn File > Lưu tệp dữ liệu để lưu bền vững.");
            stackedWidget->setCurrentIndex(0);
        }
    }

    void onBack() { stackedWidget->setCurrentIndex(0); }

public:
    PlayerWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), db(db), logic(*db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        stackedWidget = new QStackedWidget(this);
        initLayerList(); initLayerDetail();
        stackedWidget->addWidget(layerList); stackedWidget->addWidget(layerDetail);
        mainLayout->addWidget(stackedWidget);

        connect(playerTable, &QTableWidget::cellClicked, this, &PlayerWidget::onTableClicked);
        connect(searchBtn, &QPushButton::clicked, this, &PlayerWidget::applySearch);
        connect(clearSearchBtn, &QPushButton::clicked, this, &PlayerWidget::clearSearch);
        connect(searchNameInput, &QLineEdit::returnPressed, this, &PlayerWidget::applySearch);
        connect(searchPositionInput, &QLineEdit::returnPressed, this, &PlayerWidget::applySearch);
        connect(searchIdInput, &QLineEdit::returnPressed, this, &PlayerWidget::applySearch);
        connect(searchDocumentNumberInput, &QLineEdit::returnPressed, this, &PlayerWidget::applySearch);
        connect(openAddFormBtn, &QPushButton::clicked, this, &PlayerWidget::onOpenAddForm);
        connect(backBtn, &QPushButton::clicked, this, &PlayerWidget::onBack);
        connect(resetBtn, &QPushButton::clicked, this, &PlayerWidget::onResetForm);
        connect(saveBtn, &QPushButton::clicked, this, &PlayerWidget::onSaveData);
        connect(deleteBtn, &QPushButton::clicked, this, &PlayerWidget::onDeletePlayer);
        connect(statisticsBtn, &QPushButton::clicked, this, &PlayerWidget::showLeagueStatistics);
        enableThreeStateSorting(playerTable, [this]() { applySearch(); },
                                [this](const QString& status) { statusLabelList->setText(status); });
        
        loadTeamsToCombo(); refreshTable();
    }

    void refreshTable() {
        std::vector<cauThu*> allPlayers;
        for (int i = 0; i < db->getCauThuCount(); ++i)
            if (cauThu* ct = db->getCauThuAt(i)) allPlayers.push_back(ct);
        renderPlayerRows(allPlayers);
    }

    void refreshTeamChoices() { loadTeamsToCombo(); }

    void clearInputs() {
        nameInput->clear(); positionInput->clear(); nationalityInput->setText("VIE");
        documentNumberInput->clear();
        ageInput->setValue(20); numberInput->setValue(1); goalsInput->setValue(0);
        heightInput->setValue(175); weightInput->setValue(70);
        appearancesInput->setValue(0); phoneInput->clear();
        birthDateUnknownInput->setChecked(false);
        birthDateInput->setDate(QDate::currentDate().addYears(-20));
        teamCombo->setCurrentIndex(0);
        imagePathInput->clear();
        imagePlaceholder->setText("👤\n3 x 4\nTrống");
    }

    void showLeagueStatistics() {
        const auto statistics = logic.playerStatistics();
        const cauThu *tallest = statistics.tallest;
        const cauThu *shortest = statistics.shortest;
        const cauThu *youngest = statistics.youngest;
        const auto describe = [](const cauThu* player, const QString& detail) {
            return player ? QString("%1 — %2").arg(QString::fromStdString(player->getTen()), detail)
                          : QString("Chưa có dữ liệu");
        };
        QMessageBox::information(this, "Thống kê cầu thủ",
            QString("Cao nhất: %1\nThấp nhất: %2\nTrẻ nhất: %3")
                .arg(describe(tallest, tallest ? QString("%1 cm").arg(tallest->getChieuCao()) : QString()),
                     describe(shortest, shortest ? QString("%1 cm").arg(shortest->getChieuCao()) : QString()),
                     describe(youngest, youngest ? QDateTime::fromSecsSinceEpoch(youngest->getNgaySinh())
                                                        .date().toString("dd/MM/yyyy") : QString())));
    }
};
#endif