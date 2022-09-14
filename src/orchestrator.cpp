/* orchestrator.cpp
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

#include "orchestrator.h"

#include <QMutexLocker>
#include <QDateTime>

#define MAX_QUEUE_SIZE  12
#define MIN_QUEUE_SIZE  1

namespace RQPlayer {

Orchestrator::Orchestrator()
    : m_stopRequested(false)
{
}

Orchestrator::~Orchestrator()
{
}

void Orchestrator::stop()
{
    m_stopRequested = true;
}

void Orchestrator::enqueueVideoFrame(const QVideoFrame &frame)
{
    QMutexLocker lock(&m_videoQueueMutex);
    while (m_videoFrameQueue.size() >= MAX_QUEUE_SIZE) {
        m_videoQueueNotFull.wait(&m_videoQueueMutex);
    }
    m_videoFrameQueue.enqueue(frame);
}

void Orchestrator::enqueueAudioFrame(const QAudioBuffer &abuf)
{
    QMutexLocker lock(&m_audioQueueMutex);
    while (m_audioFrameQueue.size() >= MAX_QUEUE_SIZE) {
        m_audioQueueNotFull.wait(&m_audioQueueMutex);
    }
    m_audioFrameQueue.enqueue(abuf);
}

int Orchestrator::videoFrameQueueSize() const
{
    QMutexLocker lock(&m_videoQueueMutex);
    return m_videoFrameQueue.size();
}

int Orchestrator::audioFrameQueueSize() const
{
    QMutexLocker lock(&m_audioQueueMutex);
    return m_audioFrameQueue.size();
}

bool Orchestrator::sendVideoFrame()
{
    QMutexLocker lock(&m_videoQueueMutex);
    if (m_videoFrameQueue.isEmpty()) {
        return false;
    }
    emit videoFrameReady(m_videoFrameQueue.dequeue());
    m_videoQueueNotFull.wakeOne();
    return true;
}

bool Orchestrator::sendAudioFrame()
{
    QMutexLocker lock(&m_audioQueueMutex);
    if (m_audioFrameQueue.isEmpty()) {
        return false;
    }
    emit audioFrameReady(m_audioFrameQueue.dequeue());
    m_audioQueueNotFull.wakeOne();
    return true;

}

void Orchestrator::run()
{
    const auto frameDurMsec = 40;
    QDateTime nextFrameTime = QDateTime::currentDateTime().addMSecs(
                frameDurMsec);
    while (!m_stopRequested) {
        if (videoFrameQueueSize() >= MIN_QUEUE_SIZE
                && audioFrameQueueSize() >= MIN_QUEUE_SIZE) {
            sendVideoFrame();
            sendAudioFrame();
        }
        else {
            QThread::usleep(10000);
            continue;
        }
        auto sleepMsec = QDateTime::currentDateTime().msecsTo(
                    nextFrameTime);
        if (sleepMsec > 0) {
            QThread::usleep(sleepMsec * 1000);
            nextFrameTime = nextFrameTime.addMSecs(frameDurMsec);
        }
        else {
            nextFrameTime = QDateTime::currentDateTime().addMSecs(frameDurMsec);
        }
    }
}

} // namespace RQPlayer
