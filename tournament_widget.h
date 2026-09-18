#pragma once
#ifndef TOURNAMENT_WIDGET_H
#define TOURNAMENT_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <vector>
#include <cmath>
#include "database.h"

class TournamentWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    QGraphicsView *bracketView;
    QGraphicsScene *bracketScene;
    QPushButton *generateBracketBtn;
    QLabel *statusLabel;

public:
    TournamentWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), db(db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        QHBoxLayout *controlLayout = new QHBoxLayout();
        controlLayout->addWidget(new QLabel("🏟️ Sơ đồ giải đấu: 8 Đội tuyển tham dự hệ thống Knock-out"));

        generateBracketBtn = new QPushButton("🎯 Cập Nhật & Tạo Sơ Đồ Cây");
        generateBracketBtn->setStyleSheet(
            "QPushButton { background-color: #D32F2F; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }"
            );
        controlLayout->addWidget(generateBracketBtn);
        mainLayout->addLayout(controlLayout);

        bracketScene = new QGraphicsScene();
        bracketView = new QGraphicsView(bracketScene);
        // Thiết kế nền tối nhẹ (màu xám ghi) để nổi bật các ô đấu màu đen, vàng, cam, đỏ
        bracketView->setStyleSheet("QGraphicsView { border: 1px solid #ccc; background-color: #333333; }");
        mainLayout->addWidget(bracketView);

        statusLabel = new QLabel("✓ Nhấn nút để tính toán sơ đồ cây dựa trên dữ liệu trận đấu thực tế");
        statusLabel->setStyleSheet("QLabel { color: #FFF; font-weight: bold; padding: 5px; }");
        mainLayout->addWidget(statusLabel);

        connect(generateBracketBtn, &QPushButton::clicked, this, &TournamentWidget::onGenerateBracket);
    }

public slots:
    void onGenerateBracket() {
        bracketScene->clear();

        // Cấu hình tọa độ hiển thị đồ họa
        const int boxWidth = 150;
        const int boxHeight = 45;
        const int startX = 30;
        const int colSpacing = 230;

        // Mảng lưu tên các đội ở từng vòng (Round 0: Tứ kết, Round 1: Bán kết, Round 2: Chung kết, Round 3: Vô địch)
        QString bracketTeams[4][8];

        // Bước 1: Khởi tạo Vòng Tứ kết (Mặc định lấy 8 đội đầu tiên trong DB)
        for (int i = 0; i < 8; ++i) {
            doiBong* team = db->getDoiBongById(i + 1);
            bracketTeams[0][i] = team ? QString::fromStdString(team->getTen()) : QString("Đội %1").arg(i + 1);
        }

        // Bước 2: Duyệt qua lịch sử trận đấu thực tế trong Database để đẩy đội thắng tiến vào vòng trong
        int totalMatches = db->getTranDauCount();
        for (int i = 1; i <= totalMatches; ++i) {
            tranDau* match = db->getTranDauById(i);
            if (!match) continue;

            doiBong* d1 = db->getDoiBongById(match->getMaDoiBong1());
            doiBong* d2 = db->getDoiBongById(match->getMaDoiBong2());
            if (!d1 || !d2) continue;

            QString t1Name = QString::fromStdString(d1->getTen());
            QString t2Name = QString::fromStdString(d2->getTen());

            // Xác định ai là người thắng cuộc trong trận đấu này
            QString winner = "";
            if (match->getSoBanThangDoi1() > match->getSoBanThangDoi2()) winner = t1Name;
            else if (match->getSoBanThangDoi2() > match->getSoBanThangDoi1()) winner = t2Name;

            if (winner.isEmpty()) continue; // Hòa thì bỏ qua

            // Kiểm tra trận đấu thuộc vòng Tứ kết (Round 0 đấu nhau -> Tiến lên bán kết Round 1)
            for (int k = 0; k < 8; k += 2) {
                if ((bracketTeams[0][k] == t1Name && bracketTeams[0][k+1] == t2Name) ||
                    (bracketTeams[0][k] == t2Name && bracketTeams[0][k+1] == t1Name)) {
                    bracketTeams[1][k / 2] = winner;
                }
            }

            // Kiểm tra trận đấu thuộc vòng Bán kết (Round 1 đấu nhau -> Tiến lên chung kết Round 2)
            for (int k = 0; k < 4; k += 2) {
                if (!bracketTeams[1][k].isEmpty() && !bracketTeams[1][k+1].isEmpty()) {
                    if ((bracketTeams[1][k] == t1Name && bracketTeams[1][k+1] == t2Name) ||
                        (bracketTeams[1][k] == t2Name && bracketTeams[1][k+1] == t1Name)) {
                        bracketTeams[2][k / 2] = winner;
                    }
                }
            }

            // Kiểm tra trận Chung kết (Round 2 đấu nhau -> Lên ngôi vô địch Round 3)
            if (!bracketTeams[2][0].isEmpty() && !bracketTeams[2][1].isEmpty()) {
                if ((bracketTeams[2][0] == t1Name && bracketTeams[2][1] == t2Name) ||
                    (bracketTeams[2][0] == t2Name && bracketTeams[2][1] == t1Name)) {
                    bracketTeams[3][0] = winner;
                }
            }
        }

        // Bước 3: Render giao diện đồ họa cây giải đấu lên QGraphicsScene
        for (int round = 0; round < 4; ++round) {
            int numBoxes = (8 >> round); // Số lượng ô hiển thị: 8 -> 4 -> 2 -> 1
            if (numBoxes == 0) numBoxes = 1;

            for (int i = 0; i < numBoxes; ++i) {
                int x = startX + (round * colSpacing);
                int y = 40 + (i * (80 << round)) + (((1 << round) - 1) * 40);

                QString currentTeamName = bracketTeams[round][i];
                if (currentTeamName.isEmpty()) {
                    currentTeamName = "WAITING...";
                } else {
                    // ✅ FIX: Chuyển tên đội bóng sang IN HOA (UPPERCASE) theo đúng thiết kế yêu cầu
                    currentTeamName = currentTeamName.toUpper();
                }

                // ✅ FIX: Thiết lập dải màu sắc phân cấp từ nhỏ đến lớn theo yêu cầu
                QColor boxColor;
                QColor borderColor = Qt::transparent;

                if (round == 0) {
                    boxColor = QColor("#000000"); // 1. Tứ kết: ĐEN
                    borderColor = QColor("#555555");
                } else if (round == 1) {
                    boxColor = QColor("#FFD700"); // 2. Bán kết: VÀNG
                    borderColor = QColor("#D4AF37");
                } else if (round == 2) {
                    boxColor = QColor("#FF8F00"); // 3. Chung kết: CAM
                    borderColor = QColor("#E65100");
                } else if (round == 3) {
                    boxColor = QColor("#D32F2F"); // 4. Vô địch: ĐỎ
                    borderColor = QColor("#B71C1C");
                    if (currentTeamName != "WAITING...") {
                        currentTeamName = "👑 " + currentTeamName;
                    }
                }

                // Vẽ khung chữ nhật biểu diễn đội tuyển
                bracketScene->addRect(
                    x, y, boxWidth, boxHeight,
                    QPen(borderColor, round == 3 ? 3 : 1.5),
                    QBrush(boxColor)
                    );

                // Vẽ nhãn văn bản tên đội tuyển
                QGraphicsTextItem *text = bracketScene->addText(currentTeamName);

                // ✅ FIX: Cấu hình chữ màu TRẮNG ĐẬM (`Qt::white`, `QFont::Bold`)
                text->setDefaultTextColor(Qt::white);
                text->setFont(QFont("Arial", 9, QFont::Bold));

                // Căn chỉnh chữ nằm giữa ô một cách thẩm mỹ
                text->setPos(x + 8, y + 12);

                // Vẽ các đường line liên kết nhánh cây (Sử dụng màu trắng/xám sáng để nổi bật trên nền tối)
                if (round < 3 && i % 2 == 0 && numBoxes > 1) {
                    int nextY = 40 + ((i / 2) * (80 << (round + 1))) + (((1 << (round + 1)) - 1) * 40);
                    int lineStartX = x + boxWidth;
                    int lineEndX = lineStartX + (colSpacing - boxWidth) / 2;

                    bracketScene->addLine(lineStartX, y + boxHeight / 2, lineEndX, y + boxHeight / 2, QPen(QColor("#AAAAAA"), 2));
                    int partnerY = 40 + ((i + 1) * (80 << round)) + (((1 << round) - 1) * 40);
                    bracketScene->addLine(lineStartX, partnerY + boxHeight / 2, lineEndX, partnerY + boxHeight / 2, QPen(QColor("#AAAAAA"), 2));

                    bracketScene->addLine(lineEndX, y + boxHeight / 2, lineEndX, partnerY + boxHeight / 2, QPen(QColor("#AAAAAA"), 2));
                    bracketScene->addLine(lineEndX, nextY + boxHeight / 2, x + colSpacing, nextY + boxHeight / 2, QPen(QColor("#AAAAAA"), 2));
                }
            }
        }
        statusLabel->setText("✓ Cập nhật sơ đồ cây giải đấu thành công! Giao diện phân cấp Đen -> Vàng -> Cam -> Đỏ.");
    }
};

#endif
