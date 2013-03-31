/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

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

#include "oauthwindow.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <QSsl>
#include <QSslSocket>
#include <QWebView>
#include <QWebFrame>
#include <QNetworkReply>
#include <QSslConfiguration>

#include "Qvernote.h"
#include "QvernoteAPI.h"


OAuthWindow::OAuthWindow(QWidget *parent) :
    QDialog(parent)
{
    struct timeb tmb;

    ftime(&tmb);
    int time = tmb.time;
    int millis = tmb.millitm;

    // Create the URLs needed for authentication with Evernote
    urlBase = "https://" + QString(EVERNOTE_HOST);
    accessUrlBase = urlBase + "/OAuth.action?";
    credUrlBase = urlBase +
            "/oauth?oauth_consumer_key=" + QString(EDAM_CONSUMER_KEY) +
            "&oauth_signature=" + QString(EDAM_CONSUMER_SECRET) +
            "%26&oauth_signature_method=PLAINTEXT&oauth_timestamp=" + QString::number(time) +
            "&oauth_nonce=" + QString::number(millis);
    temporaryCredUrl = credUrlBase + "&oauth_callback=nnoauth";
    permanentCredUrl = credUrlBase + "&oauth_token=";

    // Build the window
    setWindowTitle(tr("Please Grant Qvernote Access"));
    setWindowIcon(QIcon(":password.png"));
    setContentsMargins(0, 0, 0, 0);
    resize(800, 480);

    setLayout(&grid);

    //grid.addWidget(&userLoginPage);

    error = false;
    errorMessage = "";

    // Check that SSL sockets are supported
    if (!QSslSocket::supportsSsl()) {
        errorMessage = tr("SSL Support not found.  Aborting connection");
        error = true;
        close();
        return;
    }

    // Turn on TLS (sometimes it isn't on by default)
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setProtocol(QSsl::SslV3);
    QSslConfiguration::setDefaultConfiguration(config);

    // Since this page loads async, we need flags to be sure we don't load something twice
    authTokenReceived = false;
    userLoginPageLoaded = false;

    // Load the temporary URL to start the authentication procesess.  When
    // finished, this QWebView will contain the URL to start the
    // authentication process.
    QUrl tu(temporaryCredUrl);
    connect(&tempAuthPage, SIGNAL(loadFinished(bool)), this, SLOT(tempAuthPageLoaded(bool)));
    connect(tempAuthPage.page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,
            const QList<QSslError> & )), this, SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
    connect(tempAuthPage.page()->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(tempAuthPageReply(QNetworkReply*)));

    qDebug() << "Temporary URL:" << tu.toString();
    tempAuthPage.load(tu);
}


void OAuthWindow::tempAuthPageLoaded(bool rc) {
    qDebug() << "Temporary credentials received from Evernote";
    if (!rc) {
        errorMessage = tr("Error receiving temporary credentials");
        error = true;
        QWebFrame *mainFrame = tempAuthPage.page()->mainFrame();
        QString contents = mainFrame->toHtml();
        qDebug() << "Reply contents:" << contents;
        close();
        return;
    }

    QWebFrame *mainFrame = tempAuthPage.page()->mainFrame();
    QString contents = mainFrame->toPlainText();
    qDebug() << "Temporary Cred Contents: " << contents;
    int index = contents.indexOf("&oauth_token_secret");
    contents = contents.left(index);
    QUrl accessUrl(accessUrlBase + contents);
    qDebug() << "AccessUrl:" << accessUrl;

    connect(userLoginPage.page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,
            const QList<QSslError> & )), this, SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
    connect(userLoginPage.page()->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(userLoginReply(QNetworkReply*)));
    userLoginPage.load(accessUrl);
    grid.addWidget(&userLoginPage);
}


void OAuthWindow::tempAuthPageReply(QNetworkReply* reply) {
    qDebug() << "error: " << reply->error();
    if (reply->error() != QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//        qDebug() << "status:" << statusCode;
//        qDebug() << "error: " << reply->error();
        return;
    }
}


void OAuthWindow::permanentCredentialsReceived(bool rc) {
    if (authTokenReceived)
        return;
    QWebFrame *mainFrame;
    mainFrame = authRequestPage.page()->mainFrame();
    QString contents = mainFrame->toPlainText();

    if (!rc) {
        errorMessage = tr("Error receiving permanent credentials");
        qDebug() << "Bad return code while receiveng permanent credentials";
        error = true;
        close();
        return;
    }

    if (contents.startsWith("oauth_token=S%3D")) {
        qDebug() << "Permanent Auth Response: " << contents;
        qDebug() << "Permanent credentials received received from Evernote";
        authTokenReceived = true;
        qDebug() << "Good authorization token received.";
        QString decoded;
        QByteArray enc;
        enc.append(contents);
        decoded = QUrl::fromEncoded(enc).toString();
        response = decoded;
        userLoginPage.disconnect(this);
        tempAuthPage.disconnect(this);
        authRequestPage.disconnect(this);
        error = false;
        errorMessage = "";
        close();
    }
}


void OAuthWindow::userLoginReply(QNetworkReply *reply) {
    if (userLoginPageLoaded)
        return;
    qDebug() << "Authentication reply received from Evernote";
    qDebug() << "error: " << reply->error();
    QString searchReq = "?oauth_token=";
    qDebug() << "Reply:" << reply->url().toString();

    int pos = reply->url().toString().indexOf(searchReq);
    if (pos>0) {
        QString token = reply->url().toString();
        token = token.mid(pos+searchReq.length());
        if (token.indexOf("auth_verifier") <= 0) {
            errorMessage = tr("Error receiving authorization");
            error = true;
            //close();
            return;
        }

        if (reply->isFinished()) {
            qDebug() << "Loading URL";
            qDebug() << "Permanent URL: " << permanentCredUrl;
            qDebug() << "Token: " << token;
            connect(&authRequestPage, SIGNAL(loadFinished(bool)), this, SLOT(permanentCredentialsReceived(bool)));
            connect(authRequestPage.page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,
                    const QList<QSslError> & )), this, SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
            authRequestPage.load(QUrl(permanentCredUrl + token));
            userLoginPageLoaded = true;
        }
    }
}


void OAuthWindow::sslErrorHandler(QNetworkReply* qnr, const QList<QSslError> & errlist) {
    //qDebug() << "---frmBuyIt::sslErrorHandler: ";
    // show list of all ssl errors
    //foreach (QSslError err, errlist)
    //    qDebug() << "ssl error: " << err;
    qnr->ignoreSslErrors();
}
