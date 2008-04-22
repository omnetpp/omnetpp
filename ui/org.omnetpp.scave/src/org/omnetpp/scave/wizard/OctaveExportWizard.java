package org.omnetpp.scave.wizard;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.ScaveExport;

public class OctaveExportWizard extends AbstractExportWizard {

	public static final String ID = "org.omnetpp.scave.wizard.OctaveExportWizard";
	public static final String SECTION_NAME = "OctaveExportWizard";
	
	public OctaveExportWizard() {
		IDialogSettings settings = ScavePlugin.getDefault().getDialogSettings();
		IDialogSettings section = settings.getSection(SECTION_NAME);
		if (section == null) {
			section = settings.addNewSection(SECTION_NAME);
		}
		setDialogSettings(section);
	}
	
	@Override
	public void addPages() {
		page = new OctaveExportPage("Octave export", "Export in Octave text format", null);
		addPage(page);
	}

	@Override
	protected ScaveExport createExporter() {
		return ExporterFactory.createExporter("octave");
	}
	
	/*---------------------
	 *        Pages
	 *---------------------*/
	
	static class OctaveExportPage extends ExportWizardPage
	{
		protected OctaveExportPage(String pageName, String title,
				ImageDescriptor titleImage) {
			super(pageName, title, titleImage);
		}

		@Override
		protected String[] getFileDialogFilterExtensions() {
			return new String[] { "*.octave", "*.*" };
		}
	}
}
