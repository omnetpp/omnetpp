package org.omnetpp.test.gui.scave;

import java.util.regex.Pattern;

import org.apache.commons.lang.ArrayUtils;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.ScaveEditorAccess;
import org.omnetpp.test.gui.core.ProjectFileTestCase;

import com.simulcraft.test.gui.util.WorkspaceUtils;

public class ScaveFileTestCase
    extends ProjectFileTestCase
{
	protected ScaveEditorAccess editor;
	
    public ScaveFileTestCase() {
        super("test.anf");
    }
    
	protected void createFile(String fileName, String content) throws Exception {
		WorkspaceUtils.createFileWithContent(projectName + "/" + fileName, content);
	}

	protected void removeFile(String fileName) throws Exception {
		String filePath = projectName + "/" + fileName;
		WorkspaceUtils.assertFileExists(filePath);
		WorkspaceUtils.ensureFileNotExists(filePath);
	}
	
	protected static GenericTreeNode[] forest(GenericTreeNode... trees) {
		return trees;
	}
	
	protected static GenericTreeNode n(String label, GenericTreeNode... children) {
		GenericTreeNode node = new GenericTreeNode(Pattern.quote(label));
		for (GenericTreeNode child : children)
			node.addChild(child);
		return node;
	}
	
	protected GenericTreeNode[] remove(GenericTreeNode[] array, int index) {
		return (GenericTreeNode[])ArrayUtils.remove(array, index);
	}

	protected String[][] remove(String[][] array, int index) {
		return (String[][])ArrayUtils.remove(array, index);
	}
	
	protected GenericTreeNode[] add(GenericTreeNode[] array, GenericTreeNode... elements) {
		GenericTreeNode[] result = array;
		for (GenericTreeNode element : elements)
			result = (GenericTreeNode[])ArrayUtils.add(result, element); 
		return result;
	}
	
	protected String[][] table(String[]... rows) {
		return rows;
	}
	
	protected String[] row(String... row) {
		return row;
	}
	
	protected String[] buildVectorsTableRow(int rowIndex) {
		return row(
				"/project/",	// directory
				String.format("test-%d.vec", rowIndex),	// file name
				String.format("config-%d", rowIndex), // config name
				String.format("%d", rowIndex), // run number
				String.format("run-%d", rowIndex), // run id
				String.format("module-%s", rowIndex), // module
				String.format("vector-%d", rowIndex), // vector name
				"1", // vector id
				String.format("%d", rowIndex), // experiment
				String.format("%d", rowIndex), // measurement
				String.format("%d", rowIndex), // replication
				"1", // count
				String.format("%s", (double)rowIndex), // mean
				"n.a.", // stddev
				String.format("%s", (double)rowIndex), // min
				String.format("%s", (double)rowIndex), // max
				String.format("%s", new BigDecimal(0.0d)), // minTime
				String.format("%s", new BigDecimal(0.0d))); // maxTime
	}

	protected String[] buildScalarsTableRow(int rowIndex) {
		return row(
				"/project/", // directory
				String.format("test-%d.sca", rowIndex), // file name
				String.format("config-%d", rowIndex), // config name
				String.format("%d", rowIndex), // run number
				String.format("run-%d", rowIndex), // run id
				String.format("module-%s", rowIndex), // module
				String.format("scalar-%d", rowIndex), // name
				String.format("%d", rowIndex), // experiment
				String.format("%d", rowIndex), // measurement
				String.format("%d", rowIndex), // replication
				String.format("%s", (double)rowIndex)); // value
	}
	
	protected void createFiles(int numOfRuns) throws Exception {
		createAnalysisFile();
		for (int runNumber = 1; runNumber <= numOfRuns; ++runNumber) {
			createScalarFile(runNumber);
			createVectorFile(runNumber);
		}
	}
	
	protected void createAnalysisFile() throws Exception {
		createFile(
				fileName,
				createAnalysisFileContent());
	}
	
	protected String createAnalysisFileContent() {
		return 	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
				"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">" +
				"<inputs>" +
				"<inputs name=\"test-*.vec\"/>" +
				"<inputs name=\"test-*.sca\"/>" +
				"</inputs>" +
				"<datasets/>" +
				"<chartSheets/>" +
				"</scave:Analysis>";
	}
	

	protected void createScalarFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.sca", runNumber),
				createScalarFileContent(runNumber));
	}

	protected static String createScalarFileContent(int runNumber) {
		return String.format(
				"run run-%1$d\n" +
				"attr run-number %1$d\n" +
				"attr config config-%1$d\n" +
				"attr experiment %1$d\n" +
				"attr measurement %1$d\n" +
				"attr replication %1$d\n" +
				"scalar module-%1$d scalar-%1$d %1$d\n",
				runNumber);
	}
	
	protected void createVectorFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.vec", runNumber),
				createVectorFileContent(runNumber));
	}

	protected static String createVectorFileContent(int runNumber) {
		return String.format(
				"run run-%1$d\n" +
				"attr run-number %1$d\n" +
				"attr config config-%1$d\n" +
				"attr experiment %1$d\n" +
				"attr measurement %1$d\n" +
				"attr replication %1$d\n" +
				"vector 1 module-%1$d vector-%1$d TV\n" +
				"1	0.0	%2$f\n",
				runNumber, (double)runNumber);
	}
}
