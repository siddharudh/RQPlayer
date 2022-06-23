/* framespresenter.h
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

#ifndef RQPLAYER_FRAMESPRESENTER_H
#define RQPLAYER_FRAMESPRESENTER_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

namespace RQPlayer {

class FramesPresenter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)

public:
    explicit FramesPresenter(QObject *parent = nullptr);
    ~FramesPresenter();

    QAbstractVideoSurface *videoSurface() const { return m_surface; }
    void setVideoSurface(QAbstractVideoSurface *surface);

    const QVideoSurfaceFormat &format() const { return m_format; }
    void setFormat(const QVideoSurfaceFormat &format);

public slots:
    void presentFrame(const QVideoFrame &frame);

private:
    QAbstractVideoSurface *m_surface = nullptr;
    QVideoSurfaceFormat m_format;

};

} // namespace RQPlayer

#endif // RQPLAYER_FRAMESPRESENTER_H
