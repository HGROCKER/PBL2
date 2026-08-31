#pragma once
#ifndef PLAYER_WIDGET_H
#define PLAYER_WIDGET_H

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
#include <QStackedWidget>
#include <QHeaderView>
#include "database.h"
#include "search_features.h"

class PlayerWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    long long selectedPlayerId = -1;
    bool isEditMode = false;

    QStackedWidget *stackedWidget;
    QWidget *layerList;
    QWidget *layerDetail;

    // Layer 1: List & Search
    QTableWidget *playerTable;
    QLineEdit *searchNameInput, *searchPositionInput;
    QComboBox *searchTeamCombo;
    QSpinBox *searchMinAge, *searchMaxAge, *searchMinNumber, *searchMaxNumber;
    QSpinBox *searchMinGoals, *searchMaxGoals;
    QPushButton *searchBtn, *clearSearchBtn, *openAddFormBtn;
    QLabel *statusLabelList;

    // Layer 2: Detail & Edit Form
    QLineEdit *nameInput, *positionInput, *nationalityInput;
    QSpinBox *ageInput, *numberInput, *goalsInput, *heightInput, *weightInput;
    QComboBox *teamCombo;
    QLabel *imagePlaceholder;
    QPushButton *saveBtn, *resetBtn, *backBtn, *deleteBtn;
    QLabel *detailTitleLabel;
    QLabel *statusLabelDetail;

    void initLayerList() {
        layerList = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(layerList);

        // [Giữ nguyên code bộ lọc tìm kiếm như bản trước]
        QGroupBox *searchGroup = new QGroupBox("🔎 Bộ Lọc Tìm Kiếm Cầu Thủ");
        searchGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #1976D2; padding-top: 10px; }");
        QGridLayout *searchLayout = new QGridLayout(searchGroup);

        searchLayout->addWidget(new QLabel("Tên:"), 0, 0);
        searchNameInput = new QLineEdit(); searchLayout->addWidget(searchNameInput, 0, 1);
        searchLayout->addWidget(new QLabel("Vị Trí:"), 0, 2);
        searchPositionInput = new QLineEdit(); searchLayout->addWidget(searchPositionInput, 0, 3);
        
        searchLayout->addWidget(new QLabel("Đội Bóng:"), 1, 0);
        searchTeamCombo = new QComboBox();
        searchTeamCombo->addItem("Tất cả", -1);
        searchLayout->addWidget(searchTeamCombo, 1, 1);

        searchLayout->addWidget(new QLabel("Tuổi từ:"), 1, 2);
        searchMinAge = new QSpinBox(); searchMinAge->setRange(0, 100); searchLayout->addWidget(searchMinAge, 1, 3);
        searchLayout->addWidget(new QLabel("đến:"), 2, 0);
        searchMaxAge = new QSpinBox(); searchMaxAge->setRange(0, 100); searchLayout->addWidget(searchMaxAge, 2, 1);

        QHBoxLayout *searchBtnLayout = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Lọc Dữ Liệu");
        clearSearchBtn = new QPushButton("❌ Xóa Bộ Lọc");
        searchBtnLayout->addWidget(searchBtn);
        searchBtnLayout->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchBtnLayout, 2, 2, 1, 2);
        layout->addWidget(searchGroup);

        QHBoxLayout *actionLayout = new QHBoxLayout();
        openAddFormBtn = new QPushButton("➕ Thêm Cầu Thủ Mới");
        openAddFormBtn->setStyleSheet("QPushButton { background-color: #2E7D32; color: white; padding: 10px; border-radius: 4px; font-weight: bold; }");
        actionLayout->addWidget(openAddFormBtn);
        actionLayout->addStretch();
        layout->addLayout(actionLayout);

        playerTable = new QTableWidget();
        playerTable->setColumnCount(8);
        playerTable->setHorizontalHeaderLabels({"ID", "Tên", "Vị Trí", "Tuổi", "Số Áo", "Bàn Thắng", "CLB", "Quốc Tịch"});
        playerTable->horizontalHeader()->setStretchLastSection(true);
        playerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        playerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        layout->addWidget(playerTable);

        statusLabelList = new QLabel("✓ Hệ thống sẵn sàng.");
        statusLabelList->setStyleSheet("color: #4CAF50; font-weight: bold;");
        layout->addWidget(statusLabelList);
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
        nationalityInput = new QLineEdit(); formLayout->addWidget(nationalityInput, 0, 3);

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
        heightInput = new QSpinBox(); heightInput->setRange(150, 220); heightInput->setValue(175); formLayout->addWidget(heightInput, 3, 1);
        formLayout->addWidget(new QLabel("Cân Nặng (kg):"), 3, 2);
        weightInput = new QSpinBox(); weightInput->setRange(50, 120); weightInput->setValue(70); formLayout->addWidget(weightInput, 3, 3);

        // Hàng 4
        formLayout->addWidget(new QLabel("Trực Thuộc CLB:"), 4, 0);
        teamCombo = new QComboBox();
        teamCombo->addItem("Cầu thủ tự do", -1);
        formLayout->addWidget(teamCombo, 4, 1, 1, 3); // Kéo dài qua 3 cột
        
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
            playerTable->setItem(row, 0, new QTableWidgetItem(QString::number(ct->getId())));
            playerTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(ct->getTen())));
            playerTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(ct->getViTri())));
            playerTable->setItem(row, 3, new QTableWidgetItem(QString::number(ct->getTuoi())));
            playerTable->setItem(row, 4, new QTableWidgetItem(QString::number(ct->getSoAo())));
            playerTable->setItem(row, 5, new QTableWidgetItem(QString::number(ct->getSoBanThang())));
            doiBong *team = db->getDoiBongById(ct->getMaDoiBong());
            playerTable->setItem(row, 6, new QTableWidgetItem(team ? QString::fromStdString(team->getTen()) : "Tự do"));
            
            // Giả định backend chưa có field quốc tịch, tạm set giá trị mặc định. Nếu có, thay thế bằng ct->getQuocTich()
            playerTable->setItem(row, 7, new QTableWidgetItem("Việt Nam"));
        }
        playerTable->resizeColumnsToContents();
    }

    void loadTeamsToCombo() {
        searchTeamCombo->clear(); searchTeamCombo->addItem("Tất cả", -1);
        teamCombo->clear(); teamCombo->addItem("Cầu thủ tự do", -1);
        for (int i = 1; i <= db->getDoiBongCount(); i++) {
            doiBong *team = db->getDoiBongById(i);
            if (team) {
                searchTeamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
                teamCombo->addItem(QString::fromStdString(team->getTen()), (int)team->getId());
            }
        }
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
        statusLabelDetail->clear(); clearInputs();
        stackedWidget->setCurrentIndex(1);
    }

    void loadPlayerToForm(long long id) {
        cauThu *ct = db->getCauThuById(id);
        if (!ct) return;
        nameInput->setText(QString::fromStdString(ct->getTen()));
        positionInput->setText(QString::fromStdString(ct->getViTri()));
        ageInput->setValue(ct->getTuoi());
        numberInput->setValue(ct->getSoAo());
        goalsInput->setValue(ct->getSoBanThang());
        int tIdx = teamCombo->findData(QVariant((int)ct->getMaDoiBong()));
        if (tIdx != -1) teamCombo->setCurrentIndex(tIdx);
        nationalityInput->setText("Việt Nam"); // Tùy chỉnh backend
        imagePlaceholder->setText(QString("👤\n%1").arg(QString::fromStdString(ct->getTen())));
    }

    void onResetForm() {
        if (isEditMode && selectedPlayerId != -1) loadPlayerToForm(selectedPlayerId);
        else clearInputs();
        statusLabelDetail->setText("✓ Đã khôi phục dữ liệu gốc!");
    }

    void onSaveData() {
        if (nameInput->text().isEmpty()) {
            QMessageBox::warning(this, "Lỗi", "Tên cầu thủ không được để trống!");
            return;
        }
        cauThu ct((isEditMode ? selectedPlayerId : db->getNextCauThuId()), 
                  nameInput->text().toStdString().c_str(), positionInput->text().toStdString().c_str(), 
                  ageInput->value(), numberInput->value(), teamCombo->currentData().toInt());
        ct.setSoBanThang(goalsInput->value());
        
        if (isEditMode) {
            db->updateCauThu(selectedPlayerId, ct);
            statusLabelList->setText("✓ Cập nhật hồ sơ thành công!");
        } else {
            db->insert_cauThu_single(ct);
            statusLabelList->setText("✓ Thêm cầu thủ mới thành công!");
        }
        refreshTable();
        stackedWidget->setCurrentIndex(0);
    }

    void onDeletePlayer() {
        if (selectedPlayerId == -1) return;
        if (QMessageBox::question(this, "Xác nhận", "Xóa vĩnh viễn cầu thủ này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            db->delete_cauThu_by_id(selectedPlayerId); refreshTable();
            statusLabelList->setText("✓ Đã xóa cầu thủ!"); stackedWidget->setCurrentIndex(0);
        }
    }

    void onBack() { stackedWidget->setCurrentIndex(0); }

public:
    PlayerWidget(Database *db, QWidget *parent = nullptr) : QWidget(parent), db(db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        stackedWidget = new QStackedWidget(this);
        initLayerList(); initLayerDetail();
        stackedWidget->addWidget(layerList); stackedWidget->addWidget(layerDetail);
        mainLayout->addWidget(stackedWidget);

        connect(playerTable, &QTableWidget::cellClicked, this, &PlayerWidget::onTableClicked);
        connect(openAddFormBtn, &QPushButton::clicked, this, &PlayerWidget::onOpenAddForm);
        connect(backBtn, &QPushButton::clicked, this, &PlayerWidget::onBack);
        connect(resetBtn, &QPushButton::clicked, this, &PlayerWidget::onResetForm);
        connect(saveBtn, &QPushButton::clicked, this, &PlayerWidget::onSaveData);
        connect(deleteBtn, &QPushButton::clicked, this, &PlayerWidget::onDeletePlayer);
        
        loadTeamsToCombo(); refreshTable();
    }

    void refreshTable() {
        std::vector<cauThu*> allPlayers;
        for (int i = 1; i <= db->getCauThuCount(); ++i) {
            if (cauThu* ct = db->getCauThuById(i)) allPlayers.push_back(ct);
        }
        renderPlayerRows(allPlayers);
    }

    void clearInputs() {
        nameInput->clear(); positionInput->clear(); nationalityInput->setText("Việt Nam");
        ageInput->setValue(20); numberInput->setValue(1); goalsInput->setValue(0);
        heightInput->setValue(175); weightInput->setValue(70);
        teamCombo->setCurrentIndex(0);
        imagePlaceholder->setText("👤\n3 x 4\nTrống");
    }
};
#endif