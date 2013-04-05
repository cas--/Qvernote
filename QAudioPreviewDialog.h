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
#ifndef QAUDIOPREVIEWDIALOG_H_
#define QAUDIOPREVIEWDIALOG_H_

#include <QtGui/QDialog>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include "QvernoteWindow.h"
#include "QvernoteAPI.h"
#include <QDebug>

class QMediaBuffer : public QBuffer
{
public:
	QMediaBuffer(QObject* parent = 0) : QBuffer(parent) { }

	bool isSequential () const { return false; }
};

class QAudioPreviewDialog : public QDialog
{
	Q_OBJECT

public:
	QAudioPreviewDialog(Resource res, QWidget* parent = 0) : QDialog(parent) {
		setWindowTitle(trUtf8("Preview audio resource ") + QString::fromStdString(res.attributes.fileName));
		layout = new QGridLayout(this);
		layout->setContentsMargins(8, 8, 8, 8);
		bPlay.setText(trUtf8("Play"));
		layout->addWidget(&bPlay, 1, 1);

		bStop.setText(trUtf8("Stop"));
		layout->addWidget(&bStop, 1, 2);

		audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
		mediaObject = new Phonon::MediaObject(this);
		//metaInformationResolver = new Phonon::MediaObject(this);

		mediaObject->setTickInterval(1000);
		//connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
		connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
		             this, SLOT(stateChanged(Phonon::State,Phonon::State)));
		//connect(metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
		//             this, SLOT(metaStateChanged(Phonon::State,Phonon::State)));
		//connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
		//             this, SLOT(sourceChanged(Phonon::MediaSource)));

	    Phonon::createPath(mediaObject, audioOutput);
	    mediaBuf = QByteArray::fromRawData(res.data.body.c_str(), res.data.size);
	    mediaBuffer.setData(mediaBuf);
	    mediaBuffer.open(QIODevice::ReadOnly);
	    qDebug() << mediaBuffer.size();
	    Phonon::MediaSource source(&mediaBuffer);
	    //metaInformationResolver->setCurrentSource(source);
	    mediaObject->setCurrentSource(source);
	    seekSlider = new Phonon::SeekSlider(this);
	    seekSlider->setMediaObject(mediaObject);
	    layout->addWidget(seekSlider, 1, 3);

	    QObject::connect(&bPlay, SIGNAL(clicked()), mediaObject, SLOT(play()));
	    QObject::connect(&bStop, SIGNAL(clicked()), mediaObject, SLOT(stop()));
	}

public slots:
	void stateChanged(Phonon::State newState, Phonon::State /* oldState */) {
	     switch (newState) {
	         case Phonon::ErrorState:
	             if (mediaObject->errorType() == Phonon::FatalError) {
	                 QvernoteWindow::displayError(parentWidget(), trUtf8("Fatal Error"),
	                 mediaObject->errorString());
	             } else {
	            	 QvernoteWindow::displayError(parentWidget(), trUtf8("Error"),
	                 mediaObject->errorString());
	             }
	             break;
	         case Phonon::PlayingState:
	                 bPlay.setEnabled(false);
	                 bStop.setEnabled(true);
	                 break;
	         case Phonon::StoppedState:
	                 bStop.setEnabled(false);
	                 bPlay.setEnabled(true);
	                 break;
	         case Phonon::BufferingState:
	                 break;
	         default:
	             ;
	     }
	 }

private:
	QGridLayout* layout;
	QPushButton bPlay;
	QPushButton bStop;
	QByteArray mediaBuf;
	QMediaBuffer mediaBuffer;

	Phonon::SeekSlider *seekSlider;
	Phonon::MediaObject *mediaObject;
	//Phonon::MediaObject *metaInformationResolver;
	Phonon::AudioOutput *audioOutput;
};

#endif /* QAUDIOPREVIEWDIALOG_H_ */
