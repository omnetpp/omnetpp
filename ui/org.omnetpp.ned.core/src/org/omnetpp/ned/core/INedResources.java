/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.notification.INEDChangeListener;

/**
 * Parses all NED files in the workspace and makes them available for other
 * plugins for consistency checks among NED files etc.
 *
 * It listens to workspace resource changes and modifies its content based on
 * change notifications.
 *
 * @author andras
 */
public interface INedResources extends INEDTypeResolver {
    // utilities for predicate-based filtering of NED types using getAllNedTypes()
    public static class InstanceofPredicate implements IPredicate {
        private Class<? extends INedTypeElement> clazz;
        public InstanceofPredicate(Class<? extends INedTypeElement> clazz) {
            this.clazz = clazz;
        }
        public boolean matches(INEDTypeInfo component) {
            return clazz.isInstance(component.getNEDElement());
        }
    };
    public static final IPredicate MODULE_FILTER = new InstanceofPredicate(IModuleTypeElement.class);
    public static final IPredicate SIMPLE_MODULE_FILTER = new InstanceofPredicate(SimpleModuleElementEx.class);
    public static final IPredicate COMPOUND_MODULE_FILTER = new InstanceofPredicate(CompoundModuleElementEx.class);
    public static final IPredicate MODULEINTERFACE_FILTER = new InstanceofPredicate(ModuleInterfaceElementEx.class);
    public static final IPredicate CHANNEL_FILTER = new InstanceofPredicate(ChannelElementEx.class);
    public static final IPredicate CHANNELINTERFACE_FILTER = new InstanceofPredicate(ChannelInterfaceElementEx.class);
    public static final IPredicate NETWORK_FILTER = new IPredicate() {
        public boolean matches(INEDTypeInfo component) {
            return component.getNEDElement() instanceof IModuleTypeElement &&
                   ((IModuleTypeElement)component.getNEDElement()).isNetwork();
        }
    };
    
    
    public boolean isRefactoringInProgress();
    public void setRefactoringInProgress(boolean refactoringInProgress);
    
    public Set<IFile> getNedFiles(IProject project);
    public boolean containsNedFileElement(IFile file);
    public void setNedFileText(IFile file, String text);
    public boolean isNedFile(IResource resource);
    
    public void connect(IFile file);
    public void disconnect(IFile file);
    public int getConnectCount(IFile file);
    public boolean hasConnectedEditor(IFile file);
    
    public void readNEDFile(IFile file);
    public void forgetNEDFile(IFile file);
    public void validateAllFiles();
    public void fireBeginChangeEvent();
    public void fireEndChangeEvent();
    public void runWithBeginEndNotification(Runnable runnable);
    public void invalidate();
    public void rebuildProjectsTable();
    public void scheduleReadMissingNedFiles();
    public void readMissingNedFiles();

    public void addNEDModelChangeListener(INEDChangeListener listener);
    public void removeNEDModelChangeListener(INEDChangeListener listener);
    
    public void dumpProjectsTable();
}
