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


MainServer::MainServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainServer)
{
    ui->setupUi(this);

    mainServer = new QTcpServer(this);                                                 // 채팅을 위한 TCP서버 생성
    connect(mainServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));             // 서버연결이 되면 clientConnect 슬롯 발생

    serverSocket = new QTcpSocket(this);                                                        // 서버 -> 클라이언트
    serverSocket->connectToHost("127.168.0.0",8010);
    connect(serverSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                          // 읽을 준비가 되면 receiveData 슬롯
    connect(serverSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));

    if (!mainServer->listen(QHostAddress::Any, 8000)) {
        QMessageBox::critical(this, tr("Chatting Server"),\
                              tr("Unable to start the server: %1.")\
                              .arg(mainServer->errorString( )));                       // ip주소와 포트번호를 통한 서버 연결에 실패할 경우 에러메세지 띄움
        close( );
        return;
    }

    fileServer = new QTcpServer(this);                                                 // 파일 전송을 위한 TCP서버 생성
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));            // 서버연결이 되면 acceptConnection 슬롯 발생
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
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
//        patientModel->select();
//        patientModel->setHeaderData(0, Qt::Horizontal, QObject::tr("PA_CHART_NO_PK"));
//        patientModel->setHeaderData(1, Qt::Horizontal, QObject::tr("PA_NAME"));
//        patientModel->setHeaderData(2, Qt::Horizontal, QObject::tr("PA_BIRTH"));
//        patientModel->setHeaderData(3, Qt::Horizontal, QObject::tr("PA_GENDER"));
//        patientModel->setHeaderData(4, Qt::Horizontal, QObject::tr("PA_FIRST_VISIT"));
//        patientModel->setHeaderData(5, Qt::Horizontal, QObject::tr("PA_LAST_VISIT"));
//        patientModel->select();

//        ui->patientlistView->setModel(patientModel);
    }
}

void MainServer::clientConnect()
{
    QTcpSocket* clientConnection = mainServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()),this, SLOT(receiveData()));
    qDebug("new connection is established....");
}

void MainServer::receiveData()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    From_Who fromWho;
    Patient_Info status;
    QDataStream in(clientConnection);
    in >> fromWho >> status >> patientName >> patientChartNumber >> patientBirth >> patientFirstVisitDate >> patientLastVisitDate >> \
           patientMobile >> patientPhone >> patientEmail >> patientAddress >> patientGender << patientEmailDomain;

    qDebug() << fromWho << " " << status;

    switch(status)
    {
    case Send_Info:
        if (fromWho == doctor)
        {
            socketHash[fromWho] = clientConnection;
            // 데이터베이스에 저장

            patient_stl_path = "temp_path";

            QSqlDatabase db = QSqlDatabase::database("Patient");
            QSqlQuery query(db);
            query.prepare("insert into patient values (:PA_NAME, :PA_BIRTH, :PA_CHART_NO_PK,PA_GENDER,PA_FIRST_VISIT,PA_LAST_VISIT\
                          :PA_PHONE_NUM, :PA_EMAIL, :PA_DOMAIN, :PA_ADDRESS, :PA_DETAIL_ADDRESS, :PA_STL_PATH)");

            query.bindValue(":PA_NAME",patientName);
            query.bindValue(":PA_BIRTH",patientBirth);
            query.bindValue(":PA_CHART_NO_PK",patientChartNumber);
            query.bindValue(":PA_GENDER",patientGender);
            query.bindValue("PA_FIRST_VISIT",patientFirstVisitDate);
            query.bindValue("PA_LAST_VISIT",patientLastVisitDate);
            query.bindValue("PA_PHONE_NUM",patientPhone);
            query.bindValue("PA_EMAIL", patientEmail);
            query.bindValue("PA_DOMAIN", patientEmailDomain);
            query.bindValue("PA_ADDRESS", patientAddress);
            query.bindValue("PA_STL_PATH", patient_stl_path);
            query.exec();
            break;
        }
    case Request_Info:
        if(fromWho == doctor)
        {
            qDebug("Request , Doctor");
            socketHash[fromWho] = clientConnection;
            // 데이터베이스에서 정보 가져옴
            QSqlDatabase db = QSqlDatabase::database("Patient");
            QSqlQuery query(db);

            for(int i=0; i < patientModel->rowCount(); i++)
            {
                QByteArray dataArray;
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
                       patientMobile << patientPhone << patientEmail << patientAddress << patientGender << patientEmailDomain;

                serverSocket->write(dataArray);
                serverSocket->flush();
                while(serverSocket->waitForBytesWritten());
            }
            break;
        }
        else if(fromWho == nurse)
        {
            socketHash[fromWho] = clientConnection;
            // 데이터베이스에서 정보 가져옴

            break;
        }
    }
}

