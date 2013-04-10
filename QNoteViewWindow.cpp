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
#include "QNoteViewWindow.h"
#include <QWebFrame>
#include "QEmailNoteDialog.h"
#include "QNotesView.h"
#include "QNoteEditWindow.h"
#include "QClickableLabel.h"
//#include trUtf8("QWebViewEventFilter.h")
#include <QDesktopServices>

#ifdef Q_WS_MAEMO_5
#include <QAbstractKineticScroller>
#include "QMaemoTextSelectionSupressor.h"
#endif

//#ifdef Q_WS_MAEMO_5
//#include trUtf8("QMaemoMapViewDialog.h")
//#else
#include "QGoogleMapViewDialog.h"
//#endif
#include "QPdfPreviewDialog.h"
#include "QAudioPreviewDialog.h"

using namespace boost;

QNoteViewWindow::QNoteViewWindow(const Note& note, QWidget *parent)
    : QvernoteWindow(parent), m_Note(note), isNavControlsVisible(false)
{
	ui.setupUi(this);

	QObject::connect(ui.wvNoteView, SIGNAL(loadFinished(bool)), this, SLOT(noteLoadFinished()), Qt::QueuedConnection);
	m_hEvernote = QvernoteAPI::Instance();
	enmlReader = shared_ptr<QEnmlReader>(new QEnmlReader());
	//ui.wvNoteView->setVisible(false);
	//ui.gridLayout_2->setRowMinimumHeight(3, 400);
	//loadNote();
	ui.wvNoteView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	ui.wvNoteView->page()->setPreferredContentsSize(QSize(800, 600));
	ui.wvNoteView->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, false);
	QAbstractKineticScroller *scroller = ui.scrollArea->property("kineticScroller").value<QAbstractKineticScroller *>();
	if (scroller)
		scroller->setOvershootPolicy(QAbstractKineticScroller::OvershootAlwaysOff);

	webViewSlide = new QPropertyAnimation(ui.wvNoteView, "geometry");
	webViewSlide->setDuration(150);
	webViewSlide->setEasingCurve(QEasingCurve::InQuad);

	ui.lFavorite->setBorderEnabled(false);
	ui.lFavorite->setClickAnimEnabled(false);

	//QObject::connect(webViewSlide, SIGNAL(finished()), this, SLOT(backgroundLoad()));

#ifdef Q_WS_MAEMO_5
	supressor = new QMaemoTextSelectionSupressor(ui.wvNoteView);
	supressor->enable();
