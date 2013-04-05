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
#ifndef QTAGSELECTORDIALOG_H
#define QTAGSELECTORDIALOG_H

#include <QtGui/QDialog>
#include "ui_QTagSelectorDialog.h"
#include "QvernoteAPI.h"
#include <QGridLayout>



class QTagSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    QTagSelectorDialog(const Note& note = Note(), QWidget *parent = 0);
    ~QTagSelectorDialog();

    void loadTagsIntoView();
    void addTag(Tag& tag);

public slots:
	void addNewTag();
	void sortName();

private:
    Ui::QTagSelectorDialogClass ui;
    vector<string> m_vTagNames;
    //vector<string> tagGuids;
    QvernoteAPI* m_hEvernote;
    QRect geom;
    bool	sortOrder;
};

#endif // QTAGSELECTORDIALOG_H
