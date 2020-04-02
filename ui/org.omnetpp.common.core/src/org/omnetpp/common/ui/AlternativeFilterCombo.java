package org.omnetpp.common.ui;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.Debug;

/**
 * Custom combo that accepts a content proposal provider.
 *
 * @author Andras
 */
public class AlternativeFilterCombo extends CustomCombo {
    private IContentProposalProvider contentProposalProvider;

    public AlternativeFilterCombo(Composite parent, int style) {
        super(parent, style | SWT.SEARCH | SWT.ICON_CANCEL);

        getTextControl().addListener(SWT.DefaultSelection, (e) -> {
            if (e.detail == SWT.ICON_CANCEL)
                Display.getCurrent().asyncExec(() -> getTextControl().notifyListeners(SWT.DefaultSelection, new Event()));
        });
    }

    public void setContentProposalProvider(IContentProposalProvider contentProposalProvider) {
        this.contentProposalProvider = contentProposalProvider;
    }

    public IContentProposalProvider getContentProposalProvider() {
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
            return contentProposalProvider.getProposals(contents, position);
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
