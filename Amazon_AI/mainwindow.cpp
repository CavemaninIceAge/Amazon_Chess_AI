#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QStack>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->comboMode->addItem("人机对弈");
    ui->comboMode->addItem("双人对弈");
    ui->comboTurn->addItem("玩家先手(白)");
    ui->comboTurn->addItem("AI先手(黑)");
    connect(ui->comboMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        ui->comboTurn->setEnabled(index == 0);
    });

    memset(board, 0, sizeof(board));
    updateBoardUI();
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::updateBoardUI() {
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (!ui->tableWidget->item(i, j)) {
                ui->tableWidget->setItem(i, j, new QTableWidgetItem());
            }
            QTableWidgetItem *item = ui->tableWidget->item(i, j);

            if ((i + j) % 2 == 0) {
                item->setBackground(QColor(240, 217, 181));
            } else {
                item->setBackground(QColor(181, 136, 99));
            }

            if (board[i][j] == 1) {
                item->setText("●");
                item->setForeground(Qt::white);
                item->setTextAlignment(Qt::AlignCenter);
                QFont font = item->font();
                font.setPointSize(16);
                font.setBold(true);
                item->setFont(font);
            }
            else if (board[i][j] == 2) {
                item->setText("●");
                item->setForeground(Qt::black);
                item->setTextAlignment(Qt::AlignCenter);
                QFont font = item->font();
                font.setPointSize(16);
                font.setBold(true);
                item->setFont(font);
            }
            else if (board[i][j] == 3) { //
                item->setText("✕");
                item->setForeground(Qt::red);
                item->setTextAlignment(Qt::AlignCenter);
            }
            else {
                item->setText("");
            }
        }
    }
    if (step == 1) {
        ui->tableWidget->item(startRow, startCol)->setBackground(Qt::yellow);
    }
}
void MainWindow::on_tableWidget_cellClicked(int row, int col){
    if (!isPvPMode && currentPlayer != userColor) return;
    if (step == 0) {
        if (board[row][col] == currentPlayer) {
            startRow = row; startCol = col;
            step = 1;
            updateBoardUI();
        }
    }
    else if (step == 1) {
        if (checkPath(startRow, startCol, row, col)) {
            board[row][col] = currentPlayer;
            board[startRow][startCol] = 0;
            moveRow = row; moveCol = col;
            step = 2;
            updateBoardUI();
        } else {
            QMessageBox::warning(this, "无效移动", "路径被阻挡或不符合规则！");
        }
    }
        else if (step == 2) {
        if (checkPath(moveRow, moveCol, row, col) && (board[row][col] == 0)) {
            board[row][col] = 3;
            AmazonMove currentStep;
            currentStep.startR = startRow; currentStep.startC = startCol;
            currentStep.endR = moveRow;   currentStep.endC = moveCol;
            currentStep.arrowR = row;     currentStep.arrowC = col;
            currentStep.player = currentPlayer;
            history.push(currentStep);
            currentPlayer = (currentPlayer == 1) ? 2 : 1;
            step = 0;
            updateBoardUI();
            QApplication::processEvents();
            QTimer::singleShot(50, this, [=](){
                if (checkGameOver()) {
                    return;
                }
                if (!isPvPMode && currentPlayer != userColor) {
                    ui->labelStatus->setText("AI 正在思考中...");
                    QTimer::singleShot(500, this, [=](){
                        cpuMove();
                    });
                } else {
                    QString nextPlayer = (currentPlayer == 1) ? "白棋" : "黑棋";
                    ui->labelStatus->setText("轮到 " + nextPlayer + " 行动");
                }
            });

        } else {
            QMessageBox::warning(this, "无效操作", "请重新选择射箭点！");
        }
    }
}
bool MainWindow::checkPath(int r1, int c1, int r2, int c2) {
    if (r1 == r2 && c1 == c2) return false;
    int dr = r2 - r1;
    int dc = c2 - c1;
    if (dr != 0 && dc != 0 && abs(dr) != abs(dc)) return false;

    int stepR = (dr > 0) ? 1 : (dr < 0 ? -1 : 0);
    int stepC = (dc > 0) ? 1 : (dc < 0 ? -1 : 0);
    int currR = r1 + stepR;
    int currC = c1 + stepC;

    while (currR != r2 || currC != c2) {
        if (board[currR][currC] != 0) return false;
        currR += stepR;
        currC += stepC;
    }
    return board[r2][c2] == 0;
}

void applyMove(int board[10][10], AmazonMove m) {
    int piece = board[m.startR][m.startC];
    board[m.startR][m.startC] = 0;
    board[m.endR][m.endC] = piece;
    board[m.arrowR][m.arrowC] = 3;
}
int evaluateBoard(int tempBoard[10][10], int player) {
    int opponent = (player == 1) ? 2 : 1;
    int myMobility = 0;
    int oppMobility = 0;

    int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};

    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            if (tempBoard[r][c] == player) {
                for (int d = 0; d < 8; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    while (nr >= 0 && nr < 10 && nc >= 0 && nc < 10 && tempBoard[nr][nc] == 0) {
                        myMobility++;
                        nr += dr[d]; nc += dc[d];
                    }
                }
            } else if (tempBoard[r][c] == opponent) {
                for (int d = 0; d < 8; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    while (nr >= 0 && nr < 10 && nc >= 0 && nc < 10 && tempBoard[nr][nc] == 0) {
                        oppMobility++;
                        nr += dr[d]; nc += dc[d];
                    }
                }
            }
        }
    }

    return myMobility - oppMobility;
}
#include <vector>
#include <algorithm>
#include <ctime>

