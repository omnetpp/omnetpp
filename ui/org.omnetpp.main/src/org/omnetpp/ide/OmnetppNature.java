package org.omnetpp.ide;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.ICommand;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IProjectNature;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.IConstants;

public class OmnetppNature implements IProjectNature {
	private IProject project;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#configure()
	 */
	public void configure() throws CoreException {
		IProjectDescription desc = project.getDescription();
		addBuilder(desc, IConstants.VECTORFILEINDEXER_BUILDER_ID);
		addBuilder(desc, IConstants.MAKEFILEBUILDER_BUILDER_ID);
		project.setDescription(desc, null);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#deconfigure()
	 */
	public void deconfigure() throws CoreException {
        IProjectDescription desc = project.getDescription();
        removeBuilder(desc, IConstants.VECTORFILEINDEXER_BUILDER_ID);
        removeBuilder(desc, IConstants.MAKEFILEBUILDER_BUILDER_ID);
        project.setDescription(desc, null);
	}
	
	public void addBuilder(IProjectDescription desc, String builderId) throws CoreException {
	    if (findBuilderIndex(desc.getBuildSpec(), builderId) == -1) {
	        ICommand command = desc.newCommand();
	        command.setBuilderName(builderId);
	        desc.setBuildSpec((ICommand[])ArrayUtils.add(desc.getBuildSpec(), 0, command));
	    }
	}
	
	private void removeBuilder(IProjectDescription desc, String builderId) throws CoreException {
        int index = findBuilderIndex(desc.getBuildSpec(), builderId);
        if (index != -1)
            desc.setBuildSpec((ICommand[])ArrayUtils.remove(desc.getBuildSpec(), index));
	}

	private int findBuilderIndex(ICommand[] commands, String builderId) {
        for (int i = 0; i < commands.length; ++i)
            if (commands[i].getBuilderName().equals(builderId))
                return i;
        return -1;
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#getProject()
	 */
	public IProject getProject() {
		return project;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#setProject(org.eclipse.core.resources.IProject)
	 */
	public void setProject(IProject project) {
		this.project = project;
	}

}
