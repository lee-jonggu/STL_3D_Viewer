/********************************************************************************
** Form generated from reading UI file 'doctor.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOCTOR_H
#define UI_DOCTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Doctor
{
public:
    QTreeWidget *patienttreeWidget;
    QLabel *PatintInfo_label;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_5;
    QPushButton *patientNewpushButton;
    QPushButton *patientEditpushButton;
    QPushButton *patientDeletepushButton;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_2;
    QLabel *Patlent_Mobile_label;
    QLineEdit *patientMobilelineEdit;
    QLabel *Patlent_phone_label;
    QLineEdit *patientPhonelineEdit;
    QLabel *Patlent_Email_label;
    QLineEdit *patientEmaillineEdit;
    QLabel *label_12;
    QLineEdit *patientEmaillineEdit2;
    QComboBox *Patient_comboBox;
    QGridLayout *gridLayout_3;
    QLabel *Patlent_Address_label;
    QLineEdit *patientAddresslineEdit;
    QLineEdit *patientDetailAddresslineEdit;
    QLabel *Patlent_DetailAddress_label;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout;
    QDateEdit *patientFirstVistdateEdit;
    QLabel *Patlent_ChartNumber_label;
    QLabel *Patlent_LastVist_label;
    QLineEdit *patientBirthdatlineEdit;
    QLabel *Patlent_FirstVist_label;
    QLabel *Patlent_Name_label;
    QLineEdit *patientNamelineEdit;
    QLabel *Patlent_Gender_label;
    QLabel *Patlent_Birthday_label;
    QLineEdit *patientChartNumberlineEdit;
    QDateEdit *patientLastVistdateEdit;
    QRadioButton *patientGenderFemaleradioButton;
    QRadioButton *patientGenderNoneradioButton;
    QRadioButton *patientGenderMaleradioButton;
    QFrame *line;
    QWidget *Patient_Data_widget;
    QLabel *Patient_Data_label;
    QWidget *layoutWidget3;
    QHBoxLayout *horizontalLayout;
    QPushButton *fileButton;
    QPushButton *loadButton;

    void setupUi(QWidget *Doctor)
    {
        if (Doctor->objectName().isEmpty())
            Doctor->setObjectName("Doctor");
        Doctor->resize(1763, 987);
        patienttreeWidget = new QTreeWidget(Doctor);
        patienttreeWidget->setObjectName("patienttreeWidget");
        patienttreeWidget->setGeometry(QRect(20, 20, 731, 831));
        PatintInfo_label = new QLabel(Doctor);
        PatintInfo_label->setObjectName("PatintInfo_label");
        PatintInfo_label->setGeometry(QRect(770, 10, 121, 31));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        PatintInfo_label->setFont(font);
        layoutWidget = new QWidget(Doctor);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(1420, 10, 295, 31));
        gridLayout_5 = new QGridLayout(layoutWidget);
        gridLayout_5->setObjectName("gridLayout_5");
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        patientNewpushButton = new QPushButton(layoutWidget);
        patientNewpushButton->setObjectName("patientNewpushButton");
        patientNewpushButton->setCheckable(false);
        patientNewpushButton->setChecked(false);

        gridLayout_5->addWidget(patientNewpushButton, 0, 0, 1, 1);

        patientEditpushButton = new QPushButton(layoutWidget);
        patientEditpushButton->setObjectName("patientEditpushButton");
        patientEditpushButton->setCheckable(false);
        patientEditpushButton->setChecked(false);

        gridLayout_5->addWidget(patientEditpushButton, 0, 1, 1, 1);

        patientDeletepushButton = new QPushButton(layoutWidget);
        patientDeletepushButton->setObjectName("patientDeletepushButton");
        patientDeletepushButton->setCheckable(false);
        patientDeletepushButton->setChecked(false);

        gridLayout_5->addWidget(patientDeletepushButton, 0, 2, 1, 1);

        layoutWidget1 = new QWidget(Doctor);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(1209, 50, 511, 341));
        gridLayout_4 = new QGridLayout(layoutWidget1);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        Patlent_Mobile_label = new QLabel(layoutWidget1);
        Patlent_Mobile_label->setObjectName("Patlent_Mobile_label");

        gridLayout_2->addWidget(Patlent_Mobile_label, 0, 0, 1, 1);

        patientMobilelineEdit = new QLineEdit(layoutWidget1);
        patientMobilelineEdit->setObjectName("patientMobilelineEdit");

        gridLayout_2->addWidget(patientMobilelineEdit, 0, 1, 1, 5);

        Patlent_phone_label = new QLabel(layoutWidget1);
        Patlent_phone_label->setObjectName("Patlent_phone_label");

        gridLayout_2->addWidget(Patlent_phone_label, 1, 0, 1, 2);

        patientPhonelineEdit = new QLineEdit(layoutWidget1);
        patientPhonelineEdit->setObjectName("patientPhonelineEdit");

        gridLayout_2->addWidget(patientPhonelineEdit, 1, 2, 1, 4);

        Patlent_Email_label = new QLabel(layoutWidget1);
        Patlent_Email_label->setObjectName("Patlent_Email_label");

        gridLayout_2->addWidget(Patlent_Email_label, 2, 0, 1, 2);

        patientEmaillineEdit = new QLineEdit(layoutWidget1);
        patientEmaillineEdit->setObjectName("patientEmaillineEdit");

        gridLayout_2->addWidget(patientEmaillineEdit, 2, 2, 1, 1);

        label_12 = new QLabel(layoutWidget1);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 2, 3, 1, 1);

        patientEmaillineEdit2 = new QLineEdit(layoutWidget1);
        patientEmaillineEdit2->setObjectName("patientEmaillineEdit2");

        gridLayout_2->addWidget(patientEmaillineEdit2, 2, 4, 1, 1);

        Patient_comboBox = new QComboBox(layoutWidget1);
        Patient_comboBox->addItem(QString());
        Patient_comboBox->addItem(QString());
        Patient_comboBox->setObjectName("Patient_comboBox");

        gridLayout_2->addWidget(Patient_comboBox, 2, 5, 1, 1);


        gridLayout_4->addLayout(gridLayout_2, 0, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName("gridLayout_3");
        Patlent_Address_label = new QLabel(layoutWidget1);
        Patlent_Address_label->setObjectName("Patlent_Address_label");

        gridLayout_3->addWidget(Patlent_Address_label, 0, 0, 1, 1);

        patientAddresslineEdit = new QLineEdit(layoutWidget1);
        patientAddresslineEdit->setObjectName("patientAddresslineEdit");

        gridLayout_3->addWidget(patientAddresslineEdit, 0, 1, 1, 1);

        patientDetailAddresslineEdit = new QLineEdit(layoutWidget1);
        patientDetailAddresslineEdit->setObjectName("patientDetailAddresslineEdit");

        gridLayout_3->addWidget(patientDetailAddresslineEdit, 1, 1, 1, 1);

        Patlent_DetailAddress_label = new QLabel(layoutWidget1);
        Patlent_DetailAddress_label->setObjectName("Patlent_DetailAddress_label");

        gridLayout_3->addWidget(Patlent_DetailAddress_label, 1, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout_3, 1, 0, 1, 1);

        layoutWidget2 = new QWidget(Doctor);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(754, 50, 399, 341));
        gridLayout = new QGridLayout(layoutWidget2);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        patientFirstVistdateEdit = new QDateEdit(layoutWidget2);
        patientFirstVistdateEdit->setObjectName("patientFirstVistdateEdit");

        gridLayout->addWidget(patientFirstVistdateEdit, 4, 1, 1, 3);

        Patlent_ChartNumber_label = new QLabel(layoutWidget2);
        Patlent_ChartNumber_label->setObjectName("Patlent_ChartNumber_label");

        gridLayout->addWidget(Patlent_ChartNumber_label, 2, 0, 1, 1);

        Patlent_LastVist_label = new QLabel(layoutWidget2);
        Patlent_LastVist_label->setObjectName("Patlent_LastVist_label");

        gridLayout->addWidget(Patlent_LastVist_label, 5, 0, 1, 1);

        patientBirthdatlineEdit = new QLineEdit(layoutWidget2);
        patientBirthdatlineEdit->setObjectName("patientBirthdatlineEdit");

        gridLayout->addWidget(patientBirthdatlineEdit, 1, 1, 1, 3);

        Patlent_FirstVist_label = new QLabel(layoutWidget2);
        Patlent_FirstVist_label->setObjectName("Patlent_FirstVist_label");

        gridLayout->addWidget(Patlent_FirstVist_label, 4, 0, 1, 1);

        Patlent_Name_label = new QLabel(layoutWidget2);
        Patlent_Name_label->setObjectName("Patlent_Name_label");

        gridLayout->addWidget(Patlent_Name_label, 0, 0, 1, 1);

        patientNamelineEdit = new QLineEdit(layoutWidget2);
        patientNamelineEdit->setObjectName("patientNamelineEdit");

        gridLayout->addWidget(patientNamelineEdit, 0, 1, 1, 3);

        Patlent_Gender_label = new QLabel(layoutWidget2);
        Patlent_Gender_label->setObjectName("Patlent_Gender_label");

        gridLayout->addWidget(Patlent_Gender_label, 3, 0, 1, 1);

        Patlent_Birthday_label = new QLabel(layoutWidget2);
        Patlent_Birthday_label->setObjectName("Patlent_Birthday_label");

        gridLayout->addWidget(Patlent_Birthday_label, 1, 0, 1, 1);

        patientChartNumberlineEdit = new QLineEdit(layoutWidget2);
        patientChartNumberlineEdit->setObjectName("patientChartNumberlineEdit");

        gridLayout->addWidget(patientChartNumberlineEdit, 2, 1, 1, 3);

        patientLastVistdateEdit = new QDateEdit(layoutWidget2);
        patientLastVistdateEdit->setObjectName("patientLastVistdateEdit");

        gridLayout->addWidget(patientLastVistdateEdit, 5, 1, 1, 3);

        patientGenderFemaleradioButton = new QRadioButton(layoutWidget2);
        patientGenderFemaleradioButton->setObjectName("patientGenderFemaleradioButton");

        gridLayout->addWidget(patientGenderFemaleradioButton, 3, 2, 1, 1);

        patientGenderNoneradioButton = new QRadioButton(layoutWidget2);
        patientGenderNoneradioButton->setObjectName("patientGenderNoneradioButton");

        gridLayout->addWidget(patientGenderNoneradioButton, 3, 3, 1, 1);

        patientGenderMaleradioButton = new QRadioButton(layoutWidget2);
        patientGenderMaleradioButton->setObjectName("patientGenderMaleradioButton");

        gridLayout->addWidget(patientGenderMaleradioButton, 3, 1, 1, 1);

        line = new QFrame(Doctor);
        line->setObjectName("line");
        line->setGeometry(QRect(1163, 60, 20, 331));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        Patient_Data_widget = new QWidget(Doctor);
        Patient_Data_widget->setObjectName("Patient_Data_widget");
        Patient_Data_widget->setGeometry(QRect(740, 430, 961, 31));
        Patient_Data_label = new QLabel(Patient_Data_widget);
        Patient_Data_label->setObjectName("Patient_Data_label");
        Patient_Data_label->setGeometry(QRect(10, 0, 81, 31));
        layoutWidget3 = new QWidget(Doctor);
        layoutWidget3->setObjectName("layoutWidget3");
        layoutWidget3->setGeometry(QRect(761, 511, 911, 331));
        horizontalLayout = new QHBoxLayout(layoutWidget3);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        fileButton = new QPushButton(Doctor);
        fileButton->setObjectName("fileButton");
        fileButton->setGeometry(QRect(1590, 470, 80, 18));
        loadButton = new QPushButton(Doctor);
        loadButton->setObjectName("loadButton");
        loadButton->setGeometry(QRect(1070, 30, 80, 18));
        layoutWidget3->raise();
        layoutWidget3->raise();
        Patient_Data_widget->raise();
        patienttreeWidget->raise();
        PatintInfo_label->raise();
        layoutWidget3->raise();
        layoutWidget3->raise();
        line->raise();
        fileButton->raise();
        loadButton->raise();

        retranslateUi(Doctor);

        QMetaObject::connectSlotsByName(Doctor);
    } // setupUi

    void retranslateUi(QWidget *Doctor)
    {
        Doctor->setWindowTitle(QCoreApplication::translate("Doctor", "Doctor", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = patienttreeWidget->headerItem();
        ___qtreewidgetitem->setText(9, QCoreApplication::translate("Doctor", "Addr", nullptr));
        ___qtreewidgetitem->setText(8, QCoreApplication::translate("Doctor", "Email", nullptr));
        ___qtreewidgetitem->setText(7, QCoreApplication::translate("Doctor", "Phone", nullptr));
        ___qtreewidgetitem->setText(6, QCoreApplication::translate("Doctor", "Mobile", nullptr));
        ___qtreewidgetitem->setText(5, QCoreApplication::translate("Doctor", "First Visit", nullptr));
        ___qtreewidgetitem->setText(4, QCoreApplication::translate("Doctor", "Birth", nullptr));
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("Doctor", "Last Visit", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("Doctor", "Gender", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("Doctor", "Name", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("Doctor", "Chart No.", nullptr));
        PatintInfo_label->setText(QCoreApplication::translate("Doctor", "Patlent Info", nullptr));
        patientNewpushButton->setText(QCoreApplication::translate("Doctor", "New", nullptr));
        patientEditpushButton->setText(QCoreApplication::translate("Doctor", "Edit", nullptr));
        patientDeletepushButton->setText(QCoreApplication::translate("Doctor", "Delete", nullptr));
        Patlent_Mobile_label->setText(QCoreApplication::translate("Doctor", "Mobile", nullptr));
        Patlent_phone_label->setText(QCoreApplication::translate("Doctor", "Phone", nullptr));
        Patlent_Email_label->setText(QCoreApplication::translate("Doctor", "E-mail", nullptr));
        label_12->setText(QCoreApplication::translate("Doctor", "@", nullptr));
        Patient_comboBox->setItemText(0, QCoreApplication::translate("Doctor", "naver.com", nullptr));
        Patient_comboBox->setItemText(1, QCoreApplication::translate("Doctor", "gmail.com", nullptr));

        Patlent_Address_label->setText(QCoreApplication::translate("Doctor", "Address", nullptr));
        Patlent_DetailAddress_label->setText(QCoreApplication::translate("Doctor", "Detail Address", nullptr));
        Patlent_ChartNumber_label->setText(QCoreApplication::translate("Doctor", "Chart Number", nullptr));
        Patlent_LastVist_label->setText(QCoreApplication::translate("Doctor", "Last Vist", nullptr));
        Patlent_FirstVist_label->setText(QCoreApplication::translate("Doctor", "First Vist", nullptr));
        Patlent_Name_label->setText(QCoreApplication::translate("Doctor", "Name", nullptr));
        Patlent_Gender_label->setText(QCoreApplication::translate("Doctor", "Gender", nullptr));
        Patlent_Birthday_label->setText(QCoreApplication::translate("Doctor", "Birthday", nullptr));
        patientGenderFemaleradioButton->setText(QCoreApplication::translate("Doctor", "Female", nullptr));
        patientGenderNoneradioButton->setText(QCoreApplication::translate("Doctor", "None", nullptr));
        patientGenderMaleradioButton->setText(QCoreApplication::translate("Doctor", "Male", nullptr));
        Patient_Data_label->setText(QCoreApplication::translate("Doctor", "Data", nullptr));
        fileButton->setText(QCoreApplication::translate("Doctor", "File", nullptr));
        loadButton->setText(QCoreApplication::translate("Doctor", "Load", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Doctor: public Ui_Doctor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOCTOR_H
