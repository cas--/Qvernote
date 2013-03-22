/********************************************************************************
** Form generated from reading UI file 'QAboutDIalog.ui'
**
** Created: Sat Jun 5 18:33:50 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QABOUTDIALOG_H
#define UI_QABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QAboutDIalogClass
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *QAboutDIalogClass)
    {
        if (QAboutDIalogClass->objectName().isEmpty())
            QAboutDIalogClass->setObjectName(QString::fromUtf8("QAboutDIalogClass"));
        QAboutDIalogClass->resize(800, 300);
        verticalLayout = new QVBoxLayout(QAboutDIalogClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(300, -1, 300, -1);
        label = new QLabel(QAboutDIalogClass);
        label->setObjectName(QString::fromUtf8("label"));
        label->setPixmap(QPixmap(QString::fromUtf8(":/IconAdd/Evernote-64.png")));
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        label_2 = new QLabel(QAboutDIalogClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        label_3 = new QLabel(QAboutDIalogClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);


        retranslateUi(QAboutDIalogClass);

        QMetaObject::connectSlotsByName(QAboutDIalogClass);
    } // setupUi

    void retranslateUi(QDialog *QAboutDIalogClass)
    {
        QAboutDIalogClass->setWindowTitle(QApplication::translate("QAboutDIalogClass", "About", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        label_2->setText(QApplication::translate("QAboutDIalogClass", "Qvernote 0.1 ", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("QAboutDIalogClass", "Created by AE Software 2010", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QAboutDIalogClass: public Ui_QAboutDIalogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QABOUTDIALOG_H
