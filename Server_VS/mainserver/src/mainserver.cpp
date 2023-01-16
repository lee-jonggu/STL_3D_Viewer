#include "mainserver.h"
#include "ui_mainserver.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QSqlQueryModel>
#include <QFileDialog>


MainServer::MainServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainServer), isSent(false)
{
    ui->setupUi(this);

    mainServer = new QTcpServer(this);                                                 // 채팅을 위한 TCP서버 생성
    connect(mainServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));             // 서버연결이 되면 clientConnect 슬롯 발생

    if (!mainServer->listen(QHostAddress::Any, 8000)) {
        QMessageBox::critical(this, tr("Chatting Server"),\
                              tr("Unable to start the server: %1.")\
                              .arg(mainServer->errorString( )));                       // ip주소와 포트번호를 통한 서버 연결에 실패할 경우 에러메세지 띄움
        close( );
        return;
    }

    fileServer = new QTcpServer(this);                                                 // 파일 전송을 위한 TCP서버 생성
    receivedSocket = new QTcpSocket(this);
//    connect(receivedSocket, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));            // 서버연결이 되면 acceptConnection 슬롯 발생
    if (!fileServer->listen(QHostAddress::Any, 8001)) {
        QMessageBox::critical(this, tr("File Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));                       // ip주소와 포트번호를 통한 서버 연결에 실패하면 에러메세지 띄움
        close( );
        return;
    }
    qDebug("Start listening ...");

    loadDB();
}

MainServer::~MainServer()
{
    delete ui;

    QSqlDatabase db = QSqlDatabase::database("Patient");
    if (db.isOpen()) {
        patientModel->submitAll();
        delete patientModel;
        db.close();
    }

    mainServer->close( );
    fileServer->close( );                                                               // 채팅서버와 파일 서버 닫음
}

void MainServer::loadDB()                   // DB에 저장된 환자 정보 받아오기
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
                   PA_DOMAIN           varchar2(20),\
                   PA_ADDRESS          varchar2(20),\
                   PA_DETAIL_ADDRESS   varchar2(20),\
                   PA_STL_PATH         varchar2(20));");

        patientModel = new QSqlTableModel(this,db);
        patientModel->setTable("patient");
        patientModel->setHeaderData(0, Qt::Horizontal, tr("name"));
        patientModel->setHeaderData(1, Qt::Horizontal, tr("birth"));
        patientModel->setHeaderData(2, Qt::Horizontal, tr("chart"));
        patientModel->setHeaderData(3, Qt::Horizontal, tr("gender"));
        patientModel->setHeaderData(4, Qt::Horizontal, tr("frist"));
        patientModel->setHeaderData(5, Qt::Horizontal, tr("last"));
        patientModel->setHeaderData(6, Qt::Horizontal, tr("phone"));
        patientModel->setHeaderData(7, Qt::Horizontal, tr("email"));
        patientModel->setHeaderData(8, Qt::Horizontal, tr("domain"));
        patientModel->setHeaderData(9, Qt::Horizontal, tr("addr"));
        patientModel->setHeaderData(10, Qt::Horizontal, tr("detail"));
        patientModel->setHeaderData(11, Qt::Horizontal, tr("path"));
        ui->tableView->setModel(patientModel);

        query.exec(QString("select * from patient"));
        patientModel->select();
    }
}

void MainServer::clientConnect()
{
    QTcpSocket* clientConnection = mainServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()),this, SLOT(receiveData()));
//    connect(clientConnection, SIGNAL(readyRead()),this, SLOT(sendData()));
    qDebug("new connection is established....");
}


