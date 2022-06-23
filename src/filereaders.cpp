/* filereaders.cpp
 *
 * Copyright (C) 2021-2022 Siddharudh P T <siddharudh@gmail.com>
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

#include "filereaders.h"

#include <QDateTime>
#include <QDebug>
#include <cstdio>


namespace RQPlayer {

VideoFileReader::VideoFileReader(const QString &fileName,
                                 const QVideoSurfaceFormat &format,
                                 QObject *parent)
    : QThread(parent), m_fileName(fileName), m_format(format),
      m_stopRequested(false)
{
}

void VideoFileReader::run()
{
    if (m_fileName.isEmpty()) {
        qDebug() << "VideoFileReader: Empty file name";
        return;
    }
    if (!m_format.isValid() || m_format.frameSize().isEmpty()
            || m_format.pixelFormat() == QVideoFrame::Format_Invalid) {
        qDebug() << "VideoFileReader: Invalid format:" << m_format;
        return;
    }
    // qDebug() << "VideoFileReader: format:" << m_format;
    const auto bytesCount = m_format.frameWidth() * m_format.frameHeight() * 2;
    const auto frameDurMsec
            = m_format.frameRate() > 0 ? 1000.0 /  m_format.frameRate() : 40;
    while (!m_stopRequested) {
        qDebug() << "VideoFileReader: Attempting to open file:" << m_fileName;
        FILE *vfp = fopen(m_fileName.toStdString().c_str(), "r");
        if (vfp == nullptr) {
            qDebug() << "VideoFileReader: Failed to open file:" << m_fileName;
            QThread::sleep(1);
            continue;
        }
        qDebug() << "VideoFileReader: file opened for reading:" << m_fileName;
        QDateTime nextFrameTime = QDateTime::currentDateTime().addMSecs(
                    frameDurMsec);
        while (!m_stopRequested) {
            QVideoFrame frame(bytesCount, m_format.frameSize(),
                              m_format.frameWidth(), m_format.pixelFormat());
            frame.map(QAbstractVideoBuffer::WriteOnly);
            size_t c = fread(frame.bits(), bytesCount, 1, vfp);
            frame.unmap();
            if (c) {
                emit frameReady(frame);
                auto sleepMsec = QDateTime::currentDateTime().msecsTo(
                            nextFrameTime);
                if (sleepMsec > 0) {
                    QThread::usleep(sleepMsec * 1000);
                }
                nextFrameTime = nextFrameTime.addMSecs(frameDurMsec);
            }
            else if (feof(vfp) || ferror(vfp)) {
                qDebug() << "AudioFileReader: EOF or Error on file:" << m_fileName;
                fclose(vfp);
                break;
            }
            else {
                qDebug() << "VideoFileReader: Failed to read:" << m_fileName;
            }
        }
    }
}


AudioFileReader::AudioFileReader(const QString &fileName,
                                 const QAudioFormat &format,
                                 int videoFrameRate,
                                 QObject *parent)
    : QThread(parent), m_fileName(fileName), m_format(format),
      m_videoFrameRate(videoFrameRate), m_stopRequested(false)
{
}

void AudioFileReader::run()
{
    if (m_fileName.isEmpty()) {
        qDebug() << "AudioFileReader: Empty file name";
        return;
    }
    if (!m_format.isValid()) {
        qDebug() << "AudioFileReader: Invalid format:" << m_format;
        return;
    }
    qDebug() << "AudioFileReader: format:" << m_format;
    const auto frameDurMsec
            = m_videoFrameRate > 0 ? 1000.0 /  m_videoFrameRate : 40;
    const auto numAudioFramesPerVideoFrame
            = m_format.framesForDuration(frameDurMsec * 1000);
    while (!m_stopRequested) {
        qDebug() << "AudioFileReader: Attempting to open file:" << m_fileName;
        FILE *afp = fopen(m_fileName.toStdString().c_str(), "r");
        if (afp == nullptr) {
            qDebug() << "AudioFileReader: Failed to open file:" << m_fileName;
            QThread::sleep(1);
            continue;
        }
        qDebug() << "AudioFileReader: file opened for reading:" << m_fileName;
        QDateTime nextFrameTime = QDateTime::currentDateTime().addMSecs(frameDurMsec);
        while (!m_stopRequested) {
            QAudioBuffer abuf(numAudioFramesPerVideoFrame, m_format);
            size_t c = fread(abuf.data(), abuf.byteCount(), 1, afp);
            if (c) {
                emit samplesReady(abuf);
                auto sleepMsec = QDateTime::currentDateTime().msecsTo(
                            nextFrameTime);
                if (sleepMsec > 0) {
                    QThread::usleep(sleepMsec * 1000);
                }
                nextFrameTime = nextFrameTime.addMSecs(frameDurMsec);
            }
            else if (feof(afp) || ferror(afp)) {
                qDebug() << "AudioFileReader: EOF or Error on file:" << m_fileName;
                fclose(afp);
                break;
            }
            else {
                qDebug() << "VideoFileReader: Failed to read:" << m_fileName;
            }
        }
    }
}

} // namespace RQPlayer
