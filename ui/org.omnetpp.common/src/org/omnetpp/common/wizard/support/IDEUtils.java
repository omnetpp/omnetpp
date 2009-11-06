package org.omnetpp.common.wizard.support;

import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;


/**
 * Static utility methods for the FreeMarker templates, exposed to the user 
 * via BeansWrapper.
 * 
 * IMPORTANT: This class must be backward compatible across OMNeT++ versions
 * at all times. DO NOT REMOVE FUNCTIONS OR CHANGE THEIR SIGNATURES; add new methods 
 * instead, if needed.
 * 
 * @author Andras
 */
public class IDEUtils {
	
	public static IWorkspaceRoot getWorkspaceRoot() {
		return ResourcesPlugin.getWorkspace().getRoot();
	}

//FIXME	
//	public static NEDResources getNEDResources() {
//		return NEDResourcesPlugin.getNEDResources();
//	}
//
//	public static MsgResources getMsgResources() {
//		return NEDResourcesPlugin.getMSGResources();
//	}

}
