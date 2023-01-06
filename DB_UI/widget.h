#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QPushButton;
class QFile;
class QProgressDialog;
class LogThread;

typedef enum {
    Send_Info,
    Get_Info,
} Patient_Info;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    const int PORT_NUMBER = 8000;                                   // 포트번호

    void loadDB();

private slots:
    void on_patientNewpushButton_clicked();


    void on_patientGenderMalepushButton_clicked();

    void on_patientGenderFemalepushButton_clicked();

    void on_patientGenderNonepushButton_clicked();

    void receiveData( );                                            // 서버에서 데이터가 올 때
    void sendData( );                                               // 서버로 데이터를 보낼 때
    void sendProtocol(Patient_Info, QByteArray*, int = 1020);              // 데이터 전송

private:
    Ui::Widget *ui;
    int makeChartNo();                                                  // 차트번호 생성
    QString patientFirstVisitDate, patientLastVisitDate;                // 환자 첫 방문, 마지막 방문
    QString patientName, patientGender, patientMobile, patientPhone;    // 환자 이름, 성별, 전화번호, 핸드폰 번호
    QString patientEmail, patientAddress, patientDetailAddress;         // 환자 이메일, 주소, 상세주소
    QString patientBirth, patient_last_visit;                           // 환자 생년원일, 마지막 진료일
    int patientChartNumber;                                             // 차트번호
    QSqlTableModel *patientModel;

    void closeEvent(QCloseEvent*) override;
    QTcpSocket *clientSocket;                                       // 클라이언트용 소켓
    QTcpSocket *fileClient;
    QProgressDialog* progressDialog;                                // 파일 진행 확인
    QFile* file;                                                    // 서버로 보내는 파일
    qint64 loadSize;                                                // 파일의 크기
    qint64 byteToWrite;                                             // 보내는 파일의 크기
    qint64 totalSize;                                               // 전체 파일의 크기
    QByteArray outBlock;                                            // 전송을 위한 데이터
    bool isSent;                                                    // 파일 서버에 접속되었는지 확인
    LogThread* logThread;

};
#endif // WIDGET_H
