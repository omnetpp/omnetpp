package org.omnetpp.runtimeenv.widgets;


/**
 * The TextViewer widget implements this listener to receive
 * notifications when changes to the model occur.
 */
public interface TextChangeListener {
    /**
     * Called when the content has changed.
     */
    public void textChanged(TextViewerContent textViewer);
}


