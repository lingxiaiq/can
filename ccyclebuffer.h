//环形缓冲区头文件
#ifndef CCYCLEBUFFER_H
#define CCYCLEBUFFER_H

#include <QtGlobal>
#include <QReadWriteLock>

class CCycleBuffer
{
public:
    bool isFull();
    bool isEmpty();
    void Empty();
    quint32 GetLength();
    quint32 GetFreeLength();
    CCycleBuffer(quint32 size);
    ~CCycleBuffer();
    quint32 Write(quint8 * buf, quint32 count);
    quint32 Read(quint8 * buf, quint32 count);

private:
    bool m_bEmpty;
    bool m_bFull;
    char * m_pBuf;
    quint32 m_nBufSize;
    quint32 m_nReadPos;
    quint32 m_nWritePos;

    QReadWriteLock *lock;
};

#endif // CCYCLEBUFFER_H
