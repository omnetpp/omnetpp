package org.omnetpp.test.gui.scave;

import org.omnetpp.test.gui.core.ProjectFileTestCase;

import com.simulcraft.test.gui.util.WorkspaceUtils;

public class ScaveFileTestCase
    extends ProjectFileTestCase
{
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
}
