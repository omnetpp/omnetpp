/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
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
            KeyType keyType = KeyType.getKeyType(key);
            if (keyType == KeyType.CONFIG) {
                if (key.equals(ConfigRegistry.CFGID_NETWORK.getName())) {
                    // network key value hover
                    String networkName = doc.getValue(section, key);
                    INedTypeInfo network = analyzer.resolveNetwork(NedResourcesPlugin.getNedResources(), networkName);
                    if (network != null) {
                        IRegion valueRegion = InifileTextUtil.getValueRegion(textDoc, lineNumber);
                        if (InifileTextUtil.contains(valueRegion, offset))
                            return new IHyperlink[] {new NedHyperlink(valueRegion, network.getNedElement())};
                    }
                }
            }
            else if (keyType == KeyType.PARAM) {
                IRegion keyRegion = InifileTextUtil.getKeyRegion(textDoc, lineNumber);
                IRegion linkRegion = InifileTextUtil.getKeyPartRegion(textDoc, keyRegion, offset);
                if (linkRegion != null) {
                    ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key, new Timeout(InifileEditor.HYPERLINKDETECTOR_TIMEOUT));
                    String pattern = textDoc.get(linkRegion.getOffset(), linkRegion.getLength());
                    Collection<INedElement> elements = InifileTextUtil.findNamedElements(pattern, resList);

                    List<IHyperlink> links = new ArrayList<IHyperlink>();
                    for (INedElement element : elements) {
                        links.add(new NedHyperlink(linkRegion, element, InifileTextUtil.getLinkText(element)));
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
}
