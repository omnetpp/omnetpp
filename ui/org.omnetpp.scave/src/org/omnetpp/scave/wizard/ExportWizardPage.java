package org.omnetpp.scave.wizard;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;

import java.io.File;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.wizard.IWizard;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave.editors.ui.FileSelectionPanel;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.engine.StringVector;

/**
 * Abstract base class for data export pages.
 * It provides data and file selection controls.
 *
 * @author tomi
 */
public abstract class ExportWizardPage extends WizardPage {

	private Button[] groupByCheckboxes;
	private Combo precisionCombo;
	FileSelectionPanel fileSelectionPanel;
	
	private static final String FILENAME_KEY = "filename";
	private static final String GROUPBY_KEY = "groupBy";
	private static final String PRECISION_KEY = "precision";
	private static final String DATA_KEY = "org.omnetpp.scave.wizard.ExportWizardPage"; 
	
	protected ExportWizardPage(String pageName, String title,
			ImageDescriptor titleImage) {
		super(pageName, title, titleImage);
	}
	
	protected abstract String[] getFileDialogFilterExtensions();
	
	protected String getFileName() {
		return fileSelectionPanel.getFileName();
	}
	
	protected void setFileName(String fileName) {
		fileSelectionPanel.setFileName(fileName);
	}

	protected ResultItemFields getGroupBy() {
		StringVector fields = new StringVector();
		for (Button radio : groupByCheckboxes)
			if (radio.getSelection())
				fields.add((String)radio.getData(DATA_KEY));
				
		return new ResultItemFields(fields);
	}
	
	protected int getPrecision() {
		String value = precisionCombo.getText();
		if (value != null && !value.equals(""))
			return Integer.parseInt(value);
		else
			return 18;
	}
	
	public void createControl(Composite parent) {
		Composite panel = new Composite(parent, SWT.NONE);
		panel.setLayout(new GridLayout(1, false));
		createPanels(panel);
		updateDescription();
		restoreDialogSettings();
	}
	
	protected void createPanels(Composite parent) {
		createFileSelectionPanel(parent);
		createColumnSelectionPanel(parent);
		createPrecisionPanel(parent);
	}
	
	protected void createColumnSelectionPanel(Composite parent) {
		Group group = new Group(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		group.setLayout(new GridLayout(2, true));
		group.setText("Scalars grouped by");
		groupByCheckboxes = new Button[4];
		int i = 0;
		groupByCheckboxes[i++] = createCheckboxForField(group, "file name", FILE);
		groupByCheckboxes[i++] = createCheckboxForField(group, "run name", RUN);
		groupByCheckboxes[i++] = createCheckboxForField(group, "module name", MODULE);
		groupByCheckboxes[i++] = createCheckboxForField(group, "scalar name", NAME);
	}
	
	protected Button createCheckboxForField(Composite parent, String name, String field) {
		Button checkbox = new Button(parent, SWT.CHECK);
		checkbox.setText(name);
		checkbox.setData(DATA_KEY, field);
		return checkbox;
	}
	
	protected void createPrecisionPanel(Composite parent) {
		Group group = new Group(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		group.setLayout(new GridLayout(2, false));
		group.setText("Number format");
		Label label = new Label(group, SWT.NONE);
		label.setText("Precision:");
		precisionCombo = new Combo(group, SWT.READ_ONLY);
		for (int prec = 5; prec <= 18; ++prec)
			precisionCombo.add(String.valueOf(prec));
		precisionCombo.select(precisionCombo.getItemCount()-1);
	}
	
	protected void createFileSelectionPanel(Composite parent) {
		fileSelectionPanel = 
			new FileSelectionPanel(parent, SWT.NONE, "To file:", SWT.SAVE, "Save to file",
					getFileDialogFilterExtensions());
		fileSelectionPanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		setControl(fileSelectionPanel);
		
		fileSelectionPanel.addPropertyChangeListener(new IPropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent event) {
				if (FileSelectionPanel.PROP_FILENAME.equals(event.getProperty()))
					updatePageCompletion();
			}
		});
	}
	
	protected void updateDescription() {
		IWizard wizard = getWizard();
		if (wizard instanceof AbstractExportWizard) {
			AbstractExportWizard exportWizard = (AbstractExportWizard)wizard;
			int scalars = (int)exportWizard.selectedScalars.size();
			int vectors = (int)exportWizard.selectedVectors.size();
			int histograms = (int)exportWizard.selectedHistograms.size();
			setDescription(String.format("There are %d scalars, %d vectors, %d histograms selected.",
										scalars, vectors, histograms));
		}
	}
	
	protected void updatePageCompletion() {
		if (!validFile()) {
			setErrorMessage("File is not set, or it is a directory.");
			setPageComplete(false);
		}
		else {
			setMessage(null);
			setPageComplete(true);
		}
	}
	
	protected boolean validFile() {
		File file = new File(getFileName());
		return file.getPath().length() != 0 && !file.isDirectory();
	}
	
	protected void saveDialogSettings() {
		IDialogSettings settings = getDialogSettings();
		if (settings != null) {
			settings.put(FILENAME_KEY, getFileName());
			settings.put(PRECISION_KEY, precisionCombo.getText());
			for (Button cb : groupByCheckboxes) {
				String field = (String)cb.getData(DATA_KEY);
				settings.put(GROUPBY_KEY+"."+field, cb.getSelection());
			}
		}
	}
	
	protected void restoreDialogSettings() {
		IDialogSettings settings = getDialogSettings();
		if (settings != null) {
			String fileName = settings.get(FILENAME_KEY);
			if (fileName != null)
				setFileName(fileName);
			String precision = settings.get(PRECISION_KEY);
			if (precision != null && !precision.equals(""))
				precisionCombo.setText(precision);
			for (Button cb : groupByCheckboxes) {
				String field = (String)cb.getData(DATA_KEY);
				boolean selected = settings.getBoolean(GROUPBY_KEY+"."+field);
				cb.setSelection(selected);
			}
		}
	}
}
