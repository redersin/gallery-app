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
 * Eric Gregory <eric@yorba.org>
 */

#include <QApplication>

#include "media-table.h"
#include "database.h"

/*!
 * \brief MediaTable::MediaTable
 * \param db
 * \param parent
 */
MediaTable::MediaTable(Database* db, QObject* parent)
    : QObject(parent), db_(db)
{
}

/*!
 * \brief MediaTable::verify_files
 * Runs though the table, removes references to files
 * that have been deleted from disk.
 */
void MediaTable::verify_files()
{
    QSqlQuery query(*db_->get_db());
    QList<qint64> to_delete;
    query.prepare("SELECT id, filename FROM MediaTable");
    if (!query.exec())
        db_->log_sql_error(query);

    // Stat each file. Make a list of files that no longer exist.
    while (query.next()) {
        // stat'ing and sync'ing file info over even several hundred photos is an
        // expensive operation since it involves lots of I/O, so spin the event
        // loop so that the UI remains responsive
        QApplication::processEvents();

        qint64 id = query.value(0).toLongLong();
        QFile file(query.value(1).toString());

        if (!file.exists())
            to_delete.append(id);
    }

    // Delete any references to non-existent files.
    db_->get_db()->transaction();
    foreach (qint64 id, to_delete) {
        // spin the event loop so that the UI remains responsive
        QApplication::processEvents();

        remove(id);
    }
    
    db_->get_db()->commit();
}

/*!
 * \brief MediaTable::get_id_for_media Returns the row ID for the given photo.
 * \param filename
 * \return Returns the row ID for the given photo. If none exists, -1 will be returned.
 */
qint64 MediaTable::get_id_for_media(const QString& filename)
{
    // If there's a row for this file, return the ID.
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT id FROM MediaTable WHERE filename = :filename");
    query.bindValue(":filename", filename);
    if (!query.exec())
        db_->log_sql_error(query);

    if (query.next())
        return query.value(0).toLongLong();

    // No row found.
    return -1;
}

/*!
 * \brief MediaTable::create_id_for_media Creates a row for the given photo and returns the new ID
 * \param filename
 * \param timestamp
 * \param exposure_time
 * \param original_orientation
 * \param filesize
 * \return
 */
qint64 MediaTable::create_id_for_media(const QString& filename,
                                       const QDateTime& timestamp, const QDateTime& exposure_time,
                                       Orientation original_orientation, qint64 filesize)
{
    // Add the row.
    QSqlQuery query(*db_->get_db());
    query.prepare("INSERT INTO MediaTable (filename, timestamp, exposure_time, "
                  "original_orientation, filesize) VALUES (:filename, :timestamp, "
                  ":exposure_time, :original_orientation, :filesize)");
    query.bindValue(":filename", filename);
    query.bindValue(":timestamp", timestamp.toMSecsSinceEpoch());
    query.bindValue(":exposure_time", exposure_time.toMSecsSinceEpoch());
    query.bindValue(":original_orientation", original_orientation);
    query.bindValue(":filesize", filesize);
    if (!query.exec())
        db_->log_sql_error(query);

    return query.lastInsertId().toLongLong();
}

/*!
 * \brief MediaTable::update_media Updates a given row
 * \param media_id
 * \param filename
 * \param timestamp
 * \param exposure_time
 * \param original_orientation
 * \param filesize
 */
void MediaTable::update_media(qint64 media_id, const QString& filename,
                              const QDateTime& timestamp, const QDateTime& exposure_time,
                              Orientation original_orientation, qint64 filesize)
{
    // Add the row.
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE MediaTable SET filename = :filename, "
                  "timestamp = :timestamp, exposure_time = :exposure_time, "
                  "original_orientation = :original_orientation, "
                  "filesize = :filesize WHERE id = :id");
    query.bindValue(":filename", filename);
    query.bindValue(":timestamp", timestamp.toMSecsSinceEpoch());
    query.bindValue(":exposure_time", exposure_time.toMSecsSinceEpoch());
    query.bindValue(":original_orientation", original_orientation);
    query.bindValue(":filesize", filesize);
    query.bindValue(":id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief MediaTable::remove Removes a photo from the database.
 * \param mediaId
 */
void MediaTable::remove(qint64 mediaId)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("DELETE FROM MediaTable WHERE id = :id");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief MediaTable::get_media_size
 * \param media_id
 * \return
 */
QSize MediaTable::get_media_size(qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT width, height FROM MediaTable WHERE id = :id LIMIT 1");
    query.bindValue(":id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);

    QSize size;
    if (query.next()) {
        int width = query.value(0).toInt();
        int height = query.value(1).toInt();
        if (width > 0 && height > 0)
            size = QSize(width, height);
    }

    return size;
}

/*!
 * \brief MediaTable::set_media_size
 * \param media_id
 * \param size
 */
void MediaTable::set_media_size(qint64 media_id, const QSize& size)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE MediaTable SET width = :width, height = :height "
                  "WHERE id = :id");
    query.bindValue(":id", media_id);
    query.bindValue(":width", size.width());
    query.bindValue(":height", size.height());
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief MediaTable::set_original_orientation
 * \param media_id
 * \param orientation
 */
void MediaTable::set_original_orientation(qint64 media_id, const Orientation& orientation)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("UPDATE MediaTable SET orientation = :orientation WHERE id = :id");
    query.bindValue(":id", media_id);
    query.bindValue(":orientation", orientation);
    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief MediaTable::get_file_timestamp
 * \param media_id
 * \return
 */
QDateTime MediaTable::get_file_timestamp(qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT timestamp FROM MediaTable WHERE id = :id");
    query.bindValue(":id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);

    QDateTime timestamp;
    if (query.next()) {
        timestamp.setMSecsSinceEpoch(query.value(0).toLongLong());
    }

    return timestamp;
}

/*!
 * \brief MediaTable::get_exposure_time
 * \param media_id
 * \return
 */
QDateTime MediaTable::get_exposure_time(qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT exposure_time FROM MediaTable WHERE id = :id");
    query.bindValue(":id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);

    QDateTime exposure_time;
    if (query.next()) {
        exposure_time.setMSecsSinceEpoch(query.value(0).toLongLong());
    }

    return exposure_time;
}

/*!
 * \brief MediaTable::get_row Gets a row that already exists
 * \param media_id
 * \param size
 * \param original_orientation
 * \param file_timestamp
 * \param exposure_date_time
 */
void MediaTable::get_row(qint64 media_id, QSize& size, Orientation& 
                         original_orientation, QDateTime& file_timestamp, QDateTime& exposure_date_time)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT width, height, timestamp, exposure_time, "
                  "original_orientation FROM MediaTable WHERE id = :id LIMIT 1");
    query.bindValue(":id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);

    if (!query.next())
        db_->log_sql_error(query);

    size = QSize(query.value(0).toInt(), query.value(1).toInt());

    file_timestamp.setMSecsSinceEpoch(query.value(2).toLongLong());
    exposure_date_time.setMSecsSinceEpoch(query.value(3).toLongLong());
    original_orientation = static_cast<Orientation>(query.value(4).toInt());
}

/*!
 * \brief MediaTable::row_needs_update
 * Returns true if row is from an older schema.  In that case, update_media()
 * should be called to repopulate the row.
 * \param media_id
 * \return
 */
bool MediaTable::row_needs_update(qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT 1 FROM MediaTable WHERE timestamp IS NULL "
                  "AND id = :id LIMIT 1");
    query.bindValue(":id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);

    return query.next();
}
