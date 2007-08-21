package org.omnetpp.ned.editor.text.actions;


import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.ned.editor.text.NedEditorMessages;

/**
 * A toolbar action which toggles the presentation model of the
 * connected text editor. The editor shows either the highlight range
 * only or always the whole document.
 *
 * @author rhornig
 */
public class NedPresentationAction extends TextEditorAction {

	/**
	 * Constructs and updates the action.
	 */
	public NedPresentationAction() {
		super(NedEditorMessages.getResourceBundle(), "TogglePresentation.", null); 
		update();
	}
	
	/* (non-Javadoc)
	 * Method declared on IAction
	 */
	@Override
	public void run() {

		ITextEditor editor= getTextEditor();

		editor.resetHighlightRange();
		boolean show= editor.showsHighlightRangeOnly();
		setChecked(!show);
		editor.showHighlightRangeOnly(!show);
	}
	
	/* (non-Javadoc)
	 * Method declared on TextEditorAction
	 */
	@Override
	public void update() {
		setChecked(getTextEditor() != null && getTextEditor().showsHighlightRangeOnly());
		setEnabled(true);
	}
}
