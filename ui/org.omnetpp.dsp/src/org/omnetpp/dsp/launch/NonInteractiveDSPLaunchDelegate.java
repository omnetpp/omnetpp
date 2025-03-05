package org.omnetpp.dsp.launch;

import java.util.Map;
import java.util.function.Supplier;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.model.IDebugTarget;
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

}
