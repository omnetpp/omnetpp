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
    	testExport(".*scalar-dataset.*", "test-scalars.csv", "CSV",
            	"/project/test-1.sca_run-1_module-1_scalar-1,/project/test-2.sca_run-2_module-2_scalar-2\n" +
            	"1,2\n");
    }
    
    public void testCsvExportVectors() throws Exception {
    	testExport(".*vector-dataset.*", "test-vectors.csv", "CSV",
    			"time,module-2.vector-2\n" +
    			"0,2\n" +
    			"time,module-1.vector-1\n" +
    			"0,1\n");
    }

    public void testMatlabExportScalars() throws Exception {
    	testExport(".*scalar-dataset.*", "test-scalars.m", "Matlab",
    			"scalars.description=\"\"\n" +
    			"scalars._project_test_1_sca_run_1_module_1_scalar_1=[\n" +
    			"1;\n" +
    			"]\n" +
    			"\n" +
    			"scalars._project_test_2_sca_run_2_module_2_scalar_2=[\n" +
    			"2;\n" +
    			"]\n" +
    			"\n");
    }
    
    public void testMatlabExportVectors() throws Exception {
    	testExport(".*vector-dataset.*", "test-vectors.m", "Matlab",
    			"vector_2.description=\"\"\n" +
    			"vector_2.X=[\n" +
    			"0;\n" +
    			"]\n" +
    			"\n" +
    			"vector_2.Y=[\n" +
    			"2;\n" +
    			"]\n" +
    			"\n" +
    			"vector_1.description=\"\"\n" +
    			"vector_1.X=[\n" +
    			"0;\n" +
    			"]\n" +
    			"\n" +
    			"vector_1.Y=[\n" +
    			"1;\n" +
    			"]\n" +
    			"\n");
    }
    
    public void testOctaveExportScalars() throws Exception {
    	testExport(".*scalar-dataset.*", "test-scalars.octave", "Octave",
    			"# name: scalars\n" +
    			"# type: struct\n" +
    			"# length: 3\n" +
    			"# name: description\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: string\n" +
    			"# elements: 1\n" +
    			"# length: 0\n" +
    			"\n" +
    			"# name: _project_test_1_sca_run_1_module_1_scalar_1\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: matrix\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"1\n" +
    			"# name: _project_test_2_sca_run_2_module_2_scalar_2\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: matrix\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"2\n");
    }
    
    public void testOctaveExportVectors() throws Exception {
    	testExport(".*vector-dataset.*", "test-vectors.octave", "Octave",
    			"# name: vector_2\n" +
    			"# type: struct\n" +
    			"# length: 3\n" +
    			"# name: description\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: string\n" +
    			"# elements: 1\n" +
    			"# length: 0\n" +
    			"\n" +
    			"# name: X\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: matrix\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"0\n" +
    			"# name: Y\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: matrix\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"2\n" +
    			"# name: vector_1\n" +
    			"# type: struct\n" +
    			"# length: 3\n" +
    			"# name: description\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: string\n" +
    			"# elements: 1\n" +
    			"# length: 0\n" +
    			"\n" +
    			"# name: X\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: matrix\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"0\n" +
    			"# name: Y\n" +
    			"# type: cell\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"# name: <cell-element>\n" +
    			"# type: matrix\n" +
    			"# rows: 1\n" +
    			"# columns: 1\n" +
    			"1\n");
    }
    
    protected void testExport(String datasetLabel, String fileName, String fileType, String content) throws Exception {
    	String path = "/" + projectName + "/" + fileName;
    	
        TreeAccess tree = datasetsPage.getDatasetsTree();
        tree.findTreeItemByContent(datasetLabel).chooseFromContextMenu("Export to File|" + fileType + ".*");

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
