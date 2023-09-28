/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.apache.commons.lang3.text.WordUtils;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.ui.StyledTextUndoRedoManager;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.common.wizard.support.InfoLink;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Chart.DialogPage;

/**
 * Dialog for defining a new XSWT chart page. Contains a built-in wizard
 * for generating a page from a simple specification string that lists
 * the required controls.
 *
 * @author andras
 */
public class NewChartPageDialog extends TitleAreaDialog {
    private String title;
    private Text nameText;
    private StyledText formText;
    private DialogPage result;

    public NewChartPageDialog(Shell parentShell) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "New Chart Page";
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
            shell.setText(title);
    }

    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("New Chart Page");
        setMessage("Enter details for new chart dialog page");

        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(2, false));

        SWTFactory.createLabel(composite, "Name:", 1);
        nameText = SWTFactory.createText(composite, SWT.BORDER, 1, GridData.FILL_HORIZONTAL);

        Group contentsGroup = SWTFactory.createGroup(composite, "Page content", 1, 2, GridData.FILL_BOTH);

        InfoLink infoLink = new InfoLink(contentsGroup, SWT.WRAP);
        infoLink.setText("If you enter the list of properties to be edited, we can generate the XSWT code for you (<a>help</a>):");
        infoLink.setHoverText("Use comma to separate property names. It is recommended that you use camel-case names. "
                + "By default, a text field will be generated for each variable, but this can be changed by appending a suffix character. "
                + "Use ? for checkbox, % for numeric inputfield (spinner), $ for multi-line text, "
                + "words separated by / for radio buttons (e.g. red/gree/blue), = for slider, "
                + "name:option1/option2/option3 for combo box. "
                + "Widgets can be visually grouped with the grouplabel{...} syntax.");
        infoLink.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1, 1));

        formText = SWTFactory.createStyledText(contentsGroup, SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL, 1, GridData.FILL_BOTH);
        new StyledTextUndoRedoManager(formText);

        SWTFactory.createWrapLabel(contentsGroup, "Help: editField, label\", checkbox?, spinner%, multilineText$, radio1/radio2/radio3, slider=, comboLabel:option1/option2/option3, groupLabel{...}, -- (separator)", 1);
        SWTFactory.createWrapLabel(contentsGroup, "Example: title, errorBars?, orientation:horizontal/vertical, axes { xTitle, yTitle }", 1);

        Dialog.applyDialogFont(dialogArea);

        return composite;
    }

    protected void okPressed() {
        String xswtForm = generate(formText.getText());
        result = new DialogPage(null /* assign later! */, nameText.getText(), xswtForm);
        super.okPressed();
    }

    public DialogPage getResult() {
        return result;
    }

    protected String generate(String spec) {
        String out = "";
        out += "<?xml version='1.0' encoding='UTF-8'?>\n";
        out += "<xswt xmlns:x='http://sweet_swt.sf.net/xswt'>\n";
        out += "\n";
        out += "  <import xmlns='http://sweet_swt.sf.net/xswt'>\n";
        out += "    <package name='java.lang'/>\n";
        out += "    <package name='org.eclipse.swt.widgets' />\n";
        out += "    <package name='org.eclipse.swt.graphics' />\n";
        out += "    <package name='org.eclipse.swt.layout' />\n";
        out += "    <package name='org.eclipse.swt.custom' />\n";
        out += "    <package name='org.omnetpp.common.wizard.support' />\n";
        out += "    <package name='org.omnetpp.ned.editor.wizards.support' />\n";
        out += "    <package name='org.omnetpp.cdt.wizard.support' />\n";
        out += "  </import>\n";
        out += "\n";
        out += "  <layout x:class='GridLayout' numColumns='2'/>\n";
        out += "\n";
        out += "  <x:children>\n";
        out += "\n";
        out += "    <!-- This page was generated from the following spec:\n";
        out += StringUtils.indentLines(spec.trim(), "             ") + "\n";
        out += "    -->\n";
        out += "\n";

        spec = spec.replace("{", "{,").replace("}", ",},").replace(",,", ",");

        for (String token : StringUtils.split(spec, ',')) {
            String field = token.trim();
            out += "    <!-- " + field + " -->\n";
            String rawName = field.replaceFirst("^([-a-zA-Z0-9_ \t]*).*", "$1");
            String suffix = StringUtils.removeStart(field,rawName).trim();
            String label = makeLabel(rawName.trim());
            String name = makeIdentifier(rawName.trim());

            if (field.equals("")) {
                // skip
            }
            else if (rawName.trim().equals("--")) {
                // separator
                out += "    <label x:style='SEPARATOR|HORIZONTAL'>\n";
                out += "      <layoutData x:class='GridData' horizontalSpan='2' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n";
                out += "    </label>\n";
            }
            else if (field.matches("[^:]*/.*")) {
                // radio group
                out += "    <composite>\n";
                out += "      <layoutData x:class='GridData' horizontalSpan='2'/>\n";
                out += "      <layout x:class='GridLayout'/>\n";
                out += "      <x:children>\n";
                for (String j : field.split("/"))
                    out += "        <button x:id='" + makeIdentifier(j.trim()) + "' text='" + makeLabel(j.trim()) + "' x:style='RADIO'/>\n";
                out += "      </x:children>\n";
                out += "    </composite>\n";
            }
            else if (suffix.equals("")) {
                // text
                out += "    <label text='" + label + ":'/>\n";
                out += "    <text x:id='" + name + "' x:style='BORDER'>\n";
                out += "      <layoutData x:class='GridData' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n";
                out += "    </text>\n";
            }
            else if (suffix.equals("\"")) {
                // label
                out += "    <label text='" + label + "'>\n";
                out += "      <layoutData x:class='GridData' horizontalSpan='2' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n";
                out += "    </label>\n";
            }
            else if (suffix.equals("$")) {
                // multiline
                out += "    <label text='" + label + ":'/>\n";
                out += "    <text x:id='" + name + "' x:style='BORDER|WRAP|MULTI'>\n";
                out += "      <layoutData x:class='GridData' heightHint='60' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n";
                out += "    </text>\n";
            }
            else if (suffix.equals("%")) {
                // spinner
                out += "    <label text='" + label + ":'/>\n";
                out += "    <spinner x:id='" + name + "' minimum='0' maximum='100' x:style='BORDER'/>\n";
            }
            else if (suffix.equals("?")) {
                // checkbox
                out += "    <button x:id='" + name + "' text='" + label + "' x:style='CHECK'>\n";
                out += "      <layoutData x:class='GridData' horizontalSpan='2'/>\n";
                out += "    </button>\n";
            }
            else if (suffix.equals("=")) {
                // group scale
                out += "    <label text='" + label + " (0..100):'/>\n";
                out += "    <scale x:id='" + name + "' minimum='0' maximum='100'/>\n";
            }
            else if (suffix.startsWith(":")) {
                // combo
                out += "    <label text='" + label + ":'/>\n";
                out += "    <combo x:id='" + name + "' x:style='BORDER|READ_ONLY'>\n";
                for (String option : suffix.substring(1).split("/")) {
                    out += "      <add x:p0='" + option.trim() + "'/>\n";
                }
                out += "    </combo>\n";
            }
            else if (suffix.equals("{")) {
                // group start
                out += "    <group text='" + label + "'>\n";
                out += "      <layoutData x:class='GridData' horizontalSpan='2' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n";
                out += "      <layout x:class='GridLayout' numColumns='2'/>\n";
                out += "      <x:children>\n";
            }
            else if (suffix.equals("}")) {
                // group end
                out += "      </x:children>\n";
                out += "    </group>\n";
            }
            else {
                // text
                out += "    <label text='" + field.replace("'", "") + ":'/>\n";
                out += "    <text x:id='" + field.replace("'", "") + "' x:style='BORDER'>\n";
                out += "      <layoutData x:class='GridData' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n";
                out += "    </text>\n";
            }
        }
        out += "\n";
        out += "  </x:children>\n";
        out += "</xswt>\n";
        return out;
    }

    private String makeLabel(String rawName) {
        String label = rawName.replace("_", " ").replaceAll("([a-z0-9])([A-Z])", "$1 $2");
        return StringUtils.capitalize(StringUtils.lowerCase(label));
    }

    private String makeIdentifier(String rawName) {
        String tmp = rawName.trim().replace(" ", "_").replaceAll("([a-z0-9])([A-Z])", "$1_$2").toLowerCase();
        return StringUtils.makeValidIdentifier(tmp);
    }

}
