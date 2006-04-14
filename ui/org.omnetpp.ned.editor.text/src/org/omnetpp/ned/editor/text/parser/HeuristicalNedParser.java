package org.omnetpp.ned.editor.text.parser;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.text.IDocument;

public class HeuristicalNedParser implements INedFileParser {

    /**
     * Parses the input given by the argument.
     *
     * @param file  the element containing the input text
     * @return an element collection representing the parsed input
     */
    public NedElement parse(IDocument doc) {
		// TODO heuristic parsing should come here
    	System.out.println("HeuristicalNedParser.parse() called");
        NedElement result;
        result = new NedElement(null, "Hello world", 20, 40);
        new NedElement(result, "Bello world", 56, 56);
        return result;
    }
    
    public IAdaptable getContentOutline(IAdaptable adaptable) {
        return parse(null);
    }

}