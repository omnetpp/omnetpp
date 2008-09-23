package org.omnetpp.ned.core;

import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.SysoutNedErrorStore;
import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeInfo;
import org.omnetpp.ned.model.interfaces.IMsgTypeResolver;
import org.omnetpp.ned.model.pojo.NamespaceElement;

/**
 * FIXME levy: please add description, to the class, and to the methods!!!!
 */
public class MsgResources implements IMsgTypeResolver {
    private static final String NAMESPACE_PROPERTY = "namespace";
    private static final String MSG_EXTENSION = "msg";
    
    // instance variable
    private static MsgResources instance = null;

    // associate IFiles with their NEDElement trees
    private final Map<IFile, MsgFileElementEx> msgFiles = new HashMap<IFile, MsgFileElementEx>();
    private final Map<MsgFileElementEx, IFile> msgElementFiles = new HashMap<MsgFileElementEx,IFile>();
    
    private final Map<String, IMsgTypeElement> msgTypes = new HashMap<String, IMsgTypeElement>();
    
    protected MsgResources() {
        NEDElement.setDefaultMsgTypeResolver(this);
        readAllMsgFiles();
    }
    
    public static MsgResources getInstance() {
        if (instance == null) 
            instance = new MsgResources();
        return instance;
    }

    public synchronized void readAllMsgFiles() {
        IProject[] omnetppProjects = ProjectUtils.getOmnetppProjects();

        for (IProject project : omnetppProjects) {
            for (IFile file : getMsgFiles(project))
                if (!msgFiles.containsKey(file))
                    try {
                        readMsgFile(file);
                    }
                    catch (IOException e) {
                        NEDResourcesPlugin.logError("Error during reading message file: "+file.getName(), e);
                    }
                    catch (CoreException e) {
                        NEDResourcesPlugin.logError("Error during gathering message files: "+file.getName(), e);
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
            NEDResourcesPlugin.logError("Error during gathering message files", e);
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
               NEDResourcesPlugin.getNEDResources().getNedSourceFolderFor((IFile)resource) != null;
    }

    public synchronized IMsgTypeElement lookupMsgType(String name) {
        return msgTypes.get(name);
    }

    public IMsgTypeInfo createTypeInfoFor(IMsgTypeElement node) {
        return new MsgTypeInfo(node);
    }

    private void readMsgFile(IFile file) throws IOException, CoreException {
        String source = FileUtils.readTextFile(file.getContents());
        MsgFileElementEx element = NEDTreeUtil.parseMsgSource(source, new SysoutNedErrorStore(), file.toString());
        msgFiles.put(file, element);
        msgElementFiles.put(element, file);
        
        for (IMsgTypeElement typeElement : element.getTopLevelTypeNodes())
            msgTypes.put(typeElement.getName(), typeElement);
    }

    public String getCppNamespaceForFile(MsgFileElementEx element) {
        NamespaceElement namespace = element.getFirstNamespaceChild();

        if (namespace != null)
            return namespace.getName();
        else
            return null;
    }
}
