package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextHoverExtension;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.information.IInformationProviderExtension2;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.swt.graphics.Point;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;

/**
 * Example implementation for an <code>ITextHover</code> which hovers over NED code.
 *
 * @author rhornig
 */
// TODO for the "F2 to focus" stuff, see ITextHoverExtension & IInformationControlCreator
public class NedTextHover implements ITextHover, ITextHoverExtension, IInformationProviderExtension2 {

	//private IEditorPart editor = null;
	
	public NedTextHover(IEditorPart editor) {
		//this.editor = editor;
	}

	public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
		String word = getWordUnderCursor(textViewer, hoverRegion, NedCompletionHelper.nedWordDetector);

		// if we find a NED component with that name, display its source code
		INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();    	
		INEDTypeInfo component = res.lookupNedType(word, null);
		
		if (component!=null)
		    return HoverSupport.addHTMLStyleSheet(
		                    StringUtils.makeHtmlDocu(component.getNEDElement().getComment()));

		// otherwise, give up (TODO we might try harder though, ie using context info)
		return "";
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
			return null;
		}
	}

	public IRegion getHoverRegion(ITextViewer textViewer, int offset) {
		Point selection= textViewer.getSelectedRange();
		if (selection.x <= offset && offset < selection.x + selection.y)
			return new Region(selection.x, selection.y);
		return new Region(offset, 0);
	}
    /*
     * @see org.eclipse.jface.text.ITextHoverExtension#getHoverControlCreator()
     */
    public IInformationControlCreator getHoverControlCreator() {
        return HoverSupport.getHoverControlCreator();
    }

    /* (non-Javadoc)
     * @see org.eclipse.jface.text.information.IInformationProviderExtension2#getInformationPresenterControlCreator()
     */
    public IInformationControlCreator getInformationPresenterControlCreator() {
        return HoverSupport.getInformationPresenterControlCreator();
    }
}
