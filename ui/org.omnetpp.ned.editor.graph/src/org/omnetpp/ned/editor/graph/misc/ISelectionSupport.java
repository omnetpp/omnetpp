package org.omnetpp.ned.editor.graph.misc;

/**
 * @author rhornig
 * interface to allow selection of a toplevel component from outside of a graphical editor
 */
public interface ISelectionSupport {
    
        /**
         * @param componentName The name of the component that should be shown and selected
         */
        public void selectComponent(String componentName);
}
