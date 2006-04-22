package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.ned.editor.text.NedHelper;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.resources.INEDComponent;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;

/**
 * Example implementation for an <code>ITextHover</code> which hovers over NED code.
 */
public class NedTextHover implements ITextHover {

	public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
		String word = getWordUnderCursor(textViewer, hoverRegion, NedHelper.nedWordDetector);

		// if we find a NED component with that name, display its source code
		// TODO for the "F2 to focus" stuff, see ITextHoverExtension & IInformationControlCreator
		NEDResources res = NEDResourcesPlugin.getNEDResources();    	
		INEDComponent component = res.getComponent(word);
		if (component!=null)
			return ModelUtil.generateNedSource(component.getNEDElement(), true);

		// otherwise, give up (TODO we might try harder though, ie using context info)
		return word; //+": not a NED module, channel or interface";
		
//XXX
//        String msg = "Default";
//        IDocument doc = textViewer.getDocument();
//        ITypedRegion region;
//        if (doc instanceof IDocumentExtension3) {
//            IDocumentExtension3 docext3 = (IDocumentExtension3) doc;
//            try {
//                region = docext3.getPartition(NedOutlinePartitionScanner.PARTITIONING_ID, hoverRegion.getOffset(), false);
//            } catch (BadLocationException e) {
//                return msg;
//            } catch (BadPartitioningException e) {
//                return msg;
//            }
//            return region.getType();
//            
//        }
//        return msg;

//		if (hoverRegion != null) {
//			try {
//				if (hoverRegion.getLength() > -1)
//					return textViewer.getDocument().get(hoverRegion.getOffset(), hoverRegion.getLength());
//			} catch (BadLocationException x) {
//			}
//		}
//		return NedEditorMessages.getString("NedTextHover.emptySelection"); //$NON-NLS-1$
	}
	
	private String getWordUnderCursor(ITextViewer viewer, IRegion region, IWordDetector wordDetector) {
		try {
			// if mouse hovers over a selection, return that
			IDocument docu = viewer.getDocument();
			if (region.getLength()!=0)
				return docu.get(region.getOffset(), region.getLength());

			// if mouse is not over a word, return null
			int offset = region.getOffset();
			if (!wordDetector.isWordPart(docu.getChar(offset - 1)))
				return null;

			//XXX code copied from IncrementalCompletionProcessor.detectWordRegion(), except that that one
			// has a small bug (it also accepts numbers) -- extract this code to some util class?
			int length = 0;

			// find the first char that may not be the trailing part of a word.
			while (offset > 0 && wordDetector.isWordPart(docu.getChar(offset - 1)))
				offset--;

			// check if the first char of the word is also ok.
			if (!wordDetector.isWordStart(docu.getChar(offset)))
				return null; // this is not a word (but probably a number)
			
			// now we should stand on the first char of the word; now iterate through 
			// the rest of chars until a character cannot be recognized as an in/word char
			while(offset + length < docu.getLength() && wordDetector.isWordPart(docu.getChar(offset + length)))
				length++;
			
			return docu.get(offset, length);
		
		} catch (BadLocationException e) {
		}
		return null;
	}

	public IRegion getHoverRegion(ITextViewer textViewer, int offset) {
		Point selection= textViewer.getSelectedRange();
		if (selection.x <= offset && offset < selection.x + selection.y)
			return new Region(selection.x, selection.y);
		return new Region(offset, 0);
	}
}
