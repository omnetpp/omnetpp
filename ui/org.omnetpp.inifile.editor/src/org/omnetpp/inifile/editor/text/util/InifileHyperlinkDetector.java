/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.util;

import java.util.Set;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.omnetpp.common.util.Pair;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.ned.core.NedResources;
import org.omnetpp.ned.core.ui.misc.NedHyperlink;
import org.omnetpp.ned.model.INedElement;

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
        IDocument textDoc = textViewer.getDocument();
        IReadonlyInifileDocument doc = editorData.getInifileDocument();
        InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
        NedElementDetector detector = new NedElementDetector(doc, analyzer, textDoc, NedResources.getInstance());
        Pair<IRegion, Set<INedElement>> regionAndElements = detector.detectNedElementsAtOffset(region.getOffset()); // XXX detect elements in the whole region
        if (regionAndElements != null) {
            IRegion linkRegion = regionAndElements.first;
            Set<INedElement> elements = regionAndElements.second;
            IHyperlink[] result = new IHyperlink[elements.size()];
            int i = 0;
            for (INedElement element : elements) {
                result[i] = new NedHyperlink(linkRegion, element, NedElementDetector.getLinkText(element));
                i++;
            }
            return result;
        }
        return null;
    }
}
