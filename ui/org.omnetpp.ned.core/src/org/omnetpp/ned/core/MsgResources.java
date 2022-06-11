/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.SysoutNedErrorStore;
import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeInfo;
import org.omnetpp.ned.model.interfaces.IMsgTypeResolver;
import org.omnetpp.ned.model.pojo.NamespaceElement;

/**
 * Parses all Msg files in the workspace and makes them available for other plugins.
 *
 * It listens to workspace resource changes and modifies its content based on
 * change notifications.
 *
 * @author levy
 */
public class MsgResources implements IMsgTypeResolver, IResourceChangeListener {
    private static final String MSG_EXTENSION = "msg";

    // singleton instance
    private static MsgResources instance = null;

    // associate IFiles with their NedElement trees
    private final Map<IFile, MsgFileElementEx> msgFiles = new HashMap<IFile, MsgFileElementEx>();
    private final Map<MsgFileElementEx, IFile> msgElementFiles = new HashMap<MsgFileElementEx,IFile>();

    // associate types declared in Msg files to their names
    private final Map<String, IMsgTypeElement> msgTypes = new HashMap<String, IMsgTypeElement>();

    protected MsgResources() {
        NedElement.setDefaultMsgTypeResolver(this);
        rebuild();
        // register as a workspace listener
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    public void dispose() {
        // remove ourselves from the listener list
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }

    public static MsgResources getInstance() {
        if (instance == null)
            instance = new MsgResources();
        return instance;
    }

    private void rebuild() {
        // forget those files which are no longer in our projects or NED folders
        // Note: use "trash" list to avoid ConcurrentModificationException in msgFiles
        List <IFile> trash = new ArrayList<IFile>();

        for (IFile file : msgFiles.keySet())
            if (!isMsgFile(file))
                trash.add(file);
        try {
            for (IFile file : trash)
                forgetMsgFile(file);
        }
        finally {
        }

        scheduleReadAllMsgFiles();
    }

    /**
     * Read all Msg files in a separate job, because it might take a long time.
     */
    private void scheduleReadAllMsgFiles() {
        WorkspaceJob job = new WorkspaceJob("Parsing Msg files...") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                readAllMsgFiles();
                return Status.OK_STATUS;
            }
        };
        job.setRule(ResourcesPlugin.getWorkspace().getRoot());
        job.setPriority(Job.SHORT);
        job.setSystem(false);
        job.schedule();
    }

    public synchronized void readAllMsgFiles() throws CoreException {
        IProject[] omnetppProjects = ProjectUtils.getOmnetppProjects();
        for (IProject project : omnetppProjects) {
            for (IFile file : getMsgFiles(project)) {
                if (!msgFiles.containsKey(file)) {
                    try {
                        readMsgFile(file);
                    }
                    catch (IOException e) {
                        NedResourcesPlugin.logError("Error during reading message file: "+file.getName(), e);
                    }
                    catch (CoreException e) {
                        NedResourcesPlugin.logError("Error during gathering message files: "+file.getName(), e);
                    }
                }
            }
        }
    }

    public synchronized Set<IFile> getMsgFiles(IProject project) {
        final Set<IFile> files = new HashSet<IFile>();

        try {
            project.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (isMsgFile(resource))
                        files.add((IFile)resource);
                    return true;
                }
            });
        }
        catch (CoreException e) {
            NedResourcesPlugin.logError("Error during gathering message files", e);
        }
        return files;
    }

    public synchronized MsgFileElementEx getMsgFileElement(IFile file) throws IOException, CoreException {
        Assert.isTrue(isMsgFile(file), "file is not a MSG file");

        if (!msgFiles.containsKey(file))
            readMsgFile(file);

        return msgFiles.get(file);
    }

    public synchronized IFile getMsgFile(MsgFileElementEx msgFileElement) {
        return msgElementFiles.get(msgFileElement);
    }

    public synchronized boolean isMsgFile(IResource resource) {
        return resource instanceof IFile &&
               MSG_EXTENSION.equalsIgnoreCase(((IFile)resource).getFileExtension()) &&
               NedResourcesPlugin.getNedResources().getNedSourceFolderFor((IFile)resource) != null;
    }

    public synchronized IMsgTypeElement lookupMsgType(String name) {
        return msgTypes.get(name);
    }

    public IMsgTypeInfo createTypeInfoFor(IMsgTypeElement node) {
        return new MsgTypeInfo(node);
    }

    public synchronized void forgetMsgFile(IFile file) {
        if (msgFiles.containsKey(file)) {
            MsgFileElementEx element = msgFiles.get(file);
            msgFiles.remove(file);
            msgElementFiles.remove(element);

            // purge cached types
            for (IMsgTypeElement typeElement : element.getTopLevelTypeNodes())
                msgTypes.remove(typeElement.getMsgTypeInfo().getFullyQualifiedCppClassName());
        }
    }

    private void readMsgFile(IFile file) throws IOException, CoreException {
        // forget types that used to be in this msg file
        forgetMsgFile(file);

        // register msg file
        String source = FileUtils.readTextFile(file.getContents(), file.getCharset());
        MsgFileElementEx element = NedTreeUtil.parseMsgSource(source, new SysoutNedErrorStore(), file.toString());  //TODO why SysoutNedErrorStore?
        msgFiles.put(file, element);
        msgElementFiles.put(element, file);

        // register types in msg file
        String currentNamespace = null;
        for (INedElement child : element.getChildren()) {
            if (child instanceof NamespaceElement)
                currentNamespace = ((NamespaceElement) child).getName();
            else if (child instanceof IMsgTypeElement) {
                IMsgTypeElement typeElement = (IMsgTypeElement)child;
                String name = typeElement.getName();
                String fullyQualifieldCppName = currentNamespace == null ? name : currentNamespace + "::" + name;
                msgTypes.put(fullyQualifieldCppName, typeElement);
            }
        }
    }

    /**
     * Synchronize the plugin with the resources in the workspace
     */
    public synchronized void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null)
                return;

            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    try {
                        IResource resource = delta.getResource();
                        // printDelta(delta);
                        if (isMsgFile(resource)) {
                            IFile file = (IFile)resource;
                            switch (delta.getKind()) {
                            case IResourceDelta.REMOVED:
                                forgetMsgFile(file);
                                break;
                            case IResourceDelta.ADDED:
                                readMsgFile(file);
                                break;
                            case IResourceDelta.CHANGED:
                                // we are only interested in content changes; ignore marker and property changes
                                if ((delta.getFlags() & IResourceDelta.CONTENT) != 0)
                                    readMsgFile(file);
                                break;
                            }
                        }
                        else if (ProjectUtils.isNedFoldersFile(resource)) {
                            rebuild();
                        }
                        else if (resource instanceof IProject) {
                            switch (delta.getKind()) {
                            case IResourceDelta.REMOVED:
                            case IResourceDelta.ADDED:
                            case IResourceDelta.OPEN:
                                rebuild();
                                break;
                            case IResourceDelta.CHANGED:
                                // change in natures and referenced projects will be reported as description changes
                                if ((delta.getFlags() & IResourceDelta.DESCRIPTION) != 0)
                                    rebuild();
                                break;
                            }
                        }
                        return true;
                    }
                    catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }
            });
        }
        catch (CoreException e) {
            NedResourcesPlugin.logError("Error during workspace change notification: ", e);
        }
    }
}
