package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.layout.FillLayout;

public class OverviewPage extends ScrolledForm {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section inputFilesSection = null;
	private Section dataSection = null;
	private Section datasetsSection = null;
	private Section chartSheetsSection = null;

	public OverviewPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL);
		initialize();
	}

	private void initialize() {
		setExpandHorizontal(true);
		getBody().setLayout(new GridLayout());
		createInputFilesSection();
		createDataSection();
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
	 * This method initializes inputFilesSection	
	 *
	 */
	private void createInputFilesSection() {
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		inputFilesSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		inputFilesSection.setExpanded(true);
		inputFilesSection.setLayoutData(gridData);
		inputFilesSection.setText("Input files");
		inputFilesSection.setClient(new InputFilesPanel(inputFilesSection, SWT.NONE));
	}

	/**
	 * This method initializes dataSection	
	 *
	 */
	private void createDataSection() {
		GridData gridData1 = new GridData();
		gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		dataSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		dataSection.setExpanded(true);
		dataSection.setLayoutData(gridData1);
		dataSection.setText("Data");
		dataSection.setClient(new DataPanel(dataSection, SWT.NONE));
	}

	/**
	 * This method initializes datasetsSection	
	 *
	 */
	private void createDatasetsSection() {
		GridData gridData2 = new GridData();
		gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		datasetsSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		datasetsSection.setExpanded(true);
		datasetsSection.setLayoutData(gridData2);
		datasetsSection.setText("Datasets");
		datasetsSection.setClient(new DatasetsPanel(datasetsSection, SWT.NONE));
	}

	/**
	 * This method initializes chartSheetsSection	
	 *
	 */
	private void createChartSheetsSection() {
		GridData gridData3 = new GridData();
		gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		chartSheetsSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		chartSheetsSection.setExpanded(true);
		chartSheetsSection.setLayoutData(gridData3);
		chartSheetsSection.setText("Chart sheets and charts");
		chartSheetsSection.setClient(new ChartSheetsPanel(chartSheetsSection, SWT.NONE));
	}
}
