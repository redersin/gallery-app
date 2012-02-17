/*
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef GALLERY_PREVIEW_MANAGER_H_
#define GALLERY_PREVIEW_MANAGER_H_

#include <QObject>
#include <QFileInfo>
#include <QSet>
#include <QString>

#include "core/data-object.h"
#include "media/media-source.h"

class PreviewManager : public QObject {
  Q_OBJECT
  
 public:
  static const int PREVIEW_WIDTH_MAX = 360;
  static const int PREVIEW_HEIGHT_MAX = 360;
  
  static const QString PREVIEW_DIR;
  
  static void Init();
  static void Terminate();
  
  static PreviewManager* instance();
  
  QFileInfo PreviewFileFor(const MediaSource* media) const;
  
 private slots:
  void on_media_added_removed(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  static PreviewManager* instance_;
  
  PreviewManager();
  
  bool VerifyPreview(MediaSource* media);
};

#endif  // GALLERY_PREVIEW_MANAGER_H_