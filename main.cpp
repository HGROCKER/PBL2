#include <iostream>
#include <cstring>
#include <vector>
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QDateTime>
#include <QHeaderView>
#include <QIcon>
#include "vector.h"
#include "csdl.h"
#include "database.h"
#include "player_widget.h"
#include "team_widget.h"
#include "search_features.h"
#include "match_widget.h"
#include "tournament_widget.h"
using namespace std;
// ==================== HELPER PRINT ====================
void printSeparator(const std::string& title) {
    std::cout << "\n================================ " << title << " ================================\n" << std::endl;
}

class MainWindow : public QMainWindow {
Q_OBJECT
private:
Database db;
PlayerWidget *playerWidget;
TeamWidget *teamWidget;
MatchWidget *matchWidget;
TournamentWidget *tournamentWidget;
QLabel *statusLabel;
public:
MainWindow() {
setWindowTitle("He Thong Quan Ly Giai Dau Bong Da PLB2 - Professional");
setGeometry(100, 100, 1200, 700);
loadSampleData();
QWidget *centralWidget = new QWidget();
QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
statusLabel = new QLabel("Trạng thái: Ứng dụng đã khởi chạy thành công.");
mainLayout->addWidget(statusLabel);
QTabWidget *tabWidget = new QTabWidget();
playerWidget = new PlayerWidget(&db);
teamWidget = new TeamWidget(&db);
matchWidget = new MatchWidget(&db);
tournamentWidget = new TournamentWidget(&db);
tabWidget->addTab(playerWidget, "Quản Lý Cầu Thủ");
tabWidget->addTab(teamWidget, "Quản Lý Đội Bóng");
tabWidget->addTab(matchWidget, "Nhật Ký Trận Đấu");
tabWidget->addTab(tournamentWidget, "Sơ Đồ Cây Vòng Bảng");
mainLayout->addWidget(tabWidget);
setCentralWidget(centralWidget);
createMenuBar();
}
private:
void createMenuBar() {
QMenu *fileMenu = menuBar()->addMenu("&File");
QAction *loadAction = fileMenu->addAction("Load Sample Data");
fileMenu->addSeparator();
QAction *exitAction = fileMenu->addAction("Exit");
QMenu *helpMenu = menuBar()->addMenu("&Help");
QAction *aboutAction = helpMenu->addAction("About App");
connect(loadAction, &QAction::triggered, this, &MainWindow::onLoadSampleData);
connect(exitAction, &QAction::triggered, this, &QWidget::close);
connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}
void loadSampleData() {
std::vector<std::pair<std::string, std::string>> teamTemplates = {
{"Manchester United", "Manchester"}, {"Liverpool", "Liverpool"},
{"Arsenal", "London"}, {"Chelsea", "London"},
{"Manchester City", "Manchester"}, {"Real Madrid", "Madrid"},
{"Barcelona", "Barcelona"}, {"Bayern Munich", "Munich"}
};
for (int i = 0; i < 8; ++i) {
doiBong team(i + 1, teamTemplates[i].first.c_str(), "HLV Truong", teamTemplates[i].second.c_str());
team.setDiemXepHang(0);
db.insert_doiBong_single(team);
}
std::vector<std::string> positions = {"GK", "LB", "CB", "CB", "RB", "CM", "CM", "AM", "LW", "RW", "ST"};
int playerGlobalId = 1;
for (int teamId = 1; teamId <= 8; ++teamId) {
doiBong* t = db.getDoiBongById(teamId);
std::string teamName = t ? t->getTen() : "Team";
for (size_t p = 0; p < positions.size(); ++p) {
std::string pName = teamName + " Player " + std::to_string(p + 1);
int randomAge = 18 + (playerGlobalId % 15);
int shirtNumber = (p == 0) ? 1 : (int)(p + 1 + (playerGlobalId % 3));
cauThu ct(playerGlobalId, pName.c_str(), positions[p].c_str(), randomAge, shirtNumber, teamId);
ct.setSoBanThang(playerGlobalId % 5);
db.insert_cauThu_single(ct);
playerGlobalId++;
}
}
tranDau td1(1, 1, 2, 2, 1, 1704067200, 1, 2);
tranDau td2(2, 3, 4, 0, 3, 1704153600, 2, 1);
db.insert_tranDau_single(td1);
db.insert_tranDau_single(td2);
}
private slots:
void onLoadSampleData() {
db.dbCauThu.clear();
db.dbDoiBong.clear();
db.dbTranDau.clear();
db.indexCauThuTuoi.clear();
db.indexCauThuMaDoiBong.clear();
db.indexDoiBongDiaPhuong.clear();
db.indexTranDauNgay.clear();
loadSampleData();
playerWidget->refreshTable();
teamWidget->refreshTable();
QMessageBox::information(this, "Thong Bao", "He thong da nap du lieu mau thanh cong!");
}
void onAbout() {
QString aboutText = QString::fromUtf8(R"(
He thong Quan Ly Bong Da PLB2 - Professional
Phien ban phat hanh: 1.0.0
Nen tang cong nghe: Qt6 GUI C++ Framework (C++17)

Cac module tinh nang:

Quan ly cau thu, doi hinh dang ky & ho so cau lac bo thi dau (Full CRUD)
Toi uu hoa tim kiem, truy van bang mang chi muc phu (Secondary Indexes)
Ve cay so do phan cap giai dau Knock-out tu dong qua Graphics View

)");
QMessageBox::about(this, "Thong Tin Ung Dung PLB2", aboutText);
}
};
// ==================== GUI ENTRY POINT ====================
int run_gui(int argc, char *argv[]) {
QApplication app(argc, argv);
MainWindow window;
window.show();
return app.exec();
}
// ==================== CONSOLE TESTS ====================
int main(int argc, char *argv[]) {
if (argc > 1 && std::string(argv[1]) == "--gui") {
return run_gui(argc, argv);
}
Database db;
doiBong db1(1, "Manchester United", "Erik ten Hag", "Manchester");
doiBong db2(2, "Liverpool", "Jurgen Klopp", "Liverpool");
db.insert_doiBong_single(db1);
db.insert_doiBong_single(db2);
cauThu ct1(1, "Cristiano Ronaldo", "ST", 39, 7, 1);
db.insert_cauThu_single(ct1);
std::cout << "Tat ca cac bai kiem tra Console da hoan thanh thanh cong!" << std::endl;
return 0;
}
#include "main.moc"
