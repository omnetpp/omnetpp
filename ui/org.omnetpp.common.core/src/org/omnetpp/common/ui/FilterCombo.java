package org.omnetpp.common.ui;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.Debug;

/**
 * Custom combo that accepts a content proposal provider.
 *
 * @author Andras
 */
public class FilterCombo extends CustomCombo {
    private IContentProposalProvider contentProposalProvider;

    public FilterCombo(Composite parent, int style) {
        super(parent, style | SWT.SEARCH | SWT.ICON_CANCEL);
    }

    @Override
    protected void checkSubclass() {
        // allow subclassing
    }

    public void setContentproposalProvider(IContentProposalProvider contentProposalProvider) {
        this.contentProposalProvider = contentProposalProvider;
    }

    public IContentProposalProvider getContentproposalProvider() {
        return contentProposalProvider;
    }

    @Override
    protected void onOpen() {
        if (contentProposalProvider == null)
            return; // normal operation

        Text textControl = getTextControl();
        String contents = textControl.getText();
        int position = textControl.getCaretPosition();
        removeAll();
        setText(contents);
        IContentProposal[] proposals = Debug.timed("FilterCombo: computing proposals",  10,  () -> {
            return contentProposalProvider.getProposals(contents, position); //TODO limit number of proposals? "[left out 2667 more]"
        });
        Debug.time("FilterCombo: adding proposals", 10, () -> {
            for (IContentProposal proposal : proposals)
                add(proposal.getContent(), null);
        });
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + " {" + getMessage() + "}";
    }
}
