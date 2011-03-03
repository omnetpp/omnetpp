/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.util;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
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
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.core.ui.misc.NedHyperlink;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

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
            KeyType keyType = KeyType.getKeyType(key);
            if (keyType == KeyType.CONFIG) {
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
            else if (keyType == KeyType.PARAM) {
                IRegion keyRegion = getKeyRegion(textDoc, lineNumber);
                IRegion linkRegion = getLinkRegion(textDoc, keyRegion, offset);
                if (linkRegion != null) {
                    ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key, new Timeout(InifileEditor.HYPERLINKDETECTOR_TIMEOUT));
                    Set<INedElement> elements = new LinkedHashSet<INedElement>();
                    String pattern = textDoc.get(linkRegion.getOffset(), linkRegion.getLength());
                    for (ParamResolution res : resList) {
                        INedElement element = findNamedElement(pattern, res);
                        if (element != null)
                            elements.add(element);
                    }

                    List<IHyperlink> links = new ArrayList<IHyperlink>();
                    for (INedElement element : elements) {
                        links.add(new NedHyperlink(linkRegion, element, getLinkText(element)));
                    }
                    if (!links.isEmpty())
                        return links.toArray(new IHyperlink[links.size()]);
                }
            }

        } catch (BadLocationException e) {
        }
        catch (ParamResolutionDisabledException e) {
            // no hyperlinks if param resolution is disabled
        } catch (ParamResolutionTimeoutException e) {
            // no hyperlinks if param resolution timed out
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
    
    // gets the pattern from the key at the offset position (excluding the [*] part)
    protected IRegion getLinkRegion(IDocument textDoc, IRegion keyRegion, int offset) throws BadLocationException {
        if (!contains(keyRegion, offset))
            return null;
        int start = offset;
        while (start >= keyRegion.getOffset() && isPatternChar(textDoc.getChar(start)))
            --start;
        int end = offset;
        while (end < keyRegion.getOffset() + keyRegion.getLength() && isPatternChar(textDoc.getChar(end)))
            ++end;
        return new Region(start+1, end-start-1);
    }
    
    private boolean isPatternChar(char ch) {
        return ch != '.' && ch != '[' && ch != ']';
    }
    
    protected INedElement findNamedElement(String pattern, ParamResolution resolution) {
        // try as parameter name
        ParamElementEx param = resolution.paramAssignment!=null ? resolution.paramAssignment : resolution.paramDeclaration;
        if (param != null && ParamUtil.matchesPattern(pattern, param.getName()))
            return param;
        // try as submodule name
        for (ISubmoduleOrConnection element : resolution.elementPath)
            if (element instanceof IHasName && ParamUtil.matchesPattern(pattern, ((IHasName)element).getName()))
                return element;
        // try as network name
        if (resolution.elementPath.length >= 2) {
            if (resolution.elementPath[1] != null && (resolution.elementPath[1].getCompoundModule() instanceof IHasName) &&
                    ParamUtil.matchesPattern(pattern, ((IHasName)resolution.elementPath[1].getCompoundModule()).getName()))
                return resolution.elementPath[1].getCompoundModule();
        }
        else if (param != null && param.getOwner() instanceof IHasName && ParamUtil.matchesPattern(pattern, ((IHasName)param.getOwner()).getName()))
            return param.getOwner();
        return null;
    }
    
    /**
     * Returns the text displayed to disambiguate multiple target elements.
     */
    protected String getLinkText(INedElement element) {
        String text = null;
        INedTypeElement typeElement = null;
        
        if (element instanceof ParamElement) {
            ParamElement param = (ParamElement)element;
            typeElement = param.getEnclosingTypeElement();
            text = typeElement.getName() + "." + param.getName();
        }
        else if (element instanceof INedTypeElement) {
            typeElement = (INedTypeElement)element;
            text = typeElement.getName();
        }
        else if (element instanceof ISubmoduleOrConnection) {
            ISubmoduleOrConnection submodule = (ISubmoduleOrConnection)element;
            typeElement = submodule.getEnclosingTypeElement();
            if (submodule instanceof SubmoduleElement)
                text = typeElement.getName() + "." + ((SubmoduleElement)submodule).getName();
            else
                text = typeElement.getName();
        }
        else
            Assert.isTrue(false, "Element is: " + element);
        
        if (typeElement.getNedTypeInfo().isInnerType())
            text = typeElement.getEnclosingTypeElement().getName() + "." + text;
        
        return text;
    }
}
