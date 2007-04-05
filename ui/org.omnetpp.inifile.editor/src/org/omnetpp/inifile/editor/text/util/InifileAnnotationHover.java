package org.omnetpp.inifile.editor.text.util;


import java.util.Iterator;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.source.Annotation;
import org.eclipse.jface.text.source.IAnnotationHover;
import org.eclipse.jface.text.source.ISourceViewer;

/** 
 * Provides hover info for annotations.
 */
//XXX copied from NedAnnotationHover. Maybe there's a standard implementation somewhere?
public class InifileAnnotationHover implements IAnnotationHover {

    /* (non-Javadoc)
	 * Method declared on IAnnotationHover
	 */
	public String getHoverInfo(ISourceViewer sourceViewer, int lineNumber) {
        Iterator annIter = sourceViewer.getAnnotationModel().getAnnotationIterator();
		try {
            String annotationText = "";
            
            while (annIter.hasNext()) {
                Annotation ann = (Annotation)annIter.next();
                Position currPos = sourceViewer.getAnnotationModel().getPosition(ann);
                
                if (currPos == null) {
                	System.out.println("Annotation ignored: "+ann);
                	continue;
                }
                
                int offset = currPos.offset;
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
