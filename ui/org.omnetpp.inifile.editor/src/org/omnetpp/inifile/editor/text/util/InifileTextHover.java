package org.omnetpp.inifile.editor.text.util;


import org.eclipse.jface.internal.text.html.HTMLTextPresenter;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextHoverExtension;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.editors.text.EditorsUI;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Presents hover information for ini files.
 */
public class InifileTextHover implements ITextHover, ITextHoverExtension {
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

	/*
	 * @see org.eclipse.jface.text.ITextHoverExtension#getHoverControlCreator()
	 */
	public IInformationControlCreator getHoverControlCreator() {
		return new IInformationControlCreator() {
			public IInformationControl createInformationControl(Shell parent) {
				return new DefaultInformationControl(parent, SWT.NONE, new HTMLTextPresenter(true), EditorsUI.getTooltipAffordanceString());
			}
		};
	}
}
