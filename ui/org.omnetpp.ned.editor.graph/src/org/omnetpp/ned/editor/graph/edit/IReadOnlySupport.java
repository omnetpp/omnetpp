package org.omnetpp.ned.editor.graph.edit;

/**
 * @author rhornig
 * Interface to allow checking wheter the edit part should be edited
 */
public interface IReadOnlySupport {
    /**
     * @return Whether this edit part can be edited.  
     */
    public boolean isEditable();
    
    /**
     * @param editable Whether the edit part will be allowed to be edited
     */
    public void setEditable(boolean editable);
}