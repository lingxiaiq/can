#include "ccyclebuffer.h"
#include <string.h>

CCycleBuffer::CCycleBuffer(quint32 size)
{
    m_nBufSize = size;
    m_nReadPos = 0;
    m_nWritePos = 0;
    m_pBuf = new char[m_nBufSize];
    m_bEmpty = true;
    m_bFull = false;

    lock = new QReadWriteLock;
}

CCycleBuffer::~CCycleBuffer()
{
    delete[] m_pBuf;
    delete  lock;
}

quint32 CCycleBuffer::Write(quint8 * buf, quint32 count)
{
    if(count <= 0)
        return 0;

    lock->lockForWrite();

    m_bEmpty = false;

    // 缓冲区已满，不能继续写入
    if(m_bFull)
    {
        lock->unlock();
        return 0;
    }
    else if(m_nReadPos == m_nWritePos) // 缓冲区为空（全满的情况上面刚已处理）时
    {
        /*                          == 内存模型 ==
        (empty)            m_nReadPos                (empty)
        |----------------------------------|-----------------------------------------|
        m_nWritePos									m_nBufSize
        */
        quint32 leftcount = m_nBufSize - m_nWritePos;
        if(leftcount >= count)
        {
            memcpy(m_pBuf + m_nWritePos, buf, count);
            m_nWritePos += count;

            if(m_nWritePos == m_nReadPos + m_nBufSize)	// 应该是必须的
                m_nWritePos = m_nReadPos = 0;

            m_bFull = (m_nWritePos == m_nReadPos);	//

            if( 0 == count)		// 应该是必须的
            {
                m_bFull = false;
                m_bEmpty = true;
            }
            lock->unlock();
            return count;
        }
        else
        {
            memcpy(m_pBuf + m_nWritePos, buf, leftcount);
            m_nWritePos = (m_nReadPos >= count - leftcount) ? count - leftcount : m_nWritePos;
            memcpy(m_pBuf, buf + leftcount, m_nWritePos);

            m_bFull = (m_nWritePos == m_nReadPos);

            if( m_nWritePos == m_nBufSize)	// 应该是必须的
            {
                m_bFull = false;
                m_bEmpty = true;
                Q_ASSERT(0 == count);
            }

            Q_ASSERT(m_nReadPos <= m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return leftcount + m_nWritePos;
        }
    }
    else if(m_nReadPos < m_nWritePos) // 有剩余空间可写入
    {
        /*                           == 内存模型 ==
        (empty)                 (data)                     (empty)
        |-------------------|----------------------------|---------------------------|
        m_nReadPos                m_nWritePos       (leftcount)
        */
        // 剩余缓冲区大小(从写入位置到缓冲区尾)
        int leftcount = m_nBufSize - m_nWritePos;
        if(leftcount >= count)   // 有足够的剩余空间存放
        {
            memcpy(m_pBuf + m_nWritePos, buf, count);
            m_nWritePos += count;

            if(m_nWritePos == m_nReadPos + m_nBufSize)	// 应该是必须的
                m_nWritePos = m_nReadPos = 0;

            m_bFull = (m_nReadPos == m_nWritePos);

            Q_ASSERT(m_nReadPos < m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return count;
        }
        else	// 剩余空间不足 《 if(leftcount >= count) 》
        {
            // 先填充满剩余空间，再回头找空间存放
            memcpy(m_pBuf + m_nWritePos, buf, leftcount);
            /*int*/ m_nWritePos = (m_nReadPos >= count - leftcount) ? count - leftcount : m_nReadPos;
            memcpy(m_pBuf, buf + leftcount, m_nWritePos);
            m_bFull = (m_nReadPos == m_nWritePos);
            Q_ASSERT(m_nReadPos <= m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return leftcount + m_nWritePos;
        }
    }
    else	// m_nWritePos < m_nReadPos 的时候 《 m_nReadPos < m_nWritePos 》
    {
        /*                          == 内存模型 ==
        (data)                 (empty)                     (data)
        |-------------------|----------------------------|---------------------------|
        m_nWritePos    (leftcount)    m_nReadPos
        */
        quint32 leftcount = m_nReadPos - m_nWritePos;
        if(leftcount >= count)
        {
            // 有足够的剩余空间存放
            memcpy(m_pBuf + m_nWritePos, buf, count);
            m_nWritePos += count;
            m_bFull = (m_nReadPos == m_nWritePos);
            Q_ASSERT(m_nReadPos <= m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return count;
        }
        else
        {
            // 剩余空间不足时要丢弃后面的数据
            memcpy(m_pBuf + m_nWritePos, buf, leftcount);
            m_nWritePos += leftcount;
            m_bFull = (m_nReadPos == m_nWritePos);	// 一定为TRUE
            Q_ASSERT(m_bFull);
            Q_ASSERT(m_nReadPos <= m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return leftcount;
        }
    }
}

quint32 CCycleBuffer::Read(quint8 *buf, quint32 count)
{
    if(count <= 0) return 0;

    lock->lockForRead();
    m_bFull = false;

    if(m_bEmpty)       // 缓冲区空，不能继续读取数据
    {
        lock->unlock();
        return 0;
    }
    else if(m_nReadPos == m_nWritePos)   // 缓冲区满时
    {
        /*                          == 内存模型 ==
        (data)						m_nReadPos                (data)
        |--------------------------------|--------------------------------------------|
        m_nWritePos								  m_nBufSize
        */
        int leftcount = m_nBufSize - m_nReadPos;
        if(leftcount >= count)
        {
            memcpy(buf, m_pBuf + m_nReadPos, count);
            m_nReadPos += count;
            m_bEmpty = (m_nReadPos == m_nWritePos); // 应该是这样吧 m_bEmpty = (m_nReadPos == m_nWritePos + m_nBufSize);
            lock->unlock();
            return count;
        }
        else
        {
            memcpy(buf, m_pBuf + m_nReadPos, leftcount);
            m_nReadPos = (m_nWritePos >= count - leftcount) ? count - leftcount : m_nWritePos;
            memcpy(buf + leftcount, m_pBuf, m_nReadPos);
            m_bEmpty = (m_nReadPos == m_nWritePos);
            lock->unlock();
            return leftcount + m_nReadPos;
        }
    }
    else if(m_nReadPos < m_nWritePos)   // 写指针在后(未读数据是连接的)
    {
        /*                          == 内存模型 ==
        (read)                 (unread)                      (read)
        |-------------------|----------------------------|---------------------------|
        m_nReadPos                m_nWritePos                     m_nBufSize
        */
        int leftcount = m_nWritePos - m_nReadPos;
        int c = (leftcount >= count) ? count : leftcount;
        memcpy(buf, m_pBuf + m_nReadPos, c);
        m_nReadPos += c;
        m_bEmpty = (m_nReadPos == m_nWritePos);
        Q_ASSERT(m_nReadPos <= m_nBufSize);
        Q_ASSERT(m_nWritePos <= m_nBufSize);
        lock->unlock();
        return c;
    }
    else          // 读指针在后(未读数据 可能 是不连接的)
    {
        /*                          == 内存模型 ==
        (unread)                (read)                      (unread)
        |-------------------|----------------------------|---------------------------|
        m_nWritePos                  m_nReadPos                  m_nBufSize
        */
        int leftcount = m_nBufSize - m_nReadPos;
        if(leftcount > count)   // 未读缓冲区够大，直接读取数据
        {
            memcpy(buf, m_pBuf + m_nReadPos, count);
            m_nReadPos += count;
            m_bEmpty = (m_nReadPos == m_nWritePos); // 应该是这样吧 m_bEmpty = (m_nReadPos == m_nWritePos + m_nBufSize);
            Q_ASSERT(m_nReadPos <= m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return count;
        }
        else       // 未读缓冲区不足，需回到缓冲区头开始读
        {
            memcpy(buf, m_pBuf + m_nReadPos, leftcount);
            m_nReadPos = (m_nWritePos >= count - leftcount) ? count - leftcount : m_nWritePos;
            memcpy(buf + leftcount, m_pBuf, m_nReadPos);			//midify bug by zason
            m_bEmpty = (m_nReadPos == m_nWritePos);
            Q_ASSERT(m_nReadPos <= m_nBufSize);
            Q_ASSERT(m_nWritePos <= m_nBufSize);
            lock->unlock();
            return leftcount + m_nReadPos;
        }
    }
}

quint32 CCycleBuffer::GetLength()
{
    lock->lockForRead();
    if(m_bEmpty)
    {
        lock->unlock();
        return 0;
    }
    else if(m_bFull)
    {
        lock->unlock();
        return m_nBufSize;
    }
    else if(m_nReadPos < m_nWritePos)	// 中间为有效数据（字节）段
    {
        lock->unlock();
        return m_nWritePos - m_nReadPos;
    }
    else	// 两头为有效数据（字节）段
    {
        lock->unlock();
        return m_nBufSize - m_nReadPos + m_nWritePos;
    }
}

quint32 CCycleBuffer::GetFreeLength()
{
    lock->lockForRead();
    if(m_bEmpty)
    {
        lock->unlock();
        return m_nBufSize;
    }
    else if(m_bFull)
    {
        lock->unlock();
        return 0;
    }
    else if(m_nReadPos < m_nWritePos)	// 中间为有效数据（字节）段
    {
        lock->unlock();
        return m_nBufSize - m_nReadPos + m_nWritePos;
    }
    else	// 两头为有效数据（字节）段
    {
        lock->unlock();
        return m_nWritePos - m_nReadPos;
    }
}

void CCycleBuffer::Empty()
{
    lock->lockForWrite();
    m_nReadPos = 0;
    m_nWritePos = 0;
    m_bEmpty = true;
    m_bFull = false;
    lock->unlock();
}

bool CCycleBuffer::isFull()
{
    return m_bFull;
}

bool CCycleBuffer::isEmpty()
{
    return m_bEmpty;
}
