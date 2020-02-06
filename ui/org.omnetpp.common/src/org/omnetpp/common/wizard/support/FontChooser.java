package org.omnetpp.common.wizard.support;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FontDialog;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.wizard.IWidgetAdapter;

public class FontChooser extends Composite implements IWidgetAdapter {
    private Text text;

    public FontChooser(Composite parent, int style) {
        super(parent, SWT.NONE);

        setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        GridLayout layout = new GridLayout(2, false);
        layout.marginWidth = 0;
        layout.marginHeight = 0;
        setLayout(layout);
        text = new Text(this, style);
        text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        Button button = new Button(this, SWT.NONE);
        button.setText("...");
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent event) {
                if (!text.isDisposed()) {
                    FontDialog dialog = new FontDialog(text.getShell());

                    FontData font = Converter.tolerantStringToOptionalFontdata(text.getText());
                    if (font != null)
                        dialog.setFontList(new FontData[] {font});
                    font = dialog.open();
                    if (font != null)
                        text.setText(Converter.fontdataToString(font));
                }
            }
        });
    }

    @Override
    public Object getValue() {
        return text.getText();
    }

    @Override
    public void setValue(Object value) {
        text.setText(value.toString());
    }
}

