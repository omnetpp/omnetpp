package org.omnetpp.launch;

import java.io.File;
import java.text.DateFormat;
import java.text.MessageFormat;
import java.util.Date;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;

public class StandaloneOmnetppSimulationLaunchConfiguration extends LaunchConfigurationDelegate {

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor)
            throws CoreException {
        if (monitor == null) {
            monitor = new NullProgressMonitor();
        }
        monitor.beginTask("Launching Simulation", 10); //$NON-NLS-1$
        // check for cancellation
        if (monitor.isCanceled())
            return;
        try {
            monitor.worked(1);
            System.out.println("Simulation launched. ");
            String wdAttr = LaunchPlugin.getWorkingDirectoryPath(configuration).toString();
            String projAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
            String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
            String argAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
            System.out.println("WD: " + wdAttr);
            System.out.println("Project: " + projAttr);
            System.out.println("Program: " + progAttr);
            System.out.println("Arguments: " + argAttr);
            System.out.println("========== exetuting ==========");
            IStringVariableManager varman = VariablesPlugin.getDefault().getStringVariableManager();
            String expandedWd = varman.performStringSubstitution(wdAttr);
            String expandedProg = varman.performStringSubstitution(progAttr);
            String expandedArg = varman.performStringSubstitution(argAttr);
            IFile executableFile = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(expandedProg));
            if (executableFile == null || !executableFile.exists()) {
                System.out.println("executable not exist");
                return;
            }

            System.out.println("cd " + expandedWd);
            String cmdLine = executableFile.getRawLocation().makeAbsolute().toString() + " " + expandedArg;
            System.out.println(cmdLine);
            monitor.worked(1);
            Process process = DebugPlugin.exec(DebugPlugin.parseArguments(cmdLine), new File(expandedWd));
            monitor.worked(7);
            DebugPlugin.newProcess(launch, process, renderProcessLabel(expandedProg));
        } finally {
            monitor.done();
        }

    }

    protected String renderProcessLabel(String commandLine) {
        String format = "{0} ({1})";
        String timestamp = DateFormat.getInstance().format(new Date(System.currentTimeMillis()));
        return MessageFormat.format(format, new Object[]{commandLine, timestamp});
    }
}
