package org.omnetpp.common.editor.text;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.texteditor.ITextEditorExtension;
import org.eclipse.ui.texteditor.ITextEditorExtension2;
import org.eclipse.ui.texteditor.IUpdate;
import org.omnetpp.ned.editor.text.TextualNedEditor;

/**
 * This is a copy of TextEditorAction, except that we don't want labels etc
 * to be read from a resource file, so this one doesn't extend ResourceAction.
 * 
 * The action is initially associated with a text editor via the constructor,
 * but can subsequently be changed using <code>setEditor</code>.
 * Subclasses must implement the <code>run</code> method and if
 * required override the <code>update</code> method.
 * <p>
 * Subclasses that may modify the editor content should use {@link #canModifyEditor()}
 * in their <code>update</code> code to check whether updating the editor is most
 * likely possible (even if it is read-only - this may change for editor contents
 * that are under version control) and {@link #validateEditorInputState()} before
 * actually modifying the editor contents.
 * </p>
 */
public abstract class NedTextEditorAction extends Action implements IUpdate {
	private TextualNedEditor fTextEditor;

	public NedTextEditorAction(TextualNedEditor textEditor) {
		fTextEditor = textEditor;
	}
	
	/**
	 * Returns the action's text editor.
	 */
	protected TextualNedEditor getTextEditor() {
		return fTextEditor;
	}

	/**
	 * Retargets this action to the given editor.
	 */
	public void setEditor(TextualNedEditor editor) {
		fTextEditor = editor;
	}

	/**
	 * Always enables this action if it is connected to a text editor.
	 * If the associated editor is <code>null</code>, the action is disabled.
	 * Subclasses may override.
	 */
	public void update() {
		setEnabled(getTextEditor() != null);
	}

	/**
	 * Checks the editor's modifiable state. Returns <code>true</code> if the editor can be modified,
	 * taking in account the possible editor extensions.
	 *
	 * <p>If the editor implements <code>ITextEditorExtension2</code>,
	 * this method returns {@link ITextEditorExtension2#isEditorInputModifiable()};<br> else if the editor
	 * implements <code>ITextEditorExtension</code>, it returns {@link ITextEditorExtension#isEditorInputReadOnly()};<br>
	 * else, {@link ITextEditor#isEditable()} is returned, or <code>false</code> if the editor is <code>null</code>.</p>
	 *
	 * <p>There is only a difference to {@link #validateEditorInputState()} if the editor implements
	 * <code>ITextEditorExtension2</code>.</p>
	 *
	 * @return <code>true</code> if a modifying action should be enabled, <code>false</code> otherwise
	 * @since 3.0
	 */
	protected boolean canModifyEditor() {
		ITextEditor editor= getTextEditor();
		if (editor instanceof ITextEditorExtension2)
			return ((ITextEditorExtension2) editor).isEditorInputModifiable();
		else if (editor instanceof ITextEditorExtension)
			return !((ITextEditorExtension) editor).isEditorInputReadOnly();
		else if (editor != null)
			return editor.isEditable();
		else
			return false;
	}

	/**
	 * Checks and validates the editor's modifiable state. Returns <code>true</code> if an action
	 * can proceed modifying the editor's input, <code>false</code> if it should not.
	 *
	 * <p>If the editor implements <code>ITextEditorExtension2</code>,
	 * this method returns {@link ITextEditorExtension2#validateEditorInputState()};<br> else if the editor
	 * implements <code>ITextEditorExtension</code>, it returns {@link ITextEditorExtension#isEditorInputReadOnly()};<br>
	 * else, {@link ITextEditor#isEditable()} is returned, or <code>false</code> if the editor is <code>null</code>.</p>
	 *
	 * <p>There is only a difference to {@link #canModifyEditor()} if the editor implements
	 * <code>ITextEditorExtension2</code>.</p>
	 *
	 * @return <code>true</code> if a modifying action can proceed to modify the underlying document, <code>false</code> otherwise
	 * @since 3.0
	 */
	protected boolean validateEditorInputState() {
		ITextEditor editor= getTextEditor();
		if (editor instanceof ITextEditorExtension2)
			return ((ITextEditorExtension2) editor).validateEditorInputState();
		else if (editor instanceof ITextEditorExtension)
			return !((ITextEditorExtension) editor).isEditorInputReadOnly();
		else if (editor != null)
			return editor.isEditable();
		else
			return false;
	}
}
