package org.omnetpp.dsp.launch;

import java.util.List;
import java.util.Map;
import java.util.function.Supplier;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.IDebugTarget;
import org.eclipse.lsp4e.debug.DSPPlugin;
import org.eclipse.lsp4e.debug.debugmodel.JsonParserWithStringSubstitution;
import org.eclipse.lsp4e.debug.debugmodel.TransportStreams;
import org.eclipse.lsp4e.debug.launcher.DSPLaunchDelegate;
import org.omnetpp.dsp.debug.debugmodel.SimulationDSPDebugTarget;

public class NonInteractiveDSPLaunchDelegate extends DSPLaunchDelegate {
	
	@SuppressWarnings("restriction")
	@Override
	protected IDebugTarget createDebugTarget(SubMonitor subMonitor, Supplier<TransportStreams> streamsSupplier,
			ILaunch launch, Map<String, Object> dspParameters) throws CoreException {
		final var target = new SimulationDSPDebugTarget(launch, streamsSupplier, dspParameters);
		target.initialize(subMonitor.split(80));
		return target;
	}

    @Override
    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {
        final var builder = new DSPLaunchDelegateLaunchBuilder(configuration, mode, launch, monitor);
        var launchNotConnect = DSPPlugin.DSP_MODE_LAUNCH.equals(configuration.getAttribute(DSPPlugin.ATTR_DSP_MODE, DSPPlugin.DSP_MODE_LAUNCH));
        if (launchNotConnect) {
            var debugCmd = configuration.getAttribute(DSPPlugin.ATTR_DSP_CMD, "");
            var debugCmdArgs = configuration.getAttribute(DSPPlugin.ATTR_DSP_ARGS, List.of());
            var debugCmdEnvVars = DebugPlugin.getDefault().getLaunchManager().getEnvironment(configuration);
            builder.setLaunchDebugAdapter(debugCmd, debugCmdArgs, debugCmdEnvVars);
        }
        else {
            var server = configuration.getAttribute(DSPPlugin.ATTR_DSP_SERVER_HOST, "");
            var port = configuration.getAttribute(DSPPlugin.ATTR_DSP_SERVER_PORT, 0);
            builder.setAttachDebugAdapter(server, port);
        }
        boolean monitorDebugAdapter = configuration.getAttribute(DSPPlugin.ATTR_DSP_MONITOR_DEBUG_ADAPTER, false);
        builder.setMonitorDebugAdapter(monitorDebugAdapter);

        String dspParametersJson = configuration.getAttribute(DSPPlugin.ATTR_DSP_PARAM, "{}");
        try {
            final var jsonUtils = new JsonParserWithStringSubstitution(
                    VariablesPlugin.getDefault().getStringVariableManager());
            Map<String, Object> customParams = jsonUtils.parseJsonObjectAndRemoveNulls(dspParametersJson);
            builder.setDspParameters(customParams);
        } catch (IllegalStateException e) {
            var status = new Status(IStatus.ERROR, DSPPlugin.PLUGIN_ID, 0, "Json launch parameters were not correctly formatted.", null);
            throw new CoreException(status);
        }

        launch(builder);
    }
}
