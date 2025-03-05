/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.dsp;

import java.net.URI;

import org.apache.http.client.utils.URLEncodedUtils;
import org.eclipse.urischeme.IUriSchemeHandler;

/**
 * Implements support for the "omnetpp://dsp/debugger/attach?pid=<pid>" URL that
 * instructs the IDE to attach to the process identified by <pid> and start
 * debugging it.
 */
@SuppressWarnings("restriction")
public class DSPUriSchemeHandler implements IUriSchemeHandler {

    public static final String OMNETPP_URI_SCHEME = "omnetpp"; //$NON-NLS-1$
    public static final String OMNETPP_DSP_AUTHORITY = "dsp"; //$NON-NLS-1$
    public static final String OMNETPP_DSP_PATH_ATTACH = "/debugger/attach"; //$NON-NLS-1$

    @Override
    public void handle(String cdtUri) {
        var uri = URI.create(cdtUri);
        if (!OMNETPP_URI_SCHEME.equals(uri.getScheme()) || !OMNETPP_DSP_AUTHORITY.equals(uri.getAuthority()))
            return;

        if (OMNETPP_DSP_PATH_ATTACH.equals(uri.getPath())) {
            for (var p : URLEncodedUtils.parse(uri, "UTF-8"))
                if (p.getName().equals("pid"))
                    DSPUtils.attach(p.getValue());
        }
    }
}
