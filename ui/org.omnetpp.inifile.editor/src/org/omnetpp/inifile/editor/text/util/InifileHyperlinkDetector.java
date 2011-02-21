/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.util;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.KeyType;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NedHyperlink;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;

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
			IReadonlyInifileDocument doc = editorData.getInifileDocument();
			String section = doc.getSectionForLine(lineNumber);
			if (section==null)
				return null;
			String key = doc.getKeyForLine(lineNumber);
			if (key==null)
				return null;

			InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
			if (KeyType.getKeyType(key) == KeyType.CONFIG) {
				if (key.equals(ConfigRegistry.CFGID_NETWORK.getName())) {
					// network key value hover
					String networkName = doc.getValue(section, key);
					INedTypeInfo network = analyzer.resolveNetwork(NedResourcesPlugin.getNedResources(), networkName);
					if (network != null) {
						IRegion valueRegion = getValueRegion(textDoc, lineNumber);
						if (contains(valueRegion, offset))
							return new IHyperlink[] {new NedHyperlink(valueRegion, network.getNedElement())};
					}
				}
			}
			else {
				// per object config key hovers

				// NED element to go to
			    try {
			        ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key, new Timeout(InifileEditor.HYPERLINKDETECTOR_TIMEOUT));
			        INedElement node = resList.length==0 ? null : resList[0].paramAssignment!=null ? resList[0].paramAssignment : resList[0].paramDeclaration;
			        if (node != null) {
			            // add hyperlink on the key
			            IRegion keyRegion = getKeyRegion(textDoc, lineNumber);
			            if (contains(keyRegion, offset))
			                return new IHyperlink[] {new NedHyperlink(keyRegion, node)};
			        }
			    } catch (ParamResolutionDisabledException e) {
			        // no hyperlinks if param resolution is disabled
			    } catch (ParamResolutionTimeoutException e) {
			        // no hyperlinks if param resolution timed out
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
