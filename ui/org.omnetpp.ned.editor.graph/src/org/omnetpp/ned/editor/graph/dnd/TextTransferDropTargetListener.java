package org.omnetpp.ned.editor.graph.dnd;

import org.eclipse.swt.dnd.Transfer;

import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.Request;
import org.eclipse.gef.dnd.AbstractTransferDropTargetListener;

public class TextTransferDropTargetListener extends AbstractTransferDropTargetListener {

    public TextTransferDropTargetListener(EditPartViewer viewer, Transfer xfer) {
        super(viewer, xfer);
    }

    @Override
    protected Request createTargetRequest() {
        return new NativeDropRequest();
    }

    protected NativeDropRequest getNativeDropRequest() {
        return (NativeDropRequest) getTargetRequest();
    }

    @Override
    protected void updateTargetRequest() {
        getNativeDropRequest().setData(getCurrentEvent().data);
    }

}
