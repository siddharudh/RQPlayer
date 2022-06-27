/* filereaders.h
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


#ifndef RQPLAYER_FILEREADERS_H
#define RQPLAYER_FILEREADERS_H

#include <QThread>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
#include <QAudioFormat>
#include <QAudioBuffer>

#include <QAtomicInteger>

#include <cstdio>

namespace RQPlayer {

class VideoFileReader : public QThread
{
    Q_OBJECT
public:
    explicit VideoFileReader(const QString &fileName,
                             const QVideoSurfaceFormat &format,
                             QObject *parent = nullptr);
    void stop();

signals:
    void frameReady(const QVideoFrame &frame);

protected:
    void run() override;

private:
    QString m_fileName;
    QVideoSurfaceFormat m_format;
    QAtomicInteger<bool> m_stopRequested;

    FILE *m_vfp;
};

class AudioFileReader : public QThread
{
    Q_OBJECT
public:
    explicit AudioFileReader(const QString &fileName,
                             const QAudioFormat &format,
                             double videoFrameRate,
                             QObject *parent = nullptr);
    void stop();

signals:
    void samplesReady(const QAudioBuffer &abuf);

protected:
    void run() override;

private:
    QString m_fileName;
    QAudioFormat m_format;
    double m_videoFrameRate;
    QAtomicInteger<bool> m_stopRequested;

    FILE *m_afp;
};

} // namespace RQPlayer

#endif // RQPLAYER_FILEREADERS_H
