/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.io.StringWriter;
import java.util.ArrayList;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * XML-related utilities.
 *
 * @author andras
 */
public class XmlUtils  {

    public static ArrayList<String> collectChildTextsFromElementsWithTag(Node parent, String tag) {
        ArrayList<String> result = new ArrayList<>();

        NodeList children = parent.getChildNodes();

        for (int k = 0; k < children.getLength(); ++k) {
            Node childNode = children.item(k);
            if (childNode instanceof Element && tag.equals(childNode.getNodeName()))
                result.add(((Element)childNode).getTextContent());
        }

        return result;
    }

    public static Element getFirstElementWithTag(Node parent, String tag) {
        NodeList children = parent.getChildNodes();

        for (int k = 0; k < children.getLength(); ++k) {
            Node childNode = children.item(k);
            if (childNode instanceof Element && tag.equals(childNode.getNodeName()))
                return (Element)childNode;
        }

        return null;
    }

    public static ArrayList<Element> getElementsWithTag(Node parent, String tag) {
        ArrayList<Element> result = new ArrayList<>();

        NodeList children = parent.getChildNodes();

        for (int k = 0; k < children.getLength(); ++k) {
            Node childNode = children.item(k);
            if (childNode instanceof Element && tag.equals(childNode.getNodeName()))
                result.add((Element)childNode);
        }

        return result;
    }

    /**
     * Convert DOM tree to string.
     */
    public static String serialize(Document doc) throws TransformerException {
        TransformerFactory factory = TransformerFactory.newInstance();
        Transformer transformer = factory.newTransformer();
        transformer.setOutputProperty(OutputKeys.INDENT, "yes");
        transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "4");
        StringWriter writer = new StringWriter();
        transformer.transform(new DOMSource(doc), new StreamResult(writer));
        return writer.toString();
    }

    /**
     * Escape an XML attribute value.
     */
    public static String quoteAttr(String str) {
        str = str.replace("&", "&amp;");
        str = str.replace("<", "&lt;");
        str = str.replace(">", "&gt;");
        str = str.replace("\"", "&quot;");
        str = str.replace("\'", "&apos;");
        return str;
    }

}
