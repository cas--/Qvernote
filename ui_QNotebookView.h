/********************************************************************************
** Form generated from reading UI file 'QNotebookView.ui'
**
** Created: Wed May 19 22:20:07 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QNOTEBOOKVIEW_H
#define UI_QNOTEBOOKVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QNotebookViewClass
{
public:
    QGridLayout *gridLayout;
    QPushButton *pbNewNotebook;
    QListWidget *lvNotebookList;

    void setupUi(QWidget *QNotebookViewClass)
    {
        if (QNotebookViewClass->objectName().isEmpty())
            QNotebookViewClass->setObjectName(QString::fromUtf8("QNotebookViewClass"));
        QNotebookViewClass->resize(587, 323);
        gridLayout = new QGridLayout(QNotebookViewClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pbNewNotebook = new QPushButton(QNotebookViewClass);
        pbNewNotebook->setObjectName(QString::fromUtf8("pbNewNotebook"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/IconAdd/general_add.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbNewNotebook->setIcon(icon);
        pbNewNotebook->setIconSize(QSize(32, 32));

        gridLayout->addWidget(pbNewNotebook, 0, 0, 1, 1);

        lvNotebookList = new QListWidget(QNotebookViewClass);
        lvNotebookList->setObjectName(QString::fromUtf8("lvNotebookList"));
        lvNotebookList->setFrameShape(QFrame::NoFrame);
        lvNotebookList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lvNotebookList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lvNotebookList->setAutoScroll(false);

        gridLayout->addWidget(lvNotebookList, 1, 0, 1, 1);


        retranslateUi(QNotebookViewClass);

        QMetaObject::connectSlotsByName(QNotebookViewClass);
    } // setupUi

    void retranslateUi(QWidget *QNotebookViewClass)
    {
        QNotebookViewClass->setWindowTitle(QApplication::translate("QNotebookViewClass", "QNotebookView", 0, QApplication::UnicodeUTF8));
        pbNewNotebook->setText(QApplication::translate("QNotebookViewClass", "New Notebook", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QNotebookViewClass: public Ui_QNotebookViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QNOTEBOOKVIEW_H
