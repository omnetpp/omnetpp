package org.omnetpp.scave.editors;

import static org.omnetpp.scave.common.ScaveMarkers.MARKERTYPE_SCAVEPROBLEM;
import static org.omnetpp.scave.common.ScaveMarkers.deleteMarkers;
import static org.omnetpp.scave.common.ScaveMarkers.setMarker;
import static org.omnetpp.scave.engineext.IndexFile.isIndexFileUpToDate;
import static org.omnetpp.scave.engineext.IndexFile.isVectorFile;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.content.IContentType;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.omnetpp.scave.ContentTypes;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engineext.IndexFile;
import org.omnetpp.scave.engineext.ResultFileFormatException;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.jobs.VectorFileIndexerJob;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;

/**
 * This class is responsible for loading/unloading result files
 * (scalar and vector files), based on changes in the model
 * (the Inputs object) and in the workspace. Thus it should be
 * hooked up to listen to EMF and workspace changes.
 *
 * @author andras, tomi
 */
public class ResultFilesTracker implements INotifyChangedListener, IResourceChangeListener {
	
	private static final boolean debug = true;

	private ResultFileManagerEx manager; //backreference to the manager it operates on, the manager is owned by the editor
	private Inputs inputs; // backreference to the Inputs element we watch
	private IPath baseDir; // path of the directory which used to resolve relative paths as a base

	private Object lock = new Object();
	
	public ResultFilesTracker(ResultFileManagerEx manager, Inputs inputs, IPath baseDir) {
		this.manager = manager;
		this.inputs = inputs;
		this.baseDir = baseDir;
	}
	
	public void deactivate()
	{
		manager = null;
	}

	/**
	 * Listen to EMF model changes.
	 */
	public void notifyChanged(Notification notification) {
		if (manager == null)
			return;
		
		if (notification.isTouch())
			return;

		switch (notification.getEventType()) {
		case Notification.ADD:
		case Notification.ADD_MANY:
		case Notification.REMOVE:
		case Notification.REMOVE_MANY:
		//case Notification.MOVE:
		case Notification.SET:
		//case Notification.UNSET:
			Object notifier = notification.getNotifier();
			if (notifier instanceof Inputs || notifier instanceof InputFile)
				synchronize();
		}
	}
	
	/**
	 * Listen to workspace changes. We want to keep our result files in
	 * sync with the workspace. In addition to changes in file contents,
	 * Inputs can have wildcard filters which may match different files
	 * as files get created/deleted in the workspace.
	 */
	public void resourceChanged(IResourceChangeEvent event) {
		if (manager == null)
			return;
		
		try {
			IResourceDelta delta = event.getDelta();
			if (delta != null)
				delta.accept(new ResourceDeltaVisitor());
		} catch (CoreException e) {
			ScavePlugin.logError("Could not refresh the result files", e);
		}
	}

	class ResourceDeltaVisitor implements IResourceDeltaVisitor {
		public boolean visit(IResourceDelta delta) throws CoreException {
			IResource resource = delta.getResource();
			if (!(resource instanceof IFile))
				return true;

			IFile file = (IFile)resource;
			IFile resultFile = IndexFile.isIndexFile(file) ? IndexFile.getVectorFileFor(file) :
								isResultFile(file) ? file : null;
			
			if (resultFile == null || isDerived(resultFile) || !inputsMatches(resultFile))
				return false;
			
			switch (delta.getKind()) {
			case IResourceDelta.ADDED:
					if (debug) System.out.format("File added: %s%n", file);
					loadFile(resultFile);
					break;
			case IResourceDelta.REMOVED:
					if (debug) System.out.format("File removed: %s%n", file);
					if (isResultFile(file) && !isDerived(file))
						unloadFile(file);
					break;
			case IResourceDelta.CHANGED:
					if (debug) System.out.format("File changed: %s%n", file);
					if ((delta.getFlags() & ~IResourceDelta.MARKERS) != 0)
						loadFile(resultFile);
					break;
			}
			return false;
		}
	}

	/**
	 * Ensure that exactly the result files specified in the Inputs node are loaded.
	 * Missing files get loaded, and extra files get unloaded.
	 */
	public void synchronize() {
		if (manager == null)
			return;
		
		if (debug) System.out.println("ResultFileTracker.synchronize()");
		Set<String> loadedFiles = new HashSet<String>();
		for (ResultFile file : manager.getFiles().toArray())
			loadedFiles.add(file.getFilePath());
		
		Set<String> filesToBeLoaded = new HashSet<String>();
		List<InputFile> files = new ArrayList<InputFile>();
		List<InputFile> wildcards = new ArrayList<InputFile>();
		partitionInputFiles(files, wildcards);

		// load files specified by path
		for (InputFile inputfile : files) {
			filesToBeLoaded.add(inputfile.getName());
		}
		// load files matching with patterns
		if (wildcards.size() > 0) {
			List<IFile> allFiles = getResultFiles();
			for (IFile file : allFiles)
				for (InputFile wildcard : wildcards)
					if (matchFile(file, wildcard))
						filesToBeLoaded.add(file.getFullPath().toString());
		}
		
		Set<String> filesToBeUnloaded = new HashSet<String>(loadedFiles);
		filesToBeUnloaded.removeAll(filesToBeLoaded);
		for (String file : filesToBeUnloaded)
			unloadFile(file);
		
		filesToBeLoaded.removeAll(loadedFiles);
		for (String file : filesToBeLoaded)
			loadFile(file);
	}

