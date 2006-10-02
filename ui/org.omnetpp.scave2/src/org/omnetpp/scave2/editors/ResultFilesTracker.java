package org.omnetpp.scave2.editors;

import java.util.List;

import javax.swing.text.html.HTMLDocument.HTMLReader.IsindexAction;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.content.IContentType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.ContentTypes;

/**
 * This class is responsible for loading/unloading result files
 * (scalar and vector files), based on changes in the model
 * (the Inputs object) and in the workspace. Thus it should be 
 * hooked up to listen to EMF and workspace changes.
 *
 * @author andras, tomi
 */
public class ResultFilesTracker implements INotifyChangedListener, IResourceChangeListener {

	private ResultFileManagerEx manager; //backreference to the manager it operates on
	private Inputs inputs; // backreference to the Inputs element we watch

	public ResultFilesTracker(ResultFileManagerEx manager, Inputs inputs) {
		this.manager = manager;
		this.inputs = inputs;
	}

	/**
	 * Listen to EMF model changes.
	 */
	public void notifyChanged(Notification notification) {
		if (notification.isTouch())
			return;
		
		switch (notification.getEventType()) {
		case Notification.ADD:
		case Notification.ADD_MANY:
		case Notification.REMOVE:
		case Notification.REMOVE_MANY:
		case Notification.MOVE:
		case Notification.SET:
		case Notification.UNSET:
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
		try {
			IResourceDelta delta = event.getDelta();
			delta.accept(new ResourceDeltaVisitor());
		} catch (CoreException e) {
			e.printStackTrace();
		}
	}
	
	class ResourceDeltaVisitor implements IResourceDeltaVisitor {
		public boolean visit(IResourceDelta delta) throws CoreException {
			IResource resource = delta.getResource();
			if (!(resource instanceof IFile))
				return true;
			
			IFile file = (IFile)resource;
			switch (delta.getKind()) {
			case IResourceDelta.ADDED:
					if (isResultFile(file) && inputsMatches(file))
						loadFile(file);
					break;
			case IResourceDelta.REMOVED:
					unloadFile(file);
					break;
			case IResourceDelta.CHANGED:
					unloadFile(file);
					if (isResultFile(file) && inputsMatches(file))
						loadFile(file);
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
		System.out.println("ResultFileTracker.synchronize()");
		// TODO: handle wildcards
		//XXX also: must unload files which have been removed from Inputs
		//FIXME for now, a primitive solution, TO BE REPLACED: unload everything
		for (ResultFile file : manager.getFiles().toArray())
			manager.unloadFile(file);
		for (Object inputFileObj : inputs.getInputs()) {
			String resourcePath = ((InputFile)inputFileObj).getName();
			loadFile(resourcePath);
		}
	}
	
	private void loadFile(String resourcePath) {
		if (resourcePath != null) {
			IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
			IResource resource = workspaceRoot.findMember(resourcePath);
			if (resource instanceof IFile) {
				IFile file = (IFile)resource;
				loadFile(file);
			}
		}
	}
	
	private void loadFile(IFile file) {
		System.out.println("loadFile: "+file);
		if (isResultFile(file)) {
			String osPath = file.getLocation().toOSString();
			manager.loadFile(file.getFullPath().toString(), osPath);
		}
		else 
			throw new RuntimeException("wrong file type:"+file.getFullPath()); //XXX proper error handling (e.g. remove file from Inputs?)
	}
	
	private void unloadFile(IFile file) {
		System.out.println("unloadFile: "+file);
		ResultFile resultFile = manager.getFile(file.getFullPath().toString());
		if (resultFile != null)
			manager.unloadFile(resultFile);
	}
	
	private boolean isResultFile(IFile file) {
		try {
			if (file.getContentDescription() != null) {
				IContentType contentType = file.getContentDescription().getContentType();
				return ContentTypes.SCALAR.equals(contentType.getId()) || ContentTypes.VECTOR.equals(contentType.getId());
			}
		} catch (CoreException e) {
			System.err.println("Cannot open resource: " + file.getFullPath()); //XXX proper error message
		}
		return false;
	}
	
	private boolean inputsMatches(IFile file) {
		for (InputFile inputfile : (List<InputFile>)inputs.getInputs())
			if (inputfile.getName().equals(file.getFullPath().toString()))
				return true;
		return false;
	}
}
