package org.omnetpp.common.json;

/**
 * StringTree JSON, see http://www.stringtree.org/stringtree-json.html
 */
public class ExceptionErrorListener extends BufferErrorListener {

    public void error(String type, int col) {
        super.error(type, col);
        throw new IllegalArgumentException(buffer.toString());
    }
}
