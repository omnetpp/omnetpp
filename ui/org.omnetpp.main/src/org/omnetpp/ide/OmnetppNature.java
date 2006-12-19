package org.omnetpp.ide;

import org.eclipse.core.resources.ICommand;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IProjectNature;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.ned.resources.builder.NEDBuilder;
import org.omnetpp.scave.builder.VectorFileIndexer;

public class OmnetppNature implements IProjectNature {

	/**
	 * ID of this project nature
	 */
	public static final String NATURE_ID = "org.omnetpp.main.omnetppnature";

	private IProject project;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#configure()
	 */
	public void configure() throws CoreException {
		IProjectDescription desc = project.getDescription();
		ICommand[] commands = desc.getBuildSpec();

		boolean hasNEDBuidler = false;
		boolean hasIndexer = false;
		for (int i = 0; i < commands.length; ++i) {
			if (commands[i].getBuilderName().equals(NEDBuilder.BUILDER_ID))
				hasNEDBuidler = true;
			else if (commands[i].getBuilderName().equals(VectorFileIndexer.BUILDER_ID))
				hasIndexer = true;
			if (hasNEDBuidler && hasIndexer)
				break;
		}
		
		if (!hasNEDBuidler)
			addBuilderToProject(NEDBuilder.BUILDER_ID);
		if (!hasIndexer)
			addBuilderToProject(VectorFileIndexer.BUILDER_ID);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#deconfigure()
	 */
	public void deconfigure() throws CoreException {
		removeBuilderFromProject(NEDBuilder.BUILDER_ID);
		removeBuilderFromProject(VectorFileIndexer.BUILDER_ID);
	}
	
	public void addBuilderToProject(String id) throws CoreException {
		IProjectDescription desc = project.getDescription();
		ICommand[] commands = desc.getBuildSpec();
		ICommand[] newCommands = new ICommand[commands.length + 1];
		System.arraycopy(commands, 0, newCommands, 0, commands.length);
		ICommand command = desc.newCommand();
		command.setBuilderName(id);
		newCommands[newCommands.length - 1] = command;
		desc.setBuildSpec(newCommands);
		project.setDescription(desc, null);
		
	}
	
	private void removeBuilderFromProject(String id) throws CoreException {
		IProjectDescription description = getProject().getDescription();
		ICommand[] commands = description.getBuildSpec();
		for (int i = 0; i < commands.length; ++i) {
			if (commands[i].getBuilderName().equals(id)) {
				ICommand[] newCommands = new ICommand[commands.length - 1];
				System.arraycopy(commands, 0, newCommands, 0, i);
				System.arraycopy(commands, i + 1, newCommands, i,
						commands.length - i - 1);
				description.setBuildSpec(newCommands);
				project.setDescription(description, null);
				return;
			}
		}
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
