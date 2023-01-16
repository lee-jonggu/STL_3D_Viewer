#include "doctor.h"
#include "ui_doctor.h"
#include <QDebug>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDataStream>
#include <QTcpSocket>
#include <QTcpServer>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QProgressDialog>
#include <QFileDialog>
#define BLOCK_SIZE      1024

Doctor::Doctor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Doctor), totalSize(0), byteReceived(0), byteToWrite(0),
    mWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
    mRenderer(vtkSmartPointer<vtkRenderer>::New()),
    mInteractor(vtkSmartPointer<QVTKInteractor>::New()),
    mReader(vtkSmartPointer<vtkSTLReader>::New())
{
    ui->setupUi(this);

    patientChartNumber = 1000;

    clientSocket = new QTcpSocket(this);                                                        // 채팅을 위한 소켓 생성
    clientSocket->connectToHost("127.168.0.0",8000);                                            // local server와 연결
    clientSocket->waitForConnected();

    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug( ) << "error : "<< clientSocket->errorString( ); });                                 // 에러 발생 시 에러메세지
    connect(clientSocket, SIGNAL(readyRead( )), SLOT(receiveData( )));                          // 읽을 준비가 되면 receiveData 슬롯
//    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));                        // 연결 종료시 disconnect 슬롯

    fileClient = new QTcpSocket(this);
    fileClient->connectToHost("127.168.0.0",8001);
    fileClient->waitForConnected();


    //connect(fileClient, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));                  // 파일소켓에 데이터를 쓸 때 goOnSend 슬롯 발생
    connect(ui->fileButton, SIGNAL(clicked( )), SLOT(sendFile( )));                             // fileButton 누르면 sendFile 슬롯 발생
    connect(fileClient, SIGNAL(readyRead()), this, SLOT(readClient()));
    progressDialog = new QProgressDialog(0);                                                    // progressDialog 생성 후 0으로 초기화
    progressDialog->setAutoClose(true);                                                         // 파일전송이 끝나면 progressDialog 자동종료
    progressDialog->reset();
    setWindowTitle(tr("Chat Client"));

    ui->patienttreeWidget->hideColumn(4);
    ui->patienttreeWidget->hideColumn(5);
    ui->patienttreeWidget->hideColumn(6);
    ui->patienttreeWidget->hideColumn(7);
    ui->patienttreeWidget->hideColumn(8);
    ui->patienttreeWidget->hideColumn(9);

    QAction* loadSTLAction = new QAction(tr("load STL"));
    connect(loadSTLAction, SIGNAL(triggered()), this, SLOT(loadSTLFile()));
    loadSTLMenu = new QMenu;
    loadSTLMenu->addAction(loadSTLAction);
    ui->patienttreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Set Up the Rendering
    mWindow->AddRenderer(mRenderer); 
    ui->openGLWidget->setRenderWindow(mWindow); 

}

Doctor::~Doctor()
{
    delete ui;
    clientSocket->close( );                                                                     // 소켓 닫음
}


void Doctor::loadDB()                   // 서버에서 환자 정보 받을 때
{
    // 서버에 환자 리스트 요청
    QByteArray dataArray;
    dataArray.clear();
    QDataStream out(&dataArray, QIODevice::WriteOnly);

    out << doctor << Request_Info;
    clientSocket->write(dataArray);
    clientSocket->flush();
}


void Doctor::on_patientNewpushButton_clicked()
{
    patientChartNumber = makeChartNo();
    sendData();
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->patienttreeWidget);
    item->setText(0,QString::number(patientChartNumber));
    item->setText(1,patientName);
    item->setText(2,patientGender);
    item->setText(3,patientLastVisitDate);
    ui->patienttreeWidget->addTopLevelItem(item);

    ui->patientNamelineEdit->clear();
    ui->patientBirthdatlineEdit->clear();  
    ui->patientPhonelineEdit->clear();
    ui->patientMobilelineEdit->clear();
    ui->patientEmaillineEdit->clear();
    ui->patientEmaillineEdit2->clear();
    ui->patientAddresslineEdit->clear();
    ui->patientDetailAddresslineEdit->clear();
}

