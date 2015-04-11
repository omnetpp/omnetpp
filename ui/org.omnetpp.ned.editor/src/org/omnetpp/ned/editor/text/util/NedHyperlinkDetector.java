/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.util;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.ned.core.ui.misc.NedHyperlink;
import org.omnetpp.ned.editor.text.util.NedTextUtils.Info;

/**
 * TODO add documentation
 *
 * @author rhornig, andras
 */
public class NedHyperlinkDetector implements IHyperlinkDetector {
    private TextEditor editor;

    public NedHyperlinkDetector(TextEditor editor) {
        this.editor = editor;
    }

    public IHyperlink[] detectHyperlinks(ITextViewer textViewer, IRegion region, boolean canShowMultipleHyperlinks) {
        Info info = NedTextUtils.getNedReferenceFromSource((ITextEditor)editor, textViewer, region);
        if (info == null)
            return null;

        if (info.referredElement != null)
            return new IHyperlink[] {new NedHyperlink(info.regionToHighlight, info.referredElement)};

        return null;
    }

}
