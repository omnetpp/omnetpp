package org.omnetpp.ned.editor.text.actions;

import java.util.ResourceBundle;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.ned.editor.text.TextualNedEditor;

/**
 * Comments or uncomments lines in the selected region.
 *
 * @author andras
 */
public class ToggleCommentAction extends TextEditorAction {
	public ToggleCommentAction(ResourceBundle resourceBundle, String prefix, TextualNedEditor editor) {
		super(resourceBundle, prefix, editor);
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
					if (!getLine(doc,i).trim().startsWith("//"))
						allLinesAreComments = false;

				String replacement = "";
				if (!allLinesAreComments) {
					// add "//"
					for (int i = startLine; i <= endLine; i++)
						replacement += getLine(doc,i).replaceFirst("([^\\s])", "//$1");
				}
				else {
					// remove "//"
					for (int i = startLine; i <= endLine; i++)
						replacement += getLine(doc,i).replaceFirst("//", "");
				}

				doc.replace(doc.getLineOffset(startLine), doc.getLineOffset(endLine+1), replacement);
			} 
			catch (BadLocationException e) {
			}
		}
	}

	private String getLine(IDocument doc, int line) throws BadLocationException {
		return doc.get(doc.getLineOffset(line), doc.getLineLength(line));
	}

}
