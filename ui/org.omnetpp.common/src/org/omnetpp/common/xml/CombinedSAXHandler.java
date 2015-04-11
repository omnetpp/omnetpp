/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.xml;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class CombinedSAXHandler extends DefaultHandler {

    Map<String,ContentHandler> mapTagToHandler = new HashMap<String,ContentHandler>(5);
    Stack<ContentHandler> handlers = new Stack<ContentHandler>();

    public CombinedSAXHandler() {
        handlers.push(new DefaultHandler());
    }

    public void addContentHandler(String tagName, ContentHandler handler) {
        mapTagToHandler.put(tagName, handler);
    }

    public void addContentHandlers(Map<String,ContentHandler> map) {
        mapTagToHandler.putAll(map);
    }

    private ContentHandler currentHandler() {
        return handlers.peek();
    }

    private void enterHandler(String tag) {
        ContentHandler handler = mapTagToHandler.get(tag);
        if (handler != null)
            handlers.push(handler);
    }

    private void leaveHandler(String tag) {
        if (mapTagToHandler.containsKey(tag))
            handlers.pop();
    }

    @Override
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
        enterHandler(qName);
        currentHandler().startElement(uri, localName, qName, attributes);
    }

    @Override
    public void endElement(String uri, String localName, String qName) throws SAXException {
        currentHandler().endElement(uri, localName, qName);
        leaveHandler(qName);
    }

    @Override
    public void characters(char[] ch, int start, int length) throws SAXException {
        currentHandler().characters(ch, start, length);
    }

    @Override
    public void ignorableWhitespace(char[] ch, int start, int length) throws SAXException {
        currentHandler().ignorableWhitespace(ch, start, length);
    }

    @Override
    public void startPrefixMapping(String prefix, String uri) throws SAXException {
        currentHandler().startPrefixMapping(prefix, uri);
    }

    @Override
    public void endPrefixMapping(String prefix) throws SAXException {
        currentHandler().endPrefixMapping(prefix);
    }

    @Override
    public void processingInstruction(String target, String data) throws SAXException {
        currentHandler().processingInstruction(target, data);
    }

    @Override
    public void skippedEntity(String name) throws SAXException {
        currentHandler().skippedEntity(name);
    }
}
