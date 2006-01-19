package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.*;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.ned.editor.text.outline.NedOutlinePartitionScanner;

/**
 * Example implementation for an <code>ITextHover</code> which hovers over NED code.
 */
public class NedTextHover implements ITextHover {

	public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
        String msg = "Default";
        IDocument doc = textViewer.getDocument();
        ITypedRegion region;
        if (doc instanceof IDocumentExtension3) {
            IDocumentExtension3 docext3 = (IDocumentExtension3) doc;
            try {
                region = docext3.getPartition(NedOutlinePartitionScanner.PARTITIONING_ID, hoverRegion.getOffset(), false);
            } catch (BadLocationException e) {
                return msg;
            } catch (BadPartitioningException e) {
                return msg;
            }
            return region.getType();
            
        }
        return msg;
//		if (hoverRegion != null) {
//			try {
//				if (hoverRegion.getLength() > -1)
//					return textViewer.getDocument().get(hoverRegion.getOffset(), hoverRegion.getLength());
//			} catch (BadLocationException x) {
//			}
//		}
//		return NedEditorMessages.getString("NedTextHover.emptySelection"); //$NON-NLS-1$
	}
	
	public IRegion getHoverRegion(ITextViewer textViewer, int offset) {
		Point selection= textViewer.getSelectedRange();
		if (selection.x <= offset && offset < selection.x + selection.y)
			return new Region(selection.x, selection.y);
		return new Region(offset, 0);
	}
}
