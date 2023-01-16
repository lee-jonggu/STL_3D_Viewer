#ifndef NURSE_H
#define NURSE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class Nurse; }
QT_END_NAMESPACE

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QPushButton;

typedef enum {
    Send_Info,
    Get_Info,
    Request_Info,
} Patient_Info;

class Nurse : public QWidget
{
    Q_OBJECT

public:
    Nurse(QWidget *parent = nullptr);
    ~Nurse();

    //const int PORT_NUMBER = 8000;
    void loadDB();

private slots:
    void on_patientNewpushButton_clicked();
    void on_patientGenderMalepushButton_clicked();
    void on_patientGenderFemalepushButton_clicked();
    void on_patientGenderNonepushButton_clicked();
    void receiveData( );                                            // 서버에서 데이터가 올 때
    void sendData( );                                               // 서버로 데이터를 보낼 때

private:
    Ui::Nurse *ui;
    int makeChartNo();                                                  // 차트번호 생성
    QString patientFirstVisitDate, patientLastVisitDate;                // 환자 첫 방문, 마지막 방문
    QString patientName, patientGender, patientMobile, patientPhone;    // 환자 이름, 성별, 전화번호, 핸드폰 번호
    QString patientEmail, patientAddress, patientDetailAddress;         // 환자 이메일, 주소, 상세주소
    QString patientBirth, patient_last_visit;                           // 환자 생년원일, 마지막 진료일
    int patientChartNumber;                                             // 차트번호
    QSqlTableModel *patientModel;

    void closeEvent(QCloseEvent*) override;
    QTcpSocket *clientSocket;                                       // 클라이언트용 소켓
};
#endif // NURSE_H
