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
#include "QTagSelectorDialog.h"
#include <QMessageBox>
#include <QDebug>
#include "QTagButton.h"
#include "QvernoteWindow.h"
#include "QNewTagDialog.h"
#ifdef Q_WS_MAEMO_5
#include <QMaemo5InformationBox>
#include <QDesktopWidget>
#endif

QTagSelectorDialog::QTagSelectorDialog(const Note& note, QWidget *parent)
    : QDialog(parent), m_vTagNames(note.tagNames), m_hEvernote(QvernoteAPI::Instance()), sortOrder(false)
{
	ui.setupUi(this);

	if(note.title.empty())
	{
		this->setWindowTitle(trUtf8("Tags for Untitled"));
	}
	else
	{
		this->setWindowTitle(trUtf8("Tags for ") + QString::fromUtf8(note.title.c_str()));
	}

	QObject::connect(ui.pbNewTag, SIGNAL(clicked()), this, SLOT(addNewTag()));
	QObject::connect(ui.pbDone, SIGNAL(clicked()), this, SLOT(close()));
	QObject::connect(ui.pbSortName, SIGNAL(clicked()), this, SLOT(sortName()));

	if(m_hEvernote->loadTagList() == false)
	{
#ifdef Q_WS_MAEMO_5
		QMaemo5InformationBox::information(this, trUtf8("Error loading tag list: ") + QString(m_hEvernote->getLastErrorString().c_str()), QMaemo5InformationBox::DefaultTimeout);
#else
		QMessageBox::critical(this, trUtf8("Error loading tag list"), QString(m_hEvernote->getLastErrorString().c_str()));
#endif
	}

	m_hEvernote->sortTags(sortOrder);
	loadTagsIntoView();
}

QTagSelectorDialog::~QTagSelectorDialog()
{

}

void QTagSelectorDialog::addTag(Tag& tag)
{
	QTagButton* tagLabel;

	tagLabel = new QTagButton(tag, ui.saTagArea->widget());
	tagLabel->adjustGeometry(geom);
	tagLabel->show();

	if(std::find(m_vTagNames.begin(), m_vTagNames.end(), tag.name) != m_vTagNames.end())
	{
		tagLabel->setChecked(true);
	}

	connect(tagLabel, SIGNAL(tagToggled(bool, Tag)), /*m_parent*/this->parentWidget(), SLOT(setTag(bool, Tag)));
}

void QTagSelectorDialog::loadTagsIntoView()
{
	bool hasMoreItems;
	Tag tag;

	foreach(QObject* child, ui.saTagArea->widget()->children()) {
		delete child;
	}
	geom = QRect();
	//ui.saTagArea->resize(QSize(0,0));
	for(hasMoreItems = m_hEvernote->getFirstTag(tag); hasMoreItems; hasMoreItems = m_hEvernote->getNextTag(tag))
	{
		addTag(tag);
	}
}

void QTagSelectorDialog::addNewTag()
{
	QNewTagDialog pDlg;
	pDlg.exec();

	if(pDlg.getTagName().isEmpty())
		return;

	Tag newTag;

#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
	if(m_hEvernote->addNewTag(pDlg.getTagName().toStdString(), newTag) == true)
	{
		addTag(newTag);
	}
	else
	{
#ifdef Q_WS_MAEMO_5
		QMaemo5InformationBox::information(this, trUtf8("Failed to add new tag: ") + QString(m_hEvernote->getLastErrorString().c_str()), QMaemo5InformationBox::DefaultTimeout);
#else
		QMessageBox::critical(this, trUtf8("Failed to add new tag"), QString(m_hEvernote->getLastErrorString().c_str()));
#endif
	}
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
}

void QTagSelectorDialog::sortName()
{
	sortOrder = !sortOrder;
	m_hEvernote->sortTags(sortOrder);

	loadTagsIntoView();
}