#endif
	QWebSettings * settings = ui.wvNoteView->settings();
	settings->setUserStyleSheetUrl(QUrl::fromLocalFile("qvernote.css"));

	ui.wvNoteView->installEventFilter(new QWebViewEventFilter(ui.wvNoteView, this));

	//QToolBar* noteControlToolbar = new QToolBar();
	QMenuBar* noteViewMenu = new QMenuBar();
	QAction* actEditNote = new QAction(QIcon(":/IconAdd/general_sketch.png"), "Edit", this);
	QAction* actEmailNote = new QAction(QIcon(":/IconAdd/general_email.png"), "Email", this);
	//QAction* actZoomin = new QAction(QIcon(":/IconAdd/note_zoomin.png"), "Zoom In"), this);
	//QAction* actZoomout = new QAction(QIcon(":/IconAdd/note_zoomout.png"), "Zoom Out"), this);
	QAction* actMap = new QAction(QIcon(":/IconAdd/Maps-blue-64.png"), "Open Map", this);
	QAction* actNext = new QAction(QIcon(":/IconAdd/general_forward.png"), "Next", this);
	QAction* actPrev = new QAction(QIcon(":/IconAdd/general_back.png"), "Previous", this);

	/*noteControlToolbar->addAction(actEditNote);
	noteControlToolbar->addAction(actEmailNote);
	noteControlToolbar->addAction(actZoomin);
	noteControlToolbar->addAction(actZoomout);

	noteControlToolbar->addAction(actPrev);
	noteControlToolbar->addAction(actNext);
	*/
	//this->addToolBar(noteControlToolbar);

	noteViewMenu->addAction(actEditNote);
	noteViewMenu->addAction(actEmailNote);
	noteViewMenu->addAction(actNext);
	noteViewMenu->addAction(actPrev);
	noteViewMenu->addAction(actMap);
	this->setMenuBar(noteViewMenu);
	ui.lFavorite->setStyleSheet("background-color: #424142;");
	ui.lNoteTitle->setStyleSheet("background-color: #424142;");
	ui.lNotebookName->setStyleSheet("background-color: #424142;");
	ui.lTagIcon->setStyleSheet("background-color: #424142;");
	ui.lNoteTags->setStyleSheet("background-color: #424142;");
	initNavigationControls();

	QObject::connect(actEditNote, SIGNAL(triggered()), SLOT(startEditor()));
	QObject::connect(actEmailNote, SIGNAL(triggered()), SLOT(emailNote()));
	//QObject::connect(actZoomin, SIGNAL(triggered()), this, SLOT(zoominView()));
	//QObject::connect(actZoomout, SIGNAL(triggered()), this, SLOT(zoomoutView()));
	QObject::connect(actNext, SIGNAL(triggered()), SLOT(loadNextNote()));
	QObject::connect(actPrev, SIGNAL(triggered()), SLOT(loadPrevNote()));
	QObject::connect(actMap, SIGNAL(triggered()), SLOT(showNoteMap()));

	QObject::connect(ui.lFavorite, SIGNAL(clicked()), SLOT(onFavoriteClicked()));

	QObject::connect(ui.wvNoteView, SIGNAL(linkClicked(QUrl)), this, SLOT(openLink(QUrl)));
	backgroundLoad();
}

QNoteViewWindow::~QNoteViewWindow()
{
	delete webViewSlide;
	delete navigationControlTimer;
#ifdef Q_WS_MAEMO_5
	delete supressor;
#endif
}

void QNoteViewWindow::showEvent ( QShowEvent * )
{
	//qDebug() << __FUNCTION__ << trUtf8("Loading note");
	savedWebViewGeometry = ui.wvNoteView->geometry();
	//backgroundLoad();
}

bool QNoteViewWindow::loadNote()
{
	//qDebug() << trUtf8("Loading note");
	if(m_hEvernote->getNoteContent(m_Note) == false)
	{
		displayError(trUtf8("Unable to load note"), QString(m_hEvernote->getLastErrorString().c_str()));
		setProgressIndicator(false);
		return false;
	}

	return true;
}

void QNoteViewWindow::displayNote()
{
	enmlReader->parse(m_Note);
	ui.wvNoteView->setHtml(enmlReader->getHtml());
	ui.wvNoteView->page()->mainFrame()->addToJavaScriptWindowObject("JSApi",this);
	ui.scrollArea->ensureVisible(0, 0);
}

void QNoteViewWindow::startEditor()
{
	//emit startEditorSignal(m_Note, enmlReader.get());
	//close();
	m_pNoteEditWindow = shared_ptr<QNoteEditWindow>(new QNoteEditWindow(m_Note, false, this));
	//QNoteEditWindow* pNoteEditWindow = new QNoteEditWindow(m_Note, false, enmlReader.get(), this);
	QObject::connect(m_pNoteEditWindow.get(), SIGNAL(noteUpdated()), this, SLOT(backgroundLoad()));
	m_pNoteEditWindow->show();
}

void QNoteViewWindow::noteLoadFinished()
{

	QObject::disconnect(webViewSlide, SIGNAL(finished()), this, SLOT(startBackgroundTask()));
	QObject::connect(webViewSlide, SIGNAL(finished()), this, SLOT(resizeWebView()));
	//webViewSlide->stop();
	if(slideLeft)
		webViewSlide->setStartValue(QRect(savedWebViewGeometry.topRight(), savedWebViewGeometry.size()));
	else
		webViewSlide->setStartValue(QRect(savedWebViewGeometry.topLeft() - QPoint(savedWebViewGeometry.width(), 0), savedWebViewGeometry.size()));

	webViewSlide->setEndValue(savedWebViewGeometry);
	webViewSlide->start();
	//ui.wvNoteView->setVisible(true);
}

void QNoteViewWindow::resizeWebView()
{
	QSize s = ui.wvNoteView->page()->mainFrame()->contentsSize();
	const QRect fr = ui.wvNoteView->frameGeometry();
	const QRect cr = ui.wvNoteView->contentsRect();

	//qDebug() << trUtf8("contentSize") << s;
	//qDebug() << trUtf8("frameGeometry") << fr;
	//qDebug() << trUtf8("contentRect") << cr;
	//s.setHeight(s.height() + 1 * ui.wvNoteEdit->fontMetrics().lineSpacing());
	ui.wvNoteView->setMinimumHeight(qMax(70, s.height()  + (fr.height() - cr.height() - 1) + 20));
	ui.wvNoteView->setMinimumWidth(qMax(geometry().width() - 16, s.width()));
	ui.wvNoteView->setZoomFactor(1.0);
	updateHeader();
	setProgressIndicator(false);
}

void QNoteViewWindow::updateHeader()
{
	Notebook notebook;

	if(QvernoteStorage::Instance()->isNoteFavorite(m_Note.guid))
	{
		ui.lFavorite->setPixmap(QPixmap(":/IconAdd/Star-full-48.png"));
		ui.lFavorite->setProperty("isFavorite", QVariant(true));
	}
	else
	{
		ui.lFavorite->setPixmap(QPixmap(":/IconAdd/Star-empty-48.png"));
		ui.lFavorite->setProperty("isFavorite", QVariant(false));
	}

	ui.lNoteTitle->setText(QString::fromUtf8(m_Note.title.c_str()));

	if(m_hEvernote->getNotebookByGuid(notebook, m_Note.notebookGuid))
		ui.lNotebookName->setText(QString(trUtf8("In: ")) + QString::fromUtf8(notebook.name.c_str()));
	else
		ui.lNotebookName->setText(trUtf8("Unknown notebook"));

	/* load tag names */
	vector<string>::iterator it;
	QString tagList = 0;
	for(it = m_Note.tagNames.begin(); it < m_Note.tagNames.end(); it++)
	{
		tagList += QString::fromUtf8((*it).c_str()) + " ";
	}

	if(tagList.size() == 0)
		ui.lNoteTags->setText(trUtf8("No tags"));
	else
		ui.lNoteTags->setText(tagList);
}

void QNoteViewWindow::openLink(QUrl url)
{
	setProgressIndicator(true);
	//qDebug() << __FUNCTION__ << QByteArray::fromHex(url.toString().toAscii());


	if(url.toString().startsWith(trUtf8("http://")))
	{
		QDesktopServices::openUrl(url);
	}

	setProgressIndicator(false);
}



void QNoteViewWindow::zoominView()
{
	navigationControlTimer->start(3000);
	ui.wvNoteView->setZoomFactor(ui.wvNoteView->zoomFactor()+0.1);
	if(ui.wvNoteView->page()->mainFrame()->contentsSize().width() > width() ||
	   ui.wvNoteView->page()->mainFrame()->contentsSize().height() > height())
	{
		ui.wvNoteView->setMinimumWidth(ui.wvNoteView->page()->mainFrame()->contentsSize().width());
		ui.wvNoteView->setMinimumHeight(ui.wvNoteView->page()->mainFrame()->contentsSize().height());
	}

}

