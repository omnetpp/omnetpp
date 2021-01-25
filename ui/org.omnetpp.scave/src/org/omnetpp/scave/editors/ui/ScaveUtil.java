/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.XmlUtils;
import org.omnetpp.scave.charting.properties.PlotProperty;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

/**
 * Various utility methods
 *
 * @author andras
 */
public class ScaveUtil {
    /**
     * Collect IDs of editable fields and their default values from an XSWT form.
     * They are taken from the x:id and x:id.default attributes.
     */
    public static Map<String,String> collectEditablePropertiesFromXswt(String xswtContent) throws ParserConfigurationException, SAXException, IOException {
        Map<String,String> properties = new HashMap<>();
        DocumentBuilder db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
        Document d = db.parse(new ByteArrayInputStream(xswtContent.getBytes()));
        ArrayList<Element> controls = XmlUtils.collectElements(d.getDocumentElement(), (e) -> (e.getAttributeNode("x:id") != null));
        for (Element e : controls) {
            String name = e.getAttribute("x:id"); //TODO not quite! use proper namespace
            if (properties.containsKey(name))
                Debug.print("WARNING: Duplicate property in XSWT pages: " + name);
            Attr valueNode = e.getAttributeNode("x:id.default");
            String value = "";
            if (valueNode != null)
                value = valueNode.getValue();
            else {
                PlotProperty prop = PlotProperty.lookup(name);
                if (prop != null)
                    value = prop.getDefaultValueAsString();
            }
            properties.put(name, value);
        }
        return properties;
    }


    /**
     * Convert the given double to string, printing at most the given number
     * of significant digits.
     */
    public static String formatNumber(double d, int numSignificantDigits) {
        return (d == Math.floor(d)) ? String.format(Locale.US, "%,.0f", d) : String.format(Locale.US, "%,." + numSignificantDigits + "f", d);
        //return new DecimalFormat("###,###,###,###,###,###,###,##0." + repeat('0', numSignificantDigits)).format(d);
        //return Common.formatDouble(d, numSignificantDigits);
        //return String.format(Locale.US, "%." + numSignificantDigits + "g", d); // differs from C's printf because this one leaves trailing zeros in
    }

    /**
     * Convert the given double to string, printing at most the given number
     * of significant digits. See formatNumber(double d, int numSignificantDigits).
     */
    public static String formatNumber(BigDecimal d, int numSignificantDigits) {
        // Workaround: BigDecimal can only convert itself to string with full precision, so we
        // convert it via double. Double has ~15..17 digits of precision, so if numSignificantDigits
        // is near that, we use BigDecimal's full-precision toString() method.
        return numSignificantDigits < 15 ? formatNumber(d.doubleValue(), numSignificantDigits) : d.toString();
    }

}
