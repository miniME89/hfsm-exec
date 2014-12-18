/*
 *  Copyright (C) 2014 Marcel Lehwald
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H
#define UTILS_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace hfsmexec
{
    class Downloader : public QObject
    {
        Q_OBJECT

        public:
            Downloader();
            ~Downloader();

            void download(const QString& url, bool block = false);
            void wait();

            const QNetworkReply::NetworkError& getError() const;
            const QString& getErrorMessage() const;
            const QByteArray& getData() const;

        signals:
            void finished(const QNetworkReply::NetworkError& error, const QString& errorMessage, const QByteArray& data);

        public slots:
            void downloadFinished(QNetworkReply* reply);

        private:
           QNetworkAccessManager manager;
           QNetworkReply* reply;
           QNetworkReply::NetworkError error;
           QString errorMessage;
           QByteArray data;
    };
}

#endif
