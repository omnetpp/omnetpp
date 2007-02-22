package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.TextControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.fieldassist.ContentAssistField;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {

	// Switch between "Simple" and "Advanced"
	private Button toggleFilterTypeButton;
	private boolean showingAdvancedFilter;

	// Edit field for the "Advanced" mode
	private Composite advancedFilterPanel;
	private Text advancedFilterText;
	private FilterContentProposalProvider proposalProvider;

	// Combo boxes for the "Simple" mode
	private SashForm simpleFilterPanel;
	private CCombo runCombo;
	private CCombo moduleCombo;
	private CCombo dataCombo;

	// The "Go" button
	private Button filterButton;

	public FilteringPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}

	public Text getAdvancedFilterText() {
		return advancedFilterText;
	}

	public CCombo getModuleNameCombo() {
		return moduleCombo;
	}

	public CCombo getNameCombo() {
		return dataCombo;
	}

	public CCombo getRunNameCombo() {
		return runCombo;
	}

	public Button getFilterButton() {
		return filterButton;
	}

	public Button getToggleFilterTypeButton() {
		return toggleFilterTypeButton;
	}

	public void setFilterHints(FilterHints hints) {
		runCombo.setItems(hints.getHints(FilterUtil.FIELD_RUNNAME));
		moduleCombo.setItems(hints.getHints(FilterUtil.FIELD_MODULENAME));
		dataCombo.setItems(hints.getHints(FilterUtil.FIELD_DATANAME));
		proposalProvider.setFilterHints(hints);
	}

	public void showSimpleFilter() {
		setVisible(advancedFilterPanel, false);
		setVisible(simpleFilterPanel, true);
		showingAdvancedFilter = false;
		toggleFilterTypeButton.setText("Advanced");
		getParent().layout(true, true);
	}

	public void showAdvancedFilter() {
		setVisible(simpleFilterPanel, false);
		setVisible(advancedFilterPanel, true);
		showingAdvancedFilter = true;
		toggleFilterTypeButton.setText("Basic");
		getParent().layout(true, true);
	}

	public boolean isShowingAdvancedFilter() {
		return showingAdvancedFilter;
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

		advancedFilterPanel = new Composite(filterContainer, SWT.NONE);
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

		simpleFilterPanel = new SashForm(filterContainer, SWT.SMOOTH);
		simpleFilterPanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		setVisible(simpleFilterPanel, false);
		runCombo = createComboWithLabel(simpleFilterPanel, "Run ID:");
		moduleCombo = createComboWithLabel(simpleFilterPanel, "Module:");
		dataCombo = createComboWithLabel(simpleFilterPanel, "Name:");

		filterButton = new Button(filterContainer, SWT.NONE);
		filterButton.setText("Filter");
		filterButton.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));

		toggleFilterTypeButton = new Button(filterContainer, SWT.PUSH);
		toggleFilterTypeButton.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));

		showSimpleFilter();
	}

	private CCombo createComboWithLabel(Composite parent, String text) {
		Composite composite = new Composite(parent, SWT.NONE);
		composite.setLayout(new GridLayout(2, false));

		Label label = new Label(composite, SWT.NONE);
		label.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
		label.setText(text);

		CCombo combo = new CCombo(composite, SWT.BORDER);
		combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		combo.setVisibleItemCount(20);

		return combo;
	}

	private static void setVisible(Control control, boolean visible) {
		control.setVisible(visible);
		((GridData)control.getLayoutData()).exclude = !visible;
	}
}
