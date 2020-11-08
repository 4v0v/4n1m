/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "qminiz.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDirIterator>
#include "miniz.h"

bool QMiniZ::compressFolder(QString zip_path, QString folder_path, const QStringList& filenames)
{
    if (!folder_path.endsWith("/")) folder_path.append("/");

    mz_zip_archive* mz = new mz_zip_archive;
    mz_zip_zero_struct(mz);
    mz_bool ok = mz_zip_writer_init_file(mz, zip_path.toUtf8().data(), 0);

    for (const QString& filename : filenames)
    {
        QString file_path = folder_path + filename;
        ok = mz_zip_writer_add_file(
            mz,
            filename.toUtf8().data(),
            file_path.toUtf8().data(),
            "", 0, MZ_BEST_SPEED
        );
    }
    ok &= mz_zip_writer_finalize_archive(mz);
    mz_zip_writer_end(mz);

    delete mz;
    return ok;
}

bool QMiniZ::uncompressFolder(QString zip_name, QString zip_path)
{
    if (!QFile::exists(zip_name)) return false;

    QDir baseDir(zip_path);
    if (!baseDir.exists())
    {
        bool ok = baseDir.mkpath(".");
        Q_ASSERT(ok);
    }

    baseDir.makeAbsolute();

    mz_zip_archive* mz = new mz_zip_archive;
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_reader_init_file(mz, zip_name.toUtf8().data(), 0);
    if (!ok) return false;

    int num = mz_zip_reader_get_num_files(mz);

    mz_zip_archive_file_stat* stat = new mz_zip_archive_file_stat;

    for (int i = 0; i < num; ++i)
    {
        ok &= mz_zip_reader_file_stat(mz, i, stat);

        if (stat->m_is_directory)
        {
            QString sFolderPath = QString::fromUtf8(stat->m_filename);
            bool mkDirOK = baseDir.mkpath(sFolderPath);
            Q_ASSERT(mkDirOK);
        }
    }

    for (int i = 0; i < num; ++i)
    {
        ok &= mz_zip_reader_file_stat(mz, i, stat);

        if (!stat->m_is_directory)
        {
            QString sFullPath = baseDir.filePath(QString::fromUtf8(stat->m_filename));
            bool b = QFileInfo(sFullPath).absoluteDir().mkpath(".");
            Q_ASSERT(b);

            bool extractOK = mz_zip_reader_extract_to_file(mz, i, sFullPath.toUtf8(), 0);
            if (!extractOK) ok = false;
        }
    }

    mz_zip_reader_end(mz);
    delete mz;
    delete stat;
    return ok;
}
