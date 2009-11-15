package org.omnetpp.common.wizard.support;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
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
public class ExternalFileChooser extends AbstractChooser {

    public ExternalFileChooser(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected String browse() {
        FileDialog dialog = new FileDialog(getShell());
        dialog.setFileName(getTextControl().getText());
        return dialog.open();
    }

    @Override
    protected boolean itemExists() {
        String fileName = getTextControl().getText();
        return new File(fileName).exists();
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
        return getTextControl().getText();
    }

    public void setFileName(String file) {
        getTextControl().setText(file);
        getTextControl().selectAll();
    }

    @Override
    public void setValue(Object value) {
        if (value instanceof File) 
            super.setValue(((File)value).getPath());
        else
            super.setValue(value);
    }

}
