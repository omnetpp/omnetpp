/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.xml;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.util.Stack;

public class XMLWriter {

    private Writer writer;
    private String encoding;
    boolean indenting;
    int indentWidth;

    boolean startElementOpened;
    Stack<String> elementStack = new Stack<String>();
    boolean emptyElement;
    int indentLevel;
    boolean atLineStart;


    public XMLWriter(OutputStream os, String encoding) throws UnsupportedEncodingException {
        this(os, encoding, -1);
    }

    public XMLWriter(OutputStream os, String encoding, int indentWidth) throws UnsupportedEncodingException {
        this(new OutputStreamWriter(os, encoding), encoding, indentWidth);
    }

    public XMLWriter(Writer writer, String encoding) {
        this(writer, encoding, -1);
    }

    public XMLWriter(Writer writer, String encoding, int indentWidth) {
        this.writer = writer;
        this.encoding = encoding;
        this.indenting = indentWidth >= 0;
        this.indentWidth = indentWidth;
        this.atLineStart = true;
    }

    public void writeXMLHeader() throws IOException {
        write("<?xml version=\"1.0\" encoding=\"");
        write(escape(encoding));
        write("\" standalone=\"yes\"?>");
        writeNewline();
    }

    public void writeComment(String comment) throws IOException {
        closeStartElement(false);
        write("<!--");
        write(comment);
        write(" -->");
    }

    public void writeStartElement(String qname) throws IOException {
        closeStartElement(true);
        writeIndent();
        write("<");
        write(qname);
        indent();
        elementStack.push(qname);
        startElementOpened = true;
        emptyElement = true;
    }

    public void writeEndElement(String qname) throws IOException {
        if (elementStack.empty())
            throw new IOException("no matching XML start tag");

        String expected = elementStack.pop();
        if (!qname.equals(expected))
            throw new IOException("start/end tag mismatch " +
                "(given: "+ qname + " expected: " + expected +")");

        deindent();
        if (emptyElement) {
            write("/>");
            writeNewline();
            startElementOpened = false;
        }
        else {
            writeIndent();
            write("</");
            write(qname);
            write(">");
            writeNewline();
        }
        emptyElement = false;
    }

    private void closeStartElement(boolean addNewLine) throws IOException {
        if (startElementOpened) {
            write(">");
            if (addNewLine) {
                writeNewline();
            }
            startElementOpened = false;
        }
    }

    public void writeAttribute(String name, String value) throws IOException {
        write(" ");
        write(name);
        write("=\"");
        write(escape(value));
        write("\"");
    }

    public void writeText(String text) throws IOException {
        closeStartElement(false);
        write(escape(text));
        emptyElement = false;
    }

    public void close() throws IOException {
        writer.close();
    }

    private void writeNewline() throws IOException {
        if (indenting) {
            writer.write("\n");
            atLineStart = true;
        }
    }

    private void write(String str) throws IOException {
        writer.write(str);
        atLineStart = false;
    }

    private void indent() throws IOException {
        if (indenting) {
            indentLevel += indentWidth;
        }
    }

    private void deindent() throws IOException {
        if (indenting) {
            indentLevel -= indentWidth;
        }
    }

    private static char[] sixteenSpaces = "               ".toCharArray();

    private void writeIndent() throws IOException {
        if (indenting && atLineStart) {
            int major = indentLevel / 16;
            int minor = indentLevel % 16;

            for (int i = 0; i < major; ++i)
                writer.write(sixteenSpaces, 0, 16);
            if (minor > 0)
                writer.write(sixteenSpaces, 0, minor);
            atLineStart = false;
        }
    }

    private static String escape(String str) {
        str = str.replace("&", "&amp;");
        str = str.replace("<", "&lt;");
        str = str.replace(">", "&gt;");
        str = str.replace("\"", "&quot;");
        str = str.replace("\'", "&apos;");
        return str;
    }
}
