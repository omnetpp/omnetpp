package org.omnetpp.ned.editor.text.actions;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.source.ISourceViewer;
import org.omnetpp.ned.core.IGotoNedElement;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.editor.text.util.NedTextUtils;
import org.omnetpp.ned.editor.text.util.NedTextUtils.Info;

/**
 * Implements the "Goto Declaration" functionality in the text editor.
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
    	setEnabled(getTextEditor() != null);
	}

    @Override
    public void run() {
        ISourceViewer viewer = ((TextualNedEditor)getTextEditor()).getSourceViewerPublic();
        IRegion region = new Region(viewer.getSelectedRange().x, viewer.getSelectedRange().y); 
        Info info = NedTextUtils.getNedReferenceFromSource(getTextEditor(), viewer, region);
        if (info != null && info.referredElement != null)
            NEDResourcesPlugin.openNEDElementInEditor(info.referredElement, IGotoNedElement.Mode.TEXT);
    }

}
