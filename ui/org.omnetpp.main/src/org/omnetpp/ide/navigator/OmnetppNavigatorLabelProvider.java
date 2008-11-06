package org.omnetpp.ide.navigator;

import org.eclipse.core.resources.IResource;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.navigator.ICommonContentExtensionSite;
import org.eclipse.ui.navigator.ICommonLabelProvider;

/**
 * label provider for the omnetpp specific navigator extension
 *
 */
public class OmnetppNavigatorLabelProvider extends WorkbenchLabelProvider implements ICommonLabelProvider {

	public void init(ICommonContentExtensionSite aConfig) {
	}

	public String getDescription(Object anElement) {
		// TODO add a little more description for the omnetpp specific files
		if (anElement instanceof IResource) {
			return ((IResource) anElement).getFullPath().makeRelative().toString();
		}
		return null;
	}
	
	public void restoreState(IMemento aMemento) { 
		
	}

	public void saveState(IMemento aMemento) { 
	}

}
