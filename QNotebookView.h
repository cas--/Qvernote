#ifndef QNOTEBOOKVIEW_H
#define QNOTEBOOKVIEW_H

#include <QtGui/QWidget>
#include "ui_QNotebookView.h"
#include "QvernoteAPI.h"
#include "QNotebookListItem.h"

namespace qvernote { namespace view {
using namespace api;

class QNotebookView : public QWidget
{
    Q_OBJECT

public:
    QNotebookView(shared_ptr<QvernoteAPI> hEvernote, QWidget *parent = 0);
    ~QNotebookView();

private:
    void loadNotebookList();

private:
    Ui::QNotebookViewClass ui;
    shared_ptr<QvernoteAPI> m_hEvernote;
};

}}
#endif // QNOTEBOOKVIEW_H
