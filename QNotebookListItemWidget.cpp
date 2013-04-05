/***********************************************************************
Copyright (C) 2010 Alex R <burbulator@gmail.com>

This file is part of Qvernote.

Qvernote is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Qvernote is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Qvernote.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/
#include "QNotebookListItemWidget.h"
#include "QvernoteView.h"
#include <QWidget>

QNotebookListItemWidget::QNotebookListItemWidget(Notebook& notebook, QvernoteAPI* hEvernote, int noteCount, bool isDeleteMode, QWidget *parent)
    : QWidget(parent), m_Notebook(notebook), m_bDeleteMode(isDeleteMode), m_bItemMarked(false), m_bMouseMoved(false)
{
	ui.setupUi(this);
#ifdef Q_WS_MAEMO_5
		//setAttribute(Qt::WA_PaintOnScreen);
#endif

	//if(isDeleteMode)
	//	ui.lDeleteMark->setVisible(true);
	//else
	//	ui.lDeleteMark->setVisible(false);


	if(m_Notebook.defaultNotebook)
	{
		ui.lNotebookIcon->setPixmap(QPixmap(":/IconAdd/star-32.png"));
	}

	QStringList blacklistedNotebooks = QvernoteSettings::Instance()->getBlacklistedNotebooks().split(',');
	if(blacklistedNotebooks.contains(QString::fromStdString(m_Notebook.guid)) && !m_Notebook.guid.empty())
	{
		qDebug() << "Notebook is blacklisted: " << m_Notebook.name.c_str();
		ui.lNotebookIcon->setPixmap(QPixmap(":/IconAdd/general_presence_offline.png"));
	}

	ui.lNotebookName->setText(QString::fromUtf8(m_Notebook.name.c_str()));

	ui.lNotesCount->setText(QString::number((noteCount == -1)? hEvernote->getNotebookContentsCount(notebook) : noteCount, 10) + trUtf8(" notes"));

	Tag tag;
	bool hasNextTag;
	QString tagList;
	for(hasNextTag = hEvernote->getFirstNotebookTag(notebook.name, tag); hasNextTag; hasNextTag = hEvernote->getNextNotebookTag(notebook.name, tag))
	{
		tagList += QString::fromUtf8(tag.name.c_str()) + " ";
	}

	if(tagList.size() == 0)
	{
		ui.lNotebookTags->setText(trUtf8("No tags"));
	}
	else
	{
		ui.lNotebookTags->setText(tagList);
	}

	//QObject::connect(ui.lDeleteNotebook, SIGNAL(mousePressed()), this, SLOT(deleteNotebook()));
}

QNotebookListItemWidget::~QNotebookListItemWidget()
{

}

void QNotebookListItemWidget::mark()
{
	if(m_bItemMarked)
	{
		m_bItemMarked = false;
		ui.lDeleteMark->clear();
		emit notebookUnmarked();
	}
	else
	{
		m_bItemMarked = true;
		ui.lDeleteMark->setPixmap(QPixmap(":/IconAdd/widgets_tickmark_list.png"));
		emit notebookMarked();
	}
}
