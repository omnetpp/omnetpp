package org.omnetpp.scave.wizard;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.ScaveExport;

/**
 * Export the selected result items in CSV format.
 *
 * @author tomi
 */
public class CsvExportWizard extends ExportWizard {

	public static final String ID = "org.omnetpp.scave.wizard.CsvExportWizard";
	public static final String SECTION_NAME = "CsvExportWizard";
	
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

	@Override
	protected ScaveExport createExporter() {
		return ExporterFactory.createExporter("csv");
	}
	
	/*---------------------
	 *        Pages
	 *---------------------*/
	
	class CsvExportPage extends ExportWizardPage
	{
		protected CsvExportPage(String pageName, String title,
				ImageDescriptor titleImage) {
			super(pageName, title, titleImage);
		}

		@Override
		protected String[] getFileDialogFilterExtensions() {
			return new String[] { "*.csv", "*.*" };
		}
	}
}
