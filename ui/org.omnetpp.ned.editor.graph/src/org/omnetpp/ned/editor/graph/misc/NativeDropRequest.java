/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.Request;

public class NativeDropRequest extends Request {

    private Object data;

    public static final String ID = "$Native Drop Request";//$NON-NLS-1$

    public NativeDropRequest() {
        super(ID);
    }

    public NativeDropRequest(Object type) {
        super(type);
    }

    public Object getData() {
        return data;
    }

    public void setData(Object data) {
        this.data = data;
    }

}
