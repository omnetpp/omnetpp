/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextHoverExtension;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.information.IInformationProviderExtension2;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;

/**
 * Presents hover information for ini files.
 *
 * @author Andras
 */
public class InifileTextHover implements ITextHover, ITextHoverExtension, IInformationProviderExtension2 {
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
        IReadonlyInifileDocument doc = editorData.getInifileDocument();
        String section = doc.getSectionForLine(lineNumber);
        if (section==null)
            return null;
        String key = doc.getKeyForLine(lineNumber);

        // generate tooltip
        InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
        if (key == null)
            return InifileHoverUtils.getSectionHoverText(section, doc, analyzer, false);
        else {
            return InifileHoverUtils.getEntryHoverText(section, key, hoverRegion, doc, analyzer, textViewer.getDocument());
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

    /*
     * @see org.eclipse.jface.text.ITextHoverExtension#getHoverControlCreator()
     */
    public IInformationControlCreator getHoverControlCreator() {
        return HoverSupport.getHtmlHoverControlCreator();
    }

    /* (non-Javadoc)
     * @see org.eclipse.jface.text.information.IInformationProviderExtension2#getInformationPresenterControlCreator()
     */
    public IInformationControlCreator getInformationPresenterControlCreator() {
        return HoverSupport.getHtmlInformationPresenterControlCreator();
    }
}
