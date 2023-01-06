#include "widget.h"
#include "ui_widget.h"
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

Widget::Widget(QWidget *parent)
    : QWidget(parent), isSent(false)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // ------------------------------------------------------------ 데이터 소켓 생성 -------------------------------------------------------------- //
    clientSocket = new QTcpSocket(this);                                                        // 채팅을 위한 소켓 생성
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug( ) << clientSocket->errorString( ); });                                 // 에러 발생 시 에러메세지
    connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                          // 읽을 준비가 되면 receiveData 슬롯
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));                        // 연결 종료시 disconnect 슬롯

    //------------------------------------------------------------ 시그널  --------------------------------------------------------------//
    connect(ui->patientNewpushButton, SIGNAL(clicked( )), SLOT(sendData( )));
}

Widget::~Widget()
{
    delete ui;
    clientSocket->close( );                                                                     // 소켓 닫음
}

void Widget::loadDB()                   // DB에 저장된 환자 정보 받아오기
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

int Widget::makeChartNo()               // 차트 번호 만들기
{
    if(patientModel->rowCount() == 0) {
        return 1000;
    } else {
        auto chartNo = patientModel->data(patientModel->index(patientModel->rowCount()-1,0)).toInt();
        return ++chartNo;
    }
}

void Widget::on_patientNewpushButton_clicked()
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

void Widget::on_patientGenderMalepushButton_clicked()
{
    patientGender = "Male";
    qDebug() << patientGender;
}


void Widget::on_patientGenderFemalepushButton_clicked()
{
    patientGender = "Female";
    qDebug() << patientGender;
}


void Widget::on_patientGenderNonepushButton_clicked()
{
    patientGender = "None";
    qDebug() << patientGender;
}

void Widget::closeEvent(QCloseEvent*)
{
    clientSocket->disconnectFromHost();                                                         // 소켓 연결 종료
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();                                                    // 소켓이 연결되지 않은 상태면 잠시동안 기다림
}

void Widget::receiveData( )                                                               // 채팅자 끼리 채팅 주고 받기
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

//        ui->patientBirthdatlineEdit->setText(data[3]);                         // --------------------   성별 버튼

        ui->patientFirstVistdateEdit->setDisplayFormat(data[4]);
        ui->patientLastVistdateEdit->setDisplayFormat(data[5]);
        ui->patientMobilelineEdit->setText(data[6]);
        ui->patientPhonelineEdit->setText(data[7]);
        ui->patientEmaillineEdit->setText(data[8]);
        ui->patientAddresslineEdit->setText(data[9]);
        ui->patientDetailAddresslineEdit->setText(data[10]);
    }
}

// ---------------------------------- 데이터 전송 ---------------------------------------------------------------------------- //
void Widget::sendProtocol(Patient_Info type, QByteArray* bytearray, int size)                         /*sendProtocol 함수*/
{
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);                                          // 바이트어레이를 쓰기전용으로 설정
    out.device()->seek(0);                                                                      // 처음 위치로 감
    out << type;                                                                                // 타입 씀
    //    out << byteArray[0];
    //    out << byteArray[1];
    for(int i = 0; i < 11; i++)
    {
        out.writeRawData(bytearray[i].data(), size);                                                               // 데이터 씀
//        qDebug() << bytearray[i];
    }
    qDebug() << dataArray;
    clientSocket->write(dataArray);
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());                                                 // 데이터 보내기 위해 소켓 잠시 기다림
}

void Widget::sendData()                                                                   /*sendData 슬롯*/
{
    QString str[11];
    //QTextStream str1;
    str[0] = ui->patientNamelineEdit ->text();
    str[1] = ui->patientBirthdatlineEdit->text();
    str[2] = ui->patientChartNumberlineEdit->text();
    str[3] = patientGender;
    str[4] = ui->patientFirstVistdateEdit->text();
    str[5] = ui->patientLastVistdateEdit->text();
    str[6] = ui->patientMobilelineEdit->text();
    str[7] = ui->patientPhonelineEdit->text();
    str[8] = ui->patientEmaillineEdit ->text();
    str[9] = ui->patientAddresslineEdit->text();
    str[10] = ui->patientDetailAddresslineEdit->text();

    // str1 << str[0] << str[1] << str[2] << str[3] << str[4] << str[5] << str[6] << str[7] << str[8] << str[9] << str[10];
    if(str[0].length() || str[1].length() || str[2].length() || str[3].length() || str[4].length() || str[5].length() || str[6].length() || str[7].length()
            || str[8].length() || str[9].length() || str[10].length()) {                                                                          // 입력한 메세지가 있을 경우
        QByteArray bytearray[11];
        for(int i = 0; i < 11; i++)
        {
            bytearray[i] = str[i].toUtf8();
        }
        sendProtocol(Send_Info, bytearray);
    }
//    for(int i = 0; i < 11; i++) {
//    qDebug() << str[i];
//    }
}
