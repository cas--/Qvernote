#include "QNoteEditWindow.h"
#include "QEnmlWriter.h"
#include <QToolBar>
#include <QAction>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QWebFrame>
#include <QDebug>
#include <QFile>
#include <QWebElement>
#include <QPixmap>
#include "QTagSelectorDialog.h"
#include "QAttachmentWidget.h"
#include "QEmailNoteDialog.h"
#include "QNotesView.h"

#ifdef Q_WS_MAEMO_5
#include "modest_interface_p.h"
#endif

QNoteEditWindow::QNoteEditWindow(Note& note, bool bIsNewNote, QWidget *parent)
    : QvernoteWindow(parent), m_Note(note), isNewNote(bIsNewNote), nTodoTagCount(0), hasDirtyData(false)
{
	ui.setupUi(this);

	//QObject::connect(ui.wvNoteEdit, SIGNAL(loadFinished(bool)), this, SLOT(resizeWebView()));
	//m_hEvernote = dynamic_cast<QNotesView*>(this->parentWidget())->getHandle();
	m_hEvernote = QvernoteAPI::Instance();
	ui.wvNoteEdit->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, false);
	ui.wvNoteEdit->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

	if(isNewNote)
	{
		this->setWindowTitle(trUtf8("Edit note - Untitled"));
		//ui.wvNoteEdit->setHtml("<html><head/><body>&nbsp; </body></html>");
	}
	else
	{
		// Load and convert note content
		enmlReader = shared_ptr<QEnmlReader>(new QEnmlReader());

		if(m_Note.attributes.source != "web.clip")
			enmlReader->parse(m_Note);
		else
			enmlReader->parse(m_Note, 500000);

		nTodoTagCount = enmlReader->getTodoCount();
		m_HtmlData = enmlReader->getHtml();

		this->setWindowTitle("Edit note - " + QString::fromUtf8(m_Note.title.c_str()));

		//qDebug() << __FUNCTION__ << m_HtmlData;

		ui.wvNoteEdit->setHtml(m_HtmlData);
		ui.leNoteTitle->setText(QString::fromUtf8(m_Note.title.c_str()));
	}



	//QObject::connect(ui.wvNoteEdit->page(), SIGNAL(contentsChanged()), this, SLOT(resizeWebView()));
	QObject::connect(ui.wvNoteEdit, SIGNAL(linkClicked(QUrl)), this, SLOT(loadResource(QUrl)));

	if(m_Note.attributes.source != "web.clip")
	{
		ui.wvNoteEdit->page()->setContentEditable(true);
		ui.wvNoteEdit->setTextSizeMultiplier(1.4);
		ui.wvNoteEdit->installEventFilter(new QWebViewKeyPressFilter(this));
	}


#ifdef Q_WS_MAEMO_5
	supressor = new QMaemoTextSelectionSupressor(ui.wvNoteEdit);
	supressor->enable();
	source = NULL;
