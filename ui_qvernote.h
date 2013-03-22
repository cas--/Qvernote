/********************************************************************************
** Form generated from reading UI file 'qvernote.ui'
**
** Created: Wed May 12 22:58:29 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QVERNOTE_H
#define UI_QVERNOTE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QvernoteClass
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *QvernoteClass)
    {
        if (QvernoteClass->objectName().isEmpty())
            QvernoteClass->setObjectName(QString::fromUtf8("QvernoteClass"));
        QvernoteClass->resize(800, 600);
        centralwidget = new QWidget(QvernoteClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QvernoteClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(QvernoteClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        QvernoteClass->setMenuBar(menubar);
        statusbar = new QStatusBar(QvernoteClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        QvernoteClass->setStatusBar(statusbar);

        retranslateUi(QvernoteClass);

        QMetaObject::connectSlotsByName(QvernoteClass);
    } // setupUi

    void retranslateUi(QMainWindow *QvernoteClass)
    {
        QvernoteClass->setWindowTitle(QApplication::translate("QvernoteClass", "Qvernote", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QvernoteClass: public Ui_QvernoteClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QVERNOTE_H
