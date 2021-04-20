/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.io.StringReader;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;

/**
 * Parses an ini file. Parse results are passed back via a callback.
 * @author Andras
 */
public class InifileParser {
    /**
     * Implement this interface to store ini file contents as it gets parsed. Comments are
     * passed back with leading "#" and preceding whitespace, or as "" if there's no comment
     * on that line.
     */
    public interface ParserCallback {
        void blankOrCommentLine(int lineNumber, int numLines, String rawLine, String rawComment);
        void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment);
        void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String rawValue, String rawComment);
        void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String rawComment);
        void parseError(int lineNumber, int numLines, String message);
    }

    /**
     * A ParserCallback with all methods defined to be empty.
     */
    public static class ParserAdapter implements ParserCallback {
        public void blankOrCommentLine(int lineNumber, int numLines, String rawLine, String rawComment) {}
        public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {}
        public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String rawValue, String rawComment) {}
        public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String rawComment) {}
        public void parseError(int lineNumber, int numLines, String message) {}
    }

    /**
     * A ParserCallback for debug purposes.
     */
    public static class DebugParserAdapter implements ParserCallback {
        public void blankOrCommentLine(int lineNumber, int numLines, String rawLine, String rawComment) {
            Debug.println(lineNumber+": "+rawLine+" --> rawComment="+rawComment);
        }
        public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {
            Debug.println(lineNumber+": "+rawLine+" --> section '"+sectionName+"'  rawComment="+rawComment);
        }
        public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String rawValue, String rawComment) {
            Debug.println(lineNumber+": "+rawLine+" --> key='"+key+"' rawValue='"+rawValue+"'  rawComment="+rawComment);
        }
        public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String rawComment) {
            Debug.println(lineNumber+": "+rawLine+" --> directive='"+directive+"' args='"+args+"'  rawComment="+rawComment);
        }
        public void parseError(int lineNumber, int numLines, String message) {
            Debug.println(lineNumber+": PARSE ERROR: "+message);
        }
    }

    private static final String INCLUDE = "include"; // name of inifile directive
    public static final String CONFIG_ = "Config "; // optional name prefix for the section headers; includes a trailing space

    /**
     * Parses an IFile.
     */
    public void parse(IFile file, ParserCallback callback) throws CoreException {
        parse(new InputStreamReader(file.getContents()), callback);
    }

    /**
     * Parses a multi-line string.
     */
    public void parse(String text, ParserCallback callback) throws CoreException {
        parse(new StringReader(text), callback);
    }

    /**
     * Parses a stream.
     */
    public void parse(Reader streamReader, ParserCallback callback) throws CoreException {
        try {
            LineNumberReader reader = new LineNumberReader(streamReader);

            String concatenatedLine = "";
            String concatenatedRawLines = "";
            int startLineNumber = -1;
            int lineNumber = 0;
            while (true) {
                String rawLine = reader.readLine();
                if (rawLine == null)
                    break;
                lineNumber = reader.getLineNumber();
                if (concatenatedLine.endsWith("\\")) {
                    concatenatedLine = StringUtils.removeEnd(concatenatedLine, "\\") + rawLine;
                    concatenatedRawLines += "\n" + rawLine;
                }
                else if (!rawLine.isBlank() && (rawLine.startsWith(" ") || rawLine.startsWith("\t"))) {
                    concatenatedLine += "\n" + rawLine;
                    concatenatedRawLines += "\n" + rawLine;
                }
                else {
                    if (startLineNumber != -1) {
                        int numLines = lineNumber - startLineNumber;
                        processLine(concatenatedLine, callback, startLineNumber, numLines, concatenatedRawLines);
                    }
                    concatenatedLine = concatenatedRawLines = rawLine;
                    startLineNumber = lineNumber;
                }
            }
            if (startLineNumber != -1) {
                concatenatedLine = StringUtils.removeEnd(concatenatedLine, "\\"); // remove final stray backslash
                int numLines = lineNumber - startLineNumber + 1;
                processLine(concatenatedLine, callback, startLineNumber, numLines, concatenatedRawLines);
            }
        }
        catch (IOException e) {
            throw InifileEditorPlugin.wrapIntoCoreException(e);
        }
    }

    protected void processLine(String line, ParserCallback callback, int lineNumber, int numLines, String rawLine) {
        // separate significant content and comments
        String cleanedLine; // with comments removed
        String rawComment; // including '#' and the whitespace before it; but empty for multi-line values
        boolean isMultiline = line.contains("\n");
        if (!isMultiline) {
            int endPos = findEndContent(line, 0);
            if (endPos == -1) {
                callback.parseError(lineNumber, 1, "Unterminated string constant");
                return;
            }
            rawComment = line.substring(endPos);
            cleanedLine = line.substring(0, endPos);
        }
        else {
            cleanedLine = "";
            int k = 0;
            for (String l : line.split("\n")) {
                int endPos = findEndContent(l, 0);
                if (endPos == -1) {
                    callback.parseError(lineNumber + k, 1, "Unterminated string constant");
                    return;
                }
                cleanedLine += l.substring(0, endPos) + "\n";
                k++;
            }
            rawComment = "";
            cleanedLine = StringUtils.removeEnd(cleanedLine, "\n");
        }

        // process the line
        char firstChar = cleanedLine.isEmpty() ? 0 : cleanedLine.charAt(0);
        if (cleanedLine.isEmpty()) {
            // blank or comment
            callback.blankOrCommentLine(lineNumber, numLines, rawLine, rawLine);
        }
        else if (Character.isWhitespace(firstChar)) { // space or newline
            callback.parseError(lineNumber, 1, "Content must begin on column 1 because indentation is used for line continuation");
        }
        else if (firstChar == ';') {
            // obsolete comment line
            callback.parseError(lineNumber, numLines, "Semicolon is no longer a comment start character, please use hashmark ('#')");
        }
        else if (firstChar == 'i' && cleanedLine.matches(INCLUDE + "\\s.*")) {
            // include directive
            String directive = INCLUDE;
            String args = StringUtils.removeStart(cleanedLine, directive).trim();
            callback.directiveLine(lineNumber, numLines, rawLine, directive, args, rawComment);
        }
        else if (firstChar == '[') {
            // section heading
            if (!cleanedLine.endsWith("]")) {
                callback.parseError(lineNumber, numLines, "Syntax error in section heading");
                return;
            }
            String sectionName = StringUtils.removeEnd(StringUtils.removeStart(cleanedLine, "["), "]").trim();
            sectionName = StringUtils.removeStart(sectionName,  CONFIG_).trim(); // "Config " prefix is optional, starting from OMNeT++ 6.0
            callback.sectionHeadingLine(lineNumber, numLines, rawLine, sectionName, rawComment);
        }
        else {
            // key = value
            int equalSignPos = cleanedLine.indexOf('=');
            if (equalSignPos == -1) {
                callback.parseError(lineNumber, numLines, "Line must be in the form key=value");
                return;
            }
            String key = cleanedLine.substring(0, equalSignPos).trim();
            if (key.isEmpty()) {
                callback.parseError(lineNumber, numLines, "Line must be in the form key=value");
                return;
            }
            if (!isMultiline) {
                String value = cleanedLine.substring(equalSignPos+1).trim();
                callback.keyValueLine(lineNumber, numLines, rawLine, key, value, rawComment);
            }
            else {
                // for multi-line values, we don't separate the value and comments, because there's no
                // meaningful way of editing them separately and then put them back into the file.
                // Thus, we leave comments as part of the value string, and pass "" as rawComment.
                int rawEqualSignPos = findEqualSign(line);
                Assert.isTrue(rawEqualSignPos != -1); // we checked it before
                String rawValue = line.substring(rawEqualSignPos+1).trim();
                callback.keyValueLine(lineNumber, numLines, rawLine, key, rawValue, "");
            }
        }
    }

    /**
     * Returns the position right after the end of significant content on the
     * first line of the given string. Scanning stops at the first newline (LF) character.
     * String literals are recognized and skipped properly.
     *
     * Returns -1 if the line contains an unterminated string literal.
     */
    private static int findEndContent(String line, int fromPos) {
        int k;
        for (k = fromPos; k < line.length() && line.charAt(k) != '\n' && line.charAt(k) != '#'; k++) {
            if (line.charAt(k) == '"') {
                // string literal: skip it
                k++;
                while (k < line.length() && line.charAt(k) != '"' && line.charAt(k) != '\n') {
                    if (line.charAt(k) == '\\')  // skip \", \\, etc.
                        k++;
                    k++;
                }
                if (k >= line.length() || line.charAt(k) != '"')
                    return -1; // meaning "unterminated string literal"
            }
        }
        while (k > 0 && Character.isWhitespace(line.charAt(k-1)) && line.charAt(k-1) != '\n')
            k--;
        return k;
    }

    /**
     * Return the position of the first significant (non-comment) '=' sign
     * in the given multi-line string, or -1 of not found.
     */
    private static int findEqualSign(String rawLine) {
        int pos = 0;
        while (true) {
            int endPos = findEndContent(rawLine, pos);
            Assert.isTrue(endPos != -1);  // checked before
            int equalSignPos = rawLine.substring(0,endPos).indexOf('=', pos);
            if (equalSignPos != -1)
                return equalSignPos;
            pos = rawLine.indexOf('\n', endPos); // skip comment
            if (pos == -1)
                break;
            pos++;
        }
        return -1;
    }

    /**
     * Removes comments from the given string that may contain multiple lines.
     * If a line contains an unterminated string constant, its the entire line
     * is included in the output.
     */
    public static String stripComments(String rawLine) {
        StringBuilder buffer  = new StringBuilder();
        for (String l : rawLine.split("\n")) {
            int endPos = findEndContent(l, 0);
            buffer.append(endPos==-1 ? l : l.substring(0, endPos));
            buffer.append("\n");
        }
        return StringUtils.removeEnd(buffer.toString(), "\n");
    }


}
