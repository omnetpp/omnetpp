package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.FieldDecoration;
import org.eclipse.jface.fieldassist.FieldDecorationRegistry;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.scave.model2.FilterHints;

public class FilterField {
	
	Text text;
	FilterContentProposalProvider proposalProvider;

	public FilterField(Composite parent, int style) {
		this(new Text(parent, style));
	}
	
	public FilterField(Text text) {
		this.text = text;
		this.proposalProvider = new FilterContentProposalProvider();
		FieldDecoration contentAssistDecoration = FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_CONTENT_PROPOSAL);
		ControlDecoration decorator = new ControlDecoration(text, SWT.TOP | SWT.LEFT, text.getParent());
		decorator.setImage(contentAssistDecoration.getImage());
		decorator.setDescriptionText(contentAssistDecoration.getDescription());
		ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(text,
				new TextContentAdapter2(),
				proposalProvider,
				ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,   /*commandId. "null" works equally well. XXX no binding is found for the default command "org.eclipse.ui.edit.text.contentAssist.proposals", that's why it says "null" in the bubble. how to fix it? */
				"( ".toCharArray() /*auto-activation*/);
		
		
		final IControlContentAdapter2 contentAdapter = (IControlContentAdapter2)commandAdapter.getControlContentAdapter();

		commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
		commandAdapter.addContentProposalListener(new IContentProposalListener() {
			public void proposalAccepted(IContentProposal proposal) {
				ContentProposal filterProposal = (ContentProposal)proposal;
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
		return text;
	}
	
	public Control getLayoutControl() {
		return text;
	}
	
	public void setFilterHints(FilterHints hints) {
		proposalProvider.setFilterHints(hints);
	}
}
