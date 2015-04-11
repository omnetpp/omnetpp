/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

/**
 * Utility class to write CSV.
 *
 * @author tomi
 */
public class CsvWriter {
    char separator;
    String delimiter;
    StringBuffer buffer;

    public CsvWriter() {
        this(',');
    }

    public CsvWriter(char separator) {
        this(separator, System.getProperty("line.separator"));
    }

    public CsvWriter(char separator, String delimiter) {
        this.separator = separator;
        this.delimiter = delimiter;
        this.buffer = new StringBuffer();
    }

    public void addField(String value) {
        buffer.append(escape(value)).append(separator);
    }

    public void endRecord() {
        buffer.append(delimiter);
    }

    public String toString() {
        return buffer.toString();
    }

    public String escape(String str) {
        if (str.indexOf(separator) < 0 && str.indexOf(delimiter) < 0 && str.indexOf('"') < 0)
            return str;

        StringBuffer buffer = new StringBuffer(str);
        for (int i = 0; i < buffer.length(); ++i)
            if (buffer.charAt(i) == '"')
                buffer.insert(i++, '"');
        buffer.insert(0, '"');
        buffer.append('"');
        return buffer.toString();
    }
}

