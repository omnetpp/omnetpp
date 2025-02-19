package org.omnetpp.python;

import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.tm.internal.terminal.provisional.api.ITerminalConnector;
import org.eclipse.tm.terminal.connector.local.launcher.LocalLauncherDelegate;
import org.eclipse.tm.terminal.view.core.interfaces.constants.ITerminalsConnectorConstants;

public class PythonReplLauncherDelegate extends LocalLauncherDelegate {
    private static final String title = "OMNeT++ Python REPL";
    @Override
    public ITerminalConnector createTerminalConnector(Map<String, Object> properties) {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        String pythonReplCommand = System.getenv("OMNETPP_PYTHON_REPL_COMMAND");
        if (pythonReplCommand == null) {
            if (System.getenv("INET_ROOT") != null)
                pythonReplCommand = "cd $INET_ROOT && bin/inet_python_repl";
            else {
                IProject inetProject = root.getProject("inet");
                if (inetProject.exists())
                    pythonReplCommand = "cd " + inetProject.getLocation().toOSString() + " && bin/inet_python_repl";
                else
                    pythonReplCommand = "cd $__omnetpp_root_dir && bin/opp_python_repl";
            }
        }
        String authToken = Activator.getDefault().getAuthToken();
        String workspaceRoot = root.getLocation().toString();
        properties.put(ITerminalsConnectorConstants.PROP_FORCE_NEW, true);
        properties.put(ITerminalsConnectorConstants.PROP_ENCODING, "UTF-8");
        properties.put(ITerminalsConnectorConstants.PROP_TITLE, title);
        properties.put(ITerminalsConnectorConstants.PROP_TITLE_DISABLE_ANSI_TITLE, true);
        properties.put(ITerminalsConnectorConstants.PROP_PROCESS_WORKING_DIR, workspaceRoot);
        properties.put(ITerminalsConnectorConstants.PROP_PROCESS_ENVIRONMENT, new String[] {"AUTH_TOKEN=" + authToken, "WORKSPACE_ROOT=" + workspaceRoot});
        properties.put(ITerminalsConnectorConstants.PROP_PROCESS_MERGE_ENVIRONMENT, Boolean.TRUE);
        properties.put(ITerminalsConnectorConstants.PROP_PROCESS_ARGS, "-c '" + pythonReplCommand + "'");
        return super.createTerminalConnector(properties);
    }

    @Override
    public String getLabel() {
        return title;
    }

    protected String getDefaultTerminalTitle(Map<String, Object> properties) {
        return title;
    }
}
