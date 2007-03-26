package org.omnetpp.common.editor;

/**
 * @author rhornig
 * interface to allow selection of a toplevel component or a line range from outside of a graphical editor
 */
public interface ISelectionSupport {
    
        /**
         * @param componentName The name of the component that should be shown and selected
         */
        public void selectGraphComponent(String componentName);
        
        /**
         * Switches to text view and sets the highlight range (numbering starts with 1)
         * @param startLine
         * @param endLine
         */
        public void setTextHighlightRange(int startLine, int endLine);
}
