#include "QNoteListItemWidget.h"
#include <QDateTime>
#include <QDebug>

QNoteListItemWidget::QNoteListItemWidget(const Note& note, bool isDeleteMode, QWidget *parent)
    : QWidget(parent), m_Note(note), m_bDeleteMode(isDeleteMode), m_bItemMarked(false)
{
	ui.setupUi(this);

#ifdef Q_WS_MAEMO_5
		//setAttribute(Qt::WA_PaintOnScreen);
#endif
	//ui.gvNoteSnapshot->setScene(&snapshotScene);
	if(isDeleteMode)
		ui.lDeleteMark->setVisible(true);
	else
	{
		if(m_Note.resources.size() > 0)
			ui.lDeleteMark->setPixmap(QPixmap(":/IconAdd/email_attachment.png"));
		//else
		//	ui.lDeleteMark->setVisible(false);
	}

	//enmlReader = shared_ptr<QEnmlReader>(new QEnmlReader(m_Note));
	//enmlReader->parse();

	//QObject::connect(&m_WebView, SIGNAL(loadFinished(bool)), this, SLOT(setNoteSnapshot()));
	//m_WebView.setHtml(enmlReader->getHtml());

	/* draw note tags */
	vector<string>::iterator it;
	QString tagList = 0;
	for(it = m_Note.tagNames.begin(); it < m_Note.tagNames.end(); it++)
	{
		tagList += QString((*it).c_str()) + " ";
	}

	if(tagList.size() == 0)
		ui.lNoteTags->setText(trUtf8("No tags"));
	else
		ui.lNoteTags->setText(tagList);

	QString noteTitle = QString::fromUtf8(note.title.c_str());

#ifdef Q_WS_MAEMO_5
	ui.lNoteTitle->setStyleSheet("color: white");
	ui.lNoteTitle->setText(noteTitle);
	ui.lNoteDate->setText(QDateTime::fromTime_t(note.created / 1000).toString("M/d/yy h:m AP"));
#else
	ui.lNoteTitle->setStyleSheet("color: black");
	ui.lNoteTitle->setText(noteTitle);
	ui.lNoteDate->setText(QDateTime::fromTime_t(note.created / 1000).toString("M/d/yy h:m AP"));
#endif
}

QNoteListItemWidget::~QNoteListItemWidget()
{

}

void QNoteListItemWidget::mark()
{
	if(m_bItemMarked)
	{
		m_bItemMarked = false;
		ui.lDeleteMark->clear();
		emit noteUnmarked();
	}
	else
	{
		m_bItemMarked = true;
		ui.lDeleteMark->setPixmap(QPixmap(":/IconAdd/widgets_tickmark_list.png"));
		emit noteMarked();
	}
}

/*
void QNoteListItemWidget::setNoteSnapshot()
{
	m_Snapshot = QPixmap(m_WebView.size());
	m_WebView.setRenderHint(QPainter::SmoothPixmapTransform, true);
	m_WebView.render(&m_Snapshot);

	//snapshotItem.setPixmap(m_Snapshot.scaled(ui.gvNoteSnapshot->width(), ui.gvNoteSnapshot->height()+32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
	//snapshotScene.addItem(&snapshotItem);

	//ui.lNoteSnapshot->setPixmap(m_Snapshot.scaled(ui.lNoteSnapshot->width(), ui.lNoteSnapshot->height()+32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}
*/
