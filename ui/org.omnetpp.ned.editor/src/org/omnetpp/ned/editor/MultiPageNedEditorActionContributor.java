package org.omnetpp.ned.editor;

import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.actions.GNEDActionBarContributor;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.editor.text.actions.TextualNedEditorActionContributor;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 *
 * @author rhornig
 */
public class MultiPageNedEditorActionContributor extends MultiPageEditorActionBarContributor {
    private GNEDActionBarContributor graphContrib;
    private TextualNedEditorActionContributor textContrib;
    private IEditorPart activeEditorPart;
    /**
     * Creates a multi-page contributor.
     */
    public MultiPageNedEditorActionContributor() {
        super();
        // create the multi page editor's own acions (if any)
        graphContrib = new GNEDActionBarContributor();
        textContrib = new TextualNedEditorActionContributor();
    }

    /* (non-JavaDoc)
     * Method declared in AbstractMultiPageEditorActionBarContributor.
     */

    @Override
    public void init(IActionBars bars) {
        super.init(bars);
        graphContrib.init(bars, getPage());
        textContrib.init(bars, getPage());
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.part.MultiPageEditorActionBarContributor#setActivePage(org.eclipse.ui.IEditorPart)
     * Respond to an editor change with reassigning the local and global actions
     */
    @Override
	public void setActivePage(IEditorPart part) {
        if (activeEditorPart == part)
            return;

        // set the new active editor
        activeEditorPart = part;
        
        // first remove the old global and local action handlers
        // then add the ones for the new editor
        if (part instanceof GraphicalNedEditor) {
            textContrib.setActiveEditor(activeEditorPart);
            graphContrib.setActiveEditor(activeEditorPart);
        }
        else if (part instanceof TextualNedEditor) {
            graphContrib.setActiveEditor(activeEditorPart);
            textContrib.setActiveEditor(activeEditorPart);
        } else
            return;

        getActionBars().updateActionBars();
    }
}
