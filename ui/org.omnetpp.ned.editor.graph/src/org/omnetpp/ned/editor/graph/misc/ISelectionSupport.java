package org.omnetpp.ned.editor.graph.misc;

/**
 * @author rhornig
 * interface to allow selection of a toplevel component or a line range from outside of a graphical editor
 */
public interface ISelectionSupport {
    
        /**
         * @param componentName The name of the component that should be shown and selected
         */
        public void selectComponent(String componentName);
        
        /**
         * Switches to text view and sets the highlight range (numbering starts with 1)
         * @param startLine
         * @param endLine
         */
        public void setHighlightRange(int startLine, int endLine);
}
