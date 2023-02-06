#ifndef DOCTOR_H
#define DOCTOR_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMenu>
#include <vtkSTLReader.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h> 

QT_BEGIN_NAMESPACE
namespace Ui { class Doctor; }
QT_END_NAMESPACE

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QTcpServer;
class QPushButton;
class QFile;
class QProgressDialog;

typedef enum {
    Send_Info,
    Get_Info,
    Request_Info,
    Request_STL,
} Patient_Info;

typedef enum {
    nurse,
    doctor,
    server
} From_Who;

class Doctor : public QWidget
{
    Q_OBJECT

public:
    Doctor(QWidget* parent = nullptr);
    ~Doctor();

    //const int PORT_NUMBER = 8000;
    void loadDB();
    void loadSTL();

private slots:
    void on_patientNewpushButton_clicked();
    void on_patientGenderMaleradioButton_clicked();
    void on_patientGenderFemaleradioButton_clicked();
    void on_patientGenderNoneradioButton_clicked();
    void receiveData();                                            // 서버에서 데이터가 올 때
    void sendData();                                               // 서버로 데이터를 보낼 때
//    void acceptConnection();                                      // 파일 전송 소켓 연결
    void readClient();                                            // 파일 받는 함수
    void goOnSend(qint64 numBytes);
    void sendFile();

    void on_patienttreeWidget_customContextMenuRequested(const QPoint& pos);
    void loadSTLFile();

private:
    Ui::Doctor* ui;
    int makeChartNo();                                                  // 차트번호 생성
    QString patientFirstVisitDate, patientLastVisitDate;                // 환자 첫 방문, 마지막 방문
    QString patientName, patientGender, patientMobile, patientPhone;    // 환자 이름, 성별, 전화번호, 핸드폰 번호
    QString patientEmail, patientEmailDomain, patientAddress, patientDetailAddress;         // 환자 이메일, 주소, 상세주소 
    QString patientBirth, patientBirthY, patienBirthM, patienBirthD, patient_last_visit;
    int patientChartNumber;                                             // 차트번호
    QString patient_stl_path;
    QSqlTableModel* patientModel;
    QList<int> chartList;

    QProgressDialog* progressDialog;                                // 파일 진행 확인
    QFile* file;                                                    // 서버로 보내는 파일
    qint64 loadSize;                                                // 파일의 크기
    qint64 byteToWrite;                                             // 보내는 파일의 크기
    qint64 totalSize;                                               // 전체 파일의 크기
    QByteArray outBlock;                                            // 전송을 위한 데이터
    bool isSent;                                                    // 파일 서버에 접속되었는지 확인
    qint64 byteReceived;
    QByteArray inBlock;

    void closeEvent(QCloseEvent*) override;
    QTcpSocket* clientSocket;                                       // 클라이언트->서버
    QTcpSocket* fileClient;
    QTcpServer* doctorServer;

    QMenu* loadSTLMenu;

    // STL Reader
    vtkSmartPointer<vtkSTLReader> mReader;

    // VTK Renderer
    vtkSmartPointer<vtkRenderer> mRenderer;

    // VTK Render Window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mWindow;

    // VTK Interactor
    vtkSmartPointer<QVTKInteractor> mInteractor;
};
#endif // DOCTOR_H