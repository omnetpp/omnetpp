package org.omnetpp.common.ui;

import java.util.Collection;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;

/**
 * Content provider for showing a list or array of objects in a Table/Tree viewer.
 *
 * @author tomi
 */
public class ListContentProvider implements ITreeContentProvider {

	@SuppressWarnings("unchecked")
	public Object[] getElements(Object inputElement) {
		if (inputElement.getClass().isArray())
			return (Object[])inputElement;
		else if (inputElement instanceof Collection) {
			Collection<? extends Object> input = (Collection<? extends Object>)inputElement;
			return input.toArray(new Object[input.size()]);
		}
		else
			return ArrayUtils.EMPTY_OBJECT_ARRAY;
	}

	public Object getParent(Object element) {
		return null;
	}

	public boolean hasChildren(Object element) {
		return false;
	}

	public Object[] getChildren(Object parentElement) {
		return ArrayUtils.EMPTY_OBJECT_ARRAY;
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
	}
}
