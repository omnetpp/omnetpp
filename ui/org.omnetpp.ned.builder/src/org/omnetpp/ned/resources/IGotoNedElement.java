package org.omnetpp.ned.resources;

import org.omnetpp.ned.model.NEDElement;

/**
 * interface to allow selection of a toplevel component or a line range from outside of a graphical editor
 * @author rhornig
 */
public interface IGotoNedElement {
        enum Mode {AUTOMATIC, GRAPHICAL, TEXT};
    
        /**
         * Opens the editor and shown the specified NEDElement 
         * @param model Th ned element to be shown
         * @param mode Which editor should be used
         */
        public void showInEditor(NEDElement model, Mode mode);
}
