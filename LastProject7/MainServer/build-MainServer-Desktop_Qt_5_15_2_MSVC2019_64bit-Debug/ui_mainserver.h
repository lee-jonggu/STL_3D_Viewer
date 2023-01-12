/********************************************************************************
** Form generated from reading UI file 'mainserver.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINSERVER_H
#define UI_MAINSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainServer
{
public:

    void setupUi(QWidget *MainServer)
    {
        if (MainServer->objectName().isEmpty())
            MainServer->setObjectName(QString::fromUtf8("MainServer"));
        MainServer->resize(800, 600);

        retranslateUi(MainServer);

        QMetaObject::connectSlotsByName(MainServer);
    } // setupUi

    void retranslateUi(QWidget *MainServer)
    {
        MainServer->setWindowTitle(QCoreApplication::translate("MainServer", "MainServer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainServer: public Ui_MainServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINSERVER_H
