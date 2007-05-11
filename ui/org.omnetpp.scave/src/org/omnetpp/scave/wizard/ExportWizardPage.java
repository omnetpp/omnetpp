package org.omnetpp.scave.wizard;

import java.io.File;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.wizard.IWizard;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.engine.ScalarFields;

/**
 * Abstract base class for data export pages.
 * It provides data and file selection controls.
 *
 * @author tomi
 */
public abstract class ExportWizardPage extends WizardPage {

	private Button[] groupByCheckboxes;
	private Combo precisionCombo;
	private Text fileNameText;
	private Button browseButton;
	
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
		return fileNameText.getText();
	}
	
	protected void setFileName(String fileName) {
		fileNameText.setText(fileName);
	}

	protected ScalarFields getGroupBy() {
		int fields = 0;
		for (Button radio : groupByCheckboxes)
			if (radio.getSelection()) {
				int field = (Integer)radio.getData(DATA_KEY);
				fields |= field;
			}
				
		return new ScalarFields(fields);
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
		groupByCheckboxes[i++] = createCheckboxForField(group, "file name", ScalarFields.FILE);
		groupByCheckboxes[i++] = createCheckboxForField(group, "run name", ScalarFields.RUN);
		groupByCheckboxes[i++] = createCheckboxForField(group, "module name", ScalarFields.MODULE);
		groupByCheckboxes[i++] = createCheckboxForField(group, "scalar name", ScalarFields.NAME);
	}
	
	protected Button createCheckboxForField(Composite parent, String name, int field) {
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
		Composite panel = new Composite(parent, SWT.NONE);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		panel.setLayout(new GridLayout(3, false));
		
		Label label = new Label(panel, SWT.NONE);
		label.setText("To file:");
		fileNameText = new Text(panel, SWT.SINGLE | SWT.BORDER);
		fileNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		browseButton = new Button(panel, SWT.NONE);
		browseButton.setText("Browse...");
		
		setControl(panel);
		
		fileNameText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				updatePageCompletion();
			}
		});
	
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				handleBrowseButtonPressed();
				updatePageCompletion();
			}
		});
	}
	
	protected void updateDescription() {
		IWizard wizard = getWizard();
		if (wizard instanceof ExportWizard) {
			ExportWizard exportWizard = (ExportWizard)wizard;
			int scalars = (int)exportWizard.selectedScalars.size();
			int vectors = (int)exportWizard.selectedVectors.size();
			int histograms = (int)exportWizard.selectedHistograms.size();
			setDescription(String.format("There are %d scalars, %d vectors, %d histograms selected.",
										scalars, vectors, histograms));
		}
	}
	
	protected void handleBrowseButtonPressed() {
		FileDialog dialog = new FileDialog(getContainer().getShell(), SWT.SAVE);
		dialog.setText("Save to file");
		dialog.setFilterPath(getFileName());
		dialog.setFilterExtensions(getFileDialogFilterExtensions());
		String selectedFileName = dialog.open();

		if (selectedFileName != null) {
			setFileName(selectedFileName);
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
				int field = (Integer)cb.getData(DATA_KEY);
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
				int field = (Integer)cb.getData(DATA_KEY);
				boolean selected = settings.getBoolean(GROUPBY_KEY+"."+field);
				cb.setSelection(selected);
			}
		}
	}
}
