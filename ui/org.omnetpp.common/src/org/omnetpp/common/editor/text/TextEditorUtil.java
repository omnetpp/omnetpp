package org.omnetpp.common.editor.text;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.rules.IWordDetector;

/**
 * TextEditor-related utility functions
 * 
 * @author Andras
 */
public class TextEditorUtil {

	/**
	 * Detects the boundary of a single word under the current position (defined by the wordDetector)  
	 * @throws BadLocationException 
	 */
	public static IRegion detectWordRegion(ITextViewer viewer, int documentOffset, IWordDetector wordDetector) throws BadLocationException {
		int offset = documentOffset;
		int length = 0;

		// find the first char that may not be the trailing part of a word.
		while (offset > 0 && wordDetector.isWordPart(viewer.getDocument().getChar(offset - 1)))
			offset--;

		// check if the first char of the word is also ok.
		if (offset > 0 && wordDetector.isWordStart(viewer.getDocument().getChar(offset - 1)))
			offset--;
		// now we should stand on the first char of the word
		if (offset + length < viewer.getDocument().getLength() 
				&& wordDetector.isWordStart(viewer.getDocument().getChar(offset + length)))
			length++;
		// now iterate through the rest of chars until a character cannot be recognized as an in/word char
		while(offset + length < viewer.getDocument().getLength() 
				&& wordDetector.isWordPart(viewer.getDocument().getChar(offset + length)))
			length++;

		return new Region(offset, length);
	}

	public static String get(ITextViewer viewer, IRegion region) throws BadLocationException {
		return viewer.getDocument().get(region.getOffset(), region.getLength());
	}
	
	public static String getWordRegion(ITextViewer viewer, int documentOffset, IWordDetector wordDetector) throws BadLocationException {
		return get(viewer, detectWordRegion(viewer, documentOffset, wordDetector));
	}
}
