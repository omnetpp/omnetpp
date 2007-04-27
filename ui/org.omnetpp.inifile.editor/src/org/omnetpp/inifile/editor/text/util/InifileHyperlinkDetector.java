package org.omnetpp.inifile.editor.text.util;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.ned.core.ui.misc.NEDHyperlink;
import org.omnetpp.ned.model.INEDElement;

/**
 * Adds hyperlinks to the ini file.
 *
 * @author Andras
 */
public class InifileHyperlinkDetector implements IHyperlinkDetector {
	private InifileEditorData editorData;

	public InifileHyperlinkDetector(InifileEditorData editorData) {
		this.editorData = editorData;
	}

    public IHyperlink[] detectHyperlinks(ITextViewer textViewer, IRegion region, boolean canShowMultipleHyperlinks) {
    	try {
    		IDocument textDoc = textViewer.getDocument();
    		int offset = region.getOffset();
			int lineNumber = textDoc.getLineOfOffset(offset)+1;

			// identify section+key being hovered
			IInifileDocument doc = editorData.getInifileDocument();
			String section = doc.getSectionForLine(lineNumber);
			if (section==null)
				return null;
			String key = doc.getKeyForLine(lineNumber);
			if (key==null)
				return null;

			// get position of key within the line hovered
			int lineOffset = textDoc.getLineOffset(lineNumber-1);
			int nextLineOffset = textDoc.getLineOffset(lineNumber);
			int hoverPos = offset - lineOffset;
			String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
			int keyPos = lineContents.indexOf(key);
			if (keyPos == -1 || hoverPos < keyPos || hoverPos > keyPos+key.length())
				return null;

			// NED element to go to
			InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
			ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
			INEDElement node = resList.length==0 ? null : resList[0].paramValueNode!=null ? resList[0].paramValueNode : resList[0].paramDeclNode;
			if (node == null)
				return null;

			// add hyperlink on the key
			return new IHyperlink[] {new NEDHyperlink(new Region(lineOffset+keyPos, key.length()), node)};

    	} catch (BadLocationException e) {
		}
    	return null;
    }

}
