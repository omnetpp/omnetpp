package org.omnetpp.ned.editor.text.util;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.ned.core.ui.misc.NEDHyperlink;
import org.omnetpp.ned.editor.text.util.NedTextUtils.Info;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * TODO add documentation
 *
 * @author rhornig, andras
 */
public class NEDHyperlinkDetector implements IHyperlinkDetector {
    private TextEditor editor;

    public NEDHyperlinkDetector(TextEditor editor) {
        this.editor = editor;
    }

    public IHyperlink[] detectHyperlinks(ITextViewer textViewer, IRegion region, boolean canShowMultipleHyperlinks) {
        Info info = NedTextUtils.getNedHoverContext((ITextEditor)editor, textViewer, region);
        if (info == null)
            return null;

        if (info.referredElement != null)
            return new IHyperlink[] {new NEDHyperlink(info.regionToHighlight, info.referredElement)};

        return null;
    }

}
