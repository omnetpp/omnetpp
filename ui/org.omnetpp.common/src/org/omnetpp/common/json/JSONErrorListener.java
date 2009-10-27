package org.omnetpp.common.json;

/**
 * StringTree JSON, see http://www.stringtree.org/stringtree-json.html
 */
public interface JSONErrorListener {
    void start(String text);
    void error(String message, int column);
    void end();
}
