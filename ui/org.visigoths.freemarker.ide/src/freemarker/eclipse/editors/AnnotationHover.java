package freemarker.eclipse.editors;


import java.util.Iterator;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.text.source.IAnnotationHover;
import org.eclipse.jface.text.source.IAnnotationModel;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.ui.texteditor.MarkerAnnotation;



/**
 * @version $Id: AnnotationHover.java,v 1.2 2003/02/21 12:05:33 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class AnnotationHover implements IAnnotationHover {

    public String getHoverInfo(ISourceViewer aViewer, int aLine) {
        String info = null;
        IMarker marker = getMarkerForLine(aViewer, aLine);
        if (marker != null) {
            String message = marker.getAttribute(IMarker.MESSAGE, (String)null);
            if (message != null && message.trim().length() > 0) {
                info = message.trim();
            }
        }
        return info;
    }

    @SuppressWarnings("rawtypes")
    protected IMarker getMarkerForLine(ISourceViewer aViewer, int aLine) {
        IMarker marker = null;
        IAnnotationModel model = aViewer.getAnnotationModel();
        if (model != null) {
            Iterator e = model.getAnnotationIterator();
            if(e.hasNext()) {
                Object o = e.next();
                if (o instanceof MarkerAnnotation) {
                    MarkerAnnotation a = (MarkerAnnotation)o;
                    marker = a.getMarker();
                }
            }
        }
        return marker;
    }

}
