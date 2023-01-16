#include "nurse.h"
#include "ui_nurse.h"
#include <QDebug>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDataStream>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QProgressDialog>

#define BLOCK_SIZE      1024

Nurse::Nurse(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Nurse)
{
    ui->setupUi(this);

    clientSocket = new QTcpSocket(this);                                                        // 채팅을 위한 소켓 생성
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug( ) << clientSocket->errorString( ); });                                 // 에러 발생 시 에러메세지
    connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                          // 읽을 준비가 되면 receiveData 슬롯
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));                        // 연결 종료시 disconnect 슬롯
    connect(ui->patientNewpushButton, SIGNAL(clicked( )), SLOT(sendData( )));
}

Nurse::~Nurse()
{
    delete ui;
    clientSocket->close( );                                                                     // 소켓 닫음
}

void Nurse::loadDB()                   // DB에 저장된 환자 정보 받아오기
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","Patient");
    db.setDatabaseName("patient.db");
    if(db.open()) {
        QSqlQuery query(db);
        query.exec("create table if not exists patient(\
                   PA_NAME             varchar2(20),\
                   PA_BIRTH            date,\
                   PA_CHART_NO_PK      number(10),\
                   PA_GENDER           varchar2(20),\
                   PA_FIRST_VISIT      date,\
                   PA_LAST_VISIT       date,\
                   PA_PHONE_NUM        varchar2(20),\
                   PA_EMAIL            varchar2(20),\
                   PA_ADDRESS          varchar2(20),\
                   PA_DETAIL_ADDRESS   varchar2(20),\
                   PA_STL_PATH         varchar2(20));");

        patientModel = new QSqlTableModel(this,db);
        patientModel->setTable("patient");
        patientModel->select();
        patientModel->setHeaderData(0, Qt::Horizontal, QObject::tr("PA_CHART_NO_PK"));
        patientModel->setHeaderData(1, Qt::Horizontal, QObject::tr("PA_NAME"));
        patientModel->setHeaderData(2, Qt::Horizontal, QObject::tr("PA_BIRTH"));
        patientModel->setHeaderData(3, Qt::Horizontal, QObject::tr("PA_GENDER"));
        patientModel->setHeaderData(4, Qt::Horizontal, QObject::tr("PA_FIRST_VISIT"));
        patientModel->setHeaderData(5, Qt::Horizontal, QObject::tr("PA_LAST_VISIT"));

        ui->patientlistView->setModel(patientModel);
    }
}

int Nurse::makeChartNo()               // 차트 번호 만들기
{
    if(patientModel->rowCount() == 0) {
        return 1000;
    } else {
        auto chartNo = patientModel->data(patientModel->index(patientModel->rowCount()-1,0)).toInt();
        return ++chartNo;
    }
}

void Nurse::on_patientNewpushButton_clicked()
{
    int chartNo = makeChartNo();

    /* 환자 관리 UI에서 LineEdit 따오기 */
    patientName = ui->patientNamelineEdit ->text();
    patientChartNumber = chartNo;
    patientBirth = ui->patientBirthdatlineEdit->text();
    patientFirstVisitDate = ui->patientFirstVistdateEdit->text();
    patientLastVisitDate = ui->patientLastVistdateEdit->text();
    patientMobile = ui->patientMobilelineEdit->text();
    patientPhone = ui->patientMobilelineEdit->text();
    patientEmail = ui->patientEmaillineEdit->text() + "@" + ui->patientEmaillineEdit2->text();
    patientAddress = ui->patientAddresslineEdit->text() + ui->patientDetailAddresslineEdit->text();

    QString patient_stl_path = "temp_path";

    QSqlDatabase db = QSqlDatabase::database("Patient");
    QSqlQuery query(db);
    query.prepare("insert into patient values (:PA_NAME, :PA_BIRTH, :PA_CHART_NO_PK,PA_GENDER,PA_FIRST_VISIT,PA_LAST_VISIT\
                  :PA_PHONE_NUM, :PA_EMAIL, :PA_ADDRESS, :PA_DETAIL_ADDRESS, :PA_STL_PATH)");

            query.bindValue(":PA_NAME",patientName);
    query.bindValue(":PA_BIRTH",patientBirth);
    query.bindValue(":PA_CHART_NO_PK",patientChartNumber);
    query.bindValue(":PA_GENDER",patientGender);
    query.bindValue("PA_FIRST_VISIT",patientFirstVisitDate);
    query.bindValue("PA_LAST_VISIT",patientLastVisitDate);
    query.bindValue("PA_PHONE_NUM",patientPhone);
    query.bindValue("PA_EMAIL", patientEmail);
    query.bindValue("PA_ADDRESS", patientAddress);
    query.bindValue("PA_STL_PATH", patient_stl_path);
    query.exec();
}