void Doctor::on_patientGenderMaleradioButton_clicked()
{
    patientGender = "Male";
}

void Doctor::on_patientGenderFemaleradioButton_clicked()
{
    patientGender = "Female";
}

void Doctor::on_patientGenderNoneradioButton_clicked()
{
    patientGender = "None";
}

void Doctor::closeEvent(QCloseEvent*)
{
    clientSocket->disconnectFromHost();                                                         // 소켓 연결 종료
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();                                                    // 소켓이 연결되지 않은 상태면 잠시동안 기다림
}

void Doctor::receiveData( )
{ 
    QTcpSocket *serverSocket = (QTcpSocket *)sender();
    QByteArray bytearray = serverSocket->read(BLOCK_SIZE);
    From_Who fromWho;
    Patient_Info status;
    QDataStream in(bytearray);
    in >> fromWho >> status >> patientName >> patientChartNumber >> patientBirth >> patientFirstVisitDate >> patientLastVisitDate >> \
           patientMobile >> patientPhone >> patientEmail >> patientAddress >> patientGender >> patientEmailDomain << patient_stl_path; 

    chartList.push_back(patient_stl_path.toInt());

    switch(status)
    {
    case Send_Info:
        if (fromWho == server)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->patienttreeWidget);
            item->setText(0,QString::number(patientChartNumber));
            item->setText(1,patientName);
            item->setText(2,patientGender);
            item->setText(3,patientLastVisitDate);
            ui->patienttreeWidget->addTopLevelItem(item);
        }
        break;
    default:
        break;
    }
}

int Doctor::makeChartNo()               // 차트 번호 만들기`
{
    qDebug() << "top " << ui->patienttreeWidget->topLevelItemCount();
    qDebug() << "chart no : " << patientChartNumber;
    if(ui->patienttreeWidget->topLevelItemCount() == 0) {
        return 1000;
    } else {
        return ++patientChartNumber;
    }
}

