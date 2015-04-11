/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.INedElement;
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
     * Returns true if some NED files are being loaded, or are scheduled to be loaded.
     */
    public boolean isLoadingInProgress();

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

    /**
     * Creates an immutable copy of the NED parse trees and all other data structures.
     * The immutable copy can be used for lengthy computations (e.g. validation or other
     * analysis) in a background thread without locking the INedResources object.
     *
     * Elements in the immutable copy can be related back to the original trees
     * with INedElement's getOriginal() method.
     *
     * Any model change event (NedModelChangeEvent) will cause subsequent calls to this method
     * to return a different, up-to-date instance. (Marker changes do not count.)
     *
     * Use isImmutableCopyUpToDate() to check whether an immutable copy you obtained
     * earlier is still current.
     */
    public INedTypeResolver getImmutableCopy();

    /**
     * Checks whether an immutable copy returned from an earlier getImmutableCopy() call
     * is still up to date.
     */
    public boolean isImmutableCopyUpToDate(INedTypeResolver copy);

    /**
     * Parses the given NED expression, and returns the parse tree. Returns null if the expression
     * contains a syntax error. The default implementation (NedResources) caches the result,
     * so it is typically very fast.
     */
    public INedElement getParsedNedExpression(String expression);

}
