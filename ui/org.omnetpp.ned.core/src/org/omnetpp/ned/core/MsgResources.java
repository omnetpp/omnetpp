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

public class MsgResources implements IMsgTypeResolver {
    private static final String MSG_EXTENSION = "msg";

    // associate IFiles with their NEDElement trees
    private final Map<IFile, MsgFileElementEx> msgFiles = new HashMap<IFile, MsgFileElementEx>();
    private final Map<MsgFileElementEx, IFile> msgElementFiles = new HashMap<MsgFileElementEx,IFile>();
    
    private final Map<String, IMsgTypeElement> msgTypes = new HashMap<String, IMsgTypeElement>();
    
    public MsgResources() {
        NEDElement.setDefaultMsgTypeResolver(this);
    }

    public synchronized void readAllMsgFiles() throws CoreException, IOException {
        IProject[] omnetppProjects = ProjectUtils.getOmnetppProjects();

        for (IProject project : omnetppProjects) {
            for (IFile file : getMsgFiles(project))
                if (!msgFiles.containsKey(file))
                    readMsgFile(file);
        }
    }

    public synchronized Set<IFile> getMsgFiles(IProject project) throws CoreException {
        final Set<IFile> files = new HashSet<IFile>();

        project.accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (isMsgFile(resource))
                    files.add((IFile)resource);
                return true;
            }
        });
        
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
        return resource instanceof IFile && MSG_EXTENSION.equalsIgnoreCase(((IFile)resource).getFileExtension());
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
}
