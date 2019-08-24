#include <sys/ioctl.h>
#include <fcntl.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QTextStream>
#include <QFile>
#include <QTimer>
#include <QPushButton>
#include <QDebug>
CCycleBuffer cycleBuffer =  CCycleBuffer(2000);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pixMapSmallRed.load(":/new/prefix1/picSmallRed.png");
    pixMapSmallGreen.load(":/new/prefix1/picSmallGreen.png");
    pixMapSmallGray.load(":/new/prefix1/picSmallGray.png");

    clearMaxMin();

    startcan(0);

    QFile fileConfig("AEB_Config.txt");
    fileConfig.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream streamConfig(&fileConfig);
//    streamConfig.setCodec("utf-8");
    /*


    quint32 SubDisDefTest;
    quint32 SubDisMaxMin;

    quint32 factoryTemp;
    quint32 SubTempFacAEB;
     */
    streamConfig.seek(0);
    defDisF1 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisF2 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisF3 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisF4 = streamConfig.readLine().mid(0, 3).toUInt();

    defDisB1 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisB2 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisB3 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisB4 = streamConfig.readLine().mid(0, 3).toUInt();

    defDisL1 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisL2 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisL3 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisL4 = streamConfig.readLine().mid(0, 3).toUInt();

    defDisR1 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisR2 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisR3 = streamConfig.readLine().mid(0, 3).toUInt();
    defDisR4 = streamConfig.readLine().mid(0, 3).toUInt();

    subDisDefTest = streamConfig.readLine().mid(0, 3).toUInt();
   // ui->label->append("subDisDefTest=" + QString::number(subDisDefTest));
    subDisMaxMin = streamConfig.readLine().mid(0, 3).toInt();
   // ui->label->append("subDisMaxMin=" + QString::number(subDisMaxMin));
    factoryTemp = streamConfig.readLine().mid(0, 3).toUInt();
    subTempFacAEB = streamConfig.readLine().mid(0, 3).toUInt();
    testTimeSec = streamConfig.readLine().mid(0, 3).toUInt();

    ui->lineEdit_F1Define->setText(QString::number(defDisF1));
    ui->lineEdit_F2Define->setText(QString::number(defDisF2));
    ui->lineEdit_F3Define->setText(QString::number(defDisF3));
    ui->lineEdit_F4Define->setText(QString::number(defDisF4));
    ui->lineEdit_B1Define->setText(QString::number(defDisB1));
    ui->lineEdit_B2Define->setText(QString::number(defDisB2));
    ui->lineEdit_B3Define->setText(QString::number(defDisB3));
    ui->lineEdit_B4Define->setText(QString::number(defDisB4));
    ui->lineEdit_L1Define->setText(QString::number(defDisL1));
    ui->lineEdit_L2Define->setText(QString::number(defDisL2));
    ui->lineEdit_L3Define->setText(QString::number(defDisL3));
    ui->lineEdit_L4Define->setText(QString::number(defDisL4));
    ui->lineEdit_R1Define->setText(QString::number(defDisR1));
    ui->lineEdit_R2Define->setText(QString::number(defDisR2));
    ui->lineEdit_R3Define->setText(QString::number(defDisR3));
    ui->lineEdit_R4Define->setText(QString::number(defDisR4));
    testCount = testTimeSec;
    startBit = 0;
    timerTest = new QTimer;
    connect(timerTest, SIGNAL(timeout()), this, SLOT(timerTestUpdate()));
   // timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    stopcan(btg->checkedId());
}

