package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave2.editors.ScaveEditor;

public class DatasetsAndChartsPage extends ScrolledForm {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section datasetsSection = null;
	private Section chartSheetsSection = null;
	private ScaveEditor scaveEditor = null;  // the containing editor

	public DatasetsAndChartsPage(Composite parent, int style, ScaveEditor scaveEditor) {
		super(parent, style | SWT.V_SCROLL);
		this.scaveEditor = scaveEditor;
		initialize();
	}
	
	public TreeViewer getDatasetsTreeViewer() {
		DatasetsPanel panel = (DatasetsPanel)datasetsSection.getClient();
		return panel.getTreeViewer();
	}
	
	public TreeViewer getChartSheetsTreeViewer() {
		ChartSheetsPanel panel = (ChartSheetsPanel)chartSheetsSection.getClient();
		return panel.getTreeViewer();
	}

	private void initialize() {
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		createDatasetsSection();
		createChartSheetsSection();
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null) {
			formToolkit = new FormToolkit(Display.getCurrent());
		}
		return formToolkit;
	}

	/**
	 * This method initializes datasetsSection	
	 */
	private void createDatasetsSection() {
		GridData gridData2 = new GridData();
		gridData2.horizontalAlignment = GridData.FILL;
		gridData2.verticalAlignment = GridData.FILL;
		gridData2.grabExcessHorizontalSpace = true;
		gridData2.grabExcessVerticalSpace = true;
		datasetsSection = getFormToolkit().createSection(
				getBody(),
				Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		datasetsSection.setExpanded(true);
		datasetsSection.setLayoutData(gridData2);
		datasetsSection.setText("Datasets");
		datasetsSection.setDescription("Here you can browse the datasets you have created from the input.");
		final DatasetsPanel datasetsPanel = new DatasetsPanel(datasetsSection, SWT.NONE);
		datasetsSection.setClient(datasetsPanel);

		final TreeViewer treeViewer = datasetsPanel.getTreeViewer();
		//TODO buttons
	}

	/**
	 * This method initializes chartSheetsSection	
	 */
	private void createChartSheetsSection() {
		GridData gridData3 = new GridData();
		gridData3.horizontalAlignment = GridData.FILL;
		gridData3.verticalAlignment = GridData.FILL;
		gridData3.grabExcessHorizontalSpace = true;
		chartSheetsSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		chartSheetsSection.setExpanded(true);
		chartSheetsSection.setLayoutData(gridData3);
		chartSheetsSection.setText("Chart sheets and charts");
		chartSheetsSection.setDescription("Here you can browse the charts you have created for the datasets.");
		ChartSheetsPanel chartSheetsPanel = new ChartSheetsPanel(chartSheetsSection, SWT.NONE); 
		chartSheetsSection.setClient(chartSheetsPanel);

		final TreeViewer treeViewer = chartSheetsPanel.getTreeViewer();
		//TODO buttons
	}
}
