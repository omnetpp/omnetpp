package org.omnetpp.ned.editor.graph.dialogs;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.image.ImageSelectionDialog;
import org.omnetpp.common.properties.ImageCellEditor;
import org.omnetpp.common.util.StringUtils;

/**
 * Represents an image selection widget in the Properties dialog
 * 
 * @author Andras
 */
public class ImageFieldEditor implements IFieldEditor {
    private Composite composite;
    private Text text;
    private Button button;
    private boolean grayed = false; 
    private String defaultFilter = ""; // dialog filter to use if text is empty
    private ControlDecoration problemDecoration;
    
    public ImageFieldEditor(Composite parent, int style) {
        composite = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(2, false);
        composite.setLayout(layout);
        layout.horizontalSpacing = layout.verticalSpacing = layout.marginHeight = layout.marginWidth = 0; 

        text = new Text(composite, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        ((GridData)text.getLayoutData()).widthHint = 20 * PropertiesDialog.computeDialogFontAverageCharWidth(composite);
        new ContentAssistCommandAdapter(text, new TextContentAdapter(), 
                new ImageCellEditor.ImageContentProposalProvider(),
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, "/".toCharArray(), true);
        problemDecoration = new ControlDecoration(text, SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);
        
        button = new Button(composite, SWT.PUSH);
        button.setText("...");
        GridData gridData = new GridData(SWT.CENTER, SWT.CENTER, false, false);
        int textHeight = text.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        gridData.heightHint = textHeight;
        button.setLayoutData(gridData);

        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (isGrayed())
                    setGrayed(false);
            }
        });
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                String initialValue = StringUtils.defaultIfEmpty(text.getText(), defaultFilter);
                ImageSelectionDialog dialog = new ImageSelectionDialog(composite.getShell(), initialValue);
                if (dialog.open() == Dialog.OK) 
                    text.setText(dialog.getImageId());
            }
        });
    }
    
    public void setDefaultFilter(String defaultFilter) {
        this.defaultFilter = defaultFilter;
    }
    
    public Composite getControl() {
        return composite;
    }

    public boolean isGrayed() {
        return grayed;
    }

    public void setGrayed(boolean grayed) {
        if (grayed) {
            text.setText("");
            text.setBackground(GREY);
        } 
        else {
            text.setBackground(null); // default system color
        }
        this.grayed = grayed; // note: must be AFTER text.setText()
    }

    public String getText() {
        return text.getText();
    }

    public void setText(String content) {
        text.setText(content);
        text.selectAll();
    }

    public boolean isEnabled() {
        return text.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        if (text.isEnabled() != enabled) {
            text.setEnabled(enabled);
            button.setEnabled(enabled);
        }
    }

    public void setMessage(int severity, String text) {
        PropertiesDialog.updateProblemDecoration(problemDecoration, severity, text);
    }
    
    public void addModifyListener(ModifyListener listener) {
        text.addModifyListener(listener);
    }

    public void removeModifyListener(ModifyListener listener) {
        text.removeModifyListener(listener);
    }
}