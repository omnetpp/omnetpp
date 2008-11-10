package org.omnetpp.cdt.makefile;

import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.makegen.IManagedBuilderMakefileGenerator;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.MultiStatus;

public class MakefileGenerator implements IManagedBuilderMakefileGenerator {
    protected IProject project;
    protected IManagedBuildInfo info;
    protected IProgressMonitor monitor;
    
    public MakefileGenerator() {
        System.out.println("CTOR");    
    }
    
    public void initialize(IProject project, IManagedBuildInfo info, IProgressMonitor monitor) {
        System.out.println("%%% initialize");
        this.project = project;
        this.info = info;
        this.monitor = monitor;
    }


    public MultiStatus generateMakefiles(IResourceDelta delta) throws CoreException {
        System.out.println("%%% generateMakefiles");
        // TODO Auto-generated method stub
        return null;
    }

    public MultiStatus regenerateMakefiles() throws CoreException {
        System.out.println("%%% regenerateMakefiles");
        // TODO Auto-generated method stub
        return null;
    }

    public void generateDependencies() throws CoreException {
        System.out.println("%%% generateDependencies");
        // TODO Auto-generated method stub
    }

    public void regenerateDependencies(boolean force) throws CoreException {
        System.out.println("%%% regenerateDependencies");
        // TODO Auto-generated method stub
    }

    public IPath getBuildWorkingDir() {
        System.out.println("%%% getBuildWorkingDir");
        // TODO Auto-generated method stub
        return project.getFullPath().removeFirstSegments(1);
    }

    public String getMakefileName() {
        System.out.println("%%% getMakefileName");
        // TODO Auto-generated method stub
        return null;
    }

    public boolean isGeneratedResource(IResource resource) {
        System.out.println("%%% isGeneratedResource " + resource);
        // TODO Auto-generated method stub
        return false;
    }


}
