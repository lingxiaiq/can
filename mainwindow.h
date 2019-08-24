#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "thread.h"
#include <QButtonGroup>
#include <QBuffer>
#include "ccyclebuffer.h"
#include <QTimer>

extern CCycleBuffer cycleBuffer ;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     static QByteArray array;
   // CCycleBuffer cycleBuffer;
    static QBuffer Mybuffer;
    void clearMaxMin();

protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
private slots:
    void on_send_clicked();
    void msg(uint* str);
    void stopcan(int v);
    void startcan(int v);
    void on_can0_toggled(bool checked);
    void on_can1_toggled(bool checked);

    void on_pushButton_Start_clicked();
    void timerTestUpdate();
private:
    Ui::MainWindow *ui;

    int socket;
    struct sockaddr_can addr;
    Thread *t;
    QButtonGroup* btg;
    QString showString;
    quint32 id;
    QPixmap pixMapSmallRed;
    QPixmap pixMapSmallGreen;
    QPixmap pixMapSmallGray;
    bool firstNum301;
    bool firstNum302;
    bool firstNum303;
    bool firstNum304;

    quint8 cycleReadBuf[11];
    qint32 realDis[4];

    qint32 maxF1;
    qint32 minF1;
    qint32 maxF2;
    qint32 minF2;
    qint32 maxF3;
    qint32 minF3;
    qint32 maxF4;
    qint32 minF4;

    qint32 maxB1;
    qint32 minB1;
    qint32 maxB2;
    qint32 minB2;
    qint32 maxB3;
    qint32 minB3;
    qint32 maxB4;
    qint32 minB4;

    qint32 maxL1;
    qint32 minL1;
    qint32 maxL2;
    qint32 minL2;
    qint32 maxL3;
    qint32 minL3;
    qint32 maxL4;
    qint32 minL4;

    qint32 maxR1;
    qint32 minR1;
    qint32 maxR2;
    qint32 minR2;
    qint32 maxR3;
    qint32 minR3;
    qint32 maxR4;
    qint32 minR4;

    qint32 defDisF1;
    qint32 defDisF2;
    qint32 defDisF3;
    qint32 defDisF4;

    qint32 defDisB1;
    qint32 defDisB2;
    qint32 defDisB3;
    qint32 defDisB4;

    qint32 defDisL1;
    qint32 defDisL2;
    qint32 defDisL3;
    qint32 defDisL4;

    qint32 defDisR1;
    qint32 defDisR2;
    qint32 defDisR3;
    qint32 defDisR4;

    qint32 subDisDefTest;
    qint32 subDisMaxMin;

    qint32 factoryTemp;
    qint32 subTempFacAEB;
    qint32 testTimeSec;
    qint32 testCount;

    QTimer *timerTest;
    qint8 startBit;
};
#endif // MAINWINDOW_H
