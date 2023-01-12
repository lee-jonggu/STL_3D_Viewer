#ifndef MAINSERVER_H
#define MAINSERVER_H

#include <QWidget>
#include <QList>
#include <QHash>
#include <QMenu>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>

class QLabel;
class QTcpServer;
class QTcpSocket;
class QFile;
class QProgressDialog;
class LogThread;

namespace Ui {
class MainServer;
}

typedef enum {
    Send_Info,
    Get_Info,
    Request_Info,
} Patient_Info;

typedef enum {
    nurse,
    doctor,
    server
} From_Who;

class MainServer : public QWidget
{
    Q_OBJECT

public:
    explicit MainServer(QWidget *parent = nullptr);
    ~MainServer();

    void sendProtocol(Patient_Info, QByteArray*, int = 1020);
    void sendData();


private:
    const int BLOCK_SIZE = 1024;
    const int PORT_NUMBER = 8000;

    Ui::MainServer *ui;
    QTcpServer *mainServer;
    QTcpServer *fileServer;
    QHash<From_Who, QTcpSocket*> socketHash;
    QTcpSocket *serverSocket;

    QMenu* menu;
    QFile* file;
    QProgressDialog* progressDialog;
    qint64 totalSize;
    qint64 byteReceived;
    QByteArray inBlock;
    QByteArray outBlock;

    QString patientFirstVisitDate, patientLastVisitDate;                // 환자 첫 방문, 마지막 방문
    QString patientName, patientGender, patientMobile, patientPhone;    // 환자 이름, 성별, 전화번호, 핸드폰 번호
    QString patientEmail, patientEmailDomain, patientAddress, patientDetailAddress;         // 환자 이메일, 주소, 상세주소
    QString patientBirth, patient_last_visit;                           // 환자 생년원일, 마지막 진료일
    int patientChartNumber;                                             // 차트번호
    QString patient_stl_path;
    QSqlTableModel *patientModel;
    void loadDB();

private slots:
    void acceptConnection();                /* 파일 서버 */
    void readClient();

    void clientConnect( );                  /* 채팅 서버 */
    void receiveData( );

    void goOnSend(qint64);
    void sendFile();

signals:
};
#endif // MAINSERVER_H