void Doctor::sendData()
{
    qDebug("sendData");
    QByteArray dataArray;
    dataArray.clear();
    QDataStream out(&dataArray, QIODevice::WriteOnly);                                          // 바이트어레이를 쓰기전용으로 설정

    qDebug() << "Send Data " << patientChartNumber;

    patientName = ui->patientNamelineEdit->text();
    patientBirth = ui->patientBirthdatlineEdit->text();
    patientFirstVisitDate = ui->patientFirstVistdateEdit->text();
    patientLastVisitDate = ui->patientLastVistdateEdit->text();
    patientMobile = ui->patientMobilelineEdit->text();
    patientPhone = ui->patientMobilelineEdit->text();
    patientEmail = ui->patientEmaillineEdit->text();
    patientEmailDomain = ui->patientEmaillineEdit2->text();
    patientAddress = ui->patientAddresslineEdit->text() + ui->patientDetailAddresslineEdit->text();
    patient_stl_path = QString("C:/Users/KOSA/Desktop/STL/%1.stl").arg(patientChartNumber);
    chartList.push_back(patient_stl_path.toInt());


    out.device()->seek(0);
    out << doctor << Send_Info << patientName << patientChartNumber << patientBirth << patientFirstVisitDate << patientLastVisitDate << \
           patientMobile << patientPhone << patientEmail << patientAddress << patientGender << patientEmailDomain << patient_stl_path;

    clientSocket->write(dataArray);
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

// 파일전송
void Doctor::goOnSend(qint64 numBytes)                                                    /*goOnSend 슬롯*/
{
    outBlock.clear();
    byteToWrite -= numBytes;                                                                    // 남은 파일 용량으로 보낼 용량을 계속해서 뺌
    outBlock = file->read(qMin(byteToWrite, numBytes));                                         // 보낼 용량만큼 파일을 읽어서 outBlock 변수에 저장
    QTcpSocket* fileClient = dynamic_cast<QTcpSocket*>(sender());
    fileClient->write(outBlock);                                                                // 파싱한 파일을 소켓으로 보냄 

    if (byteToWrite == 0) {                                                                     // 파일전송이 끝난 경우
        qDebug("File sending completed!"); 
    }
}

void Doctor::sendFile()                                                                   /*sendFile 슬롯*/
{
    QTcpSocket* fileClient = dynamic_cast<QTcpSocket*>(sender());

    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();                                                                           // 남은 파일 용량, 파일 전체 용량, 보낼 파일 0으로 초기화

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);                                                             // 파일생성
        file->open(QFile::ReadOnly);                                                            // 파일 읽기전용으로 오픈

        qDebug() << QString("file %1 is opened").arg(filename);
//        progressDialog->setValue(0);                                                            // progressDialog 0으로 세팅



        byteToWrite = totalSize = file->size();                                                 // 남은 파일 용량
        loadSize = 1024;

        QDataStream out(&outBlock, QIODevice::WriteOnly);                                       // 파싱해서 보낼 파일을 쓰기모드로 설정
        out << qint64(0) << qint64(0) << filename << "";                          // 파싱해서 보낼 파일에 파일전체용량(0), 남은 용량(0), 파일명

        totalSize += outBlock.size();                                                           // 전체 파일크기에 보낼 파일 크기를 더함
        byteToWrite += outBlock.size();                                                         // 남은 파일크기에 보낼 파일 크기를 더함

        out.device()->seek(0);                                                                  // 처음 위치로 감
        out << totalSize << qint64(outBlock.size());                                            // 보낼 파일에 전체 크기와 보낼 파일의 용량을 씀

        fileClient->write(outBlock);                                                            // 소켓으로 파싱한 파일 보냄

//        progressDialog->setMaximum(totalSize);                                                  // progressDialog 최대값 totalSize로 설정
//        progressDialog->setValue(totalSize-byteToWrite);                                        // progressDialog 값 totalSize에서 남은 용량을 뺀 값으로 설정
//        progressDialog->show();                                                                 // progressDialog 표시
    }
    qDebug() << QString("Sending file %1").arg(filename);
}

void Doctor::readClient()
{ 
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));                                  // 소켓 받아옴
    QString filename;

    if (byteReceived == 0) {                                                                             // 파일전송 시작할 경우

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename;                                             // 파일 전체크기, 받은 데이터, 파일명, 이름 받음
        qDebug() << totalSize << byteReceived;
        qDebug() << "file name : " << filename;
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

void Doctor::loadSTL()                   // 서버에서 환자 정보 받을 때
{
    // 서버에 환자 리스트 요청
    QByteArray dataArray;
    dataArray.clear();
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out << qint64(0) << qint64(0) << "???" << Request_STL;
    fileClient->write(dataArray);
    fileClient->flush();
}

void Doctor::on_patienttreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->patienttreeWidget->mapToGlobal(pos);
    loadSTLMenu->exec(globalPos);
}

void Doctor::loadSTLFile()
{
    QString pathSTL = QString("C:/Users/KOSA/Desktop/STL/%1.stl").arg(ui->patienttreeWidget->currentItem()->text(0));
    qDebug() << pathSTL;

    mReader->SetFileName(pathSTL.toStdString().c_str());
    mReader->Update();

    // Create Mapper and Actor
    vtkSmartPointer<vtkPolyDataMapper> mMapper = vtkSmartPointer<vtkPolyDataMapper>::New(); 
    mMapper->SetInputConnection(mReader->GetOutputPort());
    mMapper->SetInputConnection(mReader->GetOutputPort());

    vtkSmartPointer<vtkActor> mActor = vtkSmartPointer<vtkActor>::New();
    mActor->SetMapper(mMapper);

    ui->openGLWidget->interactor()->SetRenderWindow(mWindow);

    ui->openGLWidget->interactor()->ProcessEvents();

    mRenderer->AddActor(mActor);

    // Render 
    mWindow->Render();

    ui->openGLWidget->interactor()->Start();
}
