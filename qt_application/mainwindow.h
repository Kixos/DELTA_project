#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum eFrameType
{
    FrameType_Header = 155,
    FrameType_Null = 0,
    FrameType_DriveGeneral = 1,
    FrameType_ManipGeneral = 2,
    FrameType_LabGeneral = 3,
    FrameType_ManipCmd = 4,
    FrameType_ManipSave = 5,
    FrameType_LabCmd = 6,
    FrameType_ServodriveTelemetry = 9,
    FrameType_DriveCmd = 10,
    FrameType_MainboardToDriveGenerallSpeed = 11,
    FrameType_ManipFeedbackMotpos = 12,
    Frametype_LabFeedbackMotPos = 13,
    FrameType_DriveStatusTelemetry = 14,
    FrameType_ManipStatusTelemetry = 15,
    FrameType_LabStatusTelemetry = 16,
    FrameType_RoverPosABS = 20,
    FrameType_NvidiaURC = 21,
    FrameType_NewpointsABS = 22,
    FrameType_NewpointsREL = 23,
    FrameType_ArtagPosABS = 24,
    FrameType_ArtagPosREL = 25,
    FrameType_RoverPosREL = 30,
    FrameType_NvidiaERC = 31,
    FrameType_NewpointsERC = 32,
    FrameType_SetYawAndPos = 33,
    FrameType_ManipFeedbackAxesposMat = 34,
    FrameType_ManipFeedbackAxesposRPY = 35,
    FrameType_ManipJoinMoveAbs = 36,
    FrameType_ManipJoinMoveInc = 37,
    FrameType_ManipBaseMoveAbs = 38,
    FrameType_ManipBaseMoveInc = 39,
    FrameType_ManipToolMoveInc = 40,
    FrameType_ManipGripperMove = 41,
    FrameType_ManipBaseMoveAbsMat = 42,
    FrameType_ManipBaseMoveIncMat = 43,
    FrameType_ManipToolMoveIncMat = 44,
    Frametype_EmergencyHardStop = 201,
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openport1button_clicked();

    void on_closeport1button_clicked();

    void serial1send(const char data);

    void serial1sendSTR(const QByteArray data);

    void read1();

    void on_sendButton_clicked();

    void on_sendA_clicked();

    void on_sendVEL_clicked();

    void on_sendHPOS_clicked();

    void on_sendPFB_clicked();

    void on_sendEN_clicked();

    void on_sendPOLY3_clicked();

    void on_sendPOLY5_clicked();

    void on_sendLINEAR_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort serial1;
    QTimer *timerSerial;
    QTimer *timerLabel;
    eFrameType frameToSendBySerial;
    QString serial1buf;
    QByteArray bufread1;
    int backimp1;
    int backimp2;
    int backimp3;

};
#endif // MAINWINDOW_H
