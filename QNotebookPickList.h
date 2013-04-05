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
#ifndef QNOTEBOOKPICKLIST_H_
#define QNOTEBOOKPICKLIST_H_

#include <QDialog>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "QvernoteAPI.h"



class QNotebookButton : public QPushButton
{
	Q_OBJECT

public:
	QNotebookButton(QString notebookName, QString notebookGuid, QWidget* parent = 0) : QPushButton(parent) {
		m_notebookName = notebookName;
		m_notebookGuid = notebookGuid;
		setText(m_notebookName);

		QObject::connect(this, SIGNAL(clicked()), SLOT(notebookSelected()));
		QObject::connect(this, SIGNAL(toggled(bool)), SLOT(notebookSelected(bool)));
	}

	QString getNotebookName() { return m_notebookName; }
	QString getNotebookGuid() { return m_notebookGuid; }

public slots:
	void notebookSelected() {
		emit sendNotebookInfo(m_notebookName, m_notebookGuid);
	}

	void notebookSelected(bool checked) {
		emit sendNotebookInfo(checked, m_notebookName, m_notebookGuid);
	}
signals:
	void sendNotebookInfo(QString, QString);
	void sendNotebookInfo(bool, QString, QString);

private:
	QString m_notebookName;
	QString m_notebookGuid;
};

class QNotebookPickList : public QDialog
{
	Q_OBJECT

public:
	QNotebookPickList(QString currentNotebookName, QWidget* parent = 0)
	: QDialog(parent), m_sCurrentNotebookName(currentNotebookName), m_bIsMultipleSelection(false) {
		mainLayout = new QGridLayout(this);
		layout = new QVBoxLayout(&w);
		setWindowTitle(trUtf8("Choose notebook"));

		area.setWidget(&w);
		area.setWidgetResizable(true);
		mainLayout->addWidget(&area);
	}

	void setMultipleSelection(bool isMultipleSelection) { m_bIsMultipleSelection = isMultipleSelection; }

	void loadNotebooks(const QStringList& selectedNotebooks = QStringList()) {
		Notebook notebook;
		QNotebookButton* notebookButton;
		bool hasMoreItems;
		QvernoteAPI* api = QvernoteAPI::Instance();

		for(hasMoreItems = api->getFirstNotebook(notebook); hasMoreItems; hasMoreItems = api->getNextNotebook(notebook)) {
			if(m_sCurrentNotebookName != QString::fromStdString(notebook.name)) {
				notebookButton = new QNotebookButton(QString::fromStdString(notebook.name), QString::fromStdString(notebook.guid), this);

				if(m_bIsMultipleSelection) {
					notebookButton->setCheckable(true);

					if(selectedNotebooks.contains(QString::fromStdString(notebook.name)) ||
					   selectedNotebooks.contains(QString::fromStdString(notebook.guid)))
						notebookButton->setChecked(true);

					QObject::connect(notebookButton, SIGNAL(sendNotebookInfo(bool, QString, QString)), this, SLOT(notebookGuidSelected(bool, QString, QString)));
				}
				else
				{
					QObject::connect(notebookButton, SIGNAL(sendNotebookInfo(QString, QString)), this, SLOT(notebookGuidSelected(QString, QString)));
				}

				layout->addWidget(notebookButton);
			}
		}

		// Add "All Notes" button in case of currentNotebookName == "All Notes"
		if(m_sCurrentNotebookName == trUtf8("All Notes"))
		{
			notebookButton = new QNotebookButton(m_sCurrentNotebookName, "", this);

			if(m_bIsMultipleSelection)
				notebookButton->setCheckable(true);

			QObject::connect(notebookButton, SIGNAL(sendNotebookInfo(QString, QString)), this, SLOT(notebookGuidSelected(QString, QString)));
			layout->addWidget(notebookButton);
		}
	}

	QSize sizeHint() const {
		return QSize(800, 70*5);
	}

	QString getSelectedGuid() { return m_selectedGuid; }
	QString getSelectedName() { return m_selectedName; }

public slots:
	void notebookGuidSelected(QString notebookName, QString notebookGuid) {
		m_selectedName = notebookName;
		m_selectedGuid = notebookGuid;

		done(QDialog::Accepted);
		//close();
	}

	void notebookGuidSelected(bool checked, QString notebookName, QString notebookGuid) {
		m_selectedName = notebookName;
		m_selectedGuid = notebookGuid;
		if(m_bIsMultipleSelection)
			emit notebookSelected(checked, m_selectedName, m_selectedGuid);
		else
			done(QDialog::Accepted);
	}

signals:
	void notebookSelected(QString, QString);
	void notebookSelected(bool, QString, QString);

private:
	QScrollArea area;
	QWidget w;
	QVBoxLayout* layout;
	QGridLayout* mainLayout;
	QString m_selectedGuid;
	QString m_selectedName;
	QString m_sCurrentNotebookName;
	bool m_bIsMultipleSelection;
};

#endif /* QNOTEBOOKPICKLIST_H_ */
