package org.omnetpp.ned.editor.text.actions;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.editor.text.NedKeywords;
import org.omnetpp.common.editor.text.TextEditorAction;
import org.omnetpp.common.util.StringUtils;
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

				// find first non-blank line above our region to pick up indent level 
				String prevLine = "\n";
				for (int i = startLine-1; i >= 0 && StringUtils.isBlank(prevLine); i--)
					prevLine = getLine(doc, i);
				int prevLineBraceCount = getBraceCount(prevLine);
				String prevLineIndent = getIndent(prevLine);
				boolean prevLineContainsSectionKeyword = containsSectionKeyword(prevLine);

				String replacement = "";
				for (int i = startLine; i <= endLine; i++) {
					String line = getLine(doc,i);
					int braceCount = getBraceCount(line);
					boolean lineContainsSectionKeyword = containsSectionKeyword(line);

					String indent = prevLineIndent;
					if (prevLineBraceCount > 0)
						indent = modifyIndentLevel(indent, 2);
					else if (braceCount < 0)
						indent = modifyIndentLevel(indent, -2);
					if (lineContainsSectionKeyword)
						indent = modifyIndentLevel(indent, -1);
					if (prevLineContainsSectionKeyword)
						indent = modifyIndentLevel(indent, 1);

					replacement += replaceIndent(line, indent);

					prevLine = line;
					prevLineBraceCount = braceCount;
					prevLineIndent = indent;
					prevLineContainsSectionKeyword = lineContainsSectionKeyword;
				}

				doc.replace(doc.getLineOffset(startLine), doc.getLineOffset(endLine+1)-doc.getLineOffset(startLine), replacement);
			}
			catch (BadLocationException e) {
			}
		}
	}

	/**
	 * Return the sum of braces on the line, "{" counting as +1 and "}" as -1. 
	 */
	private int getBraceCount(String line) {
		line = removeCommentsAndStrings(line);
		return StringUtils.countMatches(line, "{") - StringUtils.countMatches(line, "}");
	}

	private boolean containsSectionKeyword(String line) {
		line = removeCommentsAndStrings(line);
		return line.matches("(?s).*\\b("+StringUtils.join(NedKeywords.SECTION_KEYWORDS, "|")+")\\b.*");
	}

	private String removeCommentsAndStrings(String line) {
		line = line.replaceFirst("//.*", "");  // remove comments
		line = line.replaceAll("\"[^\"]*\"", "\"\"");  // zap string literals (roughly - we ignore backslash escaping here)
		return line;
	}

	private String getIndent(String line) {
		return StringUtils.isBlank(line) ? "" : line.replaceFirst("(?s)^([ \t]*).*", "$1");
	}

	private String modifyIndentLevel(String indent, int level) {
		for (int i = 0; i < level; i++)
			indent += "    ";
		for (int i = 0; i > level; i--)
			indent = indent.replaceFirst("(    )|\t", "");
		return indent;
	}

	private String replaceIndent(String line, String indent) {
		return line.replaceFirst("(?s)^[ \t]*", indent);
	}

	/**
	 * Return the given line from the document, including line terminator.
	 */
	private String getLine(IDocument doc, int line) throws BadLocationException {
		return doc.get(doc.getLineOffset(line), doc.getLineLength(line));
	}

}
