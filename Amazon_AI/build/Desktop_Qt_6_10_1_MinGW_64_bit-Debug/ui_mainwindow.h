/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Game;
    QAction *actionSave_Game;
    QAction *actionLoad_Game;
    QAction *actionExit;
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QLabel *label;
    QComboBox *comboMode;
    QLabel *label_2;
    QComboBox *comboTurn;
    QTableWidget *tableWidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *labelStatus;
    QPushButton *newGameButton;
    QPushButton *btnSave;
    QPushButton *btnLoad;
    QPushButton *undoButton;
    QPushButton *exitButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        actionNew_Game = new QAction(MainWindow);
        actionNew_Game->setObjectName("actionNew_Game");
        actionSave_Game = new QAction(MainWindow);
        actionSave_Game->setObjectName("actionSave_Game");
        actionLoad_Game = new QAction(MainWindow);
        actionLoad_Game->setObjectName("actionLoad_Game");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(560, 60, 120, 161));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 54, 16));
        comboMode = new QComboBox(groupBox);
        comboMode->addItem(QString());
        comboMode->addItem(QString());
        comboMode->setObjectName("comboMode");
        comboMode->setGeometry(QRect(10, 110, 67, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 80, 54, 16));
        comboTurn = new QComboBox(groupBox);
        comboTurn->addItem(QString());
        comboTurn->addItem(QString());
        comboTurn->setObjectName("comboTurn");
        comboTurn->setGeometry(QRect(10, 50, 67, 22));
        tableWidget = new QTableWidget(centralwidget);
        if (tableWidget->columnCount() < 10)
            tableWidget->setColumnCount(10);
        if (tableWidget->rowCount() < 10)
            tableWidget->setRowCount(10);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setGeometry(QRect(10, 10, 431, 511));
        tableWidget->setRowCount(10);
        tableWidget->setColumnCount(10);
        tableWidget->horizontalHeader()->setMinimumSectionSize(40);
        tableWidget->horizontalHeader()->setDefaultSectionSize(40);
        tableWidget->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(440, 10, 111, 321));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        labelStatus = new QLabel(layoutWidget);
        labelStatus->setObjectName("labelStatus");

        verticalLayout->addWidget(labelStatus);

        newGameButton = new QPushButton(layoutWidget);
        newGameButton->setObjectName("newGameButton");

        verticalLayout->addWidget(newGameButton);

        btnSave = new QPushButton(layoutWidget);
        btnSave->setObjectName("btnSave");

        verticalLayout->addWidget(btnSave);

        btnLoad = new QPushButton(layoutWidget);
        btnLoad->setObjectName("btnLoad");

        verticalLayout->addWidget(btnLoad);

        undoButton = new QPushButton(layoutWidget);
        undoButton->setObjectName("undoButton");

        verticalLayout->addWidget(undoButton);

        exitButton = new QPushButton(layoutWidget);
        exitButton->setObjectName("exitButton");

        verticalLayout->addWidget(exitButton);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNew_Game->setText(QCoreApplication::translate("MainWindow", "New Game", nullptr));
        actionSave_Game->setText(QCoreApplication::translate("MainWindow", "Save Game", nullptr));
        actionLoad_Game->setText(QCoreApplication::translate("MainWindow", "Load Game", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\270\270\346\210\217\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\346\250\241\345\274\217\357\274\232", nullptr));
        comboMode->setItemText(0, QCoreApplication::translate("MainWindow", "\344\272\272\346\234\272\345\257\271\345\274\210", nullptr));
        comboMode->setItemText(1, QCoreApplication::translate("MainWindow", "\345\217\214\344\272\272\345\257\271\345\274\210", nullptr));

        label_2->setText(QCoreApplication::translate("MainWindow", "\345\205\210\346\211\213\351\200\211\346\213\251\357\274\232", nullptr));
        comboTurn->setItemText(0, QCoreApplication::translate("MainWindow", "\347\216\251\345\256\266\345\205\210\346\211\213\357\274\210\347\231\275\357\274\211", nullptr));
        comboTurn->setItemText(1, QCoreApplication::translate("MainWindow", "AI\345\205\210\346\211\213\357\274\210\351\273\221\357\274\211", nullptr));

        labelStatus->setText(QCoreApplication::translate("MainWindow", "\347\255\211\345\276\205\345\274\200\345\247\213...", nullptr));
        newGameButton->setText(QCoreApplication::translate("MainWindow", "\346\226\260\346\270\270\346\210\217", nullptr));
        btnSave->setText(QCoreApplication::translate("MainWindow", "\345\202\250\345\255\230\346\270\270\346\210\217", nullptr));
        btnLoad->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\346\270\270\346\210\217", nullptr));
        undoButton->setText(QCoreApplication::translate("MainWindow", "\346\202\224\346\243\213", nullptr));
        exitButton->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
