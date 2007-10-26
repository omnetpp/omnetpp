package org.omnetpp.test.gui.scave;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.omnetpp.test.gui.access.DatasetsAndChartsPageAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class ExportTest extends ScaveFileTestCase {

	DatasetsAndChartsPageAccess datasetsPage;
	
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		datasetsPage = editor.ensureDatasetsPageActive();
	}
	
    public void testCsvExportScalars() throws Exception {
    	testCsvExport("add scalars: all", "test-scalars.csv",
            	"/project/test-1.sca_run-1_module-1_scalar-1,/project/test-2.sca_run-2_module-2_scalar-2\n" +
            	"1,2\n");
    }
    
    protected void testCsvExport(String datasetLabel, String fileName, String content) throws Exception {
    	String path = "/" + projectName + "/" + fileName;
    	
        TreeAccess tree = datasetsPage.getDatasetsTree();
        tree.findTreeItemByContent(datasetLabel).chooseFromContextMenu("Export to File|CSV.*");

        ShellAccess dialog = Access.findShellWithTitle("Export");
        dialog.findTextAfterLabel("To file.*").clickAndTypeOver(getAbsolutePathForWorkspaceFile(path));
        dialog.findButtonWithLabel("Finish").selectWithMouseClick();

        WorkbenchUtils.refreshProjectFromProjectExplorerView(projectName);
        WorkspaceUtils.assertFileExistsWithContentIgnoringWhiteSpace(path, content);
    }
    
	protected void createFiles() throws Exception {
		createFile(
				fileName,

				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
				"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">" +
				"<inputs>" +
				"<inputs name=\"test-*.vec\"/>" +
				"<inputs name=\"test-*.sca\"/>" +
				"</inputs>" +
				"<datasets>" +
			    "  <datasets name=\"scalar-dataset\">" +
			    "    <items xsi:type=\"scave:Add\" filterPattern=\"\"/>" +
			    "  </datasets>" +
			    "  <datasets name=\"vector-dataset\">" +
			    "    <items xsi:type=\"scave:Add\" filterPattern=\"\" type=\"VECTOR\"/>" +
			    "  </datasets>" +
			    "</datasets>" +
				"<chartSheets/>" +
				"</scave:Analysis>");
		
		for (int runNumber = 1; runNumber <= 2; ++runNumber) {
			createFile(
					String.format("test-%d.sca", runNumber),
					createScalarFileContent(runNumber));
			createFile(
					String.format("test-%d.vec", runNumber),
					createVectorFileContent(runNumber));
		}
	}
	
	public String getAbsolutePathForWorkspaceFile(String path) {
		return ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path)).getLocation().toOSString();
	}
}