void MainWindow::msg(uint* str)
{

   // QString str1 = QString::fromUtf8(str));    //  Qt4

    cycleBuffer.Read(cycleReadBuf, 11);
    id = cycleReadBuf[1]*256 + cycleReadBuf[2];
    showString =  QString::number(cycleReadBuf[0]) +  "\t" + QString::number(id, 16) +  " "
            + QString::number(cycleReadBuf[3]) + " " + QString::number(cycleReadBuf[4]) + " "
            + QString::number(cycleReadBuf[5]) + " " + QString::number(cycleReadBuf[6]) + " "
            + QString::number(cycleReadBuf[7]) + " " + QString::number(cycleReadBuf[8]) + " "
            + QString::number(cycleReadBuf[9]) + " " + QString::number(cycleReadBuf[10]);
    ui->label->append(showString);
    if(id == 0x301)
    {
        realDis[0] = cycleReadBuf[4]*2;
        realDis[1] = cycleReadBuf[5]*2;
        realDis[2] = cycleReadBuf[6]*2;
        realDis[3] = cycleReadBuf[7]*2;
        ui->lineEdit_Front1->setText(realDis[0] == 510 ? "NB" : QString::number(realDis[0]));
        ui->lineEdit_Front2->setText(realDis[1] == 510 ? "NB" : QString::number(realDis[1]));
        ui->lineEdit_Front3->setText(realDis[2] == 510 ? "NB" : QString::number(realDis[2]));
        ui->lineEdit_Front4->setText(realDis[3] == 510 ? "NB" : QString::number(realDis[3]));
        if(startBit == 1 )
        {
            if(qAbs(realDis[0] - defDisF1) > subDisDefTest)
            {
                ui->label_PassF1->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[1] - defDisF2) > subDisDefTest)
            {
                ui->label_PassF2->setPixmap(pixMapSmallRed);
            }
            if(realDis[2] < defDisF3)
            {
                ui->label_PassF3->setPixmap(pixMapSmallRed);
            }
            if(realDis[3] < defDisF4)
            {
                ui->label_PassF4->setPixmap(pixMapSmallRed);
            }
            if(firstNum301)
            {//first come
                firstNum301 = false;
                maxF1 = realDis[0];
                minF1 = realDis[0];
                maxF2 = realDis[1];
                minF2 = realDis[1];
                maxF3 = realDis[2];
                minF3 = realDis[2];
                maxF4 = realDis[3];
                minF4 = realDis[3];
                ui->lineEdit_F1Max->setText(QString::number(maxF1));
                ui->lineEdit_F1Min->setText(QString::number(minF1));
                ui->lineEdit_F2Max->setText(QString::number(maxF2));
                ui->lineEdit_F2Min->setText(QString::number(minF2));
                ui->lineEdit_F3Max->setText(QString::number(maxF3));
                ui->lineEdit_F3Min->setText(QString::number(minF3));
                ui->lineEdit_F4Max->setText(QString::number(maxF4));
                ui->lineEdit_F4Min->setText(QString::number(minF4));

            }else
            {//followed
                //F1
                if(realDis[0] > maxF1)
                {
                    maxF1 = realDis[0];
                    ui->lineEdit_F1Max->setText(QString::number(maxF1));
                }
                if(realDis[0] < minF1)
                {
                    minF1 = realDis[0];
                    ui->lineEdit_F1Min->setText(QString::number(minF1));
                }

                //F2
                if(realDis[1] > maxF2)
                {
                    maxF2 = realDis[1];
                    ui->lineEdit_F2Max->setText(QString::number(maxF2));
                }
                if(realDis[1] < minF2)
                {
                    minF2 = realDis[1];
                    ui->lineEdit_F2Min->setText(QString::number(minF2));
                }

                //F3
                if(realDis[2] > maxF3)
                {
                    maxF3 = realDis[2];
                    ui->lineEdit_F3Max->setText(QString::number(maxF3));
                }
                if(realDis[2] < minF3)
                {
                    minF3 = realDis[2];
                    ui->lineEdit_F3Min->setText(QString::number(minF3));
                }
                //F4
                if(realDis[3] > maxF4)
                {
                    maxF4 = realDis[3];
                    ui->lineEdit_F4Max->setText(QString::number(maxF4));
                }
                if(realDis[3] < minF4)
                {
                    minF4 = realDis[3];
                    ui->lineEdit_F4Min->setText(QString::number(minF4));
                }

                if(qAbs(maxF1 - minF1) > subDisMaxMin)
                {
                    ui->label_PassF1->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxF2 - minF2) > subDisMaxMin)
                {
                    ui->label_PassF2->setPixmap(pixMapSmallRed);
                }
            }
        }

    }else if (id == 0x302)
    {
        realDis[0] = cycleReadBuf[4]*2;
        realDis[1] = cycleReadBuf[5]*2;
        realDis[2] = cycleReadBuf[6]*2;
        realDis[3] = cycleReadBuf[7]*2;
        ui->lineEdit_Back1->setText(realDis[0] == 510 ? "NB" : QString::number(realDis[0]));
        ui->lineEdit_Back2->setText(realDis[1] == 510 ? "NB" : QString::number(realDis[1]));
        ui->lineEdit_Back3->setText(realDis[2] == 510 ? "NB" : QString::number(realDis[2]));
        ui->lineEdit_Back4->setText(realDis[3] == 510 ? "NB" : QString::number(realDis[3]));
        if(startBit == 1)
        {
            if(qAbs(realDis[0] - defDisB1) > subDisDefTest)
            {
                ui->label_PassB1->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[1] - defDisB2) > subDisDefTest)
            {
                ui->label_PassB2->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[2] - defDisB3) > subDisDefTest)
            {
                ui->label_PassB3->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[3] - defDisB4) > subDisDefTest)
            {
                ui->label_PassB4->setPixmap(pixMapSmallRed);
            }
            if(firstNum302)
            {//first come
                firstNum302 = false;
                maxB1 = realDis[0];
                minB1 = realDis[0];
                maxB2 = realDis[1];
                minB2 = realDis[1];
                maxB3 = realDis[2];
                minB3 = realDis[2];
                maxB4 = realDis[3];
                minB4 = realDis[3];
                ui->lineEdit_B1Max->setText(QString::number(maxB1));
                ui->lineEdit_B1Min->setText(QString::number(minB1));
                ui->lineEdit_B2Max->setText(QString::number(maxB2));
                ui->lineEdit_B2Min->setText(QString::number(minB2));
                ui->lineEdit_B3Max->setText(QString::number(maxB3));
                ui->lineEdit_B3Min->setText(QString::number(minB3));
                ui->lineEdit_B4Max->setText(QString::number(maxB4));
                ui->lineEdit_B4Min->setText(QString::number(minB4));
            }else
            {//followed
                //B1
                if(realDis[0] > maxB1)
                {
                    maxB1 = realDis[0];
                    ui->lineEdit_B1Max->setText(QString::number(maxB1));
                }
                if(realDis[0] < minB1)
                {
                    minB1 = realDis[0];
                    ui->lineEdit_B1Min->setText(QString::number(minB1));
                }

                //B2
                if(realDis[1] > maxB2)
                {
                    maxB2 = realDis[1];
                    ui->lineEdit_B2Max->setText(QString::number(maxB2));
                }
                if(realDis[1] < minB2)
                {
                    minB2 = realDis[1];
                    ui->lineEdit_B2Min->setText(QString::number(minB2));
                }

                //B3
                if(realDis[2] > maxB3)
                {
                    maxB3 = realDis[2];
                    ui->lineEdit_B3Max->setText(QString::number(maxB3));
                }
                if(realDis[2] < minB3)
                {
                    minB3 = realDis[2];
                    ui->lineEdit_B3Min->setText(QString::number(minB3));
                }

                //B4
                if(realDis[3] > maxB4)
                {
                    maxB4 = realDis[3];
                    ui->lineEdit_B4Max->setText(QString::number(maxB4));
                }
                if(realDis[3] < minB4)
                {
                    minB4 = realDis[3];
                    ui->lineEdit_B4Min->setText(QString::number(minB4));
                }

                if(qAbs(maxB1 - minB1) > subDisMaxMin)
                {
                    ui->label_PassB1->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxB2 - minB2) > subDisMaxMin)
                {
                    ui->label_PassB2->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxB3 - minB3) > subDisMaxMin)
                {
                    ui->label_PassB3->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxB4 - minB4) > subDisMaxMin)
                {
                    ui->label_PassB4->setPixmap(pixMapSmallRed);
                }
            }
        }
    }else if (id == 0x303)
    {
        realDis[0] = cycleReadBuf[4]*2;
        realDis[1] = cycleReadBuf[5]*2;
        realDis[2] = cycleReadBuf[6]*2;
        realDis[3] = cycleReadBuf[7]*2;
        ui->lineEdit_Left1->setText(realDis[0] == 510 ? "NB" : QString::number(realDis[0]));
        ui->lineEdit_Left2->setText(realDis[1] == 510 ? "NB" : QString::number(realDis[1]));
        ui->lineEdit_Left3->setText(realDis[2] == 510 ? "NB" : QString::number(realDis[2]));
        ui->lineEdit_Left4->setText(realDis[3] == 510 ? "NB" : QString::number(realDis[3]));
        if(startBit == 1)
        {
            if(qAbs(realDis[0] - defDisL1) > subDisDefTest)
            {
                ui->label_PassL1->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[1] - defDisL2) > subDisDefTest)
            {
                ui->label_PassL2->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[2] - defDisL3) > subDisDefTest)
            {
                ui->label_PassL3->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[3] - defDisL4) > subDisDefTest)
            {
                ui->label_PassL4->setPixmap(pixMapSmallRed);
            }
            if(firstNum303)
            {//first come
                firstNum303 = false;
                maxL1 = realDis[0];
                minL1 = realDis[0];
                maxL2 = realDis[1];
                minL2 = realDis[1];
                maxL3 = realDis[2];
                minL3 = realDis[2];
                maxL4 = realDis[3];
                minL4 = realDis[3];
                ui->lineEdit_L1Max->setText(QString::number(maxL1));
                ui->lineEdit_L1Min->setText(QString::number(minL1));
                ui->lineEdit_L2Max->setText(QString::number(maxL2));
                ui->lineEdit_L2Min->setText(QString::number(minL2));
                ui->lineEdit_L3Max->setText(QString::number(maxL3));
                ui->lineEdit_L3Min->setText(QString::number(minL3));
                ui->lineEdit_L4Max->setText(QString::number(maxL4));
                ui->lineEdit_L4Min->setText(QString::number(minL4));
            }else
            {//followed
                //L1
                if(realDis[0] > maxL1)
                {
                    maxL1 = realDis[0];
                    ui->lineEdit_L1Max->setText(QString::number(maxL1));
                }
                if(realDis[0] < minL1)
                {
                    minL1 = realDis[0];
                    ui->lineEdit_L1Min->setText(QString::number(minL1));
                }

                //L2
                if(realDis[1] > maxL2)
                {
                    maxL2 = realDis[1];
                    ui->lineEdit_L2Max->setText(QString::number(maxL2));
                }
                if(realDis[1] < minL2)
                {
                    minL2 = realDis[1];
                    ui->lineEdit_L2Min->setText(QString::number(minL2));
                }

                //L3
                if(realDis[2] > maxL3)
                {
                    maxL3 = realDis[2];
                    ui->lineEdit_L3Max->setText(QString::number(maxL3));
                }
                if(realDis[2] < minL3)
                {
                    minL3 = realDis[2];
                    ui->lineEdit_L3Min->setText(QString::number(minL3));
                }
                //L4
                if(realDis[3] > maxL4)
                {
                    maxL4 = realDis[3];
                    ui->lineEdit_L4Max->setText(QString::number(maxL4));
                }
                if(realDis[3] < minL4)
                {
                    minL4 = realDis[3];
                    ui->lineEdit_L4Min->setText(QString::number(minL4));
                }
                if(qAbs(maxL1 - minL1) > subDisMaxMin)
                {
                    ui->label_PassL1->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxL2 - minL2) > subDisMaxMin)
                {
                    ui->label_PassL2->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxL3 - minL3) > subDisMaxMin)
                {
                    ui->label_PassL3->setPixmap(pixMapSmallRed);
                }
                if(qAbs(maxL4 - minL4) > subDisMaxMin)
                {
                    ui->label_PassL4->setPixmap(pixMapSmallRed);
                }
            }
        }
    }else if (id == 0x304)
    {
        realDis[0] = cycleReadBuf[4]*2;
        realDis[1] = cycleReadBuf[5]*2;
        realDis[2] = cycleReadBuf[6]*2;
        realDis[3] = cycleReadBuf[7]*2;
        ui->lineEdit_Right1->setText(realDis[0] == 510 ? "NB" : QString::number(realDis[0]));
        ui->lineEdit_Right2->setText(realDis[1] == 510 ? "NB" : QString::number(realDis[1]));
        ui->lineEdit_Right3->setText(realDis[2] == 510 ? "NB" : QString::number(realDis[2]));
        ui->lineEdit_Right4->setText(realDis[3] == 510 ? "NB" : QString::number(realDis[3]));
        if(startBit == 1)
        {
            if(qAbs(realDis[0] - defDisR1) > subDisDefTest)
            {
                ui->label_PassR1->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[1] - defDisR2) > subDisDefTest)
            {
                ui->label_PassR2->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[2] - defDisR3) > subDisDefTest)
            {
                ui->label_PassR3->setPixmap(pixMapSmallRed);
            }
            if(qAbs(realDis[3] - defDisR4) > subDisDefTest)
            {
                ui->label_PassR4->setPixmap(pixMapSmallRed);
            }
            if(firstNum304)
            {//first come
                firstNum304 = false;
                maxR1 = realDis[0];
                minR1 = realDis[0];
                maxR2 = realDis[1];
                minR2 = realDis[1];
                maxR3 = realDis[2];
                minR3 = realDis[2];
                maxR4 = realDis[3];
                minR4 = realDis[3];
                ui->lineEdit_R1Max->setText(QString::number(maxR1));
                ui->lineEdit_R1Min->setText(QString::number(minR1));
                ui->lineEdit_R2Max->setText(QString::number(maxR2));
                ui->lineEdit_R2Min->setText(QString::number(minR2));
                ui->lineEdit_R3Max->setText(QString::number(maxR3));
                ui->lineEdit_R3Min->setText(QString::number(minR3));
                ui->lineEdit_R4Max->setText(QString::number(maxR4));
                ui->lineEdit_R4Min->setText(QString::number(minR4));
            }else
            {//followed
                //R1
                if(realDis[0] > maxR1)
                {
                    maxR1 = realDis[0];
                    ui->lineEdit_R1Max->setText(QString::number(maxR1));
                }
                if(realDis[0] < minR1)
                {
                    minR1 = realDis[0];
                    ui->lineEdit_R1Min->setText(QString::number(minR1));
                }

                //R2
                if(realDis[1] > maxR2)
                {
                    maxR2 = realDis[1];
                    ui->lineEdit_R2Max->setText(QString::number(maxR2));
                }
                if(realDis[1] < minR2)
                {
                    minR2 = realDis[1];
                    ui->lineEdit_R2Min->setText(QString::number(minR2));
                }

                //R3
                if(realDis[2] > maxR3)
                {
                    maxR3 = realDis[2];
                    ui->lineEdit_R3Max->setText(QString::number(maxR3));
                }
                if(realDis[2] < minR3)
                {
                    minR3 = realDis[2];
                    ui->lineEdit_R3Min->setText(QString::number(minR3));
                }

                //R4
                if(realDis[3] > maxR4)
                {
                    maxR4 = realDis[3];
                    ui->lineEdit_R4Max->setText(QString::number(maxR4));
                }
                if(realDis[3] < minR4)
                {
                    minR4 = realDis[3];
                    ui->lineEdit_R4Min->setText(QString::number(minR4));
                }
                if(startBit == 1)
                {
                    if(qAbs(maxR1 - minR1) > subDisMaxMin)
                    {
                        ui->label_PassR1->setPixmap(pixMapSmallRed);
                    }
                    if(qAbs(maxR2 - minR2) > subDisMaxMin)
                    {
                        ui->label_PassR2->setPixmap(pixMapSmallRed);
                    }
                    if(qAbs(maxR3 - minR3) > subDisMaxMin)
                    {
                        ui->label_PassR3->setPixmap(pixMapSmallRed);
                    }
                    if(qAbs(maxR4 - minR4) > subDisMaxMin)
                    {
                        ui->label_PassR4->setPixmap(pixMapSmallRed);
                    }
                }
            }
        }
    }
}

