package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DefaultIndentLineAutoEditStrategy;
import org.eclipse.jface.text.DocumentCommand;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.TextUtilities;

import org.omnetpp.ned.editor.NedEditorPlugin;


/**
 * Auto-indent strategy for the NED language.
 *
 * @author rhornig, andras
 */
public class NedAutoIndentStrategy extends DefaultIndentLineAutoEditStrategy {

	public NedAutoIndentStrategy() {
	}

//	@Override
//	public void customizeDocumentCommand(IDocument doc, DocumentCommand command) {
//		try {
//			if (command.length != 0) return; // we ignore deletions/replacements
//			if (endsWithDelimiter(doc, command.text)) {
//			}
//			if (command.text.endsWith("}") || command.text.endsWith(":")) {
//				int line = doc.getLineOfOffset(command.offset);
//				String indentedLine = CorrectIndentationAction.getReindentedLines(doc, line, line);
//				String newIndent = CorrectIndentationAction.getIndent(indentedLine);
//				command.addCommand(doc.getLineOffset(line), doc.getLineLength(line), indentedLine, null);
//			}
//		} catch (BadLocationException e) {
//		}
//	}

	
	/* (non-Javadoc)
	 * Method declared on IAutoIndentStrategy
	 */
	@Override
	public void customizeDocumentCommand(IDocument doc, DocumentCommand command) {
		if (command.length == 0 && command.text != null && endsWithDelimiter(doc, command.text))
			smartIndentAfterNewLine(doc, command);
		else if ("}".equals(command.text)) { 
			smartInsertAfterBracket(doc, command);
		}
	}
	
	/**
	 * Returns whether or not the given text ends with one of the documents legal line delimiters.
	 * 
	 * @param d the document
	 * @param txt the text
	 * @return <code>true</code> if <code>txt</code> ends with one of the document's line delimiters, <code>false</code> otherwise
	 */
	private boolean endsWithDelimiter(IDocument d, String txt) {
		String[] delimiters= d.getLegalLineDelimiters();
		if (delimiters != null)
			return TextUtilities.endsWith(delimiters, txt) > -1;
		return false;
	}
	
	/**
	 * Returns the line number of the next bracket after end.
	 * 
	 * @param document - the document being parsed
	 * @param line - the line to start searching back from
	 * @param end - the end position to search back from
	 * @param closingBracketIncrease - the number of brackets to skip
	 * @return the line number of the next matching bracket after end
	 * @throws BadLocationException in case the line numbers are invalid in the document
	 */
	 protected int findMatchingOpenBracket(IDocument document, int line, int end, int closingBracketIncrease) throws BadLocationException {
	    int origLine = line;
		int start= document.getLineOffset(line);
		int brackcount= getBracketCount(document, start, end, false) - closingBracketIncrease;

		// sum up the brackets counts of each line (closing brackets count negative, 
		// opening positive) until we find a line the brings the count to zero
		while (brackcount < 0) {
			line--;
			if (line < 0) {
				return -1;
			}
			int linestart= document.getLineOffset(line);
			int lineend= linestart + document.getLineLength(line) - 1;
			brackcount += getBracketCount(document, linestart, lineend, false);
		}
		return line;
	}
	
	/**
	 * Returns the bracket value of a section of text. Closing brackets have a value of -1 and 
	 * open brackets have a value of 1.
	 * 
	 * @param document - the document being parsed
	 * @param start - the start position for the search
	 * @param end - the end position for the search
	 * @param ignoreCloseBrackets - whether or not to ignore closing brackets in the count
	 * @return the bracket value of a section of text
	 * @throws BadLocationException in case the positions are invalid in the document
	 */
	 private int getBracketCount(IDocument document, int start, int end, boolean ignoreCloseBrackets) throws BadLocationException {

		int begin = start;
		int bracketcount= 0;
		while (begin < end) {
			char curr= document.getChar(begin);
			begin++;
			switch (curr) {
				case '/' :
					if (begin < end) {
						char next= document.getChar(begin);
//XXX					if (next == '*') {
//							// a comment starts, advance to the comment end
//							begin= getCommentEnd(document, begin + 1, end);
//						} else if (next == '/') {
						if (next == '/') {
							// '//'-comment: nothing to do anymore on this line 
							begin= end;
						}
					}
					break;
//XXX			case '*' :
//					if (begin < end) {
//						char next= document.getChar(begin);
//						if (next == '/') {
//							// we have been in a comment: forget what we read before
//							bracketcount= 0;
//							begin++;
//						}
//					}
//					break;
				case '{' :
					bracketcount++;
					ignoreCloseBrackets= false;
					break;
				case '}' :
					if (!ignoreCloseBrackets) {
						bracketcount--;
					}
					break;
				case '"' :
//XXX				case '\'' :
					begin= getStringEnd(document, begin, end, curr);
					break;
				default :
					}
		}
		return bracketcount;
	}
	
//XXX	/**
//	 * Returns the end position of a comment starting at the given <code>position</code>.
//	 * 
//	 * @param document - the document being parsed
//	 * @param position - the start position for the search
//	 * @param end - the end position for the search
//	 * @return the end position of a comment starting at the given <code>position</code>
//	 * @throws BadLocationException in case <code>position</code> and <code>end</code> are invalid in the document
//	 */
//	 private int getCommentEnd(IDocument document, int position, int end) throws BadLocationException {
//		int currentPosition = position;
//		while (currentPosition < end) {
//			char curr= document.getChar(currentPosition);
//			currentPosition++;
//			if (curr == '*') {
//				if (currentPosition < end && document.getChar(currentPosition) == '/') {
//					return currentPosition + 1;
//				}
//			}
//		}
//		return end;
//	}
	
