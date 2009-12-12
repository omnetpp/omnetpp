package org.omnetpp.common.wizard.support;

import java.io.DataInputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;

/**
 * A control for selecting a file from the workspace.
 * Implemented as a Composite with a single-line Text and a Browse button.
 *
 * Does not replicate all methods of the Text class; rather, it exposes the
 * internal Text widget so that it can be manipulated directly.
 *
 * @author Andras
 */
public class FileChooser extends AbstractChooser {

    public FileChooser(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected String browse() {
        ElementTreeSelectionDialog dialog = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(), new WorkbenchContentProvider());
        dialog.setTitle("Select File");
        dialog.setMessage("Select file");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        dialog.setAllowMultiple(false);

        // select current file in the dialog (if does not exist, choose first ancestor that does)
        String fileName = getTextControl().getText().trim();
        if (!fileName.equals("")) {
            try {
                IResource initialSelection = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(fileName));
                if (!initialSelection.exists())
                    initialSelection = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(fileName)); // maybe it's a folder
                while (!initialSelection.exists())
                    initialSelection = initialSelection.getParent();
                dialog.setInitialSelection(initialSelection);
            }
            catch (IllegalArgumentException e) { } // on bad file name syntax
        }

        if (dialog.open() == IDialogConstants.OK_ID) {
            Object[] result = dialog.getResult();
            if (result.length > 0)
                return ((IResource)result[0]).getFullPath().toString();
        }
        return null;
    }

    @Override
    protected boolean itemExists() {
        String fileName = getTextControl().getText().trim();
        if (fileName.equals(""))
            return false;
        try {
            IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(fileName));
            return file.exists();
        }
        catch (IllegalArgumentException e) {
            return false; // on bad file name syntax
        }
    }

    @Override
    protected String getHoverText(int x, int y, SizeConstraint outSizeConstraint) {
        if (getText().isEmpty())
            return null;
        String contents = getFilePreviewContents();
        String html = (contents == null) ? "<i>Cannot open file</i>" : "<pre>"+StringUtils.quoteForHtml(contents)+"</pre>";
        return HoverSupport.addHTMLStyleSheet(html);
    }

    protected String getFilePreviewContents() {
        try {
            String fileName = getTextControl().getText();
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
        return getText();
    }

    public void setFileName(String file) {
        setText(file);
    }

    @Override
    public void setValue(Object value) {
        if (value instanceof IResource)
            super.setValue(((IResource)value).getFullPath().toString());
        else
            super.setValue(value);
    }

}
