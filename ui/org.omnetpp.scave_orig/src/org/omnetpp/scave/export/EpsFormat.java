package org.omnetpp.scave.export;

import org.apache.commons.lang3.math.NumberUtils;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.IDialogConstants;

public class EpsFormat implements IGraphicalExportFileFormat, Cloneable {

    private double width;
    private double height;

    public EpsFormat() {
        width = 8.0;
        height = 6.0;
    }

    public String getDescription() {
        return "Encapsulated Postscript (EPS)";
    }

    public String getName() {
        return "eps";
    }

    public String getFileExtension() {
        return "eps";
    }

    public double getWidth() {
        return width;
    }

    public double getHeight() {
        return height;
    }

    public void openOptionsDialog(Shell parentShell) {
        Dialog dialog = new OptionsDialog(parentShell);
        dialog.open();
    }

    public IGraphicalExportFileFormat clone() {
        try {
            return (IGraphicalExportFileFormat)super.clone();
        } catch (CloneNotSupportedException e) {
            return null;
        }
    }

    private class OptionsDialog extends Dialog {

        Text widthText;
        Text heightText;

        protected OptionsDialog(Shell parentShell) {
            super(parentShell);
        }

        @Override
        protected void configureShell(Shell newShell) {
            super.configureShell(newShell);
            newShell.setText("EPS Export Options");
        }

        @Override
        protected Control createDialogArea(Composite parent) {
            Composite composite = (Composite)super.createDialogArea(parent);
            composite.setLayout(new GridLayout(2, false));
            Label label = new Label(composite, SWT.NONE);
            label.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false, 2, 1));
            label.setText("Image size in inches:");
            label = new Label(composite, SWT.NONE);
            label.setText("Width:");
            widthText = new Text(composite, SWT.BORDER);
            widthText.setText(String.valueOf(getWidth()));
            widthText.setLayoutData(new GridData(SWT.FILL, SWT.TOP, false, false));
            widthText.addModifyListener(validator);
            label = new Label(composite, SWT.NONE);
            label.setText("Height:");
            heightText = new Text(composite, SWT.BORDER);
            heightText.setText(String.valueOf(getHeight()));
            heightText.addModifyListener(validator);
            heightText.setLayoutData(new GridData(SWT.FILL, SWT.TOP, false, false));
            restoreDialogSettings();
            return composite;
        }

        private ModifyListener validator = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                Button okButton = getButton(IDialogConstants.OK_ID);
                if (okButton != null)
                    okButton.setEnabled(getValue(widthText)>0.0 && getValue(heightText)>0.0);
            }
        };

        @Override
        protected void okPressed() {
            width = getValue(widthText);
            height = getValue(heightText);
            saveDialogSettings();
            super.okPressed();
        }

        private double getValue(Text text) {
            double value = NumberUtils.toDouble(text.getText(), 0.0);
            return value > 0.0 ? value : 0.0;
        }

        private void saveDialogSettings() {
            IDialogSettings settings = UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
            settings.put("width", widthText.getText());
            settings.put("height", heightText.getText());
        }

        private void restoreDialogSettings() {
            IDialogSettings settings = UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
            String width = settings.get("width");
            String height = settings.get("height");
            if (width != null)
                widthText.setText(width);
            if (height != null)
                heightText.setText(height);
        }
    }
}