void QNoteViewWindow::zoomoutView()
{
	navigationControlTimer->start(3000);
	if(ui.wvNoteView->page()->mainFrame()->contentsSize().width() > width())
	{
		ui.wvNoteView->setZoomFactor(ui.wvNoteView->zoomFactor()-0.1);
		ui.wvNoteView->setMinimumWidth(ui.wvNoteView->page()->mainFrame()->contentsSize().width());
		ui.wvNoteView->setMinimumHeight(ui.wvNoteView->page()->mainFrame()->contentsSize().height());
		//qDebug() << ui.wvNoteView->zoomFactor() << ui.wvNoteView->page()->mainFrame()->contentsSize();
	}
}

void QNoteViewWindow::emailNote()
{
	QEmailNoteDialog pDlg(m_Note, m_hEvernote, this);
	pDlg.exec();
}

void QNoteViewWindow::showNoteMap()
{
//#ifdef Q_WS_MAEMO_5
//	QMaemoMapViewDialog pDlg(m_Note);
//	pDlg.exec();
//#else
	if(QvernoteSettings::Instance()->getMapsProvider() == QvernoteSettings::MP_GOOGLE)
	{
		QGoogleMapViewDialog pDlg(m_Note);
		pDlg.exec();
	}
//#endif
}

void QNoteViewWindow::loadNextNote()
{
	navigationControlTimer->start(3000);
	dynamic_cast<QNotesView*>(this->parentWidget())->clearNote(m_Note);
	((QNotesView*)this->parentWidget())->getNextNote(m_Note);
	//QObject::connect(webViewSlide, SIGNAL(finished()), this, SLOT(backgroundLoad()));
	slideLeft = true;

	backgroundLoad();
}

void QNoteViewWindow::loadPrevNote()
{
	navigationControlTimer->start(3000);
	dynamic_cast<QNotesView*>(this->parentWidget())->clearNote(m_Note);
	((QNotesView*)this->parentWidget())->getPrevNote(m_Note);
	//QObject::connect(webViewSlide, SIGNAL(finished()), this, SLOT(backgroundLoad()));
	slideLeft = false;
	//webViewSlide->setStartValue(ui.wvNoteView->geometry());
	//webViewSlide->setEndValue(QRect(ui.wvNoteView->geometry().topRight(), ui.wvNoteView->size()));
	//webViewSlide->start();
	backgroundLoad();
}

void QNoteViewWindow::backgroundLoad()
{
	setProgressIndicator(true);

	QObject::disconnect(webViewSlide, SIGNAL(finished()), this, SLOT(resizeWebView()));
	QObject::connect(webViewSlide, SIGNAL(finished()), this, SLOT(startBackgroundTask()));
	webViewSlide->setStartValue(ui.wvNoteView->geometry());
	if(slideLeft)
		webViewSlide->setEndValue(QRect(ui.wvNoteView->geometry().topLeft() - QPoint(ui.wvNoteView->size().width(), 0), ui.wvNoteView->size()));
	else
		webViewSlide->setEndValue(QRect(ui.wvNoteView->geometry().topRight(), ui.wvNoteView->size()));

	webViewSlide->start();
}

