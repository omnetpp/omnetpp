package org.omnetpp.ned.editor.graph.dnd;

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