	@SuppressWarnings("unchecked")
	private void partitionInputFiles(List<InputFile> files, List<InputFile> wildcards) {
		for (InputFile inputfile : (List<InputFile>)inputs.getInputs()) {
			if (containsWildcard(inputfile.getName()))
				wildcards.add(inputfile);
			else
				files.add(inputfile);
		}
	}

	private boolean containsWildcard(String fileSpec) {
		return fileSpec.indexOf('?') >= 0 || fileSpec.indexOf('*') >= 0;
	}

	/**
	 * Returns the list of result files in the workspace.
	 */
	private List<IFile> getResultFiles() {
		final List<IFile> files = new ArrayList<IFile>();
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		try {
			root.accept(new IResourceVisitor() {
				public boolean visit(IResource resource) {
					if (resource instanceof IFile) {
						IFile file = (IFile)resource;
						if (isResultFile(file) && !isDerived(file)) {
							files.add((IFile)resource);
							return false;
						}
					}
					return true;
				}
			});
		} catch (CoreException e) {
			ScavePlugin.logError(e);
		}
		return files;
	}

	/**
	 * Loads the file specified by <code>resourcePath</code> into the ResultFileManager.
	 */
	private void loadFile(String resourcePath) {
		IPath path = new Path(resourcePath);
		if (!path.isAbsolute())
			path = baseDir.append(path);
		IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
		IResource resource = workspaceRoot.findMember(path);
		if (resource instanceof IFile) {
			IFile file = (IFile)resource;
			if (isResultFile(file) && !isDerived(file))
				loadFile(file);
		}
	}

	/**
	 * Loads the specified <code>file</code> into the ResultFileManager.
	 * If a vector file is loaded, it checks that the index file is up-to-date.
	 * When not, it generates the index first and then loads it from the index.
	 */
	private void loadFile(final IFile file) {
		Assert.isLegal(isResultFile(file));
		if (debug) System.out.format("  loadFile: %s ", file);
		synchronized (lock) {
			if (file.getLocation().toFile().exists()) {
				String resourcePath = file.getFullPath().toString();
				ResultFile resultFile = manager.getFile(resourcePath);
				if (resultFile != null)
					manager.unloadFile(resultFile);
				loadFileInternal(file);
			}
		}
	}

	/**
	 * Unloads the specified <code>file</code> from the ResultFileManager.
	 * Has no effect when the file was not loaded.
	 */
	private void unloadFile(IFile file) {
		unloadFile(file.getFullPath().toString());
	}
	
	/**
	 * Unloads the file specified  by <code>resourcePath</code> from the ResultFileManager.
	 * Has no effect when the file was not loaded.
	 */
	private void unloadFile(String resourcePath) {
		if (debug) System.out.format("  unloadFile: %s%n ", resourcePath);
		ResultFile resultFile = manager.getFile(resourcePath);
		if (resultFile != null) {
			manager.unloadFile(resultFile);
			if (debug) System.out.println("done");
		}
	}

