package org.omnetpp.common.wizard.support;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.ColorDialog;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;

public class ColorChooser extends Composite implements IWidgetAdapter {
    private Text text;

    protected static class ColorEdit {
        Text text;
        Label label;

        public ColorEdit(final Text text, final Label label) {
            this.text = text;
            this.label = label;
            updateImageLabel();
            text.addModifyListener(new ModifyListener() {
                public void modifyText(ModifyEvent e) {
                    updateImageLabel();
                }
            });
        }

        public String getText() {
            return StringUtils.trimToEmpty(text.getText());
        }

        public void setText(String color) {
            text.setText(color);
            updateImageLabel();
        }

        private void updateImageLabel() {
            Image image = ColorFactory.createColorImage(ColorFactory.asRGB(text.getText()));
            Image oldImage = label.getImage();
            label.setImage(image);
            if (oldImage != null)
                oldImage.dispose();
        }
    }

    public ColorChooser(Composite parent, int style) {
        super(parent, SWT.NONE);

        setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        GridLayout layout = new GridLayout(3, false);
        layout.marginWidth = 0;
        layout.marginHeight = 0;
        setLayout(layout);
        Label imageLabel = new Label(this, SWT.NONE);
        imageLabel.setLayoutData(new GridData(16,16));
        text = new Text(this, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        /*new ContentAssistCommandAdapter(text, new TextContentAdapter(), new ColorContentProposalProvider(),
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);*/
        Button button = new Button(this, SWT.NONE);
        button.setText("...");


        final ColorEdit colorField = new ColorEdit(text, imageLabel);
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                if (!colorField.text.isDisposed()) {
                    ColorDialog dialog = new ColorDialog(colorField.text.getShell());
                    dialog.setText(colorField.getText());
                    RGB rgb = dialog.open();
                    if (rgb != null) {
                        colorField.setText(ColorFactory.asString(rgb));
                    }
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

