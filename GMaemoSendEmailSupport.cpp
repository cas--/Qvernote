/*
 * GMaemoSendEmailSupport.cpp
 *
 *  Created on: May 26, 2010
 *      Author: alex
 */

#include "GMaemoSendEmailSupport.h"

GMaemoSendEmailSupport::GMaemoSendEmailSupport() {
	// TODO Auto-generated constructor stub

}

GMaemoSendEmailSupport::~GMaemoSendEmailSupport() {
	// TODO Auto-generated destructor stub
}

bool GMaemoSendEmailSupport::send() {
	gboolean result = TRUE;
	GSList *list = NULL;

	osso_context_t *osso = osso_initialize ("my_app", "1.0.0.", FALSE, NULL);
	list = g_slist_append(list, (void*)"filename");
	result = libmodest_dbus_client_compose_mail(osso, /*osso_context_t*/
	    NULL, /*to*/
	    NULL, /*cc*/
	    NULL, /*bcc*/
	    NULL, /*body*/
	    NULL, /*subject*/
	    list /*attachments*/);

	 g_slist_free(list);
	 if (result == FALSE)
		 g_print("Could not send via email\n");
}
