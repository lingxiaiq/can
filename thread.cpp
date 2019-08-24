#include "thread.h"
#include <QTextStream>
#include <QFile>
#include <mainwindow.h>
Thread::Thread(int s,QObject *parent) :
    QThread(parent)
{
    socket  = s;
    running = true;
}

void Thread::run()
{
    int nbytes;
    int len;
    uint nFrame = 0;
    struct can_frame frame;
    struct sockaddr_can addr;
    uint sendbuf[10] = {0};
    quint8 maya[11] = {0};

    while(running)
    {
        nbytes=recvfrom(socket,&frame,sizeof(struct can_frame),0,(struct sockaddr *)&addr,(socklen_t*)&len);
        if(frame.can_dlc == 8)
        {
            nFrame++;

            maya[0] = nFrame;
            maya[1] = (uchar)(frame.can_id/256);
            maya[2] = (uchar)frame.can_id;
            maya[3] = frame.data[0];
            maya[4] = frame.data[1];
            maya[5] = frame.data[2];
            maya[6] = frame.data[3];
            maya[7] = frame.data[4];
            maya[8] = frame.data[5];
            maya[9] = frame.data[6];
            maya[10] = frame.data[7];
            cycleBuffer.Write(maya, 11);

            emit msg(sendbuf);

        }

    }

}

void Thread::stop()
{
    running = false;
}
