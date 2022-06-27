/* framespresenter.cpp
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

#include "framespresenter.h"

namespace RQPlayer {

FramesPresenter::FramesPresenter(QObject *parent)
    : QObject(parent)
{
}

FramesPresenter::~FramesPresenter()
{
}

void FramesPresenter::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface && m_surface->isActive() && m_surface != surface) {
        m_surface->stop();
    }
    m_surface = surface;
    if (m_surface && m_format.isValid()) {
        m_format = m_surface->nearestFormat(m_format);
        m_surface->start(m_format);
    }
    emit videoSurfaceChanged(m_surface);
}

void FramesPresenter::setFormat(const QVideoSurfaceFormat &format)
{
    m_format = format;
    if (m_surface && m_format.isValid()) {
        if (m_surface->isActive()) {
            m_surface->stop();
        }
        m_format = m_surface->nearestFormat(m_format);
        m_surface->start(m_format);
    }
}

void FramesPresenter::presentFrame(const QVideoFrame &frame)
{
    if (m_surface && frame.isValid()) {
        m_surface->present(frame);
    }
}

} // namespace RQPlayer
