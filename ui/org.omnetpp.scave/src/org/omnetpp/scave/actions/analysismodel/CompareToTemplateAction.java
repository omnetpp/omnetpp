/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.swt.custom.StyledText;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Resets either the dialog pages, or the script of a chart to its template.
 */
public class CompareToTemplateAction extends AbstractScaveAction {

    class ComparisonReportDialog extends Dialog {
        private String content;
        private StyledText styledText;

        public ComparisonReportDialog(Shell parentShell) {
            super(parentShell);
            setShellStyle(SWT.RESIZE | getShellStyle());
        }

        @Override
        protected void configureShell(Shell shell) {
            super.configureShell(shell);
            shell.setText("Comparison Results");
        }

        @Override
        protected Control createDialogArea(Composite parent) {
            Composite composite = (Composite) super.createDialogArea(parent);
            composite.setLayout(new GridLayout(1, false));

            styledText = new StyledText(composite, SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL | SWT.READ_ONLY);
            styledText.setLayoutData(new GridData(GridData.FILL_BOTH));
            styledText.setText(content);

            return composite;
        }

        @Override
        protected void createButtonsForButtonBar(Composite parent) {
            Button okButton = createButton(parent, OK, "OK", true);
            okButton.addSelectionListener(SelectionListener.widgetSelectedAdapter(e -> {
                setReturnCode(OK);
                close();
            }));
        }

        public void setContent(String content) {
            this.content = content;
        }

        @Override
        protected IDialogSettings getDialogBoundsSettings() {
            return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
        }
    }


    public CompareToTemplateAction() {
        setText("Compare to Template");
        setDescription("Compares the script and the dialog of a chart to those of its template");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_CLEAR));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        List<Chart> charts = ScaveModelUtil.getChartsFromSelection(asStructuredOrEmpty(selection));

        StringBuilder builder = new StringBuilder();
        for (Chart chart : charts) {
            ChartTemplate template = scaveEditor.getChartTemplateRegistry().findTemplateByID(chart.getTemplateID());
            if (template == null) {
                if (!builder.isEmpty())
                    builder.append("\n" + StringUtils.repeat("-", 100) + "\n\n");
                builder.append("Chart " + chart.getName() + ": Chart template `" + chart.getTemplateID() + "` is not available.");
            }
            else {
                String report = ScaveModelUtil.compareChartToTemplate(chart, template);
                if (!report.isBlank()) {
                    if (!builder.isEmpty())
                        builder.append("\n" + StringUtils.repeat("-", 100) + "\n\n");
                    builder.append("Chart \"" + chart.getName() + "\": Differences from template `" + chart.getTemplateID() + "` found, see details below.\n\n");
                    builder.append(report);
                }
            }
        }

        String report = builder.toString();
        if (report.isEmpty()) {
            MessageDialog.openInformation(Display.getCurrent().getActiveShell(), "Comparison Results", "No differences found.");
        }
        else {
            ComparisonReportDialog dialog = new ComparisonReportDialog(Display.getCurrent().getActiveShell());
            dialog.setContent(report);
            dialog.open();
        }

    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return !ScaveModelUtil.getChartsFromSelection(asStructuredOrEmpty(selection)).isEmpty();
    }
}
