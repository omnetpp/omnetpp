package org.omnetpp.inifile.editor.text.actions;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.texteditor.ITextEditor;

import org.omnetpp.common.editor.text.TextEditorUtil;

/**
 * Comments or uncomments lines in the selected region.
 *
 * @author andras
 */
public class ToggleCommentAction extends InifileTextEditorAction {
	public static final String ID = "ToggleComment";

    public ToggleCommentAction(ITextEditor editor) {
		super(ID, editor);
	}

	@Override
	public void run() {
		ISelection selection = getTextEditor().getSelectionProvider().getSelection();
		if (selection instanceof ITextSelection) {
			try {
				IDocument doc = getTextEditor().getDocumentProvider().getDocument(getTextEditor().getEditorInput());
				ITextSelection textSelection = (ITextSelection) selection;
				int startLine = textSelection.getStartLine();
				int endLine = textSelection.getEndLine();

				boolean allLinesAreComments = true;
				for (int i = startLine; i <= endLine; i++)
					if (!getLine(doc,i).trim().startsWith("#"))
						allLinesAreComments = false;

				String replacement = "";
				if (!allLinesAreComments) {
					// add "#"
					for (int i = startLine; i <= endLine; i++)
                        replacement += "#"+getLine(doc,i);
				}
				else {
					// remove "#"
					for (int i = startLine; i <= endLine; i++)
						replacement += getLine(doc,i).replaceFirst("#", "");
				}

				// put back into the document
				TextEditorUtil.replaceRangeAndSelect(getTextEditor(), doc.getLineOffset(startLine), doc.getLineOffset(endLine+1), replacement, true);
			}
			catch (BadLocationException e) {
			}
		}
	}

	private String getLine(IDocument doc, int line) throws BadLocationException {
		return doc.get(doc.getLineOffset(line), doc.getLineLength(line));
	}

}
