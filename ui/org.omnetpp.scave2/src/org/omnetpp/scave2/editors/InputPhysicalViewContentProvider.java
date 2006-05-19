package org.omnetpp.scave2.editors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.internal.resources.Workspace;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;

public class InputPhysicalViewContentProvider implements ITreeContentProvider {

	private final static Object[] EMPTY_ARRAY = new Object[0];
	
	private ResultFileManager manager = new ResultFileManager(); 
	File[] files;
	Map<File,InputFile> fileToInputFileMap = new HashMap<File,InputFile>();
	Map<File,Run[]> fileToRunsMap = new HashMap<File,Run[]>();
	Map<Run,File> runToFileMap = new HashMap<Run,File>();
	
	public Object[] getChildren(Object parentElement) {
		if (parentElement instanceof Inputs)
			return getFiles((Inputs)parentElement);
		else if (parentElement instanceof File)
			return getRuns((File)parentElement);
		else if (parentElement instanceof Run)
			return EMPTY_ARRAY;
		return null;
	}

	public Object getParent(Object element) {
		if (element instanceof Inputs)
			return null;
		else if (element instanceof File)
			return getInputFile((File)element).eContainer();
		else if (element instanceof Run)
			return getFile((Run)element);
		return null;
	}

	public boolean hasChildren(Object element) {
		return (element instanceof Inputs || element instanceof File);
	}

	public Object[] getElements(Object inputElement) {
		return getChildren(inputElement);
	}

	public void dispose() {
		manager.delete();
		manager = null;
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		fileToInputFileMap.clear();
		fileToRunsMap.clear();
		runToFileMap.clear();
	}
	
	private File[] getFiles(Inputs inputs) {
		if (files == null) {
			List<File> fileList = new ArrayList<File>();
			for (Object inputFile : inputs.getInputs())
				fileList.addAll(filesFromInputFile((InputFile)inputFile));
			files = new File[fileList.size()];
			files = fileList.toArray(files);
		}
		return files;
	}
	
	private List<File> filesFromInputFile(InputFile inputFile) {
		List<File> result = new ArrayList<File>();
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		IWorkspaceRoot workspaceRoot = workspace.getRoot();
		IFile[] files = workspaceRoot.findFilesForLocation(new Path(inputFile.getName()));
		
		for (IFile ifile : files) {
			File file = loadFile(ifile.getLocation());
			fileToInputFileMap.put(file, inputFile);
			result.add(file);
		}
		return result;
	}
	
	private File loadFile(IPath path) {
		if (path.getFileExtension().equals("sca"))
			return manager.loadScalarFile(path.toOSString());
		else
			return manager.loadVectorFile(path.toOSString());
	}
	
	private InputFile getInputFile(File file) {
		return fileToInputFileMap.get(file);
	}
	
	private Run[] getRuns(File file) {
		Run[] runs = fileToRunsMap.get(file);
		if (runs == null) {
			runs = manager.getRunsInFile(file).toArray();
			fileToRunsMap.put(file, runs);
			for (Run run : runs)
				runToFileMap.put(run,file);
		}
		return runs;
	}
	
	private File getFile(Run run) {
		return runToFileMap.get(run);
	}
}