void QNoteViewWindow::initNavigationControls()
{
	clZoomin = new QClickableLabel(this);
	clZoomin->setBorderEnabled(false);
	clZoomin->setForegroundRole(QPalette::Window);
	clZoomin->setPixmap(QPixmap(":/IconAdd/note_zoomin.png"));
	clZoomin->hide();

	clZoomout = new QClickableLabel(this);
	clZoomout->setBorderEnabled(false);
	clZoomout->setForegroundRole(QPalette::Window);
	clZoomout->setPixmap(QPixmap(":/IconAdd/note_zoomout.png"));
	clZoomout->hide();

	clPrev = new QClickableLabel(this);
	clPrev->setBorderEnabled(false);
	clPrev->setForegroundRole(QPalette::Window);
	clPrev->setPixmap(QPixmap(":/IconAdd/general_back64.png"));
	clPrev->hide();

	clNext = new QClickableLabel(this);
	clNext->setBorderEnabled(false);
	clNext->setForegroundRole(QPalette::Window);
	clNext->setPixmap(QPixmap(":/IconAdd/general_forward64.png"));
	clNext->hide();

	clFullscreen = new QClickableLabel(this);
	clFullscreen->setBorderEnabled(false);
	clFullscreen->setForegroundRole(QPalette::Window);
	clFullscreen->setPixmap(QPixmap(":/IconAdd/general_fullsize64.png"));
	clFullscreen->hide();

	clEdit = new QClickableLabel(this);
	clEdit->setBorderEnabled(false);
	clEdit->setForegroundRole(QPalette::Window);
	clEdit->setPixmap(QPixmap(":/IconAdd/general_sketch.png"));
	clEdit->hide();

	clEmail = new QClickableLabel(this);
	clEmail->setBorderEnabled(false);
	clEmail->setForegroundRole(QPalette::Window);
	clEmail->setPixmap(QPixmap(":/IconAdd/general_email64.png"));
	clEmail->hide();

	//~ clMap = new QClickableLabel(this);
	//~ clMap->setBorderEnabled(false);
	//~ clMap->setForegroundRole(QPalette::Window);
	//~ clMap->setPixmap(QPixmap(":/IconAdd/Maps-blue-64.png"));
	//~ clMap->hide();

	QObject::connect(clZoomin, SIGNAL(clicked()), this, SLOT(zoominView()));
	QObject::connect(clZoomout, SIGNAL(clicked()), this, SLOT(zoomoutView()));
	QObject::connect(clNext, SIGNAL(clicked()), SLOT(loadNextNote()));
	QObject::connect(clPrev, SIGNAL(clicked()), SLOT(loadPrevNote()));
	QObject::connect(clEdit, SIGNAL(clicked()), SLOT(startEditor()));
	QObject::connect(clEmail, SIGNAL(clicked()), SLOT(emailNote()));
	//QObject::connect(clMap, SIGNAL(clicked()), SLOT(showNoteMap()));

//#ifdef Q_WS_MAEMO_5
	QObject::connect(clFullscreen, SIGNAL(clicked()), SLOT(toggleFullScreen()));
//#endif
	navigationControlTimer = new QTimer();
	navigationControlTimer->setSingleShot(true);
	QObject::connect(navigationControlTimer, SIGNAL(timeout()), SLOT(hideNavigationControls()));
}

void QNoteViewWindow::displayNavigationControls()
{
	if(isNavControlsVisible == false)
	{
		isNavControlsVisible = true;
		clZoomin->setGeometry(width() - 85, height() / 2 - 80, 0, 0);
		clZoomin->animShow(QSize(64, 64));
		clZoomout->setGeometry(width() - 85, height() / 2 + 16, 0, 0);
		clZoomout->animShow(QSize(64, 64));
		clPrev->setGeometry(width() / 2 - 90, height() - 80, 0, 0);
		clPrev->animShow(QSize(64, 64));
		clNext->setGeometry(width() / 2 + 26, height() - 80, 0, 0);
		clNext->animShow(QSize(64, 64));
		clFullscreen->setGeometry(width() - 80, height() - 80, 0, 0);
		clFullscreen->animShow(QSize(64, 64));
		clEdit->setGeometry(16, height() - 80, 0, 0);
		clEdit->animShow(QSize(64, 64));
		//clEmail->setGeometry(width() / 2 -32, height() / 2 + 16, 0, 0);
		//clEmail->animShow(QSize(64, 64));
		//clMap->setGeometry(16, height() / 2 - 80, 0, 0);
		//clMap->animShow(QSize(64, 64));

		navigationControlTimer->start(3000);
	}
}

void QNoteViewWindow::hideNavigationControls()
{
	//clZoomin->hide();
	clZoomin->animHide();
	clZoomout->animHide();
	clPrev->animHide();
	clNext->animHide();
	clFullscreen->animHide();
	clEdit->animHide();
	clEmail->animHide();
	//clMap->animHide();
	isNavControlsVisible = false;
}

