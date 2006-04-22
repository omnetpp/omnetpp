package org.omnetpp.ned.editor.text.util;


import java.util.Iterator;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.source.Annotation;
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
        Iterator annIter = sourceViewer.getAnnotationModel().getAnnotationIterator();
		try {
            String annotationText = "";
            
            while (annIter.hasNext()) {
                Annotation ann = (Annotation)annIter.next();
                int offset = sourceViewer.getAnnotationModel().getPosition(ann).offset;
                int annLine = sourceViewer.getDocument().getLineOfOffset(offset);
                if (lineNumber == annLine ) {
                    if (!"".equals(annotationText)) annotationText += "\n";
                    annotationText += ann.getText();
                }
            }
            return annotationText;
        } catch (BadLocationException e) {
        }

		return null;
	}
}
