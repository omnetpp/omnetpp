package org.omnetpp.inifile.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;

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
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);

		InifileAnalyzer ana = editorData.getInifileAnalyzer();
		if (key == null) {
			// Parameters section: display unassigned parameters
			ParamResolution[] resList = ana.getUnassignedParams(section);
			//XXX add description too
			if (resList.length==0) 
				return "Section [" + section + "] seems to contain no unassigned parameters ";
			String text = "Section [" + section + "] does not seem to assign the following parameters: \n";
			for (ParamResolution res : resList)
				text += "  - " + res.moduleFullPath + "." +res.paramNode.getName() + "\n";
			return text;
		}
		else if (keyType==KeyType.CONFIG) {
			// config key: display description
			ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
			if (entry == null)
				return null;
			String text = "[General]"+(entry.isGlobal() ? "" : " or [Config X]")+" / "+entry.getKey();
			text += " = <" + entry.getType().name().replaceFirst("CFG_", ""); 
			if (!"".equals(entry.getDefaultValue()))
				text += ", default: " + entry.getDefaultValue();
			text += "> \n\n";
			text += entry.getDescription() + "\n";
			return StringUtils.breakLines(text, 80);
		}
		else if (keyType == KeyType.PARAM) {
			// parameter assignment: display which parameters it matches
			ParamResolution[] resList = ana.getParamResolutionsForKey(section, key);
			if (resList.length==0) 
				return "Entry \"" + key + "\" does not match any module parameters ";
			String text = "Entry \"" + key + "\" applies to the following module parameters: \n";
			for (ParamResolution res : resList)
				text += "  - " + res.moduleFullPath + "." +res.paramNode.getName() + "\n";
			return text;
		}
		else if (keyType == KeyType.PER_OBJECT_CONFIG) {
			return null; // TODO
		}
		else {
			return null; // should not happen (invalid key type)
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
