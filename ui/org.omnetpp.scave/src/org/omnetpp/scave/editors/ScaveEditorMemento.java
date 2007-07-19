package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.omnetpp.scave.ScavePlugin;

public class ScaveEditorMemento {
	
	private static final String QUALIFIER = ScavePlugin.PLUGIN_ID;
	
	private static final QualifiedName
		KEY_OPENED_OBJECT_COUNT = new QualifiedName(QUALIFIER, "OpenedCount"),
		KEY_OPENED_OBJECT = new QualifiedName(QUALIFIER, "Opened"),
		KEY_ACTIVE_PAGE_INDEX = new QualifiedName(QUALIFIER, "ActivePageIndex");
	
	private Resource resource; // remembered for generating/resolving EObject references
	
	/* state stored as persistent properties of the .scave file */
	public EObject[] openedObjects;
	public int activePageIndex;
	
	public ScaveEditorMemento(ScaveEditor editor) {
		Assert.isLegal(editor != null);
		resource = editor.getResource();
	}
	
	public void saveState(IFile file) throws CoreException {
		int count = openedObjects.length;
		file.setPersistentProperty(KEY_OPENED_OBJECT_COUNT, String.valueOf(count));
		for (int i = 0; i < count; ++i) {
			QualifiedName key = KEY_OPENED_OBJECT;
			EObject object = openedObjects[i];
			String id = resource.getURIFragment(object);
			file.setPersistentProperty(new QualifiedName(key.getQualifier(), key.getLocalName()+i), id);
		}
		
		file.setPersistentProperty(KEY_ACTIVE_PAGE_INDEX, String.valueOf(activePageIndex));
	}
	
	public void restoreState(IFile file) throws CoreException {
		int count = readInt(file, KEY_OPENED_OBJECT_COUNT);
		List<EObject> openedObjects = new ArrayList<EObject>(count);
		for (int i = 0; i < count; ++i) {
			QualifiedName key = KEY_OPENED_OBJECT;
			String id = file.getPersistentProperty(new QualifiedName(key.getQualifier(), key.getLocalName()+i));
			if (id == null)
				error("Cannot restore editor state, missing"+key, null);
			EObject object = resource.getEObject(id);
			if (object != null)
				openedObjects.add(object);
		}
		
		this.openedObjects = openedObjects.toArray(new EObject[openedObjects.size()]);
		this.activePageIndex = readInt(file, KEY_ACTIVE_PAGE_INDEX);
	}
	
	private int readInt(IFile file, QualifiedName key) throws CoreException {
		try {
			return Integer.parseInt(file.getPersistentProperty(key));
		} catch (NumberFormatException e) {
			error("Cannot restore editor state, wrong "+key, e);
			return -1; // not reached
		}
	}
	
	private void error(String msg, Throwable e) throws CoreException {
		throw new CoreException(new Status(IStatus.ERROR, ScavePlugin.PLUGIN_ID, 0, msg, e));
	}
}
