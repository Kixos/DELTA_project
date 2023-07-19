#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length();
     foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){

         ui->combobox1->addItem(serialPortInfo.portName());
         timerSerial = new QTimer(this);

         frameToSendBySerial = FrameType_DriveGeneral;
     }
     serial1buf = "";
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openport1button_clicked()
{
    serial1.setPortName(ui->combobox1->currentText());
    serial1.setBaudRate(QSerialPort::Baud57600);
    serial1.setDataBits(QSerialPort::Data8);
    serial1.setParity(QSerialPort::NoParity);
    serial1.setStopBits(QSerialPort::OneStop);
    if(!(serial1.isOpen()))
    {
        serial1.open(QIODevice::ReadWrite);
        if(serial1.isOpen())
        {
            connect(&serial1, SIGNAL(readyRead()),this, SLOT(read1()));
            ui->openport1button->setEnabled(false);
            ui->closeport1button->setEnabled(true);
            qDebug() << "The COM port has been opened.";
        }
        else
        {
            qDebug() << "The COM port has not been opened.";
        }
    }
    else
    {
        qDebug() << "The COM port is already open.";
    }
}


void MainWindow::on_closeport1button_clicked()
{
    serial1.close();
    if(!(serial1.isOpen()))
    {
        timerSerial->stop();
        ui->openport1button->setEnabled(true);
        ui->closeport1button->setEnabled(false);
        qDebug() << "The COM port has been closed.";
    }
}


void MainWindow::serial1send(const char data)
{
    if(serial1.isWritable())
        {
            serial1.write(&data, 1);
        }
    else
    {
        qDebug() << "Couldn't write to the serial.";
    }
}


void MainWindow::serial1sendSTR(const QByteArray data)
{
    if(serial1.isWritable())
        {
           serial1.write(data);
        }
    else
    {
        qDebug() << "Couldn't write to the serial.";
    }
}


void MainWindow::read1()
{
    bufread1.append(serial1.readAll());
    qDebug() << "Serial read:" << bufread1;

    //QString temp = ui->labelPOS->text();
    //temp.append(bufread1);
    //ui->labelPOS->setText(temp);

    if(bufread1.isNull())
    {
        qDebug() << "Serial NULL";
    }
    bufread1.clear();
}


void MainWindow::on_sendButton_clicked()
{
    QByteArray xx = QByteArray::number(ui->spinX->value());
    QByteArray yy = QByteArray::number(ui->spinY->value());
    QByteArray zz = QByteArray::number(ui->spinZ->value());
    QByteArray sendpos = ("F " + xx + " " + yy + " " + zz + " H");

    serial1sendSTR(sendpos);
}


void MainWindow::on_sendVEL_clicked()
{
    QByteArray vel = QByteArray::number(ui->spinVEL->value());
    QByteArray sendvel = ("V" + vel + "H");

    serial1sendSTR(sendvel);
}


void MainWindow::on_sendA_clicked()
{
    serial1send('A');
}


void MainWindow::on_sendHPOS_clicked()
{
    serial1send('B');
}


void MainWindow::on_sendPFB_clicked()
{
    serial1send('C');
}


void MainWindow::on_sendEN_clicked()
{
    serial1send('D');
}


void MainWindow::on_sendPOLY3_clicked()
{
    serial1send('R');
}


void MainWindow::on_sendPOLY5_clicked()
{
    serial1send('T');
}


void MainWindow::on_sendLINEAR_clicked()
{
    serial1send('U');
}

