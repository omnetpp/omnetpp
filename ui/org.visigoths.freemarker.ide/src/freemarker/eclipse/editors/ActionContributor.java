package freemarker.eclipse.editors;

import org.eclipse.jface.action.IMenuManager;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.editors.text.TextEditorActionContributor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.RetargetTextEditorAction;

/**
 * Contributes actions to the FreeMarker template editor
 *
 * @author <a href="mailto:andras@omnetpp.org">Andras Varga</a>
 */
public class ActionContributor extends TextEditorActionContributor {
    private RetargetTextEditorAction fContentAssistProposal;

    public ActionContributor() {
        super();

        // Currently we do not use resource bundles (the text is coming from the action behind the retargetable action)
        fContentAssistProposal= new RetargetTextEditorAction(EditorMessages.getResourceBundle(), "ContentAssistProposal.");
        fContentAssistProposal.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);
    }

    @Override
    public void setActiveEditor(IEditorPart part) {
        super.setActiveEditor(part);

        ITextEditor editor= null;
        if (part instanceof ITextEditor)
            editor= (ITextEditor) part;

        // retarget the actions to the current editor
        fContentAssistProposal.setAction(getAction(editor, ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS));
    }

    @Override
    public void contributeToMenu(IMenuManager menuManager) {
        super.contributeToMenu(menuManager);

        IMenuManager editMenu = menuManager.findMenuUsingPath(IWorkbenchActionConstants.M_EDIT);
        editMenu.add(fContentAssistProposal);
    }
}
