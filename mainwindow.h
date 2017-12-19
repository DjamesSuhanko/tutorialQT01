#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSerialPort *serialPort;

    bool ON = false;

public slots:
    void connectToSerial();
    void startReading();

signals:
    void startReading(bool ok);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
