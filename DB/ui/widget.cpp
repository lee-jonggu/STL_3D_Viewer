#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadDB()                   // DB에 저장된 환자 정보 받아오기
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","Patient");
    db.setDatabaseName("patient.db");
    if(db.open()) {
        QSqlQuery query(db);
        query.exec("create table if not exists patient(\
                    PA_NAME             varchar2(20),\
                    PA_CHART_NO_PK      number(10),\
                    PA_BIRTH            date,\
                    PA_AGE              number(10),\
                    PA_GENDER           varchar2(20),\
                    PA_LAST_VISIT       date,\
                    PA_STATUS           varchar2(20),\
                    PA_STL_PATH         varchar2(20));");

//        patient_model = new QSqlTableModel(this,db);
//        patient_model->setTable("patient");
//        patient_model->select();
//        patient_model->setHeaderData(0, Qt::Horizontal, QObject::tr("PA_NAME"));
//        patient_model->setHeaderData(1, Qt::Horizontal, QObject::tr("PA_CHART_NO_PK"));
//        patient_model->setHeaderData(2, Qt::Horizontal, QObject::tr("PA_BIRTH"));
//        patient_model->setHeaderData(3, Qt::Horizontal, QObject::tr("PA_AGE"));
//        patient_model->setHeaderData(4, Qt::Horizontal, QObject::tr("PA_GENDER"));
//        patient_model->setHeaderData(5, Qt::Horizontal, QObject::tr("PA_LAST_VISIT"));
//        patient_model->setHeaderData(6, Qt::Horizontal, QObject::tr("PA_STATUS"));
    }
}

int Widget::makeChartNo()               // 차트 번호 만들기
{
    if(qm->rowCount() == 0) {           // UI에서 모델뷰 가져오기
        return 1;
    } else {
        auto chart_no = qm->data(qm->index(qm->rowCount()-1,0)).toInt();    // UI에서 받아오는 정보로 조합 예정
        return ++chart_no;
    }
}

void Widget::on_patientNewpushButton_clicked()
{
    int chart_no = makeChartNo();

    /* 환자 관리 UI에서 LineEdit 따오기*/
    patient_name = ui->patientNamelineEdit ->text();
    patient_chart_no = ui->patientChartNumberlineEdit->text().toInt();
    patient_birth = ui->patientBirthdatlineEdit->text();
    patient_age = ui->lineedit->text().toInt();
    patient_gender = ui->patientGenderMalepushButton->text();
    patient_last_visit = ui->patientLastVistdateEdit->text();
    patient_status = ui->lineedit->text();

    QString patient_stl_path = patient_name + patient_chart_no;

    QSqlDatabase db = QSqlDatabase::database("patient");
    QSqlQuery query(db);
    query.prepare("insert into patient values (:patient_name, :patient_chart_no, :patient_birth,\
                   :patient_age, :patient_gender, :patient_last_visit, :patient_status, :patient_stl_path)");

    query.bindValue(":patient_name",patient_name);
    query.bindValue(":patient_chart_no",patient_chart_no);
    query.bindValue(":patient_birth",patient_birth);
    query.bindValue(":patient_age",patient_age);
    query.bindValue("patient_gender",patient_gender);
    query.bindValue("patient_last_visit",patient_last_visit);
    query.bindValue("patient_status",patient_status);
    query.bindValue("patient_stl_path", patient_stl_path);
    query.exec();
//    patient_model->select();
}

