package org.omnetpp.scave.writers.impl;

import java.io.PrintStream;
import java.lang.management.ManagementFactory;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;

/**
 * Common base class for FileOutputVectorManager and FileOutputScalarManager
 *
 * @author Andras
 */
public abstract class OutputFileManager  {

    public static String generateRunID(String baseString) {
        String dateTime = new SimpleDateFormat("yyyyMMdd-HH:mm:ss").format(new Date());
        String pid = ManagementFactory.getRuntimeMXBean().getName();
        return baseString + "-" + dateTime + "-" + pid;
    }

    protected void writeRunHeader(PrintStream out, String runID, Map<String, String> runAttributes) {
        out.println("run " + q(runID));
        writeAttributes(out, runAttributes);
        out.println();
    }

    protected void writeAttributes(PrintStream out, Map<String, String> attributes) {
        if (attributes != null)
            for (String attr : attributes.keySet())
                out.println("attr " + q(attr) + " " + q(attributes.get(attr)));
    }

    /**
     * Quotes the given string if needed.
     */
    protected static String q(String s) {
        if (s == null || s.length() == 0)
            return "\"\"";

        boolean needsQuotes = false;
        for (int i=0; i<s.length(); i++) {
            char ch = s.charAt(i);
            if (ch == '\\' || ch == '"' || Character.isWhitespace(ch) || Character.isISOControl(ch)) {
                needsQuotes = true;
                break;
            }
        }

        if (needsQuotes) {
            StringBuilder buf = new StringBuilder();
            buf.append('"');
            for (int i=0; i<s.length(); i++) {
                char ch = s.charAt(i);
                switch (ch)
                {
                    case '\b': buf.append("\\b"); break;
                    case '\f': buf.append("\\f"); break;
                    case '\n': buf.append("\\n"); break;
                    case '\r': buf.append("\\r"); break;
                    case '\t': buf.append("\\t"); break;
                    case '"':  buf.append("\\\""); break;
                    case '\\': buf.append("\\\\"); break;
                    default:
                        if (!Character.isISOControl(ch))
                            buf.append(ch);
                        else
                            buf.append(String.format("\\x%02.2x", ch));
                }
            }
            buf.append('"');
            s = buf.toString();
        }
        return s;
    }
}