void MainWindow::cpuMove() {
    std::vector<AmazonMove> allMoves;
    allMoves.resize(25000);
    int moveCount = getAllMoves(this->board, currentPlayer, allMoves.data());

    if (moveCount <= 0) {
        ui->labelStatus->setText("游戏结束");
        QMessageBox::information(this, "Game Over", "AI 认输了！");
        return;
    }
    int bestScore = -999999;
    int bestIdx = 0;

    int searchLimit = std::min(moveCount, 1000);

    for (int i = 0; i < searchLimit; i++) {
        AmazonMove m = allMoves[i];
        int originalPiece = board[m.startR][m.startC];
        board[m.startR][m.startC] = 0;
        board[m.endR][m.endC] = originalPiece;
        board[m.arrowR][m.arrowC] = 3;

        int score = evaluateBoard(this->board, currentPlayer);

        board[m.arrowR][m.arrowC] = 0;
        board[m.endR][m.endC] = 0;
        board[m.startR][m.startC] = originalPiece;
        if (score > bestScore) {
            bestScore = score;
            bestIdx = i;
        }
    }

    executeMove(allMoves[bestIdx]);

    if (checkGameOver()) {
        ui->labelStatus->setText("AI 获胜！");
    } else {
        ui->labelStatus->setText("轮到玩家行动");
    }
}
void MainWindow::on_newGameButton_clicked() {
    isPvPMode = (ui->comboMode->currentIndex() == 1);
    userColor = (ui->comboTurn->currentIndex() == 0) ? 1 : 2;
    memset(board, 0, sizeof(board));
    board[0][3] = 2; board[0][6] = 2; board[3][0] = 2; board[3][9] = 2;
    board[6][0] = 1; board[6][9] = 1; board[9][3] = 1; board[9][6] = 1;
    step = 0;
    currentPlayer = 1;
    history.clear();
    updateBoardUI();
    if (!isPvPMode && userColor == 2) {
        ui->labelStatus->setText("AI 正在思考第一步...");
        QTimer::singleShot(600, this, &MainWindow::cpuMove);
    } else {
        ui->labelStatus->setText("请白棋行动");
    }
}

bool MainWindow::checkGameOver() {
    if (!hasAnyMove(board, currentPlayer)) {
        QString winner = (currentPlayer == 1) ? "黑棋(B)" : "白棋(W)";
        QMessageBox::information(this, "游戏结束", winner + " 获胜！");
        return true;
    }
    return false;
}
void MainWindow::executeMove(AmazonMove m) {
    int pieceColor = board[m.startR][m.startC];
    board[m.startR][m.startC] = 0;
    board[m.endR][m.endC] = pieceColor;
    board[m.arrowR][m.arrowC] = 3;
    history.push(m);
    currentPlayer = (pieceColor == 1) ? 2 : 1;
    step = 0;
    updateBoardUI();
}
void MainWindow::on_undoButton_clicked()
{
    int stepsToUndo = isPvPMode ? 1 : 2;
    if (history.size() < stepsToUndo) {
        QMessageBox::warning(this, "提示", "记录不足");
        return;
    }

    for (int i = 0; i < stepsToUndo; ++i) {
        AmazonMove last = history.pop();
        board[last.arrowR][last.arrowC] = 0;
        board[last.startR][last.startC] = last.player;
        board[last.endR][last.endC] = 0;
        currentPlayer = last.player;
    }

    step = 0;
    updateBoardUI();
    ui->labelStatus->setText("已撤销");
}
void MainWindow::on_btnSave_clicked() {
    QFile file("autosave.dat");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "错误", "无法创建存档文件！");
        return;
    }
    QDataStream out(&file);
    out << currentPlayer;
    out << step;
    out << isPvPMode;
    out << userColor;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            out << board[i][j];
        }
    }
    QList<AmazonMove> historyList = history.toList();
    out << historyList.size();
    for (const auto& m : historyList) {
        out << m.startR << m.startC << m.endR << m.endC
            << m.arrowR << m.arrowC << m.player;
    }
    file.close();
    QMessageBox::information(this, "成功", "游戏已存入 autosave.dat");
}

void MainWindow::on_btnLoad_clicked() {
    QFile file("autosave.dat");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "提示", "未找到存档文件 (autosave.dat)");
        return;
    }
    QDataStream in(&file);
    in >> currentPlayer;
    in >> step;
    in >> isPvPMode;
    in >> userColor;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            in >> board[i][j];
        }
    }

    history.clear();
    int historySize;
    in >> historySize;
    for (int k = 0; k < historySize; ++k) {
        AmazonMove m;
        in >> m.startR >> m.startC >> m.endR >> m.endC
            >> m.arrowR >> m.arrowC >> m.player;
        history.push_back(m);
    }

    file.close();
    updateBoardUI();
    QString statusText = (currentPlayer == 1) ? "轮到白棋行动" : "轮到黑棋行动";
    ui->labelStatus->setText("存档已载入，" + statusText);
    QMessageBox::information(this, "成功", "游戏状态已恢复");
}

void MainWindow::on_exitButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "退出确认", "您确定要退出程序吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        qApp->quit();
    }
}
