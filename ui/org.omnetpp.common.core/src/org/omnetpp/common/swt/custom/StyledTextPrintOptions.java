/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.common.swt.custom;

/**
 * Use StyledTextPrintOptions to specify printing options for the
 * StyledText.print(Printer, StyledTextPrintOptions) API.
 * <p>
 * The following example prints a right aligned page number in the footer,
 * sets the job name to "Example" and prints line background colors but no other
 * formatting:
 * </p>
 * <pre>
 * StyledTextPrintOptions options = new StyledTextPrintOptions();
 * options.footer = "\t\t&lt;page&gt;";
 * options.jobName = "Example";
 * options.printLineBackground = true;
 *
 * Runnable runnable = styledText.print(new Printer(), options);
 * runnable.run();
 * </pre>
 *
 * @see <a href="http://www.eclipse.org/swt/">Sample code and further information</a>
 *
 * @since 2.1
 */
public class StyledTextPrintOptions {
    /**
     * Page number placeholder constant for use in <code>header</code>
     * and <code>footer</code>. Value is <code>&lt;page&gt;</code>
     */
    public static final String PAGE_TAG = "<page>";
    /**
     * Separator constant for use in <code>header</code> and
     * <code>footer</code>. Value is <code>\t</code>
     */
    public static final String SEPARATOR = "\t";
    /**
     * Formatted text to print in the header of each page.
     * <p>"left '\t' center '\t' right"</p>
     * <p>left, center, right = &lt;page&gt; | #CDATA</p>
     * <p>Header and footer are defined as three separate regions for arbitrary
     * text or the page number placeholder &lt;page&gt;
     * (<code>StyledTextPrintOptions.PAGE_TAG</code>). The three regions are
     * left aligned, centered and right aligned. They are separated by a tab
     * character (<code>StyledTextPrintOptions.SEPARATOR</code>).
     */
    public String header = null;
    /**
     * Formatted text to print in the footer of each page.
     * <p>"left '\t' center '\t' right"</p>
     * <p>left, center, right = &lt;page&gt; | #CDATA</p>
     * <p>Header and footer are defined as three separate regions for arbitrary
     * text or the page number placeholder &lt;page&gt;
     * (<code>StyledTextPrintOptions.PAGE_TAG</code>). The three regions are
     * left aligned, centered and right aligned. They are separated by a tab
     * character (<code>StyledTextPrintOptions.SEPARATOR</code>).
     */
    public String footer = null;
    /**
     * Name of the print job.
     */
    public String jobName = null;

    /**
     * Print the text foreground color. Default value is <code>false</code>.
     */
    public boolean printTextForeground = false;
    /**
     * Print the text background color. Default value is <code>false</code>.
     */
    public boolean printTextBackground = false;
    /**
     * Print the font styles. Default value is <code>false</code>.
     */
    public boolean printTextFontStyle = false;
    /**
     * Print the line background color. Default value is <code>false</code>.
     */
    public boolean printLineBackground = false;

    /**
     * Print line numbers. Default value is <code>false</code>.
     *
     * @since 3.3
     */
    public boolean printLineNumbers = false;

    /**
     * Labels used for printing line numbers.
     *
     * @since 3.4
     */
    public String[] lineLabels = null;

}
