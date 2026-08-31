#pragma once
#ifndef TEAM_WIDGET_H
#define TEAM_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QStackedWidget>
#include <QHeaderView>
#include <QDialog>
#include <QDialogButtonBox>
#include "database.h"
#include "search_features.h"

class TeamWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    long long selectedTeamId = -1;
    bool isEditMode = false;

    QStackedWidget *stackedWidget;
    QWidget *layerList;
    QWidget *layerDetail;

    // Layer 1
    QTableWidget *teamTable;
    QLineEdit *searchNameInput, *searchCoachInput;
    QPushButton *searchBtn, *openAddFormBtn;
    QLabel *statusLabelList;

    // Layer 2 - Team Info
    QLineEdit *nameInput, *coachInput, *locationInput;
    QSpinBox *rankInput;
    QLabel *imagePlaceholder;
    QPushButton *saveBtn, *backBtn, *deleteBtn;
    
    // Layer 2 - Roster (Danh sách thành viên)
    QTableWidget *memberTable;
    QPushButton *addMemberBtn, *removeMemberBtn;

    void initLayerList() {
        layerList = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(layerList);

        QHBoxLayout *topLayout = new QHBoxLayout();
        openAddFormBtn = new QPushButton("➕ Đăng Ký Đội Bóng Mới");
        openAddFormBtn->setStyleSheet("background-color: #2E7D32; color: white; padding: 10px; font-weight: bold;");
        topLayout->addWidget(openAddFormBtn);
        topLayout->addStretch();
        layout->addLayout(topLayout);

        teamTable = new QTableWidget();
        teamTable->setColumnCount(5);
        teamTable->setHorizontalHeaderLabels({"ID", "Tên Câu Lạc Bộ", "Huấn Luyện Viên", "Bản Doanh", "Điểm XH"});
        teamTable->horizontalHeader()->setStretchLastSection(true);
        teamTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        teamTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        layout->addWidget(teamTable);

        statusLabelList = new QLabel("✓ Hệ thống quản lý CLB.");
        layout->addWidget(statusLabelList);
    }

    void initLayerDetail() {
        layerDetail = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(layerDetail);

        // Header
        QHBoxLayout *headerLayout = new QHBoxLayout();
        backBtn = new QPushButton("⬅ Trở về");
        backBtn->setStyleSheet("background-color: #757575; color: white; padding: 6px 12px; font-weight: bold; border-radius: 3px;");
        QLabel *title = new QLabel("HỒ SƠ ĐỘI BÓNG & DANH SÁCH THÀNH VIÊN");
        title->setStyleSheet("font-size: 15px; font-weight: bold; color: #8E24AA;");
        headerLayout->addWidget(backBtn);
        headerLayout->addStretch();
        headerLayout->addWidget(title);
        layout->addLayout(headerLayout);

        // Nửa trên: Thông tin Đội
        QHBoxLayout *topInfoLayout = new QHBoxLayout();
        QGroupBox *imageGroup = new QGroupBox("Logo");
        QVBoxLayout *imageLayout = new QVBoxLayout(imageGroup);
        imagePlaceholder = new QLabel("🛡️");
        imagePlaceholder->setFixedSize(120, 120);
        imagePlaceholder->setAlignment(Qt::AlignCenter);
        imagePlaceholder->setStyleSheet("background-color: #111; border: 2px solid #8E24AA; border-radius: 60px; font-size: 40px;");
        imageLayout->addWidget(imagePlaceholder);
        topInfoLayout->addWidget(imageGroup);

        QGroupBox *formGroup = new QGroupBox("Thông Tin Hành Chính");
        QGridLayout *formLayout = new QGridLayout(formGroup);
        formLayout->addWidget(new QLabel("Tên Đội:"), 0, 0);
        nameInput = new QLineEdit(); formLayout->addWidget(nameInput, 0, 1);
        formLayout->addWidget(new QLabel("HLV Trưởng:"), 0, 2);
        coachInput = new QLineEdit(); formLayout->addWidget(coachInput, 0, 3);
        formLayout->addWidget(new QLabel("Địa Phương:"), 1, 0);
        locationInput = new QLineEdit(); formLayout->addWidget(locationInput, 1, 1);
        formLayout->addWidget(new QLabel("Điểm Xếp Hạng:"), 1, 2);
        rankInput = new QSpinBox(); rankInput->setRange(0, 10000); formLayout->addWidget(rankInput, 1, 3);
        
        QHBoxLayout *teamControlLayout = new QHBoxLayout();
        saveBtn = new QPushButton("💾 LƯU THÔNG TIN ĐỘI");
        saveBtn->setStyleSheet("background-color: #1E88E5; color: white; padding: 8px; font-weight: bold;");
        deleteBtn = new QPushButton("🗑️ GIẢI TÁN ĐỘI");
        deleteBtn->setStyleSheet("background-color: #E53935; color: white; padding: 8px; font-weight: bold;");
        teamControlLayout->addWidget(saveBtn);
        teamControlLayout->addWidget(deleteBtn);
        teamControlLayout->addStretch();
        formLayout->addLayout(teamControlLayout, 2, 0, 1, 4);

        topInfoLayout->addWidget(formGroup, 1);
        layout->addLayout(topInfoLayout);

        // Nửa dưới: Bảng danh sách Cầu Thủ
        QGroupBox *rosterGroup = new QGroupBox("Danh Sách Cầu Thủ Thuộc Biên Chế (Click đúp để sửa)");
        rosterGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; margin-top: 10px; }");
        QVBoxLayout *rosterLayout = new QVBoxLayout(rosterGroup);
        
        memberTable = new QTableWidget();
        memberTable->setColumnCount(6);
        memberTable->setHorizontalHeaderLabels({"ID", "Tên Cầu Thủ", "Vị Trí", "Tuổi", "Số Áo", "Bàn Thắng"});
        memberTable->horizontalHeader()->setStretchLastSection(true);
        memberTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        memberTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Cấm sửa trực tiếp trên ô, bắt sự kiện double click
        
        rosterLayout->addWidget(memberTable);
        layout->addWidget(rosterGroup, 1);
    }

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
        teamTable->resizeColumnsToContents();
    }

    void loadMembersTable(long long teamId) {
        memberTable->setRowCount(0);
        // Quét toàn bộ cầu thủ xem ai thuộc team này (Nếu db có hàm getPlayersByTeamId thì dùng)
        for (int i = 1; i <= db->getCauThuCount(); ++i) {
            if (cauThu* ct = db->getCauThuById(i)) {
                if (ct->getMaDoiBong() == teamId) {
                    int row = memberTable->rowCount();
                    memberTable->insertRow(row);
                    memberTable->setItem(row, 0, new QTableWidgetItem(QString::number(ct->getId())));
                    memberTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(ct->getTen())));
                    memberTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(ct->getViTri())));
                    memberTable->setItem(row, 3, new QTableWidgetItem(QString::number(ct->getTuoi())));
                    memberTable->setItem(row, 4, new QTableWidgetItem(QString::number(ct->getSoAo())));
                    memberTable->setItem(row, 5, new QTableWidgetItem(QString::number(ct->getSoBanThang())));
                }
            }
        }
    }

    // Popup Dialog Edit Cầu thủ ngay trong TeamWidget
    void openPlayerEditDialog(long long playerId) {
        cauThu *ct = db->getCauThuById(playerId);
        if (!ct) return;

        QDialog dialog(this);
        dialog.setWindowTitle("Sửa Thông Tin Cầu Thủ");
        dialog.setMinimumWidth(400);
        QGridLayout *dl = new QGridLayout(&dialog);

        dl->addWidget(new QLabel("Tên:"), 0, 0);
        QLineEdit *dName = new QLineEdit(QString::fromStdString(ct->getTen())); dl->addWidget(dName, 0, 1);
        
        dl->addWidget(new QLabel("Vị Trí:"), 1, 0);
        QLineEdit *dPos = new QLineEdit(QString::fromStdString(ct->getViTri())); dl->addWidget(dPos, 1, 1);
        
        dl->addWidget(new QLabel("Số Áo:"), 2, 0);
        QSpinBox *dNum = new QSpinBox(); dNum->setRange(1, 99); dNum->setValue(ct->getSoAo()); dl->addWidget(dNum, 2, 1);
        
        dl->addWidget(new QLabel("Bàn Thắng:"), 3, 0);
        QSpinBox *dGoals = new QSpinBox(); dGoals->setRange(0, 1000); dGoals->setValue(ct->getSoBanThang()); dl->addWidget(dGoals, 3, 1);

        QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        dl->addWidget(btnBox, 4, 0, 1, 2);

        connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            cauThu updatedCt(playerId, dName->text().toStdString().c_str(), dPos->text().toStdString().c_str(), 
                             ct->getTuoi(), dNum->value(), ct->getMaDoiBong());
            updatedCt.setSoBanThang(dGoals->value());
            db->updateCauThu(playerId, updatedCt);
            loadMembersTable(selectedTeamId); // Refresh bảng con
            QMessageBox::information(this, "Thành công", "Đã cập nhật cầu thủ!");
        }
    }

