/*
 * Copyright (C) 2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 */

#ifndef GALLERY_ALBUM_DEFAULT_TEMPLATE_H_
#define GALLERY_ALBUM_DEFAULT_TEMPLATE_H_

#include <QObject>

#include "album/album-template.h"

class AlbumDefaultTemplate : public AlbumTemplate {
  Q_OBJECT
  
 public:
  static AlbumDefaultTemplate* instance();
  
 private:
  static AlbumDefaultTemplate* instance_;
  
  AlbumDefaultTemplate();
};

#endif  // GALLERY_ALBUM_DEFAULT_TEMPLATE_H_