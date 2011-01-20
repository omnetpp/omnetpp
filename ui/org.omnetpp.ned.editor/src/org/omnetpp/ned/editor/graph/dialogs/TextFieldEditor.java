package org.omnetpp.ned.editor.graph.dialogs;

import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.util.UIUtils;

/**
 * Represents a text control in the Properties dialog
 * 
 * @author Andras
 */
public class TextFieldEditor implements IFieldEditor {
    private Text text;
    private boolean grayed = false; 
    private ControlDecoration problemDecoration;
    
    public TextFieldEditor(Composite parent, int style) {
        text = new Text(parent, style);
        problemDecoration = new ControlDecoration(text, SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);

        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (isGrayed())
                    setGrayed(false);
            }
        });
    }
    
    public Text getControl() {
        return text;
    }

    public void setContentProposals(final String[] proposals, char[] autoActivationCharacters) {
        ContentProposalProvider proposalProvider = new ContentProposalProvider(true) {
            protected List<IContentProposal> getProposalCandidates(String prefix) {
                return toProposals(proposals);
            }
        };
        setContentProposals(proposalProvider, autoActivationCharacters);
    }

    public void setContentProposals(final Map<String,String> proposalsWithLabels, char[] autoActivationCharacters) {
        ContentProposalProvider proposalProvider = new ContentProposalProvider(true) {
            protected List<IContentProposal> getProposalCandidates(String prefix) {
                return toProposals(proposalsWithLabels);
            }
        };
        setContentProposals(proposalProvider, autoActivationCharacters);
    }

    public void setContentProposals(IContentProposalProvider proposalProvider, char[] autoActivationCharacters) {
        new ContentAssistCommandAdapter(text, new TextContentAdapter(), proposalProvider,
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, autoActivationCharacters, true);
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
        if ((text.getStyle() & SWT.MULTI) == 0)  // preselect text if this is a single-line field 
            text.selectAll();
    }

    public boolean isEnabled() {
        return text.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        if (text.isEnabled() != enabled)
            text.setEnabled(enabled);
    }

    public void setMessage(int severity, String text) {
        UIUtils.updateProblemDecoration(problemDecoration, severity, text);
    }
    
    public void addModifyListener(ModifyListener listener) {
        text.addModifyListener(listener);
    }

    public void removeModifyListener(ModifyListener listener) {
        text.removeModifyListener(listener);
    }
}