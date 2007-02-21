package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.TextControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.fieldassist.ContentAssistField;
import org.omnetpp.scave.model2.FilterHints;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {
	private Text advancedFilterText;
	private FilterContentProposalProvider proposalProvider;
	private Button filterButton;

	public FilteringPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public Text getFilterText() {
		return advancedFilterText;
	}

	public Button getFilterButton() {
		return filterButton;
	}
	
	public void setFilterHints(FilterHints hints) {
		if (proposalProvider != null)
			proposalProvider.setFilterHints(hints);
	}
	
	public String getFilterPattern() {
		return advancedFilterText.getText();
	}
	
	public void setFilterPattern(String filterText) {
		advancedFilterText.setText(filterText);
	}

	private void initialize() {
		GridLayout gridLayout;
		
		gridLayout = new GridLayout();
		gridLayout.marginHeight = 0;
		this.setLayout(gridLayout);
		
		Composite filterContainer = new Composite(this, SWT.NONE);
		filterContainer.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		gridLayout = new GridLayout(3, false);
		gridLayout.marginHeight = 0;
		filterContainer.setLayout(gridLayout);
		
		final Composite advancedFilterPanel = new Composite(filterContainer, SWT.NONE);
		advancedFilterPanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		advancedFilterPanel.setLayout(new GridLayout(2, false));

		Label filterLabel = new Label(advancedFilterPanel, SWT.NONE);
		filterLabel.setText("Filter:");
		filterLabel.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
		
		final IControlContentAdapter2 contentAdapter = new TextContentAdapter2();
		proposalProvider = new FilterContentProposalProvider();
		ContentAssistField advancedFilter = new ContentAssistField(advancedFilterPanel, SWT.SINGLE | SWT.BORDER, 
				new TextControlCreator(), contentAdapter, proposalProvider, null /*commandId*/, null/*auto-activation*/);
		advancedFilter.getLayoutControl().setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		ContentAssistCommandAdapter adapter = advancedFilter.getContentAssistCommandAdapter();
		advancedFilterText = (Text)adapter.getControl(); 
		adapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
		adapter.addContentProposalListener(new IContentProposalListener() {
			public void proposalAccepted(IContentProposal proposal) {
				FilterContentProposalProvider.ContentProposal filterProposal = (FilterContentProposalProvider.ContentProposal)proposal;
				contentAdapter.replaceControlContents(advancedFilterText, filterProposal.getStartIndex(), filterProposal.getEndIndex(),  filterProposal.getContent(), filterProposal.getCursorPosition());
			}
		});

		filterButton = new Button(filterContainer, SWT.NONE);
		filterButton.setText("Filter");
		filterButton.setLayoutData(new GridData(SWT.CENTER, SWT.TOP, false, false));
	}
}
