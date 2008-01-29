package org.omnetpp.scave.wizard;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.FileSelectionPanel;
import org.omnetpp.scave.engine.CsvExport;
import org.omnetpp.scave.engine.ScaveExport;
import org.omnetpp.scave.engine.CsvExport.QuoteMethod;

/**
 * Export the selected result items in CSV format.
 *
 * @author tomi
 */
public class CsvExportWizard extends AbstractExportWizard {

	public static final String ID = "org.omnetpp.scave.wizard.CsvExportWizard";
	public static final String SECTION_NAME = "CsvExportWizard";
	
	public static final String
		SEPARATOR_KEY	= "separator",
		EOL_KEY			= "eol",
		QUOTECHAR_KEY	= "quoteChar",
		HEADER_KEY		= "header";
	
	public CsvExportWizard() {
		IDialogSettings settings = ScavePlugin.getDefault().getDialogSettings();
		IDialogSettings section = settings.getSection(SECTION_NAME);
		if (section == null) {
			section = settings.addNewSection(SECTION_NAME);
		}
		setDialogSettings(section);
	}
	
	@Override
	public void addPages() {
		page = new CsvExportPage("CSV export", "Export to CSV", null);
		addPage(page);
	}
	
	public CsvExportPage getPage() {
		return (CsvExportPage)page;
	}

	@Override
	protected ScaveExport createExporter() {
		CsvExport exporter = new CsvExport();
		exporter.setSeparator(getPage().getSeparator());
		exporter.setEOL(getPage().getEOL());
		exporter.setQuoteChar(getPage().getQuoteChar());
		exporter.setQuoteMethod(QuoteMethod.DOUBLE);
		exporter.setColumnNames(getPage().getHeader());
		if (selectedVectors.size() + selectedScalars.size() + selectedHistograms.size() > 1)
			exporter.setFileNameSuffix(1);
		return exporter;
	}
	
	/*---------------------
	 *        Pages
	 *---------------------*/
	
	class CsvExportPage extends ExportWizardPage
	{
		private Combo separatorCombo;
		private Combo eolCombo;
		private Combo quoteCharCombo;
		//private Combo quoteMethodCombo;
		private Button headerCheckbox;
		
		protected CsvExportPage(String pageName, String title,
				ImageDescriptor titleImage) {
			super(pageName, title, titleImage);
		}
		
		@Override
		protected void createPanels(Composite parent) {
			super.createPanels(parent);
			createCsvOptionsPanel(parent);
		}
		
		@Override
		protected void createFileSelectionPanel(Composite parent) {
			Group group = new Group(parent, SWT.NONE);
			group.setText("Output files");
			group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
			group.setLayout(new GridLayout(1, false));
			
			fileSelectionPanel = 
				new FileSelectionPanel(group, SWT.NONE, "Base file name:", SWT.SAVE, "Save to file",
						getFileDialogFilterExtensions());
			fileSelectionPanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
			setControl(fileSelectionPanel);
			
			Label label = new Label(group, SWT.WRAP);
			label.setText("When several files are generated the file names are formed by appending '-1','-2',... to the base file name.");
			label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
			
			fileSelectionPanel.addPropertyChangeListener(new IPropertyChangeListener() {
				public void propertyChange(PropertyChangeEvent event) {
					if (FileSelectionPanel.PROP_FILENAME.equals(event.getProperty()))
						updatePageCompletion();
				}
			});
		}
		
		private void createCsvOptionsPanel(Composite parent) {
			Group group = new Group(parent, SWT.NONE);
			group.setText("CSV options");
			group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
			group.setLayout(new GridLayout(2, false));
			Label label = new Label(group, SWT.NONE);
			label.setText("Separator:");
			separatorCombo = new Combo(group, SWT.READ_ONLY);
			separatorCombo.setItems(new String[] {"Comma", "Tab", "Space", "Semicolon"});
			separatorCombo.setText("Comma");
			label = new Label(group, SWT.NONE);
			label.setText("Lines end with:");
			eolCombo = new Combo(group, SWT.READ_ONLY);
			eolCombo.setItems(new String[] {"CR LF", "CR", "LF"});
			eolCombo.setText("CR LF");
			label = new Label(group, SWT.NONE);
			label.setText("Text delimiter:");
			quoteCharCombo = new Combo(group, SWT.READ_ONLY);
			quoteCharCombo.setItems(new String[] {"\"", "'"});
			quoteCharCombo.setText("\"");
			headerCheckbox = new Button(group, SWT.CHECK);
			headerCheckbox.setText("Add header to columns");
			headerCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
		}

		@Override
		protected String[] getFileDialogFilterExtensions() {
			return new String[] { "*.csv", "*.*" };
		}
		
		public char getSeparator() {
			String text = separatorCombo.getText();
			if ("Comma".equals(text))
				return ',';
			else if ("Tab".equals(text))
				return '\t';
			else if ("Space".equals(text))
				return ' ';
			else if ("Semicolon".equals(text))
				return ';';
			else
				return ',';
		}
		
		public String getEOL() {
			String text = eolCombo.getText();
			if ("CR LF".equals(text))
				return "\r\n"; 
			else if ("CR".equals(text))
				return "\r";
			else if ("LF".equals(text))
				return "\n";
			return "\r\n";
		}
		
		public char getQuoteChar() {
			String text = quoteCharCombo.getText();
			return StringUtils.isEmpty(text) ? '"' : text.charAt(0);
		}
		
		public boolean getHeader() {
			return headerCheckbox.getSelection();
		}

		@Override
		protected void saveDialogSettings() {
			super.saveDialogSettings();
			IDialogSettings settings = getDialogSettings();
			if (settings != null) {
				settings.put(SEPARATOR_KEY, separatorCombo.getText());
				settings.put(EOL_KEY, eolCombo.getText());
				settings.put(QUOTECHAR_KEY, quoteCharCombo.getText());
				settings.put(HEADER_KEY, headerCheckbox.getSelection());
			}
		}

		@Override
		protected void restoreDialogSettings() {
			super.restoreDialogSettings();
			IDialogSettings settings = getDialogSettings();
			if (settings != null) {
				String separator = settings.get(SEPARATOR_KEY);
				if (!StringUtils.isEmpty(separator))
					separatorCombo.setText(separator);
				String eol = settings.get(EOL_KEY);
				if (!StringUtils.isEmpty(eol))
					eolCombo.setText(eol);
				String quoteChar = settings.get(QUOTECHAR_KEY);
				if (!StringUtils.isEmpty(quoteChar))
					quoteCharCombo.setText(quoteChar);
				headerCheckbox.setSelection(settings.get(HEADER_KEY)==null ? true : settings.getBoolean(HEADER_KEY));
			}
		}
	}
}
