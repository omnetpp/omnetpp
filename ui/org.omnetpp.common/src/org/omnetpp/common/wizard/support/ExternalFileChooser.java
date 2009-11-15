package org.omnetpp.common.wizard.support;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for selecting a file from the file system.
 * Implemented as a Composite with a single-line Text and a Browse button.
 * 
 * Does not replicate all methods of the Text class; rather, it exposes the 
 * internal Text widget so that it can be manipulated directly.
 * 
 * @author Andras
 */
public class ExternalFileChooser extends Composite implements IWidgetAdapter {
	private Text text;
	private Button browseButton;
	private Button previewButton;
	private HoverSupport hoverSupport;

	public ExternalFileChooser(Composite parent, int style) {
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
				browse();
			}
		});

		previewButton.addSelectionListener(new SelectionAdapter() {
		    public void widgetSelected(SelectionEvent e) {
		        preview();
		    }
		});

        IHoverTextProvider provider = new IHoverTextProvider() {
            public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
                String contents = getFilePreviewContents();
                String html = (contents == null) ? "<i>Cannot open file</i>" : "<pre>"+StringUtils.quoteForHtml(contents)+"</pre>";
                return HoverSupport.addHTMLStyleSheet(html);
            }
        };
        hoverSupport = new HoverSupport();
        hoverSupport.adapt(text, provider);
	}

	protected void browse() {
        // filesystem selection:		
		FileDialog dialog = new FileDialog(getShell());
		dialog.setFileName(text.getText());
		String result = dialog.open();
		if (result != null) {
			text.setText(result);
	        text.selectAll();
		}
	}

    protected void preview() {
        hoverSupport.makeHoverSticky(text);
    }

    protected String getFilePreviewContents() {
        try {
            String fileName = text.getText();
            File file = new File(fileName);
            if (!file.exists())
                return null;
            DataInputStream in = new DataInputStream(new FileInputStream(file));
            final int PREVIEW_MAX_BYTES = 10000;
            byte[] b = new byte[Math.min(in.available(), PREVIEW_MAX_BYTES)];
            in.readFully(b);
            in.close();
            String contents = new String(b);
            if (b.length == PREVIEW_MAX_BYTES)
                contents += "... [truncated]";
            return contents;
        }
        catch (Exception e) {
            return null;
        }
    }

	public String getFileName() {
		return text.getText();
	}

	public void setFileName(String file) {
		text.setText(file);
        text.selectAll();
	}

	public Text getTextControl() {
		return text;
	}

	public Button getBrowseButton() {
		return browseButton;
	}

	/**
	 * Adapter interface.
	 */
	public Object getValue() {
		return getFileName();
	}

	/**
	 * Adapter interface.
	 */
	public void setValue(Object value) {
		setFileName(value.toString());
	}

}
