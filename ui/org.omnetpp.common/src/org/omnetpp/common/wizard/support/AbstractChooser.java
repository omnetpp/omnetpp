package org.omnetpp.common.wizard.support;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * Abstract base class for various "chooser" widgets. The chooser contains a
 * single-line Text control, plus a Browse and a Preview button.
 * It supports validation and hover support as well.
 *
 * Does not replicate all methods of the Text class; rather, it exposes the
 * internal Text widget so that it can be manipulated directly.
 *
 * @author Andras
 */
public abstract class AbstractChooser extends Composite implements IWidgetAdapter {
    private Text text;
    private Button browseButton;
    private Button previewButton;
    private HoverSupport hoverSupport;

    public AbstractChooser(Composite parent, int style) {
        super(parent, style);
        GridLayout layout = new GridLayout(3,false);
        layout.marginHeight = layout.marginWidth = 0;
        setLayout(layout);

        text = new Text(this, SWT.SINGLE|SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
        browseButton = new Button(this, SWT.PUSH);
        browseButton.setText("Browse...");
        previewButton = new Button(this, SWT.PUSH);
        previewButton.setText("Preview");

        browseButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                String newText = browse();
                if (newText != null) {
                    text.setText(newText);
                    text.selectAll();
                    text.setFocus();
                }
            }
        });

        previewButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                preview();
            }
        });

        hoverSupport = new HoverSupport();
        hoverSupport.adapt(text, new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                return new HtmlHoverInfo(getHoverText(x, y));
            }
        });

        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                textModified();
            }
        });

        textModified();  // update Preview button state
    }

    protected void preview() {
        hoverSupport.makeHoverSticky(text);
    }

    protected void textModified() {
        boolean exists = itemExists();
        if (previewButton.isEnabled() != exists)
            previewButton.setEnabled(exists);
    }

    /**
     * Return whether the item named in the text widget exists. Enables/disables preview.
     */
    protected abstract boolean itemExists();

    /**
     * Bring up a chooser dialog, and return the result as string which will
     * replace the contents of the text widget. Return null if the user cancelled.
     */
    protected abstract String browse();

    /**
     * Used for hover and preview.
     */
    protected abstract String getHoverText(int x, int y);

    public Text getTextControl() {
        return text;
    }

    public Button getBrowseButton() {
        return browseButton;
    }

    public Button getPreviewButton() {
        return previewButton;
    }

    @Override
    public boolean setFocus() {
        return getTextControl().setFocus();
    }

    public String getText() {
        return getTextControl().getText();
    }

    public void setText(String value) {
        getTextControl().setText(value);
        getTextControl().selectAll();
    }

    /**
     * Adapter interface.
     */
    public Object getValue() {
        return getText();
    }

    /**
     * Adapter interface.
     */
    public void setValue(Object value) {
        setText(value.toString());
        textModified();
    }

}