void Nurse::on_patientGenderMalepushButton_clicked()
{
    patientGender = "Male";
    qDebug() << patientGender;
}


void Nurse::on_patientGenderFemalepushButton_clicked()
{
    patientGender = "Female";
    qDebug() << patientGender;
}


void Nurse::on_patientGenderNonepushButton_clicked()
{
    patientGender = "None";
    qDebug() << patientGender;
}

void Nurse::closeEvent(QCloseEvent*)
{
    clientSocket->disconnectFromHost();                                                         // 소켓 연결 종료
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();                                                    // 소켓이 연결되지 않은 상태면 잠시동안 기다림
}

void Nurse::receiveData( )                                                               // 채팅자 끼리 채팅 주고 받기
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender( ));                           //
    //if (clientSocket->bytesAvailable( ) > BLOCK_SIZE) return;
    QByteArray bytearray;                                                                 // 소켓으로 부터 읽은 데이터를 바이트어레이에 저장
    Patient_Info type;
    char data[11][1020];

    for(int i = 0; i < 11; i++)
    {
        if(i == 0)
        {
            bytearray = clientSocket->read(BLOCK_SIZE);
            memset(data[i], 0, 1020);
            QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정

            in.device()->seek(0);                                                                       // 처음 위치로 감
            in >> type;                                                                                 // 소켓으로 부터 타입을 읽음
            in.readRawData(data[i], 1020);
        }
        else
        {
            bytearray = clientSocket->read(BLOCK_SIZE - 4);
            memset(data[i], 0, 1020);
            QDataStream in(&bytearray, QIODevice::ReadOnly);                                            // 바이트어레이를 읽기전용으로 설정
            in.readRawData(data[i], 1020);
        }
    }

    if(type == Get_Info)
    {
        ui->patientNamelineEdit->setText(data[0]);
        ui->patientBirthdatlineEdit->setText(data[1]);
        ui->patientChartNumberlineEdit->setText(data[2]);

//        ui->patientBirthdatlineEdit->setText(data[3]);

        ui->patientFirstVistdateEdit->setDisplayFormat(data[4]);
        ui->patientLastVistdateEdit->setDisplayFormat(data[5]);
        ui->patientMobilelineEdit->setText(data[6]);
        ui->patientPhonelineEdit->setText(data[7]);
        ui->patientEmaillineEdit->setText(data[8]);
        ui->patientAddresslineEdit->setText(data[9]);
        ui->patientDetailAddresslineEdit->setText(data[10]);
    }
}

void Nurse::sendData()
{
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);                                          // 바이트어레이를 쓰기전용으로 설정
    out.device()->seek(0);
    dataArray.clear();

    out << Send_Info;
    out.writeRawData(dataArray.append(ui->patientNamelineEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientBirthdatlineEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientChartNumberlineEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(patientGender.toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientFirstVistdateEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientLastVistdateEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientMobilelineEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientPhonelineEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientEmaillineEdit ->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientAddresslineEdit->text().toStdString().data()), 1020);
    out.writeRawData(dataArray.append(ui->patientDetailAddresslineEdit->text().toStdString().data()), 1020);
    clientSocket->write(dataArray);
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}
