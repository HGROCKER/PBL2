#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "database.h"

class PlayerWidget;
class TeamWidget;
class MatchWidget;
class TournamentWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Database db;
    PlayerWidget *playerWidget = nullptr;
    TeamWidget *teamWidget = nullptr;
    MatchWidget *matchWidget = nullptr;
    TournamentWidget *tournamentWidget = nullptr;

    void createMenuBar();
    void refreshAllWidgets();

private slots:
    void onSaveDataFiles();
    void onLoadSampleData();
    void onImportArchive();
    void onExportArchive();
    void onChangeDataFolder();
    void onAbout();

public:
    explicit MainWindow(QWidget *parent = nullptr);
};

#endif
