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
    String appendFilenameExtension(String fileName);
    /** default text on the opening overview page */
    String defaultOverviewString();
    String copyrightNotice();
    String svgImage(String imageFileName);
    String typeImageTag(String source);
    String typeImageMap();
    /** Specify a named rectangular linked area on an image */
    String areaRef(String text, String url, Rectangle bounds);
    String endTypeImageMap();
    /** Display a header for a NED/MSG type */
    String typeSectionHeading(ITypeElement typeElement);
    String sectionHeading(String text, String clazz);
    String subsectionHeading(String text, String clazz);
    String beginSource();
    String endSource();
    String beginTable(String clazz);
    String endTable();
    /** even number of parameters using: (header label, class) pairs for each column */
    String tableHeading(String ...columns);
    /** output a raw in a table with the given class */
    String tableRow(String clazz, String ...cells);
    String anchor(String id);
    String link(String text, String url, String clazz);
    String link(String text, String url, String clazz, String tooltip);
    String paragraph(String text);
    String italic(String text);
    String bold(String text);
    String code(String text, String clazz);
    String styled(String text, String clazz, TextAttribute textAttribute);
    /** copy all static resources to the generated doc's target dir (css, fonts, java script etc. */
    void copyStaticResources(IPath cssPath) throws Exception;

}