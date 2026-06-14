#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logic.h"
#include <QMainWindow>
#include <QStack>
#include <QMessageBox>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tableWidget_cellClicked(int row, int column);
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_undoButton_clicked();
    void on_exitButton_clicked();
    void on_newGameButton_clicked();
private:
    Ui::MainWindow *ui;
    int step = 0;
    int currentPlayer = 1;
    int startRow, startCol;
    int moveRow, moveCol;
    int board[10][10];
    void updateBoardUI();
    bool checkPath(int r1,int c1,int r2,int c2);
    void cpuMove();
    void executeMove(AmazonMove m);
    int quickSimulate(int tempBoard[10][10], int player);
    bool checkGameOver();
    bool isPvPMode = false;
    int userColor = 1;
    QStack<AmazonMove> history;
};

#endif // MAINWINDOW_H
