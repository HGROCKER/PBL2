#include "main_window.h"

#include <QAction>
#include <QApplication>
#include <QDate>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <string>
#include "archive_storage.h"
#include "core/application_logic.h"
#include "match_widget.h"
#include "player_widget.h"
#include "team_widget.h"
#include "tournament_widget.h"
#include "ui/logic_messages.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Hệ Thống Quản Lý Giải Đấu Bóng Đá PLB2");
    resize(1400, 800);

    db.setDataDirectory((QApplication::applicationDirPath() + "/data").toStdString());
    std::string loadError;
    bool foundDataFiles = false;
    if (!db.loadFromFiles(loadError, &foundDataFiles, true)) {
        if (foundDataFiles) {
            QMessageBox::warning(this, "Chưa nhận được dữ liệu!", QString::fromStdString(loadError));
        }
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QTabWidget *tabWidget = new QTabWidget(centralWidget);
    playerWidget = new PlayerWidget(&db);
    teamWidget = new TeamWidget(&db);
    matchWidget = new MatchWidget(&db);
    tournamentWidget = new TournamentWidget(&db);
    tabWidget->addTab(playerWidget, "Quản Lý Cầu Thủ");
    tabWidget->addTab(teamWidget, "Quản Lý Đội Bóng");
    tabWidget->addTab(matchWidget, "Nhật Ký Trận Đấu");
    tabWidget->addTab(tournamentWidget, "Bảng Xếp Hạng & Vô Địch");
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 0) playerWidget->refreshTeamChoices();
        if (index == 2) matchWidget->refreshTeams();
        if (index == 3) tournamentWidget->refreshStandings();
    });
    mainLayout->addWidget(tabWidget);
    setCentralWidget(centralWidget);
    createMenuBar();
}

void MainWindow::createMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *loadAction = fileMenu->addAction("Nạp dữ liệu từ thư mục...");
    QAction *saveAction = fileMenu->addAction("Lưu tệp dữ liệu");
    QAction *importAction = fileMenu->addAction("Nhập kho lưu trữ...");
    QAction *exportAction = fileMenu->addAction("Xuất kho lưu trữ...");
    QAction *sampleAction = fileMenu->addAction("Nạp dữ liệu mẫu");
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Thoát");
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("About App");

    connect(loadAction, &QAction::triggered, this, &MainWindow::onChangeDataFolder);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveDataFiles);
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportArchive);
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportArchive);
    connect(sampleAction, &QAction::triggered, this, &MainWindow::onLoadSampleData);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::refreshAllWidgets() {
    playerWidget->refreshTeamChoices();
    playerWidget->refreshTable();
    teamWidget->refreshTable();
    matchWidget->refreshTeams();
    matchWidget->refreshTable();
}

void MainWindow::onSaveDataFiles() {
    std::string error;
    if (!db.saveToFiles(error)) {
        QMessageBox::warning(this, "Lỗi ghi dữ liệu", QString::fromStdString(error));
        return;
    }
    QMessageBox::information(this, "Thành công", "Đã ghi dữ liệu vào " + QString::fromStdString(db.getDataDirectory()));
}

void MainWindow::onLoadSampleData() {
    const QString message =
        "Thao tác này sẽ thay thế dữ liệu hiện có.\n\n"
        "Danh sách đội và 182 kết quả trận đấu năm 2016 được tổng hợp từ RSSSF. "
        "Đội hình 18 cầu thủ mỗi đội là dữ liệu minh họa được tạo tự động; "
        "huấn luyện viên và sân vận động chưa được xác minh. Tên Hà Nội đầu mùa "
        "được gộp vào đội Sài Gòn sau khi đội đổi tên giữa mùa.\n\n"
        "Nguồn: https://www.rsssf.org/tablesv/viet2016.html\n\n"
        "Tiếp tục nạp dữ liệu mẫu?";
    if (QMessageBox::question(this, "Xác nhận nạp dữ liệu mẫu", message,
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    const auto result = plb2::ApplicationLogic(db).loadSampleData();
    if (!result.succeeded()) {
        QMessageBox::warning(this, "Lỗi", logicErrorMessage(result.error));
        return;
    }
    refreshAllWidgets();
}

void MainWindow::onImportArchive() {
    const QString path = QFileDialog::getOpenFileName(
        this, "Chọn tệp dữ liệu", QString(), "Kho lưu trữ PLB2 (*.json *.txt)");
    if (path.isEmpty()) return;
    const QString importSuffix = QFileInfo(path).suffix();
    if (importSuffix.compare("txt", Qt::CaseInsensitive) != 0 &&
        importSuffix.compare("json", Qt::CaseInsensitive) != 0) {
        QMessageBox::warning(this, "Định dạng không được hỗ trợ",
                             "Chỉ hỗ trợ tệp kho lưu trữ .json hoặc .txt.");
        return;
    }
    if (QMessageBox::question(this, "Xác nhận nhập dữ liệu", "Nội dung tệp sẽ thay thế toàn bộ dữ liệu hiện tại. Tiếp tục?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    std::string error;
    if (!ArchiveStorage::load(db, path, error)) {
        QMessageBox::warning(this, "Lỗi nhập dữ liệu", QString::fromStdString(error));
        return;
    }
    refreshAllWidgets();
}

void MainWindow::onExportArchive() {
    const QString path = QFileDialog::getSaveFileName(
        this, "Lưu kho dữ liệu", QString(), "Kho lưu trữ PLB2 (*.json *.txt)");
    if (path.isEmpty()) return;
    QString outputPath = path;
    if (QFileInfo(outputPath).suffix().isEmpty()) {
        outputPath += ".json";
    } else if (QFileInfo(outputPath).suffix().compare("txt", Qt::CaseInsensitive) != 0 &&
               QFileInfo(outputPath).suffix().compare("json", Qt::CaseInsensitive) != 0) {
        QMessageBox::warning(this, "Định dạng không được hỗ trợ",
                             "Chỉ hỗ trợ xuất kho lưu trữ .json hoặc .txt.");
        return;
    }
    std::string error;
    if (!ArchiveStorage::save(db, outputPath, error))
        QMessageBox::warning(this, "Lỗi xuất dữ liệu", QString::fromStdString(error));
    else
        QMessageBox::information(this, "Thành công", "Đã xuất tệp " + outputPath);
}

void MainWindow::onChangeDataFolder() {
    const QString directory = QFileDialog::getExistingDirectory(this, "Chọn thư mục chứa dữ liệu", QString::fromStdString(db.getDataDirectory()));
    if (directory.isEmpty()) return;
    Database replacement;
    replacement.setDataDirectory(directory.toStdString());
    std::string error;
    bool foundFiles = false;
    if (!replacement.loadFromFiles(error, &foundFiles, true)) {
        QMessageBox::warning(this, "Lỗi nạp dữ liệu", QString::fromStdString(error));
        return;
    }
    if (!db.replaceWith(replacement)) {
        QMessageBox::warning(this, "Lỗi nạp dữ liệu", "Không thể cập nhật các danh sách từ thư mục đã chọn.");
        return;
    }
    db.setDataDirectory(directory.toStdString());
    refreshAllWidgets();
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "Thông Tin Ứng Dụng PLB2",
                       "Quản lý giải đấu bóng đá PLB2\n"
                       "Dữ liệu mặc định được lưu trong thư mục data cạnh ứng dụng.\n"
                       "Có thể nhập/xuất kho lưu trữ dạng văn bản (.txt).");
}
