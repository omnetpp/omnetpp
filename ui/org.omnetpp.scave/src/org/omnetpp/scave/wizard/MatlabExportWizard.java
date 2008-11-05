package org.omnetpp.scave.wizard;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.ScaveExport;

/**
 * Export the selected result items as a matlab script.
 *
 * @author tomi
 */
public class MatlabExportWizard extends AbstractExportWizard {

	public static final String ID = "org.omnetpp.scave.wizard.MatlabExportWizard";
	
	public MatlabExportWizard() {
		setDialogSettings(UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName()));
	}
	
	@Override
	public void addPages() {
		page = new MatlabExportPage("Matlab export", "Export as matlab script", null);
		addPage(page);
	}

	@Override
	protected ScaveExport createExporter() {
		return ExporterFactory.createExporter("matlab");
	}
	
	/*---------------------
	 *        Pages
	 *---------------------*/
	
	static class MatlabExportPage extends ExportWizardPage
	{
		protected MatlabExportPage(String pageName, String title,
				ImageDescriptor titleImage) {
			super(pageName, title, titleImage);
		}

		@Override
		protected String[] getFileDialogFilterExtensions() {
			return new String[] { "*.m", "*.*" };
		}
	}
}
