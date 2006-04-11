package org.omnetpp.resources;

import java.util.Hashtable;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.NedFileNode;

/**
 * Parses all NED files in the workspace and makes them available
 * for other plugins for consistence checks among NED files etc. 
 * 
 * @author andras
 */
public class NEDResources {

	public static String NED_FILE_EXTENSION = "ned";

	private Hashtable<IFile, NEDElement> nedFiles = new Hashtable();

	/**
	 * Traverse workspace and parse all NED files again
	 */
	public void reread() {
		nedFiles.clear();
    	IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
    	readMembersOf(workspaceRoot);
    }

	private void readMembersOf(IContainer parent) {
		try {
			for (IResource res : parent.members()) {
				if (res instanceof IContainer) {
					//System.out.println("container: "+res);
					readMembersOf((IContainer)res);
				}
				else if (res instanceof IFile) {
					IFile file = (IFile)res;
					if (NED_FILE_EXTENSION.equals(file.getFileExtension())) {
						readNEDFile(file);
					}
				}
			}
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private void readNEDFile(IFile file) {
		System.out.println("reading nedfile: "+file);
		String fileName = file.getLocation().toFile().getPath();
		NEDElement tree = null;
		try {
			tree = ModelUtil.loadNedSource(fileName);
		} catch (Exception e) {
			System.out.println(" EXCEPTION: "+e);
		} 
		if (tree==null) {
			// XXX do something -- add red (x) marker? 
			System.out.println(" ERROR");
		}
		else {
			System.out.println(" stored");
			nedFiles.put(file, tree);
		}
	}
}
