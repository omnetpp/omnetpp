package org.omnetpp.ned.editor.text.util;


import java.util.Iterator;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.source.Annotation;
import org.eclipse.jface.text.source.IAnnotationHover;
import org.eclipse.jface.text.source.ISourceViewer;
import org.omnetpp.common.util.StringUtils;

/** 
 * Provides annotation hover support for NED editors. Annotations include 
 * problem markers, bookmarks, etc.
 *
 * @author rhornig
 */
//XXX currently unused, DefaultAnnotationHover is installed instead
public class NedAnnotationHover implements IAnnotationHover {

    /* (non-Javadoc)
	 * Method declared on IAnnotationHover
	 */
	@SuppressWarnings("unchecked")
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
                    String text = ann.getText();
                    if (StringUtils.isNotEmpty(text)) {
                        if (annotationText.length()>0) 
                            annotationText += "\n";
                        annotationText += text;
                    }
                }
            }
            return annotationText;
        } 
		catch (BadLocationException e) {
        }

		return null;
	}
}
