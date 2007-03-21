package org.omnetpp.inifile.editor.model;

/**
 * Implement this to get notified about inifile changes.
 * @author andras
 */
public interface IInifileChangeListener {
    /**
     * @param event Callback invoked when a change occurs in the model
     */
    public void modelChanged();
}
