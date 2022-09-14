/* orchestrator.h
 *
 * Copyright (C) 2022 Siddharudh P T <siddharudh@gmail.com>
 *
 * This file is part of RQPlayer.
 *
 * RQPlayer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RQPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RQPlayer.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef RQPLAYER_ORCHESTRATOR_H
#define RQPLAYER_ORCHESTRATOR_H


#include <QThread>
#include <QVideoFrame>
#include <QAudioBuffer>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInteger>
#include <QDateTime>

namespace RQPlayer {

class Orchestrator: public QThread
{
    Q_OBJECT
public:
    Orchestrator();
    virtual ~Orchestrator();

    void stop();

public slots:
    void enqueueVideoFrame(const QVideoFrame &frame);
    void enqueueAudioFrame(const QAudioBuffer &abuf);

signals:
    void videoFrameReady(const QVideoFrame &frame);
    void audioFrameReady(const QAudioBuffer &abuf);

protected:
    void run() override;

private:
    int videoFrameQueueSize() const;
    int audioFrameQueueSize() const;
    bool sendVideoFrame();
    bool sendAudioFrame();

    QAtomicInteger<bool> m_stopRequested;

    QQueue<QVideoFrame> m_videoFrameQueue;
    QQueue<QAudioBuffer> m_audioFrameQueue;

    mutable QMutex m_videoQueueMutex, m_audioQueueMutex;
    QWaitCondition m_videoQueueNotFull, m_audioQueueNotFull;
};

} // namespace RQPlayer

#endif // RQPLAYER_ORCHESTRATOR_H
