package org.omnetpp.ned.editor.text.parser;

import org.eclipse.jface.text.IDocument;

/**
 * This interface is used as API for the Readme parser extension 
 * point. The default implementation simply looks for lines
 * in the file that start with a number and assumes that they
 * represent sections. Tools are allowed to replace this 
 * algorithm by defining an extension and supplying an 
 * alternative that implements this interface.
 */
public interface INedFileParser {
    /**
     * Parses the contents of the provided file
     * and generates a collection of sections.
     */
    public NedElement parse(IDocument doc);
}