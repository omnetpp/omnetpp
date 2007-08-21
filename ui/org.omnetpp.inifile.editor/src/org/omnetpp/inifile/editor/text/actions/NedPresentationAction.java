package org.omnetpp.inifile.editor.text.actions;


import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.inifile.editor.text.InifileEditorMessages;

/**
 * A toolbar action which toggles the presentation model of the
 * connected text editor. The editor shows either the highlight range
 * only or always the whole document.
 */
//XXX TODO rename, revise, possibly remove...
public class NedPresentationAction extends TextEditorAction {

	/**
	 * Constructs and updates the action.
	 */
	public NedPresentationAction() {
		super(InifileEditorMessages.getResourceBundle(), "TogglePresentation.", null); //$NON-NLS-1$
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
