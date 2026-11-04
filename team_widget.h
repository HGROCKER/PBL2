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
#include <QFormLayout>
#include <QFileDialog>
#include <QPixmap>
#include <QCheckBox>
#include <QDateEdit>
#include <QDate>
#include <QDateTime>
#include <QFormLayout>
#include "database.h"
#include "core/application_logic.h"
#include "search_features.h"
#include "table_helpers.h"
#include "ui/logic_messages.h"

class TeamWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    plb2::ApplicationLogic logic;
    long long selectedTeamId = -1;
    bool isEditMode = false;

    QStackedWidget *stackedWidget;
    QWidget *layerList;
    QWidget *layerDetail;

    // Layer 1
    QTableWidget *teamTable;
    QLineEdit *searchIdInput, *searchNameInput, *searchCoachInput, *searchLocationInput;
    QSpinBox *searchMinRankInput, *searchMaxRankInput;
    QPushButton *searchBtn, *clearSearchBtn, *openAddFormBtn;
    QPushButton *standingsBtn;
    QDateEdit *standingsDateInput;
    QLabel *statusLabelList;

    // Layer 2 - Team Info
    QLineEdit *nameInput, *coachInput, *locationInput, *imagePathInput, *nationalityInput;
    QSpinBox *rankInput, *winsInput, *drawsInput, *lossesInput, *goalsForInput, *goalsAgainstInput;
    QDateEdit *foundationDateInput;
    QLabel *rosterCountLabel;
    QLabel *imagePlaceholder;
    QPushButton *saveBtn, *backBtn, *deleteBtn;
    
    // Layer 2 - Roster (Danh sách thành viên)
    QTableWidget *memberTable;
    QPushButton *addMemberBtn, *removeMemberBtn;

    void initLayerList() {
        layerList = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(layerList);
        QVBoxLayout *tableLayout = new QVBoxLayout();
        QVBoxLayout *sideLayout = new QVBoxLayout();
        QGroupBox *searchGroup = new QGroupBox("Tìm kiếm / lọc đội bóng");
        QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
        QHBoxLayout *searchActions = new QHBoxLayout();
        searchBtn = new QPushButton("🔍 Tìm / Lọc");
        clearSearchBtn = new QPushButton("Xóa bộ lọc");
        searchActions->addWidget(searchBtn);
        searchActions->addWidget(clearSearchBtn);
        searchLayout->addLayout(searchActions);
        QFormLayout *filterForm = new QFormLayout();
        searchIdInput = new QLineEdit();
        searchIdInput->setPlaceholderText("Nhập ID chính xác");
        filterForm->addRow("ID:", searchIdInput);
        searchNameInput = new QLineEdit();
        filterForm->addRow("Tên đội:", searchNameInput);
        searchCoachInput = new QLineEdit();
        filterForm->addRow("Huấn luyện viên:", searchCoachInput);
        searchLocationInput = new QLineEdit();
        filterForm->addRow("Địa phương:", searchLocationInput);
        searchMinRankInput = new QSpinBox();
        searchMinRankInput->setRange(-1, 10000);
        searchMinRankInput->setSpecialValueText("Bất kỳ");
        searchMinRankInput->setValue(-1);
        searchMaxRankInput = new QSpinBox();
        searchMaxRankInput->setRange(-1, 10000);
        searchMaxRankInput->setSpecialValueText("Bất kỳ");
        searchMaxRankInput->setValue(-1);
        QHBoxLayout *rankRange = new QHBoxLayout();
        rankRange->addWidget(searchMinRankInput);
        rankRange->addWidget(new QLabel("đến"));
        rankRange->addWidget(searchMaxRankInput);
        filterForm->addRow("Điểm XH (đã lưu):", rankRange);
        searchMinRankInput->setToolTip("Lọc theo điểm xếp hạng lưu trong hồ sơ đội, không phải điểm tính từ các trận.");
        searchMaxRankInput->setToolTip(searchMinRankInput->toolTip());
        searchLayout->addLayout(filterForm);
        sideLayout->addWidget(searchGroup);
        openAddFormBtn = new QPushButton("➕ Đăng Ký Đội Bóng Mới");
        openAddFormBtn->setStyleSheet("background-color: #2E7D32; color: white; padding: 10px; font-weight: bold;");
        sideLayout->addWidget(openAddFormBtn);
        standingsDateInput = new QDateEdit(QDate::currentDate());
        standingsDateInput->setCalendarPopup(true);
        standingsDateInput->setDisplayFormat("dd/MM/yyyy");
        sideLayout->addWidget(new QLabel("Tính BXH đến ngày:"));
        sideLayout->addWidget(standingsDateInput);
        standingsBtn = new QPushButton("Tính điểm tại ngày");
        sideLayout->addWidget(standingsBtn);
        sideLayout->addStretch();

        teamTable = new QTableWidget();
        teamTable->setColumnCount(15);
        teamTable->setHorizontalHeaderLabels({"ID", "Tên CLB", "HLV", "Địa phương", "Điểm XH (đã lưu)",
                                              "Số cầu thủ", "Thắng", "Hòa", "Thua", "Bàn thắng",
                                              "Bàn thua", "Quốc tịch", "Biên chế", "Ngày thành lập",
                                              "Hiệu số"});
        teamTable->horizontalHeaderItem(4)->setToolTip(
            "Điểm xếp hạng lưu trong hồ sơ đội; bảng xếp hạng thi đấu được tính riêng từ các trận.");
        teamTable->horizontalHeader()->setStretchLastSection(true);
        teamTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        teamTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        teamTable->setMinimumHeight(400);
        tableLayout->addWidget(teamTable, 1);

        statusLabelList = new QLabel("✓ Hệ thống quản lý CLB.");
        tableLayout->addWidget(statusLabelList);
        layout->addLayout(tableLayout, 1);
        layout->addLayout(sideLayout);
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
        formLayout->addWidget(new QLabel("Điểm XH (đã lưu):"), 1, 2);
        rankInput = new QSpinBox(); rankInput->setRange(0, 10000); formLayout->addWidget(rankInput, 1, 3);
        formLayout->addWidget(new QLabel("Quốc tịch:"), 2, 0);
        nationalityInput = new QLineEdit(); nationalityInput->setMaxLength(3); formLayout->addWidget(nationalityInput, 2, 1);
        formLayout->addWidget(new QLabel("Ngày thành lập:"), 2, 2);
        foundationDateInput = new QDateEdit();
        foundationDateInput->setCalendarPopup(true);
        foundationDateInput->setDisplayFormat("dd/MM/yyyy");
        foundationDateInput->setDate(QDate::currentDate());
        formLayout->addWidget(foundationDateInput, 2, 3);
        formLayout->addWidget(new QLabel("Thắng / Hòa / Thua:"), 3, 0);
        winsInput = new QSpinBox(); winsInput->setRange(0, 100000);
        drawsInput = new QSpinBox(); drawsInput->setRange(0, 100000);
        lossesInput = new QSpinBox(); lossesInput->setRange(0, 100000);
        QHBoxLayout *recordLayout = new QHBoxLayout();
        recordLayout->addWidget(winsInput); recordLayout->addWidget(drawsInput); recordLayout->addWidget(lossesInput);
        formLayout->addLayout(recordLayout, 3, 1, 1, 3);
        formLayout->addWidget(new QLabel("Bàn thắng / bàn thua:"), 4, 0);
        goalsForInput = new QSpinBox(); goalsForInput->setRange(0, 100000);
        goalsAgainstInput = new QSpinBox(); goalsAgainstInput->setRange(0, 100000);
        QHBoxLayout *goalsLayout = new QHBoxLayout();
        goalsLayout->addWidget(goalsForInput); goalsLayout->addWidget(goalsAgainstInput);
        formLayout->addLayout(goalsLayout, 4, 1, 1, 3);
        rosterCountLabel = new QLabel("0");
        formLayout->addWidget(new QLabel("Số cầu thủ thuộc đội:"), 5, 0);
        formLayout->addWidget(rosterCountLabel, 5, 1);
        formLayout->addWidget(new QLabel("Đường dẫn ảnh:"), 6, 0);
        imagePathInput = new QLineEdit();
        imagePathInput->setPlaceholderText("Đường dẫn tệp ảnh (để trống nếu không có)");
        formLayout->addWidget(imagePathInput, 6, 1, 1, 2);
        QPushButton *browseImageBtn = new QPushButton("Chọn ảnh...");
        formLayout->addWidget(browseImageBtn, 6, 3);
        connect(browseImageBtn, &QPushButton::clicked, this, &TeamWidget::onBrowseImage);
        connect(imagePathInput, &QLineEdit::textChanged, this, [this]() {
            updateImagePreview(nameInput->text().left(1).toUpper());
        });
        connect(nameInput, &QLineEdit::textChanged, this, [this]() {
            updateImagePreview(nameInput->text().left(1).toUpper());
        });
        
        QHBoxLayout *teamControlLayout = new QHBoxLayout();
        saveBtn = new QPushButton("💾 LƯU THÔNG TIN ĐỘI");
        saveBtn->setStyleSheet("background-color: #1E88E5; color: white; padding: 8px; font-weight: bold;");
        deleteBtn = new QPushButton("🗑️ GIẢI TÁN ĐỘI");
        deleteBtn->setStyleSheet("background-color: #E53935; color: white; padding: 8px; font-weight: bold;");
        teamControlLayout->addWidget(saveBtn);
        teamControlLayout->addWidget(deleteBtn);
        teamControlLayout->addStretch();
        formLayout->addLayout(teamControlLayout, 7, 0, 1, 4);

        topInfoLayout->addWidget(formGroup, 1);
        layout->addLayout(topInfoLayout);

        // Nửa dưới: Bảng danh sách Cầu Thủ
        QGroupBox *rosterGroup = new QGroupBox("Danh Sách Cầu Thủ Thuộc Biên Chế (Click đúp để sửa)");
        rosterGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #333; margin-top: 10px; }");
        QVBoxLayout *rosterLayout = new QVBoxLayout(rosterGroup);
        
        memberTable = new QTableWidget();
        memberTable->setColumnCount(13);
        memberTable->setHorizontalHeaderLabels({"ID", "Tên", "Vị trí", "Tuổi", "Ngày sinh", "Số áo",
                                                "Bàn thắng", "Quốc tịch", "Số trận", "Điện thoại",
                                                "Cao", "Nặng", "Đội ID"});
        const int memberColumnWidths[] = {48, 115, 70, 45, 90, 45, 62, 55, 58, 78, 55, 55, 48};
        for (int column = 0; column < memberTable->columnCount(); ++column)
            memberTable->setColumnWidth(column, memberColumnWidths[column]);
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
            const int rosterSize = static_cast<int>(logic.playersForTeam(team->getId()).size());
            const QDate foundationDate = team->getNgayThanhLap() > 0
                                             ? QDateTime::fromSecsSinceEpoch(team->getNgayThanhLap()).date()
                                             : QDate();
            const int goalDifference = logic.goalDifference(team->getSoBanThang(), team->getSoBanThua());
            const QStringList values = {
                QString::number(team->getId()), QString::fromStdString(team->getTen()),
                QString::fromStdString(team->getHLV()), QString::fromStdString(team->getDiaPhuong()),
                QString::number(team->getDiemXepHang()), QString::number(team->getSoCauThu()),
                QString::number(team->getSoTranThang()), QString::number(team->getSoTranHoa()),
                QString::number(team->getSoTranThua()), QString::number(team->getSoBanThang()),
                QString::number(team->getSoBanThua()), QString::fromStdString(team->getMaQuocGia()),
                QString::number(rosterSize), foundationDate.isValid() ? foundationDate.toString("dd/MM/yyyy") : "—",
                QString::number(goalDifference)
            };
            const QVariant keys[] = {
                QVariant::fromValue<qlonglong>(team->getId()), QVariant(values[1]), QVariant(values[2]), QVariant(values[3]),
                QVariant(team->getDiemXepHang()), QVariant(team->getSoCauThu()), QVariant(team->getSoTranThang()),
                QVariant(team->getSoTranHoa()), QVariant(team->getSoTranThua()), QVariant(team->getSoBanThang()),
                QVariant(team->getSoBanThua()), QVariant(values[11]), QVariant(rosterSize),
                QVariant::fromValue<qlonglong>(team->getNgayThanhLap()), QVariant(goalDifference)
            };
            for (int column = 0; column < 15; ++column)
                teamTable->setItem(row, column, sortableItem(values[column], keys[column]));
        }
        const int columnWidths[] = {48, 125, 95, 80, 48, 55, 42, 42, 42, 50, 50, 50, 50, 85, 50};
        for (int column = 0; column < teamTable->columnCount(); ++column)
            teamTable->setColumnWidth(column, columnWidths[column]);
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

    void loadMembersTable(long long teamId) {
        memberTable->setRowCount(0);
        const auto players = logic.playersForTeam(teamId);
        for (const cauThu* ct : players) {
            const int row = memberTable->rowCount();
            memberTable->insertRow(row);
            const QDate birthDate = ct->getNgaySinh() > 0
                                        ? QDateTime::fromSecsSinceEpoch(ct->getNgaySinh()).date()
                                        : QDate();
            const int age = plb2::playerAgeAt(*ct);
            const QString values[] = {
                QString::number(ct->getId()), QString::fromStdString(ct->getTen()),
                QString::fromStdString(ct->getViTri()), age >= 0 ? QString::number(age) : "—",
                birthDate.isValid() ? birthDate.toString("dd/MM/yyyy") : "—",
                QString::number(ct->getSoAo()), QString::number(ct->getSoBanThang()),
                QString::fromStdString(ct->getMaQuocGia()), QString::number(ct->getSoTranChoi()),
                QString::number(ct->getSdt()), QString::number(ct->getChieuCao()),
                QString::number(ct->getCanNang()), QString::number(ct->getMaDoiBong())
            };
            const QVariant keys[] = {
                QVariant::fromValue<qlonglong>(ct->getId()), QVariant(values[1]), QVariant(values[2]),
                QVariant(age), QVariant::fromValue<qlonglong>(ct->getNgaySinh()),
                QVariant(ct->getSoAo()), QVariant(ct->getSoBanThang()), QVariant(values[7]),
                QVariant(ct->getSoTranChoi()), QVariant(ct->getSdt()), QVariant(ct->getChieuCao()),
                QVariant(ct->getCanNang()), QVariant(ct->getMaDoiBong())
            };
            for (int column = 0; column < 13; ++column)
                memberTable->setItem(row, column, sortableItem(values[column], keys[column]));
        }
    }

    // Popup Dialog Edit Cầu thủ ngay trong TeamWidget
    void openPlayerEditDialog(long long playerId) {
        cauThu *ct = db->getCauThuById(playerId);
        if (!ct) return;

        QDialog dialog(this);
        dialog.setWindowTitle("Sửa Thông Tin Cầu Thủ");
        dialog.setMinimumWidth(460);
        QFormLayout *form = new QFormLayout(&dialog);

        QLineEdit *dName = new QLineEdit(QString::fromStdString(ct->getTen()));
        QLineEdit *dPos = new QLineEdit(QString::fromStdString(ct->getViTri()));
        QSpinBox *dAge = new QSpinBox(); dAge->setRange(1, 100);
        const int currentAge = plb2::playerAgeAt(*ct);
        dAge->setValue(currentAge >= 0 ? currentAge : ct->getTuoi());
        QDateEdit *dBirthDate = new QDateEdit();
        dBirthDate->setCalendarPopup(true);
        dBirthDate->setDisplayFormat("dd/MM/yyyy");
        const bool unknownBirthDate = ct->getNgaySinh() <= 0;
        dBirthDate->setDate(unknownBirthDate ? QDate::currentDate()
                                             : QDateTime::fromSecsSinceEpoch(ct->getNgaySinh()).date());
        QCheckBox *dBirthDateUnknown = new QCheckBox("Chưa xác định");
        dBirthDateUnknown->setChecked(unknownBirthDate);
        dBirthDate->setEnabled(!unknownBirthDate);
        connect(dBirthDateUnknown, &QCheckBox::toggled, dBirthDate, &QDateEdit::setDisabled);
        QSpinBox *dNum = new QSpinBox(); dNum->setRange(1, 99); dNum->setValue(ct->getSoAo());
        QSpinBox *dGoals = new QSpinBox(); dGoals->setRange(0, 1000); dGoals->setValue(ct->getSoBanThang());
        QLineEdit *dNationality = new QLineEdit(QString::fromStdString(ct->getMaQuocGia()));
        dNationality->setMaxLength(3);
        QLineEdit *dDocumentNumber = new QLineEdit(QString::fromStdString(ct->getSoGiayTo()));
        dDocumentNumber->setMaxLength(40);
        QSpinBox *dAppearances = new QSpinBox(); dAppearances->setRange(0, 100000); dAppearances->setValue(ct->getSoTranChoi());
        QLineEdit *dPhone = new QLineEdit(ct->getSdt() > 0 ? QString::number(ct->getSdt()) : QString());
        QSpinBox *dHeight = new QSpinBox(); dHeight->setRange(0, 300); dHeight->setValue(ct->getChieuCao());
        QSpinBox *dWeight = new QSpinBox(); dWeight->setRange(0, 300); dWeight->setValue(ct->getCanNang());
        QLineEdit *dImagePath = new QLineEdit(QString::fromStdString(ct->getImagePath()));
        form->addRow("ID:", new QLabel(QString::number(ct->getId())));
        form->addRow("Tên:", dName);
        form->addRow("Vị trí:", dPos);
        form->addRow("Tuổi:", dAge);
        QHBoxLayout *birthDateLayout = new QHBoxLayout();
        birthDateLayout->addWidget(dBirthDate);
        birthDateLayout->addWidget(dBirthDateUnknown);
        form->addRow("Ngày sinh:", birthDateLayout);
        form->addRow("Số áo:", dNum);
        form->addRow("Bàn thắng:", dGoals);
        form->addRow("Quốc tịch:", dNationality);
        form->addRow("CMND/Hộ chiếu:", dDocumentNumber);
        form->addRow("Số trận:", dAppearances);
        form->addRow("Điện thoại:", dPhone);
        form->addRow("Chiều cao (cm):", dHeight);
        form->addRow("Cân nặng (kg):", dWeight);
        form->addRow("Đường dẫn ảnh:", dImagePath);
        doiBong *currentTeam = db->getDoiBongById(ct->getMaDoiBong());
        form->addRow("Đội bóng:", new QLabel(currentTeam ? QString::fromStdString(currentTeam->getTen()) : "Tự do"));

        QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        form->addRow(btnBox);

        connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            if (QMessageBox::question(this, "Xác nhận cập nhật",
                                      "Bạn có chắc chắn muốn cập nhật thông tin cầu thủ này?",
                                      QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
            cauThu updatedCt = *ct;
            updatedCt.setTen(dName->text().toStdString().c_str());
            updatedCt.setViTri(dPos->text().toStdString().c_str());
            updatedCt.setTuoi(dAge->value());
            updatedCt.setSoAo(dNum->value());
            updatedCt.setSoBanThang(dGoals->value());
            updatedCt.setNgaySinh(dBirthDateUnknown->isChecked() ? 0 :
                                  QDateTime(dBirthDate->date(), QTime(0, 0)).toSecsSinceEpoch());
            updatedCt.setMaQuocGia(dNationality->text().trimmed().toUpper().toStdString().c_str());
            updatedCt.setSoGiayTo(dDocumentNumber->text().trimmed().toStdString());
            updatedCt.setSoTranChoi(dAppearances->value());
            updatedCt.setSdt(dPhone->text().toInt());
            updatedCt.setChieuCao(dHeight->value());
            updatedCt.setCanNang(dWeight->value());
            updatedCt.setImagePath(dImagePath->text().trimmed().toStdString());
            const auto result = logic.savePlayer(updatedCt, true, playerId);
            if (!result.succeeded()) {
                QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
                return;
            }
            if (!result.logged)
                QMessageBox::warning(this, "Lỗi ghi nhật ký",
                                     "Dữ liệu đã đổi trong bộ nhớ nhưng không ghi được nhật ký: " +
                                         QString::fromStdString(result.logError));
            loadMembersTable(selectedTeamId); // Refresh bảng con
            QMessageBox::information(this, "Đã cập nhật trong bộ nhớ",
                                     "Chọn File > Lưu tệp dữ liệu để lưu thay đổi bền vững.");
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

    void onBrowseImage() {
        const QString path = QFileDialog::getOpenFileName(
            this, "Chọn logo đội bóng", QString(), "Tệp ảnh (*.png *.jpg *.jpeg *.bmp *.gif)");
        if (!path.isEmpty()) imagePathInput->setText(path);
    }

    void loadTeamToForm(long long id) {
        doiBong *team = db->getDoiBongById(id);
        if (!team) return;
        nameInput->setText(QString::fromStdString(team->getTen()));
        coachInput->setText(QString::fromStdString(team->getHLV()));
        locationInput->setText(QString::fromStdString(team->getDiaPhuong()));
        rankInput->setValue(team->getDiemXepHang());
        imagePathInput->setText(QString::fromStdString(team->getImagePath()));
        nationalityInput->setText(QString::fromStdString(team->getMaQuocGia()));
        foundationDateInput->setDate(team->getNgayThanhLap() > 0
                                         ? QDateTime::fromSecsSinceEpoch(team->getNgayThanhLap()).date()
                                         : QDate::currentDate());
        winsInput->setValue(team->getSoTranThang());
        drawsInput->setValue(team->getSoTranHoa());
        lossesInput->setValue(team->getSoTranThua());
        goalsForInput->setValue(team->getSoBanThang());
        goalsAgainstInput->setValue(team->getSoBanThua());
        rosterCountLabel->setText(QString::number(logic.playersForTeam(id).size()));
        updateImagePreview(QString::fromStdString(team->getTen()).left(1).toUpper());
        
        loadMembersTable(id); // Tải danh sách thành viên
    }

    void onSaveData() {
        doiBong *existing = isEditMode ? db->getDoiBongById(selectedTeamId) : nullptr;
        if (isEditMode) {
            if (!existing) {
                QMessageBox::warning(this, "Lỗi", "Không tìm thấy đội bóng cần cập nhật.");
                return;
            }
        }
        doiBong team(isEditMode ? selectedTeamId : 0,
                     nameInput->text().toStdString().c_str(),
                     coachInput->text().toStdString().c_str(),
                     locationInput->text().toStdString().c_str());
        team.setDiemXepHang(rankInput->value());
        team.setImagePath(imagePathInput->text().trimmed().toStdString());
        team.setMaQuocGia(nationalityInput->text().trimmed().toUpper().toStdString().c_str());
        team.setNgayThanhLap(QDateTime(foundationDateInput->date(), QTime(0, 0)).toSecsSinceEpoch());
        team.setSoTranThang(winsInput->value());
        team.setSoTranHoa(drawsInput->value());
        team.setSoTranThua(lossesInput->value());
        team.setSoBanThang(goalsForInput->value());
        team.setSoBanThua(goalsAgainstInput->value());
        team.setSoCauThu(existing ? existing->getSoCauThu() : 0);
        team.setSoCauThuHienTai(rosterCountLabel->text().toInt());

        if (isEditMode) {
            if (QMessageBox::question(this, "Xác nhận cập nhật",
                                      "Bạn có chắc chắn muốn cập nhật thông tin đội bóng này?",
                                      QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
        }
        const auto result = logic.saveTeam(team, isEditMode, selectedTeamId);
        if (!result.succeeded()) {
            QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
            return;
        }
        if (!result.logged)
            QMessageBox::warning(this, "Lỗi ghi nhật ký",
                                 "Dữ liệu đã đổi trong bộ nhớ nhưng không ghi được nhật ký: " +
                                     QString::fromStdString(result.logError));
        
        refreshTable();
        statusLabelList->setText("Đã cập nhật trong bộ nhớ. Chọn File > Lưu tệp dữ liệu để lưu bền vững.");
        stackedWidget->setCurrentIndex(0);
    }

    void onDeleteTeam() {
        if (selectedTeamId == -1) return;
        if (QMessageBox::question(this, "Xác nhận", "Giải tán đội bóng này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            const auto result = logic.deleteTeam(selectedTeamId);
            if (!result.succeeded()) {
                QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
                return;
            }
            if (!result.logged)
                QMessageBox::warning(this, "Lỗi ghi nhật ký",
                                     "Dữ liệu đã đổi trong bộ nhớ nhưng không ghi được nhật ký: " +
                                         QString::fromStdString(result.logError));
            refreshTable();
            statusLabelList->setText("Đã giải tán trong bộ nhớ. Chọn File > Lưu tệp dữ liệu để lưu bền vững.");
            stackedWidget->setCurrentIndex(0);
        }
    }

    void onBack() { stackedWidget->setCurrentIndex(0); }

    void onApplySearch() {
        long long teamRecordId = -1;
        const QString idText = searchIdInput->text().trimmed();
        if (!idText.isEmpty()) {
            bool validId = false;
            teamRecordId = idText.toLongLong(&validId);
            if (!validId || teamRecordId <= 0) {
                QMessageBox::warning(this, "ID không hợp lệ",
                                     "ID đội bóng phải là số nguyên dương.");
                return;
            }
        }
        if (!plb2::isValidRange(searchMinRankInput->value(), searchMaxRankInput->value())) {
            QMessageBox::warning(this, "Khoảng lọc không hợp lệ",
                                 "Giá trị Từ không được lớn hơn giá trị Đến.");
            return;
        }

        plb2::TeamSearchFilter filter;
        filter.teamRecordId = teamRecordId;
        filter.name = searchNameInput->text();
        filter.coach = searchCoachInput->text();
        filter.location = searchLocationInput->text();
        filter.minPoints = searchMinRankInput->value();
        filter.maxPoints = searchMaxRankInput->value();
        const auto teams = plb2::filterTeams(*db, filter);
        renderTeamRows(teams);
        statusLabelList->setText(QString("Tìm thấy %1 đội bóng").arg(teams.size()));
    }

    void showStandingsAtDate() {
        const long long cutoff = QDateTime(standingsDateInput->date().addDays(1), QTime(0, 0))
                                     .toSecsSinceEpoch() - 1;
        const auto standings = logic.standingsAt(cutoff);
        QString report = QString("Bảng xếp hạng đến %1\n\n").arg(standingsDateInput->date().toString("dd/MM/yyyy"));
        int rank = 1;
        for (const auto& row : standings) {
            report += QString("%1. %2 — %3 điểm (T:%4 H:%5 B:%6, BT:%7 BB:%8)\n")
                          .arg(rank++)
                          .arg(QString::fromStdString(row.teamName))
                          .arg(row.points).arg(row.wins).arg(row.draws).arg(row.losses)
                          .arg(row.goalsFor).arg(row.goalsAgainst);
        }
        QMessageBox::information(this, "Bảng xếp hạng", report);
    }

    void onClearSearch() {
        searchIdInput->clear();
        searchNameInput->clear();
        searchCoachInput->clear();
        searchLocationInput->clear();
        searchMinRankInput->setValue(-1);
        searchMaxRankInput->setValue(-1);
        refreshTable();
        statusLabelList->setText("Đã xóa bộ lọc.");
    }

public:
    TeamWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), db(db), logic(*db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        stackedWidget = new QStackedWidget(this);
        initLayerList(); initLayerDetail();
        stackedWidget->addWidget(layerList); stackedWidget->addWidget(layerDetail);
        mainLayout->addWidget(stackedWidget);

        connect(teamTable, &QTableWidget::cellClicked, this, &TeamWidget::onTableClicked);
        connect(searchBtn, &QPushButton::clicked, this, &TeamWidget::onApplySearch);
        connect(clearSearchBtn, &QPushButton::clicked, this, &TeamWidget::onClearSearch);
        connect(searchIdInput, &QLineEdit::returnPressed, this, &TeamWidget::onApplySearch);
        connect(searchNameInput, &QLineEdit::returnPressed, this, &TeamWidget::onApplySearch);
        connect(searchCoachInput, &QLineEdit::returnPressed, this, &TeamWidget::onApplySearch);
        connect(searchLocationInput, &QLineEdit::returnPressed, this, &TeamWidget::onApplySearch);
        connect(memberTable, &QTableWidget::cellDoubleClicked, this, &TeamWidget::onMemberDoubleClicked);
        connect(openAddFormBtn, &QPushButton::clicked, this, &TeamWidget::onOpenAddForm);
        connect(backBtn, &QPushButton::clicked, this, &TeamWidget::onBack);
        connect(saveBtn, &QPushButton::clicked, this, &TeamWidget::onSaveData);
        connect(deleteBtn, &QPushButton::clicked, this, &TeamWidget::onDeleteTeam);
        connect(standingsBtn, &QPushButton::clicked, this, &TeamWidget::showStandingsAtDate);
        enableThreeStateSorting(teamTable, [this]() { onApplySearch(); },
                                [this](const QString& status) { statusLabelList->setText(status); });
        enableThreeStateSorting(memberTable, [this]() { loadMembersTable(selectedTeamId); },
                                [this](const QString& status) { statusLabelList->setText(status); });
        
        refreshTable();
    }

    void refreshTable() {
        std::vector<doiBong*> allTeams;
        for (int i = 0; i < db->getDoiBongCount(); ++i)
            if (doiBong* team = db->getDoiBongAt(i)) allTeams.push_back(team);
        renderTeamRows(allTeams);
    }

    void clearInputs() {
        nameInput->clear(); coachInput->clear(); locationInput->clear(); rankInput->setValue(0);
        nationalityInput->clear();
        foundationDateInput->setDate(QDate::currentDate());
        winsInput->setValue(0); drawsInput->setValue(0); lossesInput->setValue(0);
        goalsForInput->setValue(0); goalsAgainstInput->setValue(0);
        rosterCountLabel->setText("0");
        imagePathInput->clear();
        imagePlaceholder->setPixmap(QPixmap());
        imagePlaceholder->setText("🛡️");
    }
};
#endif