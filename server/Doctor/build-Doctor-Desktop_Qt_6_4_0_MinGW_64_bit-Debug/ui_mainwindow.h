/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionManage;
    QAction *action3D;
    QAction *actionManage_2;
    QAction *action3D_2;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QToolBar *toolBar;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(640, 340);
        actionManage = new QAction(MainWindow);
        actionManage->setObjectName("actionManage");
        action3D = new QAction(MainWindow);
        action3D->setObjectName("action3D");
        actionManage_2 = new QAction(MainWindow);
        actionManage_2->setObjectName("actionManage_2");
        action3D_2 = new QAction(MainWindow);
        action3D_2->setObjectName("action3D_2");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");

        verticalLayout->addWidget(widget);

        MainWindow->setCentralWidget(centralwidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::LeftToolBarArea, toolBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 640, 17));
        MainWindow->setMenuBar(menuBar);

        toolBar->addAction(actionManage_2);
        toolBar->addSeparator();
        toolBar->addAction(action3D_2);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionManage->setText(QCoreApplication::translate("MainWindow", "Manage", nullptr));
        action3D->setText(QCoreApplication::translate("MainWindow", "3D", nullptr));
        actionManage_2->setText(QCoreApplication::translate("MainWindow", "Manage", nullptr));
        action3D_2->setText(QCoreApplication::translate("MainWindow", "3D", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
