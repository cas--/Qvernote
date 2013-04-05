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
