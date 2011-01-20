package org.omnetpp.ned.editor.graph.dialogs;

import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.fieldassist.ComboContentAdapter;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.util.UIUtils;

/**
 * Represents a combo widget in the Properties dialog.
 * 
 * Note: we always create an editable combo, because non-editable
 * ones cannot display the grayed state (at least on Windows 7)
 * 
 * @author Andras
 */
public class ComboFieldEditor implements IFieldEditor {
    private Combo combo;
    private boolean grayed = false; 
    private ControlDecoration problemDecoration;
    private String[] contentAssistLabels;

    public ComboFieldEditor(Composite parent, int style) {
        combo = new Combo(parent, style);
        problemDecoration = new ControlDecoration(combo, SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);
        combo.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (isGrayed())
                    setGrayed(false);
            }
        });
        
        new ContentAssistCommandAdapter(combo, new ComboContentAdapter(), 
                new ContentProposalProvider(true) {
                    protected List<IContentProposal> getProposalCandidates(String prefix) {
                        if (contentAssistLabels == null)
                            return toProposals(combo.getItems());
                        else
                            return toProposals(combo.getItems(), contentAssistLabels);
                    }
                },
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);
    }
    
    public Combo getControl() {
        return combo;
    }

    public boolean isGrayed() {
        return grayed;
    }

    public void setGrayed(boolean grayed) {
        if (grayed) {
            combo.setBackground(GREY);
            combo.setText("");
        } 
        else {
            combo.setBackground(null); // default system color
        }
        this.grayed = grayed; // note: must be AFTER text.setText()
    }

    public String getText() {
        return combo.getText();
    }

    public void setText(String text) {
        combo.setText(text);
        combo.setSelection(new Point(0,text.length()));
    }

    public boolean isEnabled() {
        return combo.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        if (combo.isEnabled() != enabled)
            combo.setEnabled(enabled);
    }

    public void setMessage(int severity, String text) {
        UIUtils.updateProblemDecoration(problemDecoration, severity, text);
    }
    
    public String[] getItems() {
        return combo.getItems();
    }

    public void setItems(String[] items) {
        setItems(items, null);
    }

    public void setItems(String[] items, String[] contentAssistLabels) {
        if (!Arrays.equals(items, combo.getItems())) {
            boolean wasGrayed = isGrayed();
            String save = combo.getText();

            combo.setItems(items);
            
            // restore everything that setItems() may have messed up 
            if (!save.equals(combo.getText()))
                combo.setText(save);
            if (wasGrayed != isGrayed())
                setGrayed(wasGrayed);
        }
        this.contentAssistLabels = contentAssistLabels;
    }
    
    public void addModifyListener(ModifyListener listener) {
        combo.addModifyListener(listener);
    }

    public void removeModifyListener(ModifyListener listener) {
        combo.removeModifyListener(listener);
    }
}