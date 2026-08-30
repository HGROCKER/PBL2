#pragma once
#ifndef TOURNAMENT_WIDGET_H
#define TOURNAMENT_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
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
#include "database.h"

class TournamentWidget : public QWidget {
    Q_OBJECT
private:
    Database *db;
    QGraphicsView *bracketView;
    QGraphicsScene *bracketScene;
    QComboBox *vongBangCombo;
    QSpinBox *zoomSpinBox;
    QPushButton *addRoundBtn, *generateBracketBtn;
    QLabel *statusLabel;

    struct BracketBox {
        long long doiBongId;
        QString tenDoi;
        int round;
        int position;
        QGraphicsRectItem *box;
    };
    
    std::vector<BracketBox> bracketBoxes;

    void drawBracket() {
        bracketScene->clear();
        // Vẽ 4 cột: 16 → 8 → 4 → 2 → 1
        const int startX = 50, startY = 50;
        const int boxWidth = 120, boxHeight = 50;
        const int colSpacing = 200;
        
        // Vẽ từng round
        for (int round = 0; round < 5; round++) {
            int numTeams = (16 >> round);
            int yOffset = startY + (500 / numTeams / 2);
            
            for (int i = 0; i < numTeams; i++) {
                int x = startX + (round * colSpacing);
                int y = startY + (i * (500 / numTeams));
                
                QGraphicsRectItem *box = bracketScene->addRect(
                    x, y, boxWidth, boxHeight,
                    QPen(Qt::blue, 2), QBrush(Qt::white)
                );
                
                QGraphicsTextItem *text = bracketScene->addText(
                    QString("Team %1").arg(i+1)
                );
                text->setPos(x + 5, y + 15);
            }
        }
        statusLabel->setText("✓ Vẽ bracket thành công!");
    }

public slots:
    void onGenerateBracket() {
        drawBracket();
    }

    void onZoomChanged(int value) {
        qreal scale = value / 100.0;
        bracketView->setTransform(QTransform::fromScale(scale, scale));
    }

    void onAddRound() {
        QMessageBox::information(this, "ℹ️", "Thêm vòng: Chọn số đội tham dự");
    }

public:
    TournamentWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), db(db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Control Panel
        QHBoxLayout *controlLayout = new QHBoxLayout();
        controlLayout->addWidget(new QLabel("Vòng bảng:"));
        vongBangCombo = new QComboBox();
        vongBangCombo->addItem("16 đội", 4);
        vongBangCombo->addItem("8 đội", 3);
        vongBangCombo->addItem("4 đội", 2);
        vongBangCombo->addItem("2 đội (Chung kết)", 1);
        controlLayout->addWidget(vongBangCombo);

        generateBracketBtn = new QPushButton("🎯 Tạo Bracket");
        generateBracketBtn->setStyleSheet(
            "QPushButton { background-color: #9C27B0; color: white; padding: 8px; border-radius: 4px; font-weight: bold; }"
        );
        controlLayout->addWidget(generateBracketBtn);

        controlLayout->addWidget(new QLabel("Zoom:"));
        zoomSpinBox = new QSpinBox();
        zoomSpinBox->setRange(50, 200);
        zoomSpinBox->setValue(100);
        zoomSpinBox->setSuffix("%");
        controlLayout->addWidget(zoomSpinBox);

        mainLayout->addLayout(controlLayout);

        // Bracket View
        bracketScene = new QGraphicsScene();
        bracketView = new QGraphicsView(bracketScene);
        bracketView->setStyleSheet("QGraphicsView { border: 1px solid #ccc; }");
        mainLayout->addWidget(bracketView);

        statusLabel = new QLabel("✓ Sẵn sàng tạo bracket");
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; padding: 5px; }");
        mainLayout->addWidget(statusLabel);

        connect(generateBracketBtn, &QPushButton::clicked, this, &TournamentWidget::onGenerateBracket);
        connect(zoomSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &TournamentWidget::onZoomChanged);
    }
};

#endif