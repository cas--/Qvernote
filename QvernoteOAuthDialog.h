/*********************************************************************************
Copyright (C) 2013 Randy Baumgarte (Nixnote2 oauthwindow code)
Copyright (C) 2013 Calum Lind <calumlind@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#ifndef QVERNOTEOAUTHDIALOG_H
#define QVERNOTEOAUTHDIALOG_H

#include <QDialog>
#include <QWebView>
#include <QGridLayout>
#include <QNetworkRequest>

class QvernoteOAuthDialog : public QDialog
{
    Q_OBJECT
private:
    QString iconPath;
    QWebView tempAuthPage;
    QWebView userLoginPage;
    QWebView authRequestPage;
    QGridLayout grid;
    QString temporaryCredUrl;
    QString permanentCredUrl;
    QString urlBase;
    QString credUrlBase;
    QString accessUrlBase;
    bool authTokenReceived;
    bool userLoginPageLoaded;

public:
    explicit QvernoteOAuthDialog(QWidget *parent = 0);
    QString response;
    bool error;
    QString errorMessage;

    QSize sizeHint() const {
		return QSize(800, 480);
	}

public slots:
    void tempAuthPageLoaded(bool rc);
    void permanentCredentialsReceived(bool rc);
    void userLoginReply(QNetworkReply *reply);
    void tempAuthPageReply(QNetworkReply *reply);
    void sslErrorHandler(QNetworkReply *reply);


};

#endif // QVERNOTEOAUTHDIALOG_H