void MainServer::receiveData()
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    From_Who fromWho;
    Patient_Info status;
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> fromWho >> status >> patientName >> patientChartNumber >> patientBirth >> patientFirstVisitDate >> patientLastVisitDate >> \
           patientMobile >> patientPhone >> patientEmail >> patientAddress >> patientGender >> patientEmailDomain << patient_stl_path;

    qDebug() << "receiveData " << fromWho << status << patientName << patientChartNumber << patientBirth << patientFirstVisitDate << patientLastVisitDate << \
           patientMobile << patientPhone << patientEmail << patientAddress << patientGender << patientEmailDomain << patient_stl_path;

    switch(status)
    {
    case Send_Info:
        if (fromWho == doctor)
        {
            qDebug() << "Doctor Send";
            // 데이터베이스에 저장

            patient_stl_path = QString("C:/Users/KOSA/Desktop/STL/%1.stl").arg(patientChartNumber);
            QSqlDatabase db = QSqlDatabase::database("Patient");
            QSqlQuery query(db);
            query.exec(QString("insert into patient values ('%1', %2, '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12')")\
                       .arg(patientName).arg(patientBirth).arg(patientChartNumber).arg(patientGender)\
                       .arg(patientFirstVisitDate).arg(patientLastVisitDate).arg(patientPhone)\
                       .arg(patientEmail).arg(patientEmailDomain).arg(patientAddress)\
                       .arg(patientDetailAddress).arg(patient_stl_path));
            patientModel->select();
            break;
        }
    case Request_Info:
        if(fromWho == doctor)
        {
            if(patientModel->rowCount() == 0)
            {
                return;
            }
//            sendData();
            // 데이터베이스에서 정보 가져옴
            QSqlDatabase db = QSqlDatabase::database("Patient");
            QSqlQuery query(db);
            query.exec(QString("select * from patient"));
            patientModel->select();
            qDebug() << "rowCount" <<patientModel->rowCount();
            for(int i=0; i < patientModel->rowCount(); i++)
            {
                QByteArray dataArray;
                dataArray.clear();
                QDataStream out(&dataArray, QIODevice::WriteOnly);
                patientName = patientModel->data(patientModel->index(i,0)).toString();
                patientBirth = patientModel->data(patientModel->index(i,1)).toString();
                patientChartNumber = patientModel->data(patientModel->index(i,2)).toInt();
                patientGender = patientModel->data(patientModel->index(i,3)).toString();
                patientFirstVisitDate = patientModel->data(patientModel->index(i,4)).toString();
                patientLastVisitDate = patientModel->data(patientModel->index(i,5)).toString();
                patientPhone = patientModel->data(patientModel->index(i,6)).toString();
                patientEmail = patientModel->data(patientModel->index(i,7)).toString();
                patientEmailDomain = patientModel->data(patientModel->index(i,8)).toString();
                patientAddress = patientModel->data(patientModel->index(i,9)).toString();
                patientDetailAddress = patientModel->data(patientModel->index(i,10)).toString();
                patient_stl_path = patientModel->data(patientModel->index(i,11)).toString();

                out << server << Send_Info << patientName << patientChartNumber << patientBirth << patientFirstVisitDate << patientLastVisitDate << \
                       patientMobile <<  patientPhone << patientEmail << patientAddress << patientGender << patientEmailDomain << patient_stl_path;
                clientConnection->write(dataArray);
            }
            break;
        }
        else if(fromWho == nurse)
        {
            socketHash[fromWho] = clientConnection;
            // 데이터베이스에서 정보 가져옴

            break;
        }
    case Get_Info:
        break;
    }
}

void MainServer::sendData()
{
//    QTcpSocket *clientConnection = (QTcpSocket *)sender();
//    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
//    From_Who fromWho;
//    Patient_Info status;
//    QDataStream in(&bytearray, QIODevice::ReadOnly);
//    in >> fromWho >> status >> patientName >> patientChartNumber >> patientBirth >> patientFirstVisitDate >> patientLastVisitDate >> \
//           patientMobile >> patientPhone >> patientEmail >> patientAddress >> patientGender >> patientEmailDomain;

//    // 데이터베이스에서 정보 가져옴
//    QSqlDatabase db = QSqlDatabase::database("Patient");
//    QSqlQuery query(db);
//    query.exec(QString("select * from patient"));
//    patientModel->select();
//    qDebug() << "rowCount" <<patientModel->rowCount();
//    QByteArray dataArray;
//    for(int i=0; i < patientModel->rowCount(); i++)
//    {
//        dataArray.clear();
//        QDataStream out(&dataArray, QIODevice::WriteOnly);
//        patientName = patientModel->data(patientModel->index(i,0)).toString();
//        patientBirth = patientModel->data(patientModel->index(i,1)).toString();
//        patientChartNumber = patientModel->data(patientModel->index(i,2)).toInt();
//        patientGender = patientModel->data(patientModel->index(i,3)).toString();
//        patientFirstVisitDate = patientModel->data(patientModel->index(i,4)).toString();
//        patientLastVisitDate = patientModel->data(patientModel->index(i,5)).toString();
//        patientPhone = patientModel->data(patientModel->index(i,6)).toString();
//        patientEmail = patientModel->data(patientModel->index(i,7)).toString();
//        patientEmailDomain = patientModel->data(patientModel->index(i,8)).toString();
//        patientAddress = patientModel->data(patientModel->index(i,9)).toString();
//        patientDetailAddress = patientModel->data(patientModel->index(i,10)).toString();
//        patient_stl_path = patientModel->data(patientModel->index(i,11)).toString();

//        out << server << Send_Info << patientName << patientChartNumber << patientBirth << patientFirstVisitDate << patientLastVisitDate << \
//               patientMobile <<  patientPhone << patientEmail << patientAddress << patientGender << patientEmailDomain;
//        clientConnection->write(dataArray);
//        clientConnection->flush();
//        while(clientConnection->waitForBytesWritten());
//    }
}


// 파일전송
void MainServer::acceptConnection()
{
    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(receivedSocket, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));
    qDebug("file connection is established....");
}

