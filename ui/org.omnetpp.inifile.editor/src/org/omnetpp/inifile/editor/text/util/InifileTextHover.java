package org.omnetpp.inifile.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Presents hover information for ini files.
 */
public class InifileTextHover implements ITextHover {
	private InifileEditorData editorData;

	public InifileTextHover(InifileEditorData editorData) {
		this.editorData = editorData;
	}

	/* (non-Javadoc)
	 * ITextHover method@see org.eclipse.jface.text.ITextHover#getHoverInfo(org.eclipse.jface.text.ITextViewer, org.eclipse.jface.text.IRegion)
	 */
	public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
		// determine line number
		int lineNumber;
		try {
			lineNumber = textViewer.getDocument().getLineOfOffset(hoverRegion.getOffset()) + 1;
		} catch (BadLocationException e) {
			return null;
		}

		// identify section+key being hovered
		IInifileDocument doc = editorData.getInifileDocument();
		String section = doc.getSectionForLine(lineNumber);
		if (section==null)
			return null;
		String key = doc.getKeyForLine(lineNumber);

		// generate tooltip
		InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
		if (key == null)
			return InifileUtils.getSectionTooltip(section, doc, analyzer);
		else 
			return InifileUtils.getEntryTooltip(section, key, doc, analyzer);
	}    

	/* (non-Javadoc)
	 * @see org.eclipse.jface.text.ITextHover#getHoverRegion(org.eclipse.jface.text.ITextViewer, int)
	 */
	public IRegion getHoverRegion(ITextViewer textViewer, int offset) {
		Point selection= textViewer.getSelectedRange();
		if (selection.x <= offset && offset < selection.x + selection.y)
			return new Region(selection.x, selection.y);
		return new Region(offset, 0);
	}
}
