/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.actions;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.source.ISourceViewer;
import org.omnetpp.ned.core.IGotoNedElement;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.editor.text.util.NedTextUtils;
import org.omnetpp.ned.editor.text.util.NedTextUtils.Info;

/**
 * Implements the "Open Declaration" functionality in the text editor.
 *
 * @author andras
 */
public class GotoDeclarationAction extends NedTextEditorAction {
    public static final String ID = "GotoDeclaration";

    public GotoDeclarationAction(TextualNedEditor editor) {
        super(ID, editor);
    }

    @Override
    public void update() {
        // NOTE: don't disable this action based on the current text selection (unless you properly implement selection changed)
        //       because F3 (Go to definition) will break in the text editor
        super.update();
    }

    @Override
    protected void doRun() {
        Info info = getInfoForSelection();
        if (info != null && info.referredElement != null)
            NedResourcesPlugin.openNedElementInEditor(info.referredElement, IGotoNedElement.Mode.TEXT);
    }

    /**
     * @return
     */
    private Info getInfoForSelection() {
        ISourceViewer viewer = ((TextualNedEditor)getTextEditor()).getSourceViewerPublic();
        if (viewer == null)
            return null;
        IRegion region = new Region(viewer.getSelectedRange().x, viewer.getSelectedRange().y);
        return NedTextUtils.getNedReferenceFromSource(getTextEditor(), viewer, region);
    }

}