void MainServer::readClient()
{
    Patient_Info status;
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));                                  // 소켓 받아옴
    QString filename;


    if (byteReceived == 0) {                                                                             // 파일전송 시작할 경우

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> status;                                             // 파일 전체크기, 받은 데이터, 파일명, 이름 받음


        if (status == Request_STL)
        {
            sendFile();
            return;
        }

        QFileInfo info(filename);                                                                        // 파일정보
        QString currentFileName = info.fileName();                                                       // 파일이름
        qDebug() << currentFileName;
        file = new QFile(currentFileName);                                                               // 파일 생성
        file->open(QFile::WriteOnly);                                                                    // 파일 쓰기모드로 설정
    } else {
        inBlock = receivedSocket->readAll();                                                             // 소켓으로부터 모두 읽음
        byteReceived += inBlock.size();                                                                  // 지금까지 받은 데이터 크기에 방금 받은 데이터 크기 더함
        file->write(inBlock);                                                                            // 받은 데이터를 파일에 저장
        file->flush();
    }

    if (byteReceived == totalSize) {                                                                     // 파일 전송을 완료한 경우
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();                                                                                 // 받은 데이터 저장소 비움
        byteReceived = 0;                                                                                // 누적 받은 데이터 크기 0으로 설정
        totalSize = 0;                                                                                   // 전체크기 0으로 설정

        file->close();                                                                                   // 파일 닫음
        delete file;
    }

}

//------------------------------------------------------------------------

void MainServer::goOnSend(qint64 numBytes)
{
    byteToWrite -= numBytes;                                                                    // 남은 파일 용량으로 보낼 용량을 계속해서 뺌
    outBlock = file->read(qMin(byteToWrite, numBytes));                                         // 보낼 용량만큼 파일을 읽어서 outBlock 변수에 저장
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));
    receivedSocket->write(outBlock);
}

void MainServer::sendFile()
{
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));

    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();                                                                           // 남은 파일 용량, 파일 전체 용량, 보낼 파일 0으로 초기화

    for(int i=0; i < patientModel->rowCount(); i++)
    {
        QString path = patientModel->data(patientModel->index(i,11)).toString();
        if(path.length()) {
            file = new QFile(path);                                                             // 파일생성
            file->open(QFile::ReadOnly);                                                            // 파일 읽기전용으로 오픈

            qDebug() << QString("file %1 is opened").arg(path);

            byteToWrite = totalSize = file->size();                                                 // 남은 파일 용량
            loadSize = 1024;

            QDataStream out(&outBlock, QIODevice::WriteOnly);                                       // 파싱해서 보낼 파일을 쓰기모드로 설정
            out << qint64(0) << qint64(0) << path;                          // 파싱해서 보낼 파일에 파일전체용량(0), 남은 용량(0), 파일명, 이름 넣음
            totalSize += outBlock.size();                                                           // 전체 파일크기에 보낼 파일 크기를 더함
            byteToWrite += outBlock.size();                                                         // 남은 파일크기에 보낼 파일 크기를 더함
            out.device()->seek(0);                                                                  // 처음 위치로 감
            qDebug() << totalSize << qint64(outBlock.size());
            out << totalSize << qint64(outBlock.size());                                            // 보낼 파일에 전체 크기와 보낼 파일의 용량을 씀
            receivedSocket->write(outBlock);                                                            // 소켓으로 파싱한 파일 보냄
        }
    }
    QString filename = QFileDialog::getOpenFileName(this);
//    if(filename.length()) {
//        file = new QFile(filename);                                                             // 파일생성
//        file->open(QFile::ReadOnly);                                                            // 파일 읽기전용으로 오픈

//        qDebug() << QString("file %1 is opened").arg(filename);

//        byteToWrite = totalSize = file->size();                                                 // 남은 파일 용량
//        loadSize = 1024;

//        QDataStream out(&outBlock, QIODevice::WriteOnly);                                       // 파싱해서 보낼 파일을 쓰기모드로 설정
//        out << qint64(0) << qint64(0) << filename;                          // 파싱해서 보낼 파일에 파일전체용량(0), 남은 용량(0), 파일명, 이름 넣음
//        totalSize += outBlock.size();                                                           // 전체 파일크기에 보낼 파일 크기를 더함
//        byteToWrite += outBlock.size();                                                         // 남은 파일크기에 보낼 파일 크기를 더함
//        out.device()->seek(0);                                                                  // 처음 위치로 감
//        qDebug() << totalSize << qint64(outBlock.size());
//        out << totalSize << qint64(outBlock.size());                                            // 보낼 파일에 전체 크기와 보낼 파일의 용량을 씀
//        receivedSocket->write(outBlock);                                                            // 소켓으로 파싱한 파일 보냄

//    }
    qDebug() << QString("Sending file %1").arg(filename);
}

