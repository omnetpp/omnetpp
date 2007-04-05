package org.omnetpp.inifile.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.text.NedHelper;

/**
 * Example implementation for an <code>ITextHover</code> which hovers over NED code.
 */
//XXX TODO rename, revise, possibly remove...
// TODO for the "F2 to focus" stuff, see ITextHoverExtension & IInformationControlCreator
public class NedTextHover implements ITextHover {

	private InifileEditorData editorData;
	
	public NedTextHover(InifileEditorData editorData) {
		this.editorData = editorData;
	}

	/* (non-Javadoc)
	 * ITextHover method@see org.eclipse.jface.text.ITextHover#getHoverInfo(org.eclipse.jface.text.ITextViewer, org.eclipse.jface.text.IRegion)
	 */
	public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
		int lineNumber;
		try {
			lineNumber = textViewer.getDocument().getLineOfOffset(hoverRegion.getOffset()) + 1;
		} catch (BadLocationException e) {
			return null;
		}

		IInifileDocument doc = editorData.getInifileDocument();
		String section = doc.getSectionForLine(lineNumber);
		if (section==null || !InifileAnalyzer.isParamSection(section))
			return null;
		String key = doc.getKeyForLine(lineNumber);
		
		InifileAnalyzer ana = editorData.getInifileAnalyzer();
		if (key == null) {
			ParamResolution[] resList = ana.getUnassignedParams(section);
			if (resList.length==0) 
				return "Section [" + section + "] seems to contain no unassigned parameters";
			String text = "Section [" + section + "] does not seem to assign the following parameters: \n";
			for (ParamResolution res : resList)
				text += "   " + res.moduleFullPath + "." +res.paramNode.getName() + "\n";
			return text;
		}
		else {
			ParamResolution[] resList = ana.getParamResolutionsForKey(section, key);
			if (resList.length==0) 
				return "Entry \"" + key + "\" seems to be redundant (not matching any module parameters)";
			String text = "Entry \"" + key + "\" applies to the following module parameters: \n";
			for (ParamResolution res : resList)
				text += "   " + res.moduleFullPath + "." +res.paramNode.getName() + "\n";
			return text;
		}
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
