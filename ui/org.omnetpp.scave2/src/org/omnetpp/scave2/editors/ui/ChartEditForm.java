package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ChartEditForm implements IScaveObjectEditForm {
	
	/**
	 * Features edited on this form.
	 */
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getChart_Name()
	};
	
	/**
	 * The edited chart.
	 */
	private Chart chart;
	
	// controls
	private Text nameText;
	
	public ChartEditForm(Chart chart) {
		this.chart = chart;
	}

	/**
	 * Returns the title displayed on the top of the dialog.
	 */
	public String getTitle() {
		return "Chart";
	}

	/**
	 * Returns the description displayed below the title.
	 */
	public String getDescription() {
		return "Modify properties of the chart.";
	}

	/**
	 * Returns the number of features on this form.
	 */
	public int getFeatureCount() {
		return features.length;
	}

	/**
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		return features;
	}

	public void populatePanel(Composite panel) {
		panel.setLayout(new GridLayout(2, false));
		
		Label nameLabel = new Label(panel, SWT.NONE);
		nameLabel.setText("Name:");
		nameLabel.setLayoutData(new GridData());
		nameText = new Text(panel, SWT.BORDER);
		nameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
	}

	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			return nameText.getText();
		}
		return null;
	}

	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			nameText.setText(value == null ? "" : (String)value);
		}
	}
}
