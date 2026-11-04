#pragma once
#ifndef TOURNAMENT_WIDGET_H
#define TOURNAMENT_WIDGET_H

#include <QAbstractItemView>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QStringList>
#include "database.h"
#include "core/application_logic.h"

class TournamentWidget : public QWidget {
    Q_OBJECT
private:
    plb2::ApplicationLogic logic;
    QDateEdit *standingsDateInput;
    QTableWidget *standingsTable;
    QLabel *statusLabel;

public:
    TournamentWidget(Database *db, QWidget *parent = nullptr)
        : QWidget(parent), logic(*db) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(new QLabel(
            "Bảng xếp hạng vòng tròn: mỗi cặp đội thi đấu hai trận, mỗi đội một lần đá sân nhà."));

        QHBoxLayout *controls = new QHBoxLayout();
        standingsDateInput = new QDateEdit(QDate::currentDate());
        standingsDateInput->setCalendarPopup(true);
        standingsDateInput->setDisplayFormat("dd/MM/yyyy");
        QPushButton *refreshButton = new QPushButton("Cập nhật bảng xếp hạng");
        controls->addWidget(new QLabel("Tính điểm đến hết ngày:"));
        controls->addWidget(standingsDateInput);
        controls->addWidget(refreshButton);
        controls->addStretch();
        mainLayout->addLayout(controls);

        standingsTable = new QTableWidget();
        standingsTable->setColumnCount(10);
        standingsTable->setHorizontalHeaderLabels(
            {"Hạng", "Đội bóng", "Đã đấu", "Thắng", "Hòa", "Thua",
             "Bàn thắng", "Bàn thua", "Hiệu số", "Điểm"});
        standingsTable->horizontalHeader()->setStretchLastSection(true);
        standingsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        standingsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mainLayout->addWidget(standingsTable, 1);

        statusLabel = new QLabel();
        statusLabel->setStyleSheet("QLabel { color: #FFF; font-weight: bold; padding: 5px; }");
        mainLayout->addWidget(statusLabel);

        connect(refreshButton, &QPushButton::clicked, this, &TournamentWidget::refreshStandings);
        refreshStandings();
    }

public slots:
    void refreshStandings() {
        const long long cutoff = QDateTime(standingsDateInput->date().addDays(1), QTime(0, 0))
                                     .toSecsSinceEpoch() - 1;
        const auto standings = logic.standingsAt(cutoff);
        standingsTable->setRowCount(0);

        int rank = 1;
        for (const auto& row : standings) {
            const int tableRow = standingsTable->rowCount();
            standingsTable->insertRow(tableRow);
            const int goalDifference = logic.goalDifference(row.goalsFor, row.goalsAgainst);
            const QString values[] = {
                QString::number(rank), QString::fromStdString(row.teamName),
                QString::number(row.played), QString::number(row.wins),
                QString::number(row.draws), QString::number(row.losses),
                QString::number(row.goalsFor), QString::number(row.goalsAgainst),
                QString::number(goalDifference), QString::number(row.points)
            };
            for (int column = 0; column < 10; ++column) {
                QTableWidgetItem *item = new QTableWidgetItem(values[column]);
                item->setTextAlignment(Qt::AlignCenter);
                standingsTable->setItem(tableRow, column, item);
            }
            ++rank;
        }

        const long long now = QDateTime::currentSecsSinceEpoch();
        const auto progress = logic.tournamentProgress(now);
        if (progress.complete) {
            QStringList leaders;
            for (const auto& leader : progress.leaders)
                leaders.append(QString::fromStdString(leader));
            statusLabel->setText(QString("Giải đấu đã hoàn tất. Đội vô địch: %1").arg(leaders.join(", ")));
        } else {
            statusLabel->setText(QString("Bảng điểm đến hết ngày %1. Tiến độ: %2/%3 trận; giải chưa hoàn tất.")
                                     .arg(standingsDateInput->date().toString("dd/MM/yyyy"))
                                     .arg(progress.completedMatches)
                                     .arg(progress.expectedMatches));
        }
    }
};

#endif
