#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void loadDB();

private slots:
    void on_patientNewpushButton_clicked();

private:
    Ui::Widget *ui;
    int makeChartNo();                                      // 차트번호 생성
    QString date;                                           // 마지막 진료일
    QString patient_name, patient_gender, patient_status;   // 환자 이름, 성별, 상태
    int patient_chart_no, patient_age;                      // 차트번호, 환자 나이
    QString patient_birth, patient_last_visit;              // 환자 생년원일, 마지막 진료일

};
#endif // WIDGET_H
