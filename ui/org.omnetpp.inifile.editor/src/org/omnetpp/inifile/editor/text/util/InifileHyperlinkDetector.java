package org.omnetpp.inifile.editor.text.util;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NEDHyperlink;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

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
			
			InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
			if (InifileAnalyzer.getKeyType(key) == KeyType.CONFIG) {
				if (key.equals(ConfigRegistry.CFGID_NETWORK.getKey())) {
					// network key value hover
					String networkName = doc.getValue(section, key);
					INEDTypeInfo network = analyzer.resolveNetwork(NEDResourcesPlugin.getNEDResources(), networkName);
					if (network != null) {
						IRegion valueRegion = getValueRegion(textDoc, lineNumber);
						if (contains(valueRegion, offset))
							return new IHyperlink[] {new NEDHyperlink(valueRegion, network.getNEDElement())};
					}
				}
			}
			else {
				// per object config key hovers

				// NED element to go to
				ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
				INEDElement node = resList.length==0 ? null : resList[0].paramValueNode!=null ? resList[0].paramValueNode : resList[0].paramDeclNode;
				if (node != null) {
					// add hyperlink on the key
					IRegion keyRegion = getKeyRegion(textDoc, lineNumber);
					if (contains(keyRegion, offset))
						return new IHyperlink[] {new NEDHyperlink(keyRegion, node)};
				}
			}

    	} catch (BadLocationException e) {
		}
    	return null;
    }
    
    protected IRegion getKeyRegion(IDocument textDoc, int lineNumber) throws BadLocationException {
    	// get position of key within the line hovered 
    	int lineOffset = textDoc.getLineOffset(lineNumber-1);
    	int nextLineOffset = textDoc.getLineOffset(lineNumber);
    	String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
    	int eqPos = lineContents.indexOf('=');
    	Region region = new Region(lineOffset, eqPos);
		return trimRegion(textDoc, region);
    }

    protected IRegion getValueRegion(IDocument textDoc, int lineNumber) throws BadLocationException {
    	// get position of key within the line hovered 
    	int lineOffset = textDoc.getLineOffset(lineNumber-1);
    	int nextLineOffset = textDoc.getLineOffset(lineNumber);
    	String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
    	int eqPos = lineContents.indexOf('=');
    	int hashmarkPos = lineContents.indexOf('#');
    	Region region = new Region(lineOffset + eqPos + 1, hashmarkPos == -1 ? nextLineOffset-lineOffset-eqPos - 1 - 1 : hashmarkPos - eqPos -1);
		return trimRegion(textDoc, region);
    }
    
    protected IRegion trimRegion(IDocument textDoc, IRegion r) throws BadLocationException{
    	int begin = r.getOffset();
    	int end = begin + r.getLength();
    	while(Character.isWhitespace(textDoc.getChar(begin)) && begin < end)
    		begin++;
    	while(Character.isWhitespace(textDoc.getChar(end-1)) && begin < end)
    		end--;
    	return new Region(begin, end-begin);
    }
    
    protected boolean contains(IRegion reg, int offset) {
    	return offset >= reg.getOffset() && offset < reg.getOffset() + reg.getLength();
    }
}
