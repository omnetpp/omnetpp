package org.omnetpp.ned.editor.text;


import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.editors.text.TextEditorActionContributor;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.eclipse.ui.texteditor.RetargetTextEditorAction;

import org.omnetpp.ned.editor.text.actions.ConvertToNewFormatAction;
import org.omnetpp.ned.editor.text.actions.CorrectIndentationAction;
import org.omnetpp.ned.editor.text.actions.DefineFoldingRegionAction;
import org.omnetpp.ned.editor.text.actions.FormatSourceAction;
import org.omnetpp.ned.editor.text.actions.NedTextEditorAction;
import org.omnetpp.ned.editor.text.actions.OrganizeImportsAction;
import org.omnetpp.ned.editor.text.actions.ToggleCommentAction;

/**
 * Contributes interesting Java actions to the desktop's Edit menu and the toolbar.
 *
 * @author rhornig
 */
public class TextualNedEditorActionContributor extends TextEditorActionContributor {
    private RetargetTextEditorAction fContentAssistProposal;
    private RetargetTextEditorAction fDefineFoldingRegion;
    private RetargetTextEditorAction fConvertToNewFormatAction;
    private RetargetTextEditorAction fFormatSourceAction;
    private RetargetTextEditorAction fOrganizeImportsAction;
    private RetargetTextEditorAction fToggleCommentAction;
    private RetargetTextEditorAction fCorrectIndentationAction;

	/**
	 * Default constructor.
	 */
	public TextualNedEditorActionContributor() {
		super();
		// Currently we do not use resource bundles (the text is coming from the action behind the retargetable action)
		fContentAssistProposal= new RetargetTextEditorAction(NedEditorMessages.getResourceBundle(), "ContentAssistProposal.");
		fContentAssistProposal.setActionDefinitionId(ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS);

		fDefineFoldingRegion = createRetargetAction("DefineFoldingRegion");
		fConvertToNewFormatAction = createRetargetAction("ConvertToNewFormat");
		fFormatSourceAction = createRetargetAction("FormatSource");
		fOrganizeImportsAction = createRetargetAction("OrganizeImports");
		fToggleCommentAction = createRetargetAction("ToggleComment");
		fCorrectIndentationAction = createRetargetAction("CorrectIndentation");
	}

	private static RetargetTextEditorAction createRetargetAction(String id) {
	    RetargetTextEditorAction action = new RetargetTextEditorAction(NedEditorMessages.getResourceBundle(), id+".");
	    action.setActionDefinitionId(NedTextEditorAction.ACTION_DEFINITION_PREFIX + id);
	    return action;
	}

	/*
	 * @see IEditorActionBarContributor#init(IActionBars)
	 */
	@Override
	public void init(IActionBars bars) {
		super.init(bars);

		IMenuManager menuManager= bars.getMenuManager();
		IMenuManager editMenu= menuManager.findMenuUsingPath(IWorkbenchActionConstants.M_EDIT);
		if (editMenu != null) {
			editMenu.add(new Separator());
			editMenu.add(fContentAssistProposal);
		}
	}

	/*
	 * @see IEditorActionBarContributor#setActiveEditor(IEditorPart)
	 */
	@Override
	public void setActiveEditor(IEditorPart part) {
		super.setActiveEditor(part);

		ITextEditor editor= null;
        if (part instanceof ITextEditor)
            editor= (ITextEditor) part;

        // retarget the actions to the current editor
        fContentAssistProposal.setAction(getAction(editor, ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS));
        fDefineFoldingRegion.setAction(getAction(editor, DefineFoldingRegionAction.ID));
        fConvertToNewFormatAction.setAction(getAction(editor, ConvertToNewFormatAction.ID));
        fFormatSourceAction.setAction(getAction(editor, FormatSourceAction.ID));
        fOrganizeImportsAction.setAction(getAction(editor, OrganizeImportsAction.ID));
        fToggleCommentAction.setAction(getAction(editor, ToggleCommentAction.ID));
        fCorrectIndentationAction.setAction(getAction(editor, CorrectIndentationAction.ID));

	}

	@Override
	public void contributeToMenu(IMenuManager menuManager) {
	    super.contributeToMenu(menuManager);
        MenuManager sourceMenu = new MenuManager("Source");
        sourceMenu.add(fToggleCommentAction);
        sourceMenu.add(new Separator());
        sourceMenu.add(fDefineFoldingRegion);
        sourceMenu.add(fCorrectIndentationAction);
        sourceMenu.add(fFormatSourceAction);
        sourceMenu.add(fConvertToNewFormatAction);
        sourceMenu.add(new Separator());
        sourceMenu.add(fOrganizeImportsAction);
        sourceMenu.add(new Separator());
        sourceMenu.add(fContentAssistProposal);
        menuManager.insertAfter(IWorkbenchActionConstants.M_EDIT, sourceMenu);
	}

}