void MainServer::sendData()
{
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);


}


// 파일전송
void MainServer::acceptConnection()
{
    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
    qDebug("file connection is established....");
}

void MainServer::readClient()
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));                                  // 소켓 받아옴
    QString filename, name;

    if (byteReceived == 0) {                                                                             // 파일전송 시작할 경우
        progressDialog->reset();
        progressDialog->show();                                                                          // progress다이얼로그 띄움

        QString ip = receivedSocket->peerAddress().toString();                                           // 현재 ip주소
        quint16 port = receivedSocket->peerPort();                                                       // 현재 포트번호
        qDebug() << ip << " : " << port;

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name;                                             // 파일 전체크기, 받은 데이터, 파일명, 이름 받음
        progressDialog->setMaximum(totalSize);                                                           // 다이얼로그의 전체 크기를 파일 전체 크기로 설정

        QFileInfo info(filename);                                                                        // 파일정보
        QString currentFileName = info.fileName();                                                       // 파일이름
        file = new QFile(currentFileName);                                                               // 파일 생성
        file->open(QFile::WriteOnly);                                                                    // 파일 쓰기모드로 설정
    } else {
        inBlock = receivedSocket->readAll();                                                             // 소켓으로부터 모두 읽음

        byteReceived += inBlock.size();                                                                  // 지금까지 받은 데이터 크기에 방금 받은 데이터 크기 더함
        file->write(inBlock);                                                                            // 받은 데이터를 파일에 저장
        file->flush();
    }

    progressDialog->setValue(byteReceived);

    if (byteReceived == totalSize) {                                                                     // 파일 전송을 완료한 경우
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();                                                                                 // 받은 데이터 저장소 비움
        byteReceived = 0;                                                                                // 누적 받은 데이터 크기 0으로 설정
        totalSize = 0;                                                                                   // 전체크기 0으로 설정
        progressDialog->reset();                                                                         // 다이얼로그 리셋
        progressDialog->hide();                                                                          // 다이얼로그 숨김

        file->close();                                                                                   // 파일 닫음
        delete file;
    }
}

//------------------------------------------------------------------------

void MainServer::goOnSend(qint64 numBytes)
{
   /* byteToWrite -= numBytes;                                                                    // 남은 파일 용량으로 보낼 용량을 계속해서 뺌
    outBlock = file->read(qMin(byteToWrite, numBytes));                                         // 보낼 용량만큼 파일을 읽어서 outBlock 변수에 저장
    socketHash[type]->write(outBlock);     */                                                           // 파싱한 파일을 소켓으로 보냄
}

void MainServer::sendFile()
{
//    byteToWrite = 0;
//    totalSize = 0;
//    outBlock.clear();                                                                           // 남은 파일 용량, 파일 전체 용량, 보낼 파일 0으로 초기화

//    QString filename = QFileDialog::getOpenFileName(this);
//    if(filename.length()) {
//        file = new QFile(filename);                                                             // 파일생성
//        file->open(QFile::ReadOnly);                                                            // 파일 읽기전용으로 오픈

//        qDebug() << QString("file %1 is opened").arg(filename);
//        progressDialog->setValue(0);                                                            // progressDialog 0으로 세팅

//        if (!isSent) {                                                                          // 파일을 처음 보내는 경우
//            fileClient->connectToHost(ui->serverAddress->text( ),                               // 소켓을 연결하고 isSent를 false에서 true로 바꿈
//                                      ui->serverPort->text( ).toInt( ) + 1);
//            isSent = true;
//        }

//        byteToWrite = totalSize = file->size();                                                 // 남은 파일 용량

//        QDataStream out(&outBlock, QIODevice::WriteOnly);                                       // 파싱해서 보낼 파일을 쓰기모드로 설정
//        out << qint64(0) << qint64(0) << filename << ui->name->text();                          // 파싱해서 보낼 파일에 파일전체용량(0), 남은 용량(0), 파일명, 이름 넣음

//        totalSize += outBlock.size();                                                           // 전체 파일크기에 보낼 파일 크기를 더함
//        byteToWrite += outBlock.size();                                                         // 남은 파일크기에 보낼 파일 크기를 더함

//        out.device()->seek(0);                                                                  // 처음 위치로 감
//        out << totalSize << qint64(outBlock.size());                                            // 보낼 파일에 전체 크기와 보낼 파일의 용량을 씀
//        fileClient->write(outBlock);                                                            // 소켓으로 파싱한 파일 보냄                                                            // progressDialog 표시
//    }
//    qDebug() << QString("Sending file %1").arg(filename);
}