	/**
	 * Returns the content of the given line without the leading whitespace.
	 * 
	 * @param document - the document being parsed
	 * @param line - the line being searched
	 * @return the content of the given line without the leading whitespace
	 * @throws BadLocationException in case <code>line</code> is invalid in the document
	 */
	 public String getIndentOfLine(IDocument document, int line) throws BadLocationException {
		if (line > -1) {
			int start= document.getLineOffset(line);
			int end= start + document.getLineLength(line) - 1;
			int whiteend= findEndOfWhiteSpace(document, start, end);
			return document.get(start, whiteend - start);
		}
		return ""; 
	}

	/**
	 * Returns true if there is a section keyword ("parameters", "gates", "types", "submodules", "connections")
	 * on the given line.
	 * 
	 * @param document - the document being parsed
	 * @param line - the line being searched
	 * @return true if we found a section keyword
	 * @throws BadLocationException in case <code>line</code> is invalid in the document
	 */
	 protected boolean lineContainsSectionKeyword(IDocument document, int line) throws BadLocationException {
		 if (line < 0)
			 return false;

		 final String[] keywords = {"parameters", "gates", "types", "submodules", "connections"};
		 int start= document.getLineOffset(line);
		 int len = document.getLineLength(line) - 1;
		 String linestr = document.get(start, len);
		 for (int i=0; i<len-3; i++) {
			 //	If we reach a '//', stop. Also stop if we find string constant:
			 // if a section keyword occurs on the same line with a string literal,
			 // indentation is seriously broken anyway so autoindent won't matter...
			 if (linestr.charAt(i)=='/' && linestr.charAt(i+1)=='/')
				 break;
			 if (linestr.charAt(i)=='\"')
				 break; 
			 
			 for (String keyword : keywords) {
				 int klen = keyword.length();
				 if (linestr.regionMatches(i, keyword, 0, klen) &&
					 (i==0 || !Character.isJavaIdentifierPart(linestr.charAt(i-1))) &&
					 (i+klen>=len || !Character.isJavaIdentifierPart(linestr.charAt(i+klen)))) 
				 {
					 return true;
				 }
			 }
		 }
		 return false;
	 }
	 
	/**
	 * Returns the position of the <code>character</code> in the <code>document</code> after <code>position</code>.
	 * 
	 * @param document - the document being parsed
	 * @param position - the position to start searching from
	 * @param end - the end of the document
	 * @param character - the character you are trying to match
	 * @return the next location of <code>character</code>
	 * @throws BadLocationException in case <code>position</code> is invalid in the document
	 */
	 private int getStringEnd(IDocument document, int position, int end, char character) throws BadLocationException {
		int currentPosition = position;
		while (currentPosition < end) {
			char currentCharacter= document.getChar(currentPosition);
			currentPosition++;
			if (currentCharacter == '\\') {
				// ignore escaped characters
				currentPosition++;
			}
			else if (currentCharacter == character) {
				return currentPosition;
			}
		}
		return end;
	}
	
	/**
	 * Set the indent of a new line based on the command provided in the supplied document.
	 * @param document - the document being parsed
	 * @param command - the command being performed
	 */
	 protected void smartIndentAfterNewLine(IDocument document, DocumentCommand command) {

		int docLength= document.getLength();
		if (command.offset == -1 || docLength == 0)
			return;

		try {
			int p= (command.offset == docLength ? command.offset - 1 : command.offset);
			int line= document.getLineOfOffset(p);

			StringBuffer buf= new StringBuffer(command.text);
			if (command.offset < docLength && document.getChar(command.offset) == '}') {
				// if user just typed '}', find matching '{' and apply the indent of that line
				int indLine= findMatchingOpenBracket(document, line, command.offset, 0);
				if (indLine == -1) {
					indLine= line;
				}
				buf.append(getIndentOfLine(document, indLine));
			}
			else {
				// use indent of previous line, plus one tab for each opening brace
				int start= document.getLineOffset(line);
				int whiteend= findEndOfWhiteSpace(document, start, command.offset);
				buf.append(document.get(start, whiteend - start));
				if (getBracketCount(document, start, command.offset, true) > 0
                        || lineContainsSectionKeyword(document, line)) {
					buf.append("    ");
				}
			}
			command.text= buf.toString();

		} catch (BadLocationException excp) {
            NedEditorPlugin.logError(excp);
		}
	}
	
	/**
	 * Set the indent of a bracket based on the command provided in the supplied document.
	 * @param document - the document being parsed
	 * @param command - the command being performed
	 */
	 protected void smartInsertAfterBracket(IDocument document, DocumentCommand command) {
		if (command.offset == -1 || document.getLength() == 0)
			return;

		try {
			int p= (command.offset == document.getLength() ? command.offset - 1 : command.offset);
			int line= document.getLineOfOffset(p);
			int start= document.getLineOffset(line);
			int whiteend= findEndOfWhiteSpace(document, start, command.offset);

			// shift only when line does not contain any text up to the closing bracket
			if (whiteend == command.offset) {
				// evaluate the line with the opening bracket that matches out closing bracket
				int indLine= findMatchingOpenBracket(document, line, command.offset, 1);
				if (indLine != -1 && indLine != line) {
					// take the indent of the found line
					StringBuffer replaceText= new StringBuffer(getIndentOfLine(document, indLine));
					// add the rest of the current line including the just added close bracket
					replaceText.append(document.get(whiteend, command.offset - whiteend));
					replaceText.append(command.text);
					// modify document command
					command.length= command.offset - start;
					command.offset= start;
					command.text= replaceText.toString();
				}
			}
		} catch (BadLocationException excp) {
            NedEditorPlugin.logError(excp);
		}
	}
}
