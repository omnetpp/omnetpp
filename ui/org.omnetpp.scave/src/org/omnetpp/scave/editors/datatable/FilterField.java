package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextControlCreator;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.fieldassist.ContentAssistField;
import org.omnetpp.scave.model2.FilterHints;

public class FilterField extends ContentAssistField {

	public FilterField(Composite parent, int style) {
		super(parent, style,
				new TextControlCreator(),
				new TextContentAdapter2(),
				new FilterContentProposalProvider(),
				null,  /*commandId. XXX no binding is found for the default command "org.eclipse.ui.edit.text.contentAssist.proposals", that's why it says "null" in the bubble. how to fix it? */
				"( ".toCharArray() /*auto-activation*/);
		
		ContentAssistCommandAdapter commandAdapter = getContentAssistCommandAdapter();
		final IControlContentAdapter2 contentAdapter = (IControlContentAdapter2)commandAdapter.getControlContentAdapter();
		commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
		commandAdapter.addContentProposalListener(new IContentProposalListener() {
			public void proposalAccepted(IContentProposal proposal) {
				FilterContentProposalProvider.ContentProposal filterProposal = (FilterContentProposalProvider.ContentProposal)proposal;
				contentAdapter.replaceControlContents(
						getText(),
						filterProposal.getStartIndex(),
						filterProposal.getEndIndex(),
						filterProposal.getContent(),
						filterProposal.getCursorPosition());
			}
		});
		
	}
	
	public Text getText() {
		return (Text)getContentAssistCommandAdapter().getControl();
	}
	
	public void setFilterHints(FilterHints hints) {
		IContentProposalProvider provider = getContentAssistCommandAdapter().getContentProposalProvider();
		if (provider instanceof FilterContentProposalProvider)
			((FilterContentProposalProvider)provider).setFilterHints(hints);
	}
}
