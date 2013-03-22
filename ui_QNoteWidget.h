/********************************************************************************
** Form generated from reading UI file 'QNoteWidget.ui'
**
** Created: Sat Jun 26 21:52:34 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QNOTEWIDGET_H
#define UI_QNOTEWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QNoteClass
{
public:
    QGridLayout *gridLayout;
    QGraphicsView *gvNoteSnapshot;
    QLabel *lNoteTitle;

    void setupUi(QWidget *QNoteClass)
    {
        if (QNoteClass->objectName().isEmpty())
            QNoteClass->setObjectName(QString::fromUtf8("QNoteClass"));
        QNoteClass->resize(104, 123);
        QNoteClass->setAutoFillBackground(true);
        gridLayout = new QGridLayout(QNoteClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gvNoteSnapshot = new QGraphicsView(QNoteClass);
        gvNoteSnapshot->setObjectName(QString::fromUtf8("gvNoteSnapshot"));
        gvNoteSnapshot->setAutoFillBackground(true);
        gvNoteSnapshot->setFrameShape(QFrame::StyledPanel);
        gvNoteSnapshot->setFrameShadow(QFrame::Raised);
        gvNoteSnapshot->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gvNoteSnapshot->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gvNoteSnapshot->setRenderHints(QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);

        gridLayout->addWidget(gvNoteSnapshot, 1, 0, 1, 2);

        lNoteTitle = new QLabel(QNoteClass);
        lNoteTitle->setObjectName(QString::fromUtf8("lNoteTitle"));
        QFont font;
        font.setPointSize(9);
        lNoteTitle->setFont(font);

        gridLayout->addWidget(lNoteTitle, 2, 0, 1, 1);


        retranslateUi(QNoteClass);

        QMetaObject::connectSlotsByName(QNoteClass);
    } // setupUi

    void retranslateUi(QWidget *QNoteClass)
    {
        QNoteClass->setWindowTitle(QApplication::translate("QNoteClass", "QNote", 0, QApplication::UnicodeUTF8));
        lNoteTitle->setText(QApplication::translate("QNoteClass", "title", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QNoteClass: public Ui_QNoteClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QNOTEWIDGET_H
