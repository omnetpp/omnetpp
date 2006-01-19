package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.*;
import org.eclipse.jface.text.source.IAnnotationHover;
import org.eclipse.jface.text.source.ISourceViewer;

/** 
 * The NedAnnotationHover provides the hover support for java editors.
 */
 
public class NedAnnotationHover implements IAnnotationHover {

	/* (non-Javadoc)
	 * Method declared on IAnnotationHover
	 */
	public String getHoverInfo(ISourceViewer sourceViewer, int lineNumber) {
		IDocument document= sourceViewer.getDocument();

		try {
			IRegion info= document.getLineInformation(lineNumber);
			return document.get(info.getOffset(), info.getLength());
		} catch (BadLocationException x) {
		}

		return null;
	}
}
