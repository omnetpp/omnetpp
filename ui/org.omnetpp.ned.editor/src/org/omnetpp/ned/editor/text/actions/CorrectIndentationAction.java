package org.omnetpp.ned.editor.text.actions;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;

import org.omnetpp.common.editor.text.TextEditorAction;
import org.omnetpp.ned.editor.text.TextualNedEditor;

/**
 * Comments or uncomments lines in the selected region.
 *
 * @author andras
 */
public class CorrectIndentationAction extends TextEditorAction {
	public static final String ID = "org.omnetpp.ned.editor.text.CorrectIndentation";

    public CorrectIndentationAction(TextualNedEditor editor) {
		super(editor);
		setId(ID);
		setActionDefinitionId(ID);
        setText("Correct Indentation");
        setDescription("Correct Indentation");
        setToolTipText(getDescription());
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

				// Algorithm to find the indentation for any line:
				//  1. find the line of the previous (unpaired) "{"
				//  2. check the line to be indented:
				//      a. if it starts with "}": use same indent as "{" line
				//      b. if it is a "section keyword" line, use indent of "{" line + 4 spaces
				//      c. if it is some other line, use indent of "{" line + 8 spaces
				//      d. if there's no "{" line, use zero spaces as indent
				//
				String replacement = "";
				for (int i = startLine; i <= endLine; i++) {
					replacement += "."+getLine(doc,i);
				}

				doc.replace(doc.getLineOffset(startLine), doc.getLineOffset(endLine+1)-doc.getLineOffset(startLine), replacement);
			}
			catch (BadLocationException e) {
			}
		}
	}

	private String getLine(IDocument doc, int line) throws BadLocationException {
		return doc.get(doc.getLineOffset(line), doc.getLineLength(line));
	}

}
