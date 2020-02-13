/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.List;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.swt.custom.StyledText;
import org.omnetpp.common.util.HTMLUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model2.ScaveModelUtil;


public class NewChartFromTemplateDialog extends TitleAreaDialog {
    private String title;
    private ScaveEditor editor;
    private TableViewer tableViewer;

    // the result
    private ChartTemplate selectedTemplate;
    private StyledText styledText;
    private SashForm sashForm;

    public class ChartTemplateLabelProvider extends LabelProvider {
        @Override
        public Image getImage(Object element) {
            ChartTemplate template = (ChartTemplate)element;
            String iconPath = template.getIconPath();
            if (iconPath != null)
                return ScavePlugin.getCachedImage(iconPath);
            return null;
        }

        @Override
        public String getText(Object element) {
            ChartTemplate template = (ChartTemplate)element;
            return template.getName();
        }

    };

    /**
     * Creates the dialog.
     */
    public NewChartFromTemplateDialog(Shell parentShell, ScaveEditor editor) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Select Chart Template";
        this.editor = editor;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
            shell.setText(title);
    }

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    protected Control createDialogArea(Composite parent) {
        setTitle("Select Chart Template");
        setMessage("Select template for the new chart");

        // page group
        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout());

        createLabel(composite, "Available templates:");

        sashForm = new SashForm(composite, SWT.NONE);
        sashForm.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        sashForm.setLayout(new GridLayout(2,false));

        // content area
        tableViewer = new TableViewer(sashForm, SWT.BORDER);
        GridData data = new GridData(GridData.FILL_BOTH);
        data.widthHint = 200;
        tableViewer.getTable().setLayoutData(data);

        tableViewer.setContentProvider(new ArrayContentProvider());
        tableViewer.setLabelProvider(new ChartTemplateLabelProvider());
        tableViewer.setInput(editor.getChartTemplateRegistry().getAllTemplates());

        styledText = new StyledText(sashForm, SWT.BORDER|SWT.READ_ONLY|SWT.DOUBLE_BUFFERED);
        GridData data2 = new GridData(GridData.FILL_BOTH);
        styledText.setLayoutData(data2);

        tableViewer.addPostSelectionChangedListener(new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                ChartTemplate template = getTableSelection();
                if (template != null) {
                    String html = getDescriptionAsHtml(template);
                    styledText.setText("");
                    HTMLUtils.htmlToStyledText(html, styledText, null);
                    styledText.replaceTextRange(0, 2, ""); // KLUDGE: remove blank line at top (which got there how...?)
                }
            }
        });

        Dialog.applyDialogFont(sashForm);

        tableViewer.refresh();

        String w0 = getDialogBoundsSettings().get("sash.weight0");
        String w1 = getDialogBoundsSettings().get("sash.weight1");
        if (StringUtils.isNumeric(w0) && StringUtils.isNumeric(w1))
            sashForm.setWeights(new int[] {Integer.parseInt(w0), Integer.parseInt(w1)});

        return composite;
    }

    protected String getDescriptionAsHtml(ChartTemplate template) {
        String html = "";

        html += "<p><small>";
        html += "Template ID: " + template.getId() + " / ";
        html += "type: " + template.getChartType() + " / ";
        List<String> supportedResultTypes = ScaveModelUtil.getTemplateSupportedResultTypesAsString(template);
        html += "supports: " + StringUtils.defaultIfEmpty(StringUtils.join(supportedResultTypes, ","), "-");
        html += "</small></p>\n";

        html += "<h2>" + template.getName() + "</h2>\n";

        html += StringUtils.nullToEmpty(template.getDescription());
        return html;
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        label.setText(text);
        return label;
    }

    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }

    protected ChartTemplate getTableSelection() {
        Object firstElement = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
        if (firstElement instanceof ChartTemplate)
            return (ChartTemplate)firstElement;
        return null;
    }

    @Override
    public boolean close() {
        int w0 = sashForm.getChildren()[0].getBounds().width;
        int w1 = sashForm.getChildren()[1].getBounds().width;
        getDialogBoundsSettings().put("sash.weight0", Integer.toString(w0));
        getDialogBoundsSettings().put("sash.weight1", Integer.toString(w1));

        return super.close();
    }

    protected void okPressed() {
        selectedTemplate = getTableSelection();
        super.okPressed();
    }

    /**
     * Returns the section to insert the keys into.
     */
    public ChartTemplate getSelectedTemplate() {
        return selectedTemplate;
    }
}
