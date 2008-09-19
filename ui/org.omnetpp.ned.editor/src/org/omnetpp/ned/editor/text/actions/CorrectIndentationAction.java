package org.omnetpp.ned.editor.text.actions;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.editor.text.Keywords;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.text.TextualNedEditor;

/**
 * Corrects the indentation level of lines in the selected region of the NED source.
 *
 * @author andras
 */
public class CorrectIndentationAction extends NedTextEditorAction {
	public static final String ID = "CorrectIndentation";

	public CorrectIndentationAction(TextualNedEditor editor) {
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

				String replacement = getReindentedLines(doc, startLine, endLine);

				// put back into the document
				TextEditorUtil.replaceRangeAndSelect(getTextEditor(), doc.getLineOffset(startLine), doc.getLineOffset(endLine+1) , replacement, true);
			}
			catch (BadLocationException e) {
			}
		}
	}

	public static String getReindentedLines(IDocument doc, int startLine, int endLine) throws BadLocationException {
		//
		// After "{", indent section keyword lines ("gates:") one level, and
		// and other lines two levels.
		//

		// find first non-blank line above our region to pick up indent level
		String prevLine = "\n";
		for (int i = startLine-1; i >= 0 && StringUtils.isBlank(removeCommentsAndStrings(prevLine)); i--)
			prevLine = getLine(doc, i);
		int prevLineBraceCount = getBraceCount(prevLine);
		String prevLineIndent = getIndent(prevLine);
		boolean prevLineContainsSectionKeyword = containsSectionKeyword(prevLine);

		// collect reindented lines
		String replacement = "";
		for (int i = startLine; i <= endLine; i++) {
			String line = getLine(doc,i);
			int braceCount = getBraceCount(line);
			boolean lineContainsSectionKeyword = containsSectionKeyword(line);

			String indent = prevLineIndent;
			if (prevLineBraceCount > 0)
				indent = modifyIndentLevel(indent, 2);
			if (prevLineContainsSectionKeyword)
				indent = modifyIndentLevel(indent, 1);
			if (braceCount < 0)
				indent = modifyIndentLevel(indent, -2);
			if (lineContainsSectionKeyword)
				indent = modifyIndentLevel(indent, -1);

			replacement += replaceIndent(line, indent);

			prevLine = line;
			prevLineBraceCount = braceCount;
			prevLineIndent = indent;
			prevLineContainsSectionKeyword = lineContainsSectionKeyword;
		}
		return replacement;
	}

	/**
	 * Return the sum of braces on the line, "{" counting as +1 and "}" as -1.
	 */
	private static int getBraceCount(String line) {
		line = removeCommentsAndStrings(line);
		return StringUtils.countMatches(line, "{") - StringUtils.countMatches(line, "}");
	}

	private static boolean containsSectionKeyword(String line) {
		line = removeCommentsAndStrings(line);
		return line.matches("(?s).*\\b("+StringUtils.join(Keywords.NED_SECTION_KEYWORDS, "|")+")\\b.*");
	}

	private static String removeCommentsAndStrings(String line) {
		line = line.replaceAll("\"[^\"]*\"", "\"\"");  // zap string literals (roughly - we ignore backslash escaping here)
		line = line.replaceFirst("//.*", "");  // remove comments
		return line;
	}

	public static String getIndent(String line) {
		return line.replaceFirst("(?s)^([ \t]*).*", "$1");
	}

	private static String modifyIndentLevel(String indent, int level) {
		for (int i = 0; i < level; i++)
			indent += "    ";
		for (int i = 0; i > level; i--)
			indent = indent.replaceFirst("(    )|\t", "");
		return indent;
	}

	private static String replaceIndent(String line, String indent) {
		return line.replaceFirst("(?s)^[ \t]*", indent);
	}

	/**
	 * Return the given line from the document, including line terminator.
	 */
	private static String getLine(IDocument doc, int line) throws BadLocationException {
		return doc.get(doc.getLineOffset(line), doc.getLineLength(line));
	}

}
