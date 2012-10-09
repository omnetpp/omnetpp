package org.omnetpp.common.json;

/**
 * StringTree JSON, see http://www.stringtree.org/stringtree-json.html
 */
public class StdoutStreamErrorListener extends BufferErrorListener {

    public void end() {
        System.out.print(buffer.toString());
    }
}
