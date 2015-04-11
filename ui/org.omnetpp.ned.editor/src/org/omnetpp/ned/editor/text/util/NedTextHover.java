/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextHoverExtension;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.information.IInformationProviderExtension2;
import org.eclipse.swt.graphics.Point;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.editor.text.NedCommentFormatter;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.text.util.NedTextUtils.Info;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * NED text hover. Currently it displays fully qualified name and the documentation
 * of the NED type name hovered.
 *
 * @author rhornig, andras
 */
public class NedTextHover implements ITextHover, ITextHoverExtension, IInformationProviderExtension2 {
    private IEditorPart editor = null;

    public NedTextHover(IEditorPart editor) {
        this.editor = editor;
    }

    public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
        Info info = NedTextUtils.getNedReferenceFromSource((ITextEditor)editor, textViewer, hoverRegion);
        if (info == null)
            return null;

        if (info.referredElement instanceof INedTypeElement)
            return getHoverTextFor(((INedTypeElement)info.referredElement).getNedTypeInfo());

        return HoverSupport.addHTMLStyleSheet("<pre>" + info.referredElement.getNedSource() + "</pre>"); //FIXME refine!!! ie docu, etc
    }

    protected static String getHoverTextFor(INedTypeInfo typeInfo) {
        String text = "<b>" + typeInfo.getFullyQualifiedName() +  "</b><br/>\n";

        String comment = typeInfo.getNedElement().getComment();
        if (StringUtils.isNotEmpty(comment)) {
            boolean tildeMode = comment.matches(".*(~[a-zA-Z_]).*");
            text += "<br/>" + NedCommentFormatter.makeHtmlDocu(comment, false, tildeMode, null);
        }

        return HoverSupport.addHTMLStyleSheet(text);
    }

    public IRegion getHoverRegion(ITextViewer textViewer, int offset) {
        Point selection= textViewer.getSelectedRange();
        if (selection.x <= offset && offset < selection.x + selection.y)
            return new Region(selection.x, selection.y);
        Region region = new Region(offset, 0);
        Info info = NedTextUtils.getNedReferenceFromSource((ITextEditor)editor, textViewer, region);

        if (info != null)
            return info.regionToHighlight;
        else
            return region;
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