#endif

	QToolBar* htmlEditToolbar = new QToolBar();
	QAction* actBold = new QAction(QIcon(":/IconAdd/general_bold.png"), "Bold", this);
	QAction* actItalic = new QAction(QIcon(":/IconAdd/general_italic.png"), "Italic", this);
	QAction* actUnderline = new QAction(QIcon(":/IconAdd/notes_underline.png"), "Underline", this);
	QAction* actBullets = new QAction(QIcon(":/IconAdd/notes_bullets.png"), "Bullets", this);
	QAction* actAddTodo = new QAction(QIcon(":/IconAdd/calendar_todo.png"), "Todo", this);
	QAction* actAddTag = new QAction(QIcon(":/IconAdd/general_tag.png"), "Add tag", this);
	QAction* actAttachFile = new QAction(QIcon(":/IconAdd/email_attachment.png"), "Attach file", this);
	QAction* actEmailNote = new QAction(/*QIcon(":/IconAdd/general_email.png"),*/ "Email note", this);
	QAction* actSaveNote = new QAction(QIcon(":/IconAdd/notes_save.png"), "Save note", this);

	// GPS action is class-field and defined in .h file
	actGps = new QAction(QIcon(":/IconAdd/gps_location.png"), "Location", this);

	//QAction* actZoomin = new QAction(QIcon(":/IconAdd/note_zoomin.png"), "Zoom In", this);
	//QAction* actZoomout = new QAction(QIcon(":/IconAdd/note_zoomout.png"), "Zoom Out", this);

	actBold->setCheckable(true);
	actItalic->setCheckable(true);
	actUnderline->setCheckable(true);

	htmlEditToolbar->addAction(actBold);
	htmlEditToolbar->addAction(actItalic);
	htmlEditToolbar->addAction(actUnderline);
	htmlEditToolbar->addAction(actBullets);
	htmlEditToolbar->addAction(actAddTodo);
	htmlEditToolbar->addAction(actAttachFile);
	htmlEditToolbar->addAction(actAddTag);
	htmlEditToolbar->addAction(actGps);
	htmlEditToolbar->addAction(actSaveNote);
	//htmlEditToolbar->addAction(actZoomin);
	//htmlEditToolbar->addAction(actZoomout);

	ui.menubar->addAction(actEmailNote);
	ui.menubar->addAction(actSaveNote);
	//ui.horizontalLayout->addWidget(htmlEditToolbar);

	//QPushButton* pbSaveButton = new QPushButton("Save note");
	//pbSaveButton->setIcon(QIcon(":/IconAdd/notes_save.png"));
	//QPushButton* pbEmailButton = new QPushButton("Email note");
	//pbEmailButton->setIcon(QIcon(":/IconAdd/general_email.png"));
	//htmlEditToolbar->addWidget(pbEmailButton);
	//htmlEditToolbar->addWidget(pbSaveButton);

	this->addToolBar(htmlEditToolbar);

	QObject::connect(actSaveNote, SIGNAL(triggered(bool)), this, SLOT(saveNote()));
	QObject::connect(actEmailNote, SIGNAL(triggered(bool)), this, SLOT(emailNote()));
	QObject::connect(actAddTag, SIGNAL(triggered(bool)), this, SLOT(openTagSelectorDialog()));
	//QObject::connect(this, SIGNAL(noteListUpdated()), parent->parentWidget(), SLOT(loadNotesIntoView()));

	QObject::connect(actBold, SIGNAL(toggled(bool)), this, SLOT(toggleBoldStyle()));
	QObject::connect(actItalic, SIGNAL(toggled(bool)), this, SLOT(toggleItalicStyle()));
	QObject::connect(actUnderline, SIGNAL(toggled(bool)), this, SLOT(toggleUnderlineStyle()));
	QObject::connect(actBullets, SIGNAL(triggered()), this, SLOT(addBullet()));
	QObject::connect(actAttachFile, SIGNAL(triggered()), this, SLOT(attachFile()));
	QObject::connect(actAddTodo, SIGNAL(triggered()), this, SLOT(insertTodo()));
	QObject::connect(actGps, SIGNAL(triggered()), this, SLOT(startGpsLocationUpdate()));
	//QObject::connect(actZoomin, SIGNAL(triggered()), this, SLOT(zoominView()));
	//QObject::connect(actZoomout, SIGNAL(triggered()), this, SLOT(zoomoutView()));

	//QObject::connect(ui.wvNoteEdit, SIGNAL(textEdited(const QString&)), SLOT(setDirtyData()));
	QObject::connect(ui.leNoteTitle, SIGNAL(textEdited(const QString&)), SLOT(setDirtyData()));
	ui.wvNoteEdit->page()->mainFrame()->addToJavaScriptWindowObject("JSApi",this);
	ui.wvNoteEdit->setInputMethodHints(Qt::ImhPreferLowercase|Qt::ImhNoAutoUppercase);
	ui.leNoteTitle->setInputMethodHints(Qt::ImhPreferLowercase|Qt::ImhNoAutoUppercase);
}

