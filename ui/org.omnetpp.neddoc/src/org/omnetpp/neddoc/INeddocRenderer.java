package org.omnetpp.neddoc;

import org.eclipse.core.runtime.IPath;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.text.TextAttribute;
import org.omnetpp.ned.model.interfaces.ITypeElement;

/**
 * Interface for rendering documentation from DocumentationGenerator.
 * This, in theory, allows various pages templates and/or non-HTML output formats
 * to coexist.
 */
public interface INeddocRenderer {
    /** package name under org.omnetpp.neddoc.templates where static resource files are available */
    String getTemplateName();
    String addExtension(String fileName);
    /** default text on the opening overview page */
    String defaultOverviewString();
    String ccFooterString();
    String svgObjectTag(String imageFileName);
    String typeImageTag(String source);
    String typeImageMapHeaderTag();
    /** Specify a named rectangular linked area on an image */
    String areaRefTag(String text, String url, Rectangle bounds);
    String typeImageMapTrailerTag();
    String titleTag(String text, String clazz);
    /** Display a header for a NED/MSG type */
    String typeSectionTag(ITypeElement typeElement);
    String sectionTag(String text, String clazz);
    String subSectionTag(String text, String clazz);
    String sourceHeaderTag();
    String sourceTrailerTag();
    String tableHeaderTag(String clazz);
    String tableTrailerTag();
    /** even number of parameters using: (header label, class) pairs for each column */
    String tableHeaderRowTag(String ...columns);
    /** output a raw in a table with the given class */
    String tableDataRowTag(String clazz, String ...cells);
    String anchorTag(String id);
    String refTag(String text, String url, String clazz);
    String pTag(String text);
    String iTag(String text);
    String bTag(String text);
    String spanTag(String text, String clazz, TextAttribute textAttribute);
    /** copy all static resources to the generated doc's target dir (css, fonts, java script etc. */
    void copyStaticResources(IPath cssPath) throws Exception;

}