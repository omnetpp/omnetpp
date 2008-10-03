package org.omnetpp.ned.editor.graph.parts;

/**
 * Interface to allow checking whether the edit part should be edited
 *
 * @author rhornig
 */
public interface IReadOnlySupport {
    /**
     * Returns the whether this edit part can be edited.
     */
    public boolean isEditable();

    /**
     * Set whether the edit part will be allowed to be edited
     */
    public void setEditable(boolean editable);
}