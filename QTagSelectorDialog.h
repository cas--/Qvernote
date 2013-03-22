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
