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
#include "QNoteSearchDialog.h"
#include "QvernoteView.h"
#include "QTagSelectorDialog.h"
#include "QNotebookPickList.h"

QNoteSearchDialog::QNoteSearchDialog(QWidget *parent)
    : QDialog(parent), m_parent(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pbStartSearch, SIGNAL(clicked()), this, SLOT(searchNote()));
	QObject::connect(ui.pbSearchTags, SIGNAL(clicked()), this, SLOT(openTagSelectionDialog()));
	QObject::connect(ui.pbSearchNotebook, SIGNAL(clicked()), this, SLOT(openNotebookSelectionDialog()));
}

QNoteSearchDialog::~QNoteSearchDialog()
{

}

void QNoteSearchDialog::searchNote()
{
	std::vector<std::string> tagGuids;
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
	foreach(QStringList tagData, selectedTagsList)
	{
		tagGuids.push_back(tagData[1].toStdString());
	}

	((QvernoteView*)m_parent)->searchNote(ui.leSearchString->text(), tagGuids, (selectedNotebook.size() > 1)? selectedNotebook[1].toStdString() : "");
	close();
}

void QNoteSearchDialog::openTagSelectionDialog()
{
	QString tagList;
	selectedTagsList.clear();
	QTagSelectorDialog pDlg(Note(), this);
	pDlg.exec();

	foreach(QStringList tagData, selectedTagsList) { tagList += tagData[0] + " "; }
	if(tagList.size() > 0)
		ui.pbSearchTags->setText(tagList);
	else
		ui.pbSearchTags->setText(trUtf8("Tags"));
}

void QNoteSearchDialog::openNotebookSelectionDialog()
{
	QNotebookPickList dlg(trUtf8("All Notes"), this);
	dlg.loadNotebooks();
	if(dlg.exec() == QDialog::Rejected)
		return;

	selectedNotebook.clear();
	selectedNotebook.append(dlg.getSelectedName());
	selectedNotebook.append(dlg.getSelectedGuid());

	ui.pbSearchNotebook->setText(selectedNotebook[0]);
}

void QNoteSearchDialog::setTag(bool , Tag tag)
{
	foreach(QStringList tagData, selectedTagsList) { if(tagData[0].toStdString() == tag.name) return; }

	selectedTagsList.append(QStringList() << QString::fromStdString(tag.name) << QString::fromStdString(tag.guid));
}