//#ifdef Q_WS_MAEMO_5
void QNoteViewWindow::toggleFullScreen()
{
 bool isFullScreen = windowState() & Qt::WindowFullScreen;

 if (isFullScreen)
	 showNormal();
 else
	 showFullScreen();

 navigationControlTimer->start(5);
}
//#endif

void QNoteViewWindow::onFavoriteClicked()
{
	if(ui.lFavorite->property("isFavorite").toBool() == true)
	{
		QvernoteStorage::Instance()->removeFavorite(m_Note.guid);
		ui.lFavorite->setPixmap(QPixmap(":/IconAdd/Star-empty-48.png"));
		ui.lFavorite->setProperty("isFavorite", QVariant(false));
	}
	else
	{
		QvernoteStorage::Instance()->addFavorite(m_Note.guid);
		ui.lFavorite->setPixmap(QPixmap(":/IconAdd/Star-full-48.png"));
		ui.lFavorite->setProperty("isFavorite", QVariant(true));
	}
}

void QNoteViewWindow::handleResource(QString resourceHash)
{
	QResourceHandlerDialog dlg(m_Note, resourceHash, this);
	dlg.exec();
}

QResourceHandlerDialog::QResourceHandlerDialog(Note& note, QString resourceHash, QWidget* parent)
: QDialog(parent), m_Note(note), m_resourceHash(resourceHash)
{
	setWindowTitle(trUtf8("Choose action"));
	layout = new QGridLayout(this);
	bSave.setText(trUtf8("Save"));
	bPreview.setText(trUtf8("Preview"));
	QObject::connect(&bSave, SIGNAL(clicked()), SLOT(storeResource()));
	QObject::connect(&bPreview, SIGNAL(clicked()), SLOT(previewResource()));
	layout->addWidget(&bSave, 1, 1);
	layout->addWidget(&bPreview, 1, 2);
}

void QResourceHandlerDialog::storeResource()
{
	Resource r;

	if(QvernoteAPI::Instance()->loadResourceByHash(r, m_Note.guid, m_resourceHash) == true)
	{
		QString fileName = QFileDialog::getSaveFileName(
				this,
				tr("Save resource"),
				"/MyDocs/" + QString::fromUtf8(r.attributes.fileName.c_str(), r.attributes.fileName.size()),
				tr("Images (*.png *.gif *.jpg);;Audio (*.wav *.mpg *.amr *.mp3);;Documents (*.pdf);;Other (*)"));

		if(fileName.size() > 0)
		{
			QFile file(fileName);
			if (!file.open(QIODevice::WriteOnly))
			{
				QvernoteWindow::displayError(this, trUtf8("Cannot write file"), file.errorString());
				return;
			}

			QDataStream out(&file);
			out.writeRawData(r.data.body.c_str(), r.data.size);
			file.close();
			qDebug() << "Resource saved";
		}
	}
	else
	{
		QvernoteWindow::displayError(this, trUtf8("Failed to load resource"), QString::fromStdString(QvernoteAPI::Instance()->getLastErrorString()));
	}

	close();
}

void QResourceHandlerDialog::previewResource()
{
	Resource r;

#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

	if(QvernoteAPI::Instance()->loadResourceByHash(r, m_Note.guid, m_resourceHash) == true)
	{
		switch(QEnmlContentHandler::decodeEnmlMediaType(QString::fromStdString(r.mime))) {
		case QEnmlContentHandler::MT_PDF:
		{
			QPdfPreviewDialog dlg(r, this);
			dlg.exec();
		}
		break;
		case QEnmlContentHandler::MT_AUDIO:
		{
			QAudioPreviewDialog dlg(r, this);
			dlg.exec();
		}
		break;
		default:
		{
			QvernoteWindow::displayError(parentWidget(), trUtf8("Preview is unavailable for media type"), QString::fromStdString(r.mime));
		}
		}
	}

	close();
}
