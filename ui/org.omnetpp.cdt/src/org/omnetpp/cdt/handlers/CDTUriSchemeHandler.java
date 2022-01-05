/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.handlers;

import java.net.URI;

import org.apache.http.client.utils.URLEncodedUtils;
import org.eclipse.urischeme.IUriSchemeHandler;
import org.omnetpp.cdt.CDTUtils;

public class CDTUriSchemeHandler implements IUriSchemeHandler {

	public static final String OMNETPP_CDT_SCHEME = "omnetpp"; //$NON-NLS-1$
	public static final String OMNETPP_CDT_AUTHORITY = "cdt"; //$NON-NLS-1$
	public static final String OMNETPP_CDT_PATH_ATTACH = "/debugger/attach"; //$NON-NLS-1$

	@Override
	public void handle(String cdtUri) {
		var uri = URI.create(cdtUri);
		if (!OMNETPP_CDT_SCHEME.equals(uri.getScheme()) || !OMNETPP_CDT_AUTHORITY.equals(uri.getAuthority()))
			return;
		
		if (OMNETPP_CDT_PATH_ATTACH.equals(uri.getPath())) {
			for (var p : URLEncodedUtils.parse(uri, "UTF-8"))
				if (p.getName().equals("pid"))
					CDTUtils.attach(p.getValue());
		}		
	}
}
