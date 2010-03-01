/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.INedChangeListener;

/**
 * Represents all NED files in the workspace, and makes them available. 
 * It listens to workspace resource changes and modifies its content based on
 * change notifications.
 *
 * @author andras
 */
public interface INedResources extends INedTypeResolver {
    // utilities for predicate-based filtering of NED types using getAllNedTypes()
    public static class InstanceofPredicate implements IPredicate {
        private Class<? extends INedTypeElement> clazz;
        public InstanceofPredicate(Class<? extends INedTypeElement> clazz) {
            this.clazz = clazz;
        }
        public boolean matches(INedTypeInfo component) {
            return clazz.isInstance(component.getNedElement());
        }
    };
    public static final IPredicate MODULE_FILTER = new InstanceofPredicate(IModuleTypeElement.class);
    public static final IPredicate SIMPLE_MODULE_FILTER = new InstanceofPredicate(SimpleModuleElementEx.class);
    public static final IPredicate COMPOUND_MODULE_FILTER = new InstanceofPredicate(CompoundModuleElementEx.class);
    public static final IPredicate MODULEINTERFACE_FILTER = new InstanceofPredicate(ModuleInterfaceElementEx.class);
    public static final IPredicate CHANNEL_FILTER = new InstanceofPredicate(ChannelElementEx.class);
    public static final IPredicate CHANNELINTERFACE_FILTER = new InstanceofPredicate(ChannelInterfaceElementEx.class);
    public static final IPredicate NETWORK_FILTER = new IPredicate() {
        public boolean matches(INedTypeInfo component) {
            return component.getNedElement() instanceof IModuleTypeElement &&
                   ((IModuleTypeElement)component.getNedElement()).isNetwork();
        }
    };
    
    /**
     * Adds a listener that will be notified about changes anywhere
     * in the loaded NED files.
     */
    public void addNedModelChangeListener(INedChangeListener listener);
    
    /**
     * Removes the listener added with addNedModelChangeListener().
     */
    public void removeNedModelChangeListener(INedChangeListener listener);
    
    /**
     * NED editors should call this when they get opened.
     */
    public void connect(IFile file);
    
    /**
     * NED editors should call this when they get closed.
     */
    public void disconnect(IFile file);
    
    /**
     * Returns the number of editors editing the given NED file.
     */
    public int getConnectCount(IFile file);
    
    /**
     * Returns true if there are editors editing the given NED file.
     */
    public boolean hasConnectedEditor(IFile file);
    
    /**
     * NED text editors should call this when editor content changes.
     * Parses the given text, and synchronizes the stored NED model tree to it.
     * It does NOT save the file to the disk.
     */
    public void setNedFileText(IFile file, String text);

    /**
     * Should be called at the beginning of a batch update to a NED tree.
     * It fires a NedBeginModelChangeEvent with source == null.
     * Begin/end notifications enable listeners to optimize display updates, 
     * i.e. they may choose to only update the display after receiving the
     * matching NedEndModelChangeEvent. fireBeginChangeEvent() and
     * fireEndChangeEvent() must be fired in pairs, i.e. fireEndChangeEvent()
     * must be fired even if there is an exception if the code path between 
     * the two calls.
     */
    public void fireBeginChangeEvent();
    
    /**
     * Fires a NedBeginModelChangeEvent with source == null. Allows for 
     * optimizing batch updates of a NED tree; see fireBeginChangeEvent().
     */
    public void fireEndChangeEvent();
    
    /**
     * Executes the given runnable, surrounding it with fireBeginChangeEvent() 
     * and fireEndChangeEvent() calls. The latter is called even if the runnable
     * throws an exception.
     */
    public void runWithBeginEndNotification(Runnable runnable);

    /**
     * Operations that change the NED tree without having it open in an editor
     * (see connect()/disconnect()) must call this method with argument==true
     * before the changes, and with argument==false afterwards. Calls cannot be nested. 
     */
    public void setRefactoringInProgress(boolean refactoringInProgress);

    /**
     * Returns true if refactoring is in progress, as denoted with
     * setRefactoringInProgress() calls.
     */
    public boolean isRefactoringInProgress();
}
