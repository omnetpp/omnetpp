package org.omnetpp.common.wizard.support;

import java.io.DataInputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for selecting a file from the workspace.
 * Implemented as a Composite with a single-line Text and a Browse button.
 * 
 * Does not replicate all methods of the Text class; rather, it exposes the 
 * internal Text widget so that it can be manipulated directly.
 * 
 * @author Andras
 */
public class FileChooser extends Composite implements IWidgetAdapter {
	private Text text;
	private Button browseButton;
	private Button previewButton;
	private HoverSupport hoverSupport;

	public FileChooser(Composite parent, int style) {
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
        ElementTreeSelectionDialog dialog = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(), new WorkbenchContentProvider());
        dialog.setTitle("Select File");
        dialog.setMessage("Select file");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        dialog.setAllowMultiple(false);

        // select current file in the dialog (if does not exist, choose first ancestor that does)
        String fileName = text.getText();
        IResource initialSelection = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(fileName));
        if (!initialSelection.exists())
            initialSelection = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(fileName)); // maybe it's a folder
        while (!initialSelection.exists())
            initialSelection = initialSelection.getParent();
        dialog.setInitialSelection(initialSelection);
        
        if (dialog.open() == IDialogConstants.OK_ID) {
        	Object[] result = dialog.getResult();
        	if (result.length > 0) {
        		text.setText(((IResource)result[0]).getFullPath().toString());
                text.selectAll();
        	}
        }
	}

    protected void preview() {
        hoverSupport.makeHoverSticky(text);
    }

    protected String getFilePreviewContents() {
        try {
            String fileName = text.getText();
            IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(fileName));
            if (!file.exists())
                return null;
            DataInputStream in = new DataInputStream(file.getContents(true));
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
	    String fileName = value instanceof IResource ? ((IResource)value).getFullPath().toString() : value.toString();
		setFileName(fileName);
	}

}