	/**
	 * Loads the specified <code>file</code> into the ResultFileManager.
	 * If a vector file is loaded, it checks that the index file is up-to-date.
	 * When not, it generates the index first and then loads it from the index.
	 */
	private void loadFileInternal(final IFile file) {
		ResultFileFormatException fileFormatException = null;
		Exception exception = null;
		
		try {
			final String resourcePath = file.getFullPath().toString();
			final String osPath = file.getLocation().toOSString();

			synchronized (lock) {
				// Do not try to load from the vector file whose index is not up-to-date,
				// because the ResultFileManager loads it from the vector file and it takes too much time
				// for ~100MB files.
				// Create or update the index file first, and try again.
				if (isVectorFile(file) && !isIndexFileUpToDate(file)) {
					if (debug) System.out.format("indexing: %s%n", file);
					VectorFileIndexerJob indexer = new VectorFileIndexerJob("Indexing "+file, new IFile[] {file}, lock);
					indexer.setPriority(Job.LONG);
					indexer.addJobChangeListener(new JobChangeAdapter() {
						public void done(IJobChangeEvent event) {
							// load from the newly created index file
		 				    // even if the workspace is not refreshed automatically
							if (event.getResult().getSeverity() != IStatus.ERROR) {
								synchronized (lock) {
									manager.loadFile(resourcePath, osPath);
									if (debug) System.out.println("done");
								}
							}
							else {
								unloadFile(file);
							}
						}
					});
					indexer.schedule();
				}
				else {
					manager.loadFile(resourcePath, osPath);
					if (debug) System.out.println("done");
				}
			}
		}
		catch (ResultFileFormatException e) {
			fileFormatException = e;
			ScavePlugin.logError("Wrong file: " + file.getLocation().toOSString(), e);
			if (debug) System.out.format("exception: %s ", e);
		}
		catch (Exception e) {
			exception = e;
			ScavePlugin.logError("Could not load file: " + file.getLocation().toOSString(), e);
			if (debug) System.out.format("exception: %s ", e);
		}
		
		if (fileFormatException != null) {
			if (isVectorFile(file)) {
				IFile indexFile = IndexFile.getIndexFileFor(file);
				String message = fileFormatException.getMessage();
				int lineNo = fileFormatException.getLineNo();
				setMarker(indexFile, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Wrong file: "+message, lineNo);
				setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_WARNING, "Could not load file. Reason: "+message, -1);
			}
			else {
				String message = fileFormatException.getMessage();
				int lineNo = fileFormatException.getLineNo();
				setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Wrong file: "+message, lineNo);
			}
		}
		else if (exception != null) {
			setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_WARNING, "Could not load file. Reason: "+exception.getMessage(), -1);
		}
		else {
			deleteMarkers(file, MARKERTYPE_SCAVEPROBLEM);
		}
	}
	

	/**
	 * Returns true iff <code>file</code> is a result file (scalar or vector).
	 */
	public static boolean isResultFile(IFile file) {
		try {
			if (file.getContentDescription() != null) {
				IContentType contentType = file.getContentDescription().getContentType();
				return ContentTypes.SCALAR.equals(contentType.getId()) || ContentTypes.VECTOR.equals(contentType.getId());
			}
		} catch (CoreException e) {
			// the file might not exists, so content description is not available
			String extension = file.getFileExtension();
			return "sca".equals(extension) ||  "vec".equals(extension)/* || "vci".equals(extension)*/;
		}
		return false;
	}
	
	/**
	 * Returns true if the resource or one of its parents is derived.
	 * 
	 * @return
	 */
	public static boolean isDerived(IResource resource) {
		while (!(resource instanceof IProject) && !resource.isDerived())
			resource = resource.getParent();
		return resource.isDerived();
	}
	
	/**
	 * Return true iff the <code>file</code> matches any of the input files.
	 */
	private boolean inputsMatches(IFile file) {
		for (InputFile inputfile : inputs.getInputs())
			if (matchFile(file, inputfile))
				return true;
		return false;
	}

	/**
	 * Returns true if the <code>workspaceFile</code> matches with the file specification
	 * <code>inputFile</code>.
	 */
	private boolean matchFile(IFile workspaceFile, InputFile fileSpec) {
		if (fileSpec.getName() == null)
			return false;
		IPath filePatternPath = new Path(fileSpec.getName());
		if (!filePatternPath.isAbsolute()) {
			filePatternPath = baseDir.append(filePatternPath);
		}
		String filePath = workspaceFile.getFullPath().toString();
		String filePattern = filePatternPath.toString(); 
		return matchPattern(filePath, filePattern);
	}

	/**
	 * Matches <code>str</code> against the given <code>pattern</code>.
	 * The pattern may contain '?' (any char) and '*' (any char sequence) wildcards.
	 */
	private static boolean matchPattern(String str, String pattern) {
		return matchPattern(str, 0, str.length(), pattern, 0, pattern.length());
	}

	/**
	 * Matches the given string segment with the given pattern segment.
	 */
	private static boolean matchPattern(String str, int strStart, int strEnd, String pattern, int patternStart, int patternEnd) {
		while (strStart < strEnd && patternStart < patternEnd) {
			char strChar = str.charAt(strStart);
			char patternChar = pattern.charAt(patternStart);

			switch (patternChar) {
			case '?':
				if (strChar == '/')
					return false;
				++strStart;
				++patternStart;
				break;
			case '*':
				if (patternStart + 1 == patternEnd) // last char is '*'
					return true;
				if (pattern.charAt(patternStart + 1) == strChar) { // try to match '*' with empty string
					boolean matches = matchPattern(str, strStart, strEnd, pattern, patternStart + 1, patternEnd);
					if (matches)
						return true;
				}
				// match '*' with strChar
				if (strChar == '/')
					return false;
				++strStart;
				break;
			default:
				if (strChar == patternChar) {
					++strStart;
					++patternStart;
				}
				else
					return false;
				break;
			}
		}

		return strStart == strEnd && patternStart == patternEnd;
	}
}
