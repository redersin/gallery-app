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
 * Charles Lindsay <chaz@yorba.org>
 */

#include <QDir>
#include <QGLWidget>
#include <QString>
#include <QUrl>
#include <QString>
#include <QQuickItem>
#include <cstdlib>

#include "gallery-application.h"
#include "album/album.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "album/album-page.h"
#include "database/database.h"
#include "event/event.h"
#include "event/event-collection.h"
#include "media/media-collection.h"
#include "media/media-source.h"
#include "media/preview-manager.h"
#include "photo/photo-metadata.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/qml-album-collection-model.h"
#include "qml/qml-event-collection-model.h"
#include "qml/qml-event-overview-model.h"
#include "qml/qml-media-collection-model.h"
#include "qml/qml-stack.h"
#include "util/resource.h"

GalleryApplication::GalleryApplication(int& argc, char** argv) :
    QApplication(argc, argv), form_factor_("desktop"), is_portrait_(false),
    bgu_size_(8), view_(), startup_timer_(false) {
  
  timer_.start();
  form_factors_.insert("desktop", QSize(160, 100)); // In BGU.
  form_factors_.insert("tablet", QSize(160, 100));
  form_factors_.insert("phone", QSize(71, 40));
  form_factors_.insert("sidebar", QSize(71, 40));

  pictures_dir_ = QDir(QString("%1/Pictures").arg(QDir::homePath()));

  register_qml();
  process_args();
  init_common();
}

int GalleryApplication::exec() {
  create_view();

  // Delay init_collections() so the main loop is running before it kicks off.
  QTimer::singleShot(0, this, SLOT(start_init_collections()));

  return QApplication::exec();
}

void GalleryApplication::register_qml() {
  //
  // QML Declarative types must be registered before use
  //
  // TODO: Use QML Plugins to automate this
  //

  Album::RegisterType();
  AlbumPage::RegisterType();
  Event::RegisterType();
  MediaSource::RegisterType();
  QmlAlbumCollectionModel::RegisterType();
  QmlEventCollectionModel::RegisterType();
  QmlEventOverviewModel::RegisterType();
  QmlMediaCollectionModel::RegisterType();
  QmlStack::RegisterType();
}

void GalleryApplication::usage(bool error) {
  QTextStream out(error ? stderr : stdout);
  out << "Usage: gallery [options] [pictures_dir]" << endl;
  out << "Options:" << endl;
  out << "  --landscape   run in landscape orientation (default)" << endl;
  out << "  --portrait   run in portrait orientation" << endl;
  out << "  --bgu-size X   set BGU size to X (default 8)" << endl;
  foreach (const QString& form_factor, form_factors_.keys())
    out << "  --" << form_factor << "   run in " << form_factor << " form factor" << endl;
  out << "  --startup-timer   debug-print startup time" << endl;
  out << "pictures_dir defaults to ~/Pictures, and must exist prior to running gallery" << endl;
  std::exit(error ? 1 : 0);
}

void GalleryApplication::invalid_arg(QString arg) {
  QTextStream(stderr) << "Invalid argument '" << arg << "'" << endl;
  usage(true);
}

void GalleryApplication::process_args() {
  QStringList args = arguments();

  for (int i = 1; i < args.count(); ++i) {
    QString arg = args[i];
    QString value = (i + 1 < args.count() ? args[i + 1] : "");

    if (arg == "--help" || arg == "-h") {
      usage();
    } else if (arg == "--landscape") {
      is_portrait_ = false;
    } else if (arg == "--portrait") {
      is_portrait_ = true;
    } else if (arg == "--startup-timer") {
      startup_timer_ = true;
    } else if (arg == "--bgu-size") {
      bool ok = false;
      int bgu_size = value.toInt(&ok);

      ++i; // Skip over value next iteration.
      if (ok && bgu_size > 0)
        bgu_size_ = bgu_size;
    } else {
      QString form_factor = arg.mid(2); // minus initial "--"

      if (arg.startsWith("--") && form_factors_.keys().contains(form_factor)) {
        form_factor_ = form_factor;
      } else if (i == args.count() - 1 && QDir(arg).exists()) {
        pictures_dir_ = QDir(arg);
      } else {
        invalid_arg(arg);
      }
    }
  }
}

void GalleryApplication::init_common() {
  // These need to be initialized before create_view() or init_collections().
  Resource::Init(applicationDirPath(), INSTALL_PREFIX);
  GalleryStandardImageProvider::Init();
}

void GalleryApplication::create_view() {
  //
  // Create the master QDeclarativeView that all the pages will operate within
  // using the OpenGL backing and load the root container
  //

  view_.setWindowTitle("Gallery");

  QSize size = form_factors_[form_factor_];
  if (is_portrait_)
    size.transpose();

  // Only the desktop is resizable.
  if (form_factor_ == "desktop") {
    view_.setResizeMode(QQuickView::SizeRootObjectToView);
    view_.setMinimumSize(QSize(60 * bgu_size_, 60 * bgu_size_));
  } else {
    view_.setResizeMode(QQuickView::SizeViewToRootObject);
  }

  view_.engine()->rootContext()->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
  view_.engine()->rootContext()->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
  view_.engine()->rootContext()->setContextProperty("FORM_FACTOR", QVariant(form_factor_));
  view_.engine()->rootContext()->setContextProperty("GRIDUNIT", QVariant(bgu_size_));

  view_.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
    GalleryStandardImageProvider::instance());
  view_.setSource(Resource::instance()->get_rc_url("qml/GalleryApplication.qml"));
  QObject::connect(view_.engine(), SIGNAL(quit()), this, SLOT(quit()));

  // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
  QObject* rootObject = dynamic_cast<QObject*>(view_.rootObject());
  QObject::connect(this, SIGNAL(media_loaded()), rootObject, SLOT(onLoaded()));

  view_.show();
}

void GalleryApplication::init_collections() {
  //
  // Library is currently only loaded from pictures_dir_ (default ~/Pictures),
  // no subdirectory traversal)
  //

  qDebug("Opening %s...", qPrintable(pictures_dir_.path()));

  // Not in alpha-order because initialization order is important here
  // TODO: Need to use an initialization system that deals with init order
  // issues
  PhotoMetadata::Init(); // must init before loading photos
  Database::Init(pictures_dir_, this);
  Database::instance()->get_media_table()->verify_files();
  AlbumDefaultTemplate::Init();
  MediaCollection::Init(pictures_dir_); // only init after db
  AlbumCollection::Init(); // only init after media collection
  EventCollection::Init();
  PreviewManager::Init();

  qDebug("Opened %s", qPrintable(pictures_dir_.path()));

  emit media_loaded();
  
  if (startup_timer_)
    qDebug() << "Startup took" << timer_.elapsed() << "milliseconds";
}

void GalleryApplication::start_init_collections() {
  init_collections();
}