/*
 * GMaemoSendEmailSupport.h
 *
 *  Created on: May 26, 2010
 *      Author: alex
 */

#ifdef Q_WS_MAEMO_5

#ifndef GMAEMOSENDEMAILSUPPORT_H_
#define GMAEMOSENDEMAILSUPPORT_H_

#include <libmodest-dbus-client/libmodest-dbus-client.h>


class GMaemoSendEmailSupport {
public:
	GMaemoSendEmailSupport();
	virtual ~GMaemoSendEmailSupport();

	bool send();
};

#endif /* GMAEMOSENDEMAILSUPPORT_H_ */

#endif // Q_WS_MAEMO_5
