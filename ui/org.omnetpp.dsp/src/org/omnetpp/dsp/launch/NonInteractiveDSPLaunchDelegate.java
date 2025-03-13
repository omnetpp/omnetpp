package org.omnetpp.dsp.launch;

import java.util.List;
import java.util.Map;
import java.util.function.Supplier;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.IDebugTarget;
import org.eclipse.jdt.annotation.Nullable;
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
    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, @Nullable IProgressMonitor monitor) throws CoreException {
        final var builder = new DSPLaunchDelegateLaunchBuilder(configuration, mode, launch, monitor);
        builder.launchNotConnect = DSPPlugin.DSP_MODE_LAUNCH
                .equals(configuration.getAttribute(DSPPlugin.ATTR_DSP_MODE, DSPPlugin.DSP_MODE_LAUNCH));
        builder.debugCmd = configuration.getAttribute(DSPPlugin.ATTR_DSP_CMD, "");
        builder.debugCmdArgs = configuration.getAttribute(DSPPlugin.ATTR_DSP_ARGS, List.of());
        builder.debugCmdEnvVars = DebugPlugin.getDefault().getLaunchManager().getEnvironment(configuration);
        builder.monitorDebugAdapter = configuration.getAttribute(DSPPlugin.ATTR_DSP_MONITOR_DEBUG_ADAPTER, false);
        builder.server = configuration.getAttribute(DSPPlugin.ATTR_DSP_SERVER_HOST, "");
        builder.port = configuration.getAttribute(DSPPlugin.ATTR_DSP_SERVER_PORT, 0);

        String dspParametersJson = configuration.getAttribute(DSPPlugin.ATTR_DSP_PARAM, "{}");
        try {
            final var jsonUtils = new JsonParserWithStringSubstitution(
                    VariablesPlugin.getDefault().getStringVariableManager());
            Map<String, Object> customParams = jsonUtils.parseJsonObjectAndRemoveNulls(dspParametersJson);
            builder.dspParameters.putAll(customParams);
        } catch (IllegalStateException e) {
            throw new CoreException(createErrorStatus("Json launch parameters were not correctly formatted.", null));
        }

        launch(builder);
    }

}
