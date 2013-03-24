TEMPLATE = app
TARGET = qvernote
INCLUDEPATH += /usr/include/thrift \
    /usr/include/evernote \
    /usr/include/poppler/qt4
LIBS = -lEvernote \
    -lpoppler-qt4 \
    -lthrift
QT += core \
    gui \
    webkit \
    xml \
    sql \
    phonon \
    network \
    maemo5
TRANSLATIONS += po/qvernote_ru.po \
    po/qvernote_he.po \
    po/qvernote_de.po \
    po/qvernote_fr.po \
    po/qvernote_it.po \
    po/qvernote_ca.po \
    po/qvernote_cz.po \
    po/qvernote_da.po \
    po/qvernote_el.po \
    po/qvernote_es.po \
    po/qvernote_et.po \
    po/qvernote_fi.po \
    po/qvernote_hr.po \
    po/qvernote_hu.po \
    po/qvernote_ja.po \
    po/qvernote_ko.po \
    po/qvernote_no.po \
    po/qvernote_nl.po \
    po/qvernote_pl.po \
    po/qvernote_pt.po \
    po/qvernote_ro.po \
    po/qvernote_sv.po \
    po/qvernote_zh-cn.po \
    po/qvernote_zh-tw.po
maemo5 {
    DEFINES += Q_WS_MAEMO_5
    QT += dbus
    CONFIG += mobility
    MOBILITY = contacts \
        location
    HEADERS += QMaemoMapViewDialog.h \
        QMaemoContactEmailListWidget.h \
        modest_interface_p.h \
        QMaemoTextSelectionSupressor.h \
        QOptionsDialog.h
    SOURCES += modest_interface.cpp \
        QOptionsDialog.cpp
}
HEADERS += QAudioPreviewDialog.h \
    QPdfPreviewDialog.h \
    QPopplerSupport.h \
    QGoogleMapViewDialog.h \
    QThumbnailCreator.h \
    QAbstractNoteView.h \
    QNoteListView.h \
    QNoteGridView.h \
    QKeyGrabber.h \
    QNotebookPickList.h \
    QSyncProgressWidget.h \
    QSyncProgressDialog.h \
    QEmailNoteDialog.h \
    QNewTagDialog.h \
    QTrashWindow.h \
    DataTypes/QDataTypes.hpp \
    QvernoteDataFactory.h \
    QvernoteDBSchema.h \
    QvernoteStorage.h \
    QNoteViewWindow.h \
    QAttachmentWidget.h \
    QAboutDialog.h \
    QDeleteNotebookView.h \
    QTagButton.h \
    QClickableLabel.h \
    QTagSelectorDialog.h \
    QDeleteNotesView.h \
    QNoteListItemWidget.h \
    QNonScrollableListWidget.h \
    QvernoteWindow.h \
    QNoteSearchDialog.h \
    QvernoteTypes.h \
    QNotebookListItemWidget.h \
    md5.h \
    QEnmlWriter.h \
    QNewNotebookDialog.h \
    QNotesViewGraphicsWidget.h \
    QEnmlReader.h \
    QNoteEditWindow.h \
    QNotesView.h \
    Qvernote.h \
    QvernoteAPI.h \
    QvernoteSettings.h \
    QvernoteView.h \
    qvernotelogindialog.h \
    logger/qsdebugoutput.h \
    logger/qslogdest.h \
    logger/qslog.h \
    oauth/oauthtokenizer.h \
    oauth/oauthwindow.h
SOURCES += DataTypes/QFavorite.cpp \
    DataTypes/QNoteThumbnail.cpp \
    QSyncProgressDialog.cpp \
    QEmailNoteDialog.cpp \
    QNewTagDialog.cpp \
    QTrashWindow.cpp \
    DataTypes/QData.cpp \
    DataTypes/QItem.cpp \
    DataTypes/QNote.cpp \
    DataTypes/QNoteAttributes.cpp \
    DataTypes/QNoteResource.cpp \
    DataTypes/QNotebook.cpp \
    DataTypes/QResourceAttributes.cpp \
    DataTypes/QTag.cpp \
    QvernoteDataFactory.cpp \
    QvernoteStorage.cpp \
    QNoteViewWindow.cpp \
    QAttachmentWidget.cpp \
    QAboutDialog.cpp \
    QDeleteNotebookView.cpp \
    QvernoteWindow.cpp \
    QTagButton.cpp \
    QTagSelectorDialog.cpp \
    QDeleteNotesView.cpp \
    QNoteListItemWidget.cpp \
    QNoteSearchDialog.cpp \
    QNotebookListItemWidget.cpp \
    md5.c \
    QEnmlWriter.cpp \
    QNewNotebookDialog.cpp \
    QNotesViewGraphicsWidget.cpp \
    QEnmlReader.cpp \
    QNoteEditWindow.cpp \
    QNotesView.cpp \
    QvernoteAPI.cpp \
    QvernoteSettings.cpp \
    main.cpp \
    QvernoteView.cpp \
    qvernotelogindialog.cpp \
    Qvernote.cpp \
    logger/qsdebugoutput.cpp \
    logger/qslog.cpp \
    logger/qslogdest.cpp \
    oauth/oauthtokenizer.cpp \
    oauth/oauthwindow.cpp
FORMS += QOptionsDialog.ui \
    QSyncProgressDialog.ui \
    QEmailNoteDialog.ui \
    QNewTagDialog.ui \
    QTrashWindow.ui \
    QNoteViewWindow.ui \
    QAttachmentWidget.ui \
    QAboutDialog.ui \
    QDeleteNotebookView.ui \
    QTagSelectorDialog.ui \
    QDeleteNotesView.ui \
    QNoteListItemWidget.ui \
    QNoteSearchDialog.ui \
    QNotebookListItemWidget.ui \
    QNewNotebookDialog.ui \
    QNoteEditWindow.ui \
    QNotesView.ui \
    QvernoteView.ui \
    qvernotelogindialog.ui
RESOURCES += Resources/Icons.qrc
unix {
    # VARIABLES
    PREFIX = debian/$${TARGET}
    BINDIR = $$PREFIX/opt/qvernote/
    LIBDIR = $$PREFIX/opt/lib
    DATADIR = $$PREFIX/usr/share
    DEFINES += DATADIR=\"$$DATADIR\" \
        PKGDATADIR=\"$$PKGDATADIR\"

    # MAKE INSTALL
    INSTALLS += target \
        loader \
        desktop
    INSTALLS += pixmap \
        icon26 \
        icon40 \
        icon48 \
        icon64
    target.path = $$BINDIR
    loader.path = $$BINDIR
    loader.files = $${TARGET}.sh
    desktop.path = $$DATADIR/applications/hildon
    desktop.files += $${TARGET}.desktop
    pixmap.path = $$DATADIR/pixmaps
    pixmap.files += data/48x48/$${TARGET}.png
    icon26.path = $$DATADIR/icons/hicolor/26x26/apps
    icon26.files = data/26x26/$${TARGET}.png
    icon40.path = $$DATADIR/icons/hicolor/40x40/apps
    icon40.files = data/40x40/$${TARGET}.png
    icon48.path = $$DATADIR/icons/hicolor/48x48/apps
    icon48.files = data/48x48/$${TARGET}.png
    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files = data/64x64/$${TARGET}.png
}