void MainWindow::on_send_clicked()
{
    struct can_frame frame;
    memset(&frame,0,sizeof(struct can_frame));
    std::string  str=ui->edit->text().toStdString();

    if(str.length() > 8)
    {
        QMessageBox::about(this,"error","length of send string must less than 8 bytes");
        return;
    }

    frame.can_id = 0x123;
    strcpy((char*)frame.data,str.c_str());
    frame.can_dlc = str.length();

    sendto(socket,&frame,sizeof(struct can_frame),0,(struct sockaddr*)&addr,sizeof(addr));
    return;
}

void MainWindow::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    destroy();
    exit(0);
}

void MainWindow::startcan(int v)
{
   int ret = 0;
    if(v == 0)
    {
        system("ifconfig can0 down");
        system("ip link set can0 up type can bitrate 500000 triple-sampling on");
        system("ifconfig can0 up");
    }
    else
    {
        system("ifconfig can1 down");
        system("ip link set can1 up type can bitrate 500000 triple-sampling on");
        system("ifconfig can1 up");
    }

    socket =  ::socket(PF_CAN,SOCK_RAW,CAN_RAW);

    struct can_filter filter[4];
    int i;
    for (i = 0; i < 4; ++i) {
        filter[i].can_id = 0x301 + i;
        filter[i].can_mask = CAN_SFF_MASK;
    }
    ret = setsockopt(socket, SOL_CAN_RAW, CAN_RAW_FILTER, &filter,sizeof(filter));
    if (ret < 0) {
        QMessageBox::about(this,"error","in filter error");
        exit(1);
    }
    struct ifreq ifr;
    strcpy((char *)(ifr.ifr_name),v == 0 ? "can0" : "can1");
    ioctl(socket,SIOCGIFINDEX,&ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(socket,(struct sockaddr*)&addr,sizeof(addr));
    if (ret < 0)
     {
     QMessageBox::about(this,"error","in bind error");
     exit(1);
     }

    t = NULL;

    t = new Thread(socket);

    connect(t,SIGNAL(msg(uint*)),this,SLOT(msg(uint*)));

    t->start();
}

void MainWindow::stopcan(int v)
{
    if(t)
    {
        t->stop();
        t->deleteLater();
    }

    ::close(socket);

    if(v == 0)
        system("ifconfig can0 down");
    else
        system("ifconfig can1 down");
}

void MainWindow::on_can0_toggled(bool checked)
{
    if(checked)
    {
        stopcan(1);
        startcan(0);
    }
}

void MainWindow::on_can1_toggled(bool checked)
{
    if(checked)
    {
        stopcan(0);
        startcan(1);
    }
}
void MainWindow::clearMaxMin()
{
    firstNum301 = true;
    firstNum302 = true;
    firstNum303 = true;
    firstNum304 = true;
    maxF1 = 0;
    minF1 = 0;
    maxF2 = 0;
    minF2 = 0;
    maxF3 = 0;
    minF3 = 0;
    maxF4 = 0;
    minF4 = 0;

    maxB1 = 0;
    minB1 = 0;
    maxB2 = 0;
    minB2 = 0;
    maxB3 = 0;
    minB3 = 0;
    maxB4 = 0;
    minB4 = 0;

    maxL1 = 0;
    minL1 = 0;
    maxL2 = 0;
    minL2 = 0;
    maxL3 = 0;
    minL3 = 0;
    maxL4 = 0;
    minL4 = 0;

    maxR1 = 0;
    minR1 = 0;
    maxR2 = 0;
    minR2 = 0;
    maxR3 = 0;
    minR3 = 0;
    maxR4 = 0;
    minR4 = 0;
    ui->lineEdit_F1Max->clear();
    ui->lineEdit_F1Min->clear();
    ui->lineEdit_F2Max->clear();
    ui->lineEdit_F2Min->clear();
    ui->lineEdit_F3Max->clear();
    ui->lineEdit_F3Min->clear();
    ui->lineEdit_F4Max->clear();
    ui->lineEdit_F4Min->clear();

    ui->lineEdit_B1Max->clear();
    ui->lineEdit_B1Min->clear();
    ui->lineEdit_B2Max->clear();
    ui->lineEdit_B2Min->clear();
    ui->lineEdit_B3Max->clear();
    ui->lineEdit_B3Min->clear();
    ui->lineEdit_B4Max->clear();
    ui->lineEdit_B4Min->clear();

    ui->lineEdit_L1Max->clear();
    ui->lineEdit_L1Min->clear();
    ui->lineEdit_L2Max->clear();
    ui->lineEdit_L2Min->clear();
    ui->lineEdit_L3Max->clear();
    ui->lineEdit_L3Min->clear();
    ui->lineEdit_L4Max->clear();
    ui->lineEdit_L4Min->clear();

    ui->lineEdit_R1Max->clear();
    ui->lineEdit_R1Min->clear();
    ui->lineEdit_R2Max->clear();
    ui->lineEdit_R2Min->clear();
    ui->lineEdit_R3Max->clear();
    ui->lineEdit_R3Min->clear();
    ui->lineEdit_R4Max->clear();
    ui->lineEdit_R4Min->clear();
}

void MainWindow::on_pushButton_Start_clicked()
{
    if(startBit == 0)
    {
        //startBit = 1;
        testCount = testTimeSec + 5;
        //ui->pushButton_Start->setStyleSheet("QPushButton{ font-family:'Microsoft YaHei';font-size:12px;color:#666666;}");
       // ui->pushButton_Start->setStyleSheet("QPushButton{text-size : 200;}");
        ui->pushButton_Start->setText("Wait");
      //  qDebug()<<"Test Start";
        timerTest->start(1000);
        ui->label_PassF1->setPixmap(pixMapSmallGray);
        ui->label_PassF2->setPixmap(pixMapSmallGray);
        ui->label_PassF3->setPixmap(pixMapSmallGray);
        ui->label_PassF4->setPixmap(pixMapSmallGray);

        ui->label_PassB1->setPixmap(pixMapSmallGray);
        ui->label_PassB2->setPixmap(pixMapSmallGray);
        ui->label_PassB3->setPixmap(pixMapSmallGray);
        ui->label_PassB4->setPixmap(pixMapSmallGray);

        ui->label_PassL1->setPixmap(pixMapSmallGray);
        ui->label_PassL2->setPixmap(pixMapSmallGray);
        ui->label_PassL3->setPixmap(pixMapSmallGray);
        ui->label_PassL4->setPixmap(pixMapSmallGray);

        ui->label_PassR1->setPixmap(pixMapSmallGray);
        ui->label_PassR2->setPixmap(pixMapSmallGray);
        ui->label_PassR3->setPixmap(pixMapSmallGray);
        ui->label_PassR4->setPixmap(pixMapSmallGray);


        clearMaxMin();
    }else
    {
        startBit = 0;
        timerTest->stop();
       // ui->pushButton_Start->setStyleSheet("QPushButton{text-size : 150;}");
        ui->pushButton_Start->setText("Start");
       // qDebug()<<"Test Break Off";
    }
}

void MainWindow::timerTestUpdate()
{
    if(testCount > testTimeSec)
    {
        ui->pushButton_Start->setText(QString::number(testCount - testTimeSec));
    }else if(testCount > 0)
    {

        if(testCount == testTimeSec)
        {
            startBit = 1;
            ui->pushButton_Start->setText("GO!");
        }else{
            ui->pushButton_Start->setText(QString::number(testCount));
        }
    }else
    {
        startBit = 0;
        timerTest->stop();
       //ui->pushButton_Start->setStyleSheet("QPushButton{text-size : 150;}");
       // qDebug()<<"Test Complete";
        ui->pushButton_Start->setText("Start");
        //do some other thing
    }
    testCount--;
}



