public slots:
    void onTableClicked(int row, int column) {
        if (row < 0) return;
        selectedTeamId = teamTable->item(row, 0)->text().toLongLong();
        isEditMode = true; deleteBtn->setVisible(true);
        loadTeamToForm(selectedTeamId);
        stackedWidget->setCurrentIndex(1);
    }

    void onMemberDoubleClicked(int row, int column) {
        long long pId = memberTable->item(row, 0)->text().toLongLong();
        openPlayerEditDialog(pId);
    }

    void onOpenAddForm() {
        selectedTeamId = -1; isEditMode = false; deleteBtn->setVisible(false);
        clearInputs();
        memberTable->setRowCount(0); // Đội mới chưa có thành viên
        stackedWidget->setCurrentIndex(1);
    }

    void loadTeamToForm(long long id) {
        doiBong *team = db->getDoiBongById(id);
        if (!team) return;
        nameInput->setText(QString::fromStdString(team->getTen()));
        coachInput->setText(QString::fromStdString(team->getHLV()));
        locationInput->setText(QString::fromStdString(team->getDiaPhuong()));
        rankInput->setValue(team->getDiemXepHang());
        imagePlaceholder->setText(QString::fromStdString(team->getTen()).left(1).toUpper());
        
        loadMembersTable(id); // Tải danh sách thành viên
    }

    void onSaveData() {
        if (nameInput->text().isEmpty()) { QMessageBox::warning(this, "Lỗi", "Tên đội trống!"); return; }
        doiBong team((isEditMode ? selectedTeamId : db->getNextDoiBongId()), 
                     nameInput->text().toStdString().c_str(), coachInput->text().toStdString().c_str(),
                     locationInput->text().toStdString().c_str());
        team.setDiemXepHang(rankInput->value());

        if (isEditMode) db->updateDoiBong(selectedTeamId, team);
        else db->insert_doiBong_single(team);
        
        refreshTable();
        stackedWidget->setCurrentIndex(0);
    }

    void onDeleteTeam() {
        if (selectedTeamId == -1) return;
        if (QMessageBox::question(this, "Xác nhận", "Giải tán đội bóng này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            db->delete_doiBong_by_id(selectedTeamId);
            refreshTable(); stackedWidget->setCurrentIndex(0);
        }
    }

    void onBack() { stackedWidget->setCurrentIndex(0); }

public:
    TeamWidget(Database *db, QWidget *parent = nullptr) : QWidget(parent), db(db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        stackedWidget = new QStackedWidget(this);
        initLayerList(); initLayerDetail();
        stackedWidget->addWidget(layerList); stackedWidget->addWidget(layerDetail);
        mainLayout->addWidget(stackedWidget);

        connect(teamTable, &QTableWidget::cellClicked, this, &TeamWidget::onTableClicked);
        connect(memberTable, &QTableWidget::cellDoubleClicked, this, &TeamWidget::onMemberDoubleClicked);
        connect(openAddFormBtn, &QPushButton::clicked, this, &TeamWidget::onOpenAddForm);
        connect(backBtn, &QPushButton::clicked, this, &TeamWidget::onBack);
        connect(saveBtn, &QPushButton::clicked, this, &TeamWidget::onSaveData);
        connect(deleteBtn, &QPushButton::clicked, this, &TeamWidget::onDeleteTeam);
        
        refreshTable();
    }

    void refreshTable() {
        std::vector<doiBong*> allTeams;
        for (int i = 1; i <= db->getDoiBongCount(); ++i) {
            if (doiBong* team = db->getDoiBongById(i)) allTeams.push_back(team);
        }
        renderTeamRows(allTeams);
    }

    void clearInputs() {
        nameInput->clear(); coachInput->clear(); locationInput->clear(); rankInput->setValue(0);
        imagePlaceholder->setText("🛡️");
    }
};
#endif