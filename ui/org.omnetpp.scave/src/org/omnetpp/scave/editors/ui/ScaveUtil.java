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
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.jface.viewers.StyledString;
import org.eclipse.jface.viewers.StyledString.Styler;
import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.MeasureTextFunctor;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.Options;
import org.omnetpp.common.engine.QuantityFormatter.Output;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.engineext.IMatchableObject;
import org.omnetpp.common.util.StringUtils;
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

    public static StyledString formatQuantity(double d, String unit, IMatchableObject matchableObject, int preferredPrecision, GC gc, int width, Styler unitStyler, Styler separatorStyler, Styler greyedoutStyler) {
        QuantityFormatter quantityFormatter = QuantityFormatterRegistry.getInstance().getQuantityFormatter(matchableObject);
        QuantityFormatter.Options options = quantityFormatter.getOptions();
        options.setNumAvailablePixels(gc != null ? width : Integer.MAX_VALUE);
        options.setMeasureTextFromJava(new MeasureTextFunctor() {
            @Override
            public int call(String text) {
                return gc.textExtent(text).x;
            }
        });
        int maxDigits = Math.max(options.getMinSignificantDigits(), Math.min(options.getNumAccurateDigits(), preferredPrecision));
        options.setMaxSignificantDigits(maxDigits);
        QuantityFormatter.Output output = quantityFormatter.formatQuantity(d, unit);
        int numChars = output.getText().length();
        if (!output.getFitsIntoAvailableSpace()) {
            return new StyledString("#".repeat(numChars), greyedoutStyler);
        }
        else {
            StyledString styledString = new StyledString(output.getText());
            if (unitStyler != null || separatorStyler != null) {
                String role = output.getRole();
                for (int i = 0; i < numChars; i++) {
                    char c = role.charAt(i);
                    if (separatorStyler != null && (c == ',' || c == '~'))
                        styledString.setStyle(i, 1, separatorStyler);
                    else if (unitStyler != null && c == 'u')
                        styledString.setStyle(i, 1, unitStyler);
                }
            }
            return styledString;
        }
    }

    public static StyledString formatRaw(double d, String unit, GC gc, int width, Styler unitStyler, Styler greyedoutStyler) {
        StyledString result = new StyledString(StringUtils.formatNumber(d));
        if (!StringUtils.isEmpty(unit))
            result.append(" " + UnitConversion.getShortName(unit), unitStyler);
        if (gc != null && gc.textExtent(result.getString()).x > width)
            return new StyledString("#".repeat(result.length()), greyedoutStyler);
        return result;
    }

}