QNoteEditWindow::~QNoteEditWindow()
{
#ifdef Q_WS_MAEMO_5
	delete supressor;
	if(source)
		delete source;
#endif
}

void QNoteEditWindow::hideEvent(QHideEvent* e)
{
	if(hasDirtyData)
	{
		if(QMessageBox::question(this, trUtf8("Save note"), trUtf8("Note has been modified. Save it now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
		{
			saveNote();
		}
	}

	QWidget::hideEvent(e);
}

void QNoteEditWindow::saveNote()
{
	setProgressIndicator(true);

	if(m_Note.attributes.source != "web.clip")
	{
		QWebElement doc = ui.wvNoteEdit->page()->mainFrame()->documentElement();

		m_HtmlData = doc.toOuterXml();
		QRegExp rBrTag("(<br[^>]*)>");
		QRegExp rImgTag("(<img src=\"[^>]+\")>");
		QRegExp rInputTag("(input[^>]+)>");

		m_HtmlData.replace(rBrTag, "\\1/>");
		m_HtmlData.replace(rImgTag, "\\1/>");
		m_HtmlData.replace(rInputTag, "\\1/>");
		m_HtmlData.replace('&', "&amp;");

		//qDebug() << __FUNCTION__ << m_HtmlData;
		//qDebug() << __FUNCTION__ << "resource size:" << m_Note.resources.size();
		enmlWriter = shared_ptr<QEnmlWriter>(new QEnmlWriter(&m_Note,  m_HtmlData.toUtf8()));
		enmlWriter->setNoteTitle(ui.leNoteTitle->text().toUtf8());
		enmlWriter->createNote();
	}
	else
	{
		// Allow changing title only in webclips
		m_Note.title = QString(ui.leNoteTitle->text().toUtf8()).toStdString();
	}

	if(m_Note.title.empty())
	{
		displayError(trUtf8("Unable to save note"), trUtf8("Title cannot be empty"));
		setProgressIndicator(false);
	}
	else
		m_pBackgroundJob->start();
}

bool QNoteEditWindow::createNote()
{
	if(isNewNote)
	{
		return m_hEvernote->createNewNote(m_Note/*enmlWriter->getNote()*/);
	}
	else
	{
		return m_hEvernote->updateExistingNote(m_Note/*enmlWriter->getNote()*/);
	}
}

void QNoteEditWindow::noteSaveFailure()
{
	if(isNewNote)
	{
		displayError(trUtf8("Failed to save note"), QString(m_hEvernote->getLastErrorString().c_str()));
	}
	else
	{
		displayError(trUtf8("Failed to update note"), QString(m_hEvernote->getLastErrorString().c_str()));
	}

	setProgressIndicator(false);
}

void QNoteEditWindow::noteSaveSuccess()
{
	setProgressIndicator(false);
	emit noteUpdated();
	hasDirtyData = false;
	isNewNote = false;
}

void QNoteEditWindow::toggleBoldStyle()
{
	ui.wvNoteEdit->triggerPageAction(QWebPage::ToggleBold);
}

void QNoteEditWindow::toggleItalicStyle()
{
	ui.wvNoteEdit->triggerPageAction(QWebPage::ToggleItalic);
}

void QNoteEditWindow::toggleUnderlineStyle()
{
	ui.wvNoteEdit->triggerPageAction(QWebPage::ToggleUnderline);
}

void QNoteEditWindow::addBullet()
{
	QWebElement bodyElem = ui.wvNoteEdit->page()->mainFrame()->documentElement().findFirst("body");
	bodyElem.appendInside("<ul><li> </li></ul>");
}

void QNoteEditWindow::attachFile()
{
	QStringList fileNames;
	//QString mediaType;
	//QFileDialog dlg(this, "Choose file");
	//dlg.setNameFilter(tr("Images (*.png *.gif *.jpg) Audio (*.wav *.mpg *.amr *.mp3) Documents (*.pdf) Other (*)"));
	//dlg.setFileMode(QFileDialog::ExistingFiles);

	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
			trUtf8("Choose file"),
			"/",
			trUtf8("Images (*.png *.gif *.jpg);;Audio (*.wav *.mpg *.amr *.mp3);;Documents (*.pdf);;Other (*)"),
			&selectedFilter,
			options);

	if(fileName.isEmpty())
		return;

	QFileInfo fi(fileName);
	QString htmlTag;
	QString htmlNode;
	QHtmlContentHandler::HtmlElement htmlElement = QHtmlContentHandler::getHtmlTag(fileName, htmlTag);
	QString contentType;
	QByteArray content;

	switch(htmlElement) {
	case QHtmlContentHandler::HTML_IMG:
		content = QHtmlContentHandler::getStoredContent(fileName, contentType);
		htmlNode = "<tag_image filename=\"" + fi.fileName() + "\" src=\"" + fileName + "\"><img src=\"data:" + contentType + ";base64," + content.toBase64() + "\"/></tag_image>";
		break;
	default:
		{
			QAttachmentWidget attWidget(fi);
			QByteArray widgetIcon = QAttachmentWidget::toPng(&attWidget);
			contentType = QHtmlContentHandler::getContentType(fileName);
			qDebug() << __FUNCTION__ << fileName;
			htmlNode = "<" + htmlTag + " filename=\"" + fi.fileName() + "\" src=\"" + fileName + "\"><img src=\"data:image/png;base64," + widgetIcon.toBase64() + "\"/></" + htmlTag + ">";
			break;
		}
	}
	//QByteArray content = QHtmlContentHandler::getStoredContent(fileName, contentType);
	//qDebug() << "Tag" << htmlTag;
	//ui.wvNoteEdit->page()->

	//QString newHtml = ui.wvNoteEdit->page()->mainFrame()->toHtml().replace("</body></html>",
	//		"<" + htmlTag + " src=\"" + fileName + "\"/></body></html>");

	QWebElement bodyElem = ui.wvNoteEdit->page()->mainFrame()->documentElement().findFirst("body");
	//qDebug() << "bodyElem" << bodyElem.localName();
	//QWebElement elem2 = elem.firstChild();
	//qDebug() << "firstChild" << elem2.localName();

	bodyElem.appendInside(htmlNode + "<br/><br/>");

	//qDebug() << "before set content: " << newHtml;

	//ui.wvNoteEdit->setContent(newHtml.toLocal8Bit(), "application/xhtml+xml");
	//ui.wvNoteEdit->setHtml(newHtml, QUrl("/"));


	//qDebug() << "New html: " << ui.wvNoteEdit->page()->mainFrame()->documentElement().toOuterXml();
}

void QNoteEditWindow::emailNote()
{
	qDebug() << "Email note";
	QEmailNoteDialog pDlg(m_Note, m_hEvernote, this);
	pDlg.exec();
}

void QNoteEditWindow::resizeWebView()
{
	QSize s = ui.wvNoteEdit->page()->mainFrame()->contentsSize();
	const QRect fr = ui.wvNoteEdit->frameGeometry();
	const QRect cr = ui.wvNoteEdit->contentsRect();

	//s.setHeight(s.height() + 1 * ui.wvNoteEdit->fontMetrics().lineSpacing());
	ui.wvNoteEdit->setMinimumHeight(qMax(70, s.height()  + (fr.height() - cr.height() - 1)));
}

void QNoteEditWindow::openTagSelectorDialog()
{
	QTagSelectorDialog pDlg(m_Note, this);
	pDlg.exec();
}

void QNoteEditWindow::setTag(bool state, Tag tag)
{
	if(state)
		m_hEvernote->addExistingTag(tag, /*enmlWriter->getNotePtr()*/m_Note);
	else
		m_hEvernote->removeExistingTag(tag, /*enmlWriter->getNotePtr()*/m_Note);
}

void QNoteEditWindow::loadResource(QUrl url)
{
	setProgressIndicator(true);
	//qDebug() << __FUNCTION__ << QByteArray::fromHex(url.toString().toAscii());
	Resource r;

	if(m_hEvernote->loadResourceByHash(r, m_Note.guid, QByteArray::fromHex(url.toString().toAscii()).data()) == true)
	{
		QString fileName = QFileDialog::getSaveFileName(
				this,
				trUtf8("Save resource"),
		        "/MyDocs/" + QString::fromStdString(r.attributes.fileName),
		        trUtf8("Images (*.png *.gif *.jpg);;Audio (*.wav *.mpg *.amr *.mp3);;Documents (*.pdf);;Other (*)"));

		if(fileName.size() > 0)
		{
			QFile file(fileName);
			if (!file.open(QIODevice::WriteOnly))
			{
				displayError(trUtf8("Cannot write file"), file.errorString());
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
		displayError(trUtf8("Failed to load resource"), QString::fromStdString(m_hEvernote->getLastErrorString()));
	}
	setProgressIndicator(false);
}

void QNoteEditWindow::insertTodo()
{
	QWebElement bodyElem = ui.wvNoteEdit->page()->mainFrame()->documentElement().findFirst("body");
	QString todoTag = "<tag_todo checked=\"false\"><input id=\"" + QString::number(nTodoTagCount) + "\" type=\"checkbox\" onclick=\"javascript:JSApi.setChecked(" + QString::number(nTodoTagCount) + ")\"/></tag_todo><br/>";

	bodyElem.appendInside(todoTag);
	nTodoTagCount++;
}

void QNoteEditWindow::setChecked(int tagId)
{
	QWebElement inputElem =  ui.wvNoteEdit->page()->mainFrame()->documentElement().findFirst("input[id=\"" + QString::number(tagId) + "\"]");

	//qDebug() << __FUNCTION__ << "Tag:" << inputElem.tagName();
	QWebElement tagTodoElem = inputElem.parent();
	//qDebug() << __FUNCTION__ << "Tag:" << tagTodoElem.tagName();

	if(tagTodoElem.attribute("checked") == "false")
		tagTodoElem.setAttribute("checked", "true");
	else
		tagTodoElem.setAttribute("checked", "false");
}

/*
void QNoteEditWindow::zoominView()
{
	ui.wvNoteEdit->setZoomFactor(ui.wvNoteEdit->zoomFactor()+0.1);
}

void QNoteEditWindow::zoomoutView()
{
	ui.wvNoteEdit->setZoomFactor(ui.wvNoteEdit->zoomFactor()-0.1);
}
*/

void QNoteEditWindow::startGpsLocationUpdate()
{
#ifdef Q_WS_MAEMO_5
	actGps->setEnabled(false);

	if(source)
		delete source;

	source = QGeoPositionInfoSource::createDefaultSource(this);
	if (source)
	{
		connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
	    source->setUpdateInterval(5000);
	    source->startUpdates();
	    displayInformationBox(trUtf8("GPS position search started. Please wait..."));
	}
#else
	QMessageBox::critical(this, "Error", "Maemo5 specific code");
#endif
}

#ifdef Q_WS_MAEMO_5
void QNoteEditWindow::positionUpdated(const QGeoPositionInfo& update)
{
	m_Note.attributes.latitude = update.coordinate().latitude();
	m_Note.attributes.longitude = update.coordinate().longitude();

	//qDebug() << m_Note.attributes.latitude << m_Note.attributes.longitude;

	displayInformationBox(trUtf8("GPS position updated"));
	source->stopUpdates();

	actGps->setEnabled(true);
}
#endif
