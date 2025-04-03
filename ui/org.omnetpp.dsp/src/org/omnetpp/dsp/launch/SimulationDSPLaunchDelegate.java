/*--------------------------------------------------------------*
  Copyright (C) 2025 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.dsp.launch;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.function.Supplier;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugException;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchListener;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.core.model.ILaunchConfigurationDelegate;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.lsp4e.debug.DSPPlugin;
import org.eclipse.lsp4e.debug.debugmodel.TransportStreams;
import org.eclipse.lsp4e.debug.debugmodel.TransportStreams.DefaultTransportStreams;
import org.eclipse.lsp4e.debug.debugmodel.TransportStreams.SocketTransportStreams;
import org.eclipse.osgi.util.NLS;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.dsp.Activator;
import org.omnetpp.dsp.debug.debugmodel.SimulationDSPDebugTarget;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.SimulationRunLaunchDelegate;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.JsonPrimitive;

public class SimulationDSPLaunchDelegate extends SimulationRunLaunchDelegate {
		
	@SuppressWarnings("restriction")
	@Override
	public void launch(ILaunchConfiguration config, String mode, ILaunch launch,
			IProgressMonitor monitor) throws CoreException {

		// convert configuration to be suitable for CDT's GDB or LSP4E's DAP debugger
        var wcfg = OmnetppLaunchUtils.createUpdatedLaunchConfig(config, mode, true);
        OmnetppLaunchUtils.replaceConfigurationInLaunch(launch, wcfg);

        // setup launch listener 
        ILaunchManager lm = DebugPlugin.getDefault().getLaunchManager();
        lm.addLaunchListener(new ILaunchListener() {

            @Override
            public void launchRemoved(ILaunch launch) {
                // nothing
            }

            @Override
            public void launchChanged(ILaunch launchParam) {
                if (launchParam == launch) {
                    // the first process is the debugger, the second is the debuggee (the simulation)
                    if (launch.getProcesses().length >= 2)
                    {
                        try {
                            String workingDir = wcfg.getAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, "");
                            String commandLine = wcfg.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "")
                                        + " " + wcfg.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");

                            OmnetppLaunchUtils.printToConsole(launch.getProcesses()[1], "Debugging...\n\n$ cd "+workingDir+"\n$ "+commandLine+"\n\n", false);
                        } catch (CoreException e) {
                            e.printStackTrace();
                        }

                        lm.removeLaunchListener(this);
                    }
                }
            }

            @Override
            public void launchAdded(ILaunch launch) {
                // nothing
            }
        });        
                
        // setup DSP specific attributes
		final var dspConfig = new DSPConfig(wcfg, mode, launch, monitor);
		dspConfig.launchNotConnect = DSPPlugin.DSP_MODE_LAUNCH
				.equals(wcfg.getAttribute(DSPPlugin.ATTR_DSP_MODE, DSPPlugin.DSP_MODE_LAUNCH));
		dspConfig.debugCmd = StringUtils.substituteVariables(wcfg.getAttribute(IOmnetppLaunchConstants.ATTR_DSP_CMD, IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_NAME));

        List<String> dspargs = new ArrayList<String>(wcfg.getAttribute(IOmnetppLaunchConstants.ATTR_DSP_ARGS, IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_ARGS));
        dspargs.replaceAll(arg -> {
			try {
				return StringUtils.substituteVariables(arg);
			} catch (CoreException e) {
				// return the variable unchanged if substitution fails
				return arg;
			}
		});
        dspConfig.debugCmdArgs = dspargs;
        
		dspConfig.debugCmdEnvVars = DebugPlugin.getDefault().getLaunchManager().getEnvironment(wcfg);
		dspConfig.monitorDebugAdapter = wcfg.getAttribute(DSPPlugin.ATTR_DSP_MONITOR_DEBUG_ADAPTER, false);
		dspConfig.server = wcfg.getAttribute(DSPPlugin.ATTR_DSP_SERVER_HOST, "");
		dspConfig.port = wcfg.getAttribute(DSPPlugin.ATTR_DSP_SERVER_PORT, 0);

		try {
			final JsonParserWithStringSubstitution jsonUtils = new JsonParserWithStringSubstitution(
					VariablesPlugin.getDefault().getStringVariableManager());
			Map<String, Object> customParams = jsonUtils.parseJsonObjectAndRemoveNulls(wcfg.getAttribute(DSPPlugin.ATTR_DSP_PARAM, "{}"));
			dspConfig.dspParameters.putAll(customParams);			
		} catch (IllegalStateException e) {
			throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0,"Json launch parameters were not correctly formatted.", null));
		}
		

		final var subMonitor = SubMonitor.convert(dspConfig.monitor, 100);

		// handle custom DSP launch parameters specified on the "Debug Adapter" tab
		boolean customDebugAdapter = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_CUSTOM_DEBUG_ADAPTER, false);
		boolean customLaunchParameters = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_CUSTOM_LAUNCH_PARAMS, false);

		if (customDebugAdapter) {
			dspConfig.launchNotConnect = DSPPlugin.DSP_MODE_LAUNCH
					.equals(dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_MODE, DSPPlugin.DSP_MODE_LAUNCH));
			dspConfig.debugCmd = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_CMD, "");
			dspConfig.debugCmdArgs = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_ARGS, List.of());
			dspConfig.debugCmdEnvVars = DebugPlugin.getDefault().getLaunchManager().getEnvironment(dspConfig.configuration);
			dspConfig.monitorDebugAdapter = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_MONITOR_DEBUG_ADAPTER,
					false);
			dspConfig.server = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_SERVER_HOST, "");
			dspConfig.port = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_SERVER_PORT, 0);
		}
		if (customLaunchParameters) {
			String dspParJson = dspConfig.configuration.getAttribute(DSPPlugin.ATTR_DSP_PARAM, "");
			if (!dspParJson.isBlank()) {
				try {
					final var jsonUtils = new JsonParserWithStringSubstitution(
							VariablesPlugin.getDefault().getStringVariableManager());
					Map<String, Object> customParams = jsonUtils.parseJsonObjectAndRemoveNulls(dspParJson);
					dspConfig.dspParameters.putAll(customParams);
				} catch (IllegalStateException | CoreException e) {
					throw new CoreException(
							new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "Json launch parameters were not correctly formatted.", null));
				}					
			}
		}

		// DSP supports run/debug as a simple flag to the debug server.
		// See LaunchRequestArguments.noDebug
		if (ILaunchManager.DEBUG_MODE.equals(dspConfig.mode)) {
			subMonitor.setTaskName("Starting debug session");
			dspConfig.dspParameters.put("noDebug", false);
		} else if (ILaunchManager.RUN_MODE.equals(dspConfig.mode)) {
			subMonitor.setTaskName("Starting run session");
			dspConfig.dspParameters.put("noDebug", true);
		} else {
			throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, NLS.bind("Unsupported launch mode '{0}'.", dspConfig.mode), null));
		}

		final Supplier<TransportStreams> streamSupplier;
		try {
			if (dspConfig.launchNotConnect) {
				InputStream inputStream;
				OutputStream outputStream;
				Runnable cleanup;
				final var command = new ArrayList<String>();

				final var debugCmd = dspConfig.debugCmd;
				if (debugCmd == null || debugCmd.isBlank()) {
					throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "Debug command unspecified.", null));
				}
				command.add(StringUtils.substituteVariables(debugCmd));

				final var debugCmdArgs = dspConfig.debugCmdArgs;
				if (debugCmdArgs != null && !debugCmdArgs.isEmpty()) 
				    for (var debugCmdArg : debugCmdArgs) 
				    	command.add(StringUtils.substituteVariables(debugCmdArg));
				
				subMonitor
						.subTask(NLS.bind("Launching debug adapter: {0}", "\"" + String.join("\" \"", command) + "\""));
				Process debugAdapterProcess = DebugPlugin.exec(command.toArray(String[]::new), null,
						dspConfig.debugCmdEnvVars);
				if (dspConfig.monitorDebugAdapter) {
					IProcess debugAdapterMonitorProcess = DebugPlugin.newProcess(dspConfig.launch, debugAdapterProcess, "Debug Adapter - monitor");
					// Uglish workaround: should instead have a dedicated ProcessFactory reading
					// process attribute rather than launch one
					String initialCaptureOutput = dspConfig.launch.getAttribute(DebugPlugin.ATTR_CAPTURE_OUTPUT);
					dspConfig.launch.setAttribute(DebugPlugin.ATTR_CAPTURE_OUTPUT, Boolean.toString(true));
					dspConfig.launch.setAttribute(DebugPlugin.ATTR_CAPTURE_OUTPUT, initialCaptureOutput);

					final var bytes = new ConcurrentLinkedQueue<Byte>();
					inputStream = new InputStream() {
						@Override
						public int read() throws IOException {
							while (debugAdapterProcess.isAlive()) {
								final Byte b = bytes.poll();
								if (b != null) {
									return b;
								} else {
									try {
										Thread.sleep(50);
									} catch (InterruptedException e) {
										Thread.currentThread().interrupt();
										Activator.logError(e);
									}
								}
							}
							return -1;
						}
					};
					final var consoleEncoding = dspConfig.launch.getAttribute(DebugPlugin.ATTR_CONSOLE_ENCODING);
					final var consoleCharset = consoleEncoding == null //
							? Charset.defaultCharset()
							: Charset.forName(consoleEncoding);
					debugAdapterMonitorProcess.getStreamsProxy().getOutputStreamMonitor()
							.addListener((text, mon) -> {
								for (byte b : text.getBytes(consoleCharset)) {
									bytes.add(b);
								}
							});
					outputStream = new OutputStream() {
						@Override
						public void write(int b) throws IOException {
							debugAdapterMonitorProcess.getStreamsProxy()
									.write(new String(new byte[] { (byte) b }));
						}
					};
					cleanup = () -> {
						try {
							debugAdapterMonitorProcess.terminate();
							debugAdapterProcess.destroy();
						} catch (DebugException e) {
							Activator.logError(e);
						}
					};
				} else {
					inputStream = debugAdapterProcess.getInputStream();
					outputStream = debugAdapterProcess.getOutputStream();
					cleanup = debugAdapterProcess::destroyForcibly;
				}
				// TODO: encapsulate logic starting the process in the supplier
				streamSupplier = () -> new DefaultTransportStreams(inputStream, outputStream) {
					@Override
					public void close() {
						super.close();
						cleanup.run();
					}
				};
			} else {
				final var server = dspConfig.server;
				if (server == null || server.isBlank()) {
					throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "Debug server host unspecified.", null));
				}
				if (dspConfig.port < 1 || dspConfig.port > 65535) {
					throw new CoreException(
							new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "Debug server port unspecified or out of range 1-65535.", null));
				}

				subMonitor.subTask(NLS.bind("Connecting to debug adapter: {0}:{1}", dspConfig.server, dspConfig.port));
				streamSupplier = () -> new SocketTransportStreams(server, dspConfig.port);
			}

			subMonitor.setWorkRemaining(80);

			var target = createDSPDebugTarget(subMonitor, streamSupplier, dspConfig.launch, dspConfig.dspParameters);			
			dspConfig.launch.addDebugTarget(target);
		} catch (OperationCanceledException e1) {
			throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "Failed to start debugging", e1));
		} finally {
			subMonitor.done();
		}
	}
	
	@SuppressWarnings("restriction")
	protected SimulationDSPDebugTarget createDSPDebugTarget(SubMonitor subMonitor, Supplier<TransportStreams> streamsSupplier,
			ILaunch launch, Map<String, Object> dspParameters) throws CoreException {

		// provide all necessary DSP launch parameters if they are not provided 
		// by the launcher JSON file on the "Debug Adapter" tab
		dspParameters.putIfAbsent("type", "lldb-dap");
		dspParameters.putIfAbsent("request", "launch");
		var cfg = launch.getLaunchConfiguration();		
		dspParameters.putIfAbsent("name", cfg.getName());
		
		dspParameters.putIfAbsent("env", cfg.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ENVIRONMENT_MAP, Collections.emptyMap()));

		var cwd = StringUtils.substituteVariables(cfg.getAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, "."));
        dspParameters.putIfAbsent("cwd", cwd);
        
        if (!dspParameters.containsKey("program") || !dspParameters.containsKey("args")) {
            var commandLine = new ArrayList<String>(Arrays.asList(OmnetppLaunchUtils.createCommandLine(launch.getLaunchConfiguration(), "")));
            var program = commandLine.removeFirst();
            dspParameters.putIfAbsent("program", program);
    		dspParameters.putIfAbsent("args", commandLine);
        }

		// create init commands for lldb from a list of lldbinit files defined in the launch configuration 
		// (.lldbinit file in each project root + omnetpp root)
		var initCommands = new ArrayList<String>();
		cfg.getAttribute(IOmnetppLaunchConstants.OPP_LLDB_INIT_FILES, List.of()).forEach(file -> {
			try {
				var filePath = Path.of(StringUtils.substituteVariables(file));
				if (Files.isRegularFile(filePath)) {
					initCommands.add("command source " + filePath);    				
				}
			} catch (CoreException e) {				
			}
		});
		dspParameters.putIfAbsent("initCommands", initCommands);
        
		// create the debug target and initialize it (actually launching the debuggee)
		final var target = new SimulationDSPDebugTarget(launch, streamsSupplier, dspParameters);
		target.initialize(subMonitor.split(80));
		
		return target;
	}

	public String showCommandLine(ILaunchConfiguration cfg, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {
		return cfg.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "")
                + " " + cfg.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
	}
	
	private static class DSPConfig {
		private ILaunchConfiguration configuration;
		private String mode;
		private ILaunch launch;
		private IProgressMonitor monitor;
		private boolean launchNotConnect;
		private String debugCmd;
		private List<String> debugCmdArgs;
		private String [] debugCmdEnvVars;
		private boolean monitorDebugAdapter;
		private String server;
		private int port;
		private Map<String, Object> dspParameters;

		/**
		 * Construct a builder using the normal arguments to
		 * {@link ILaunchConfigurationDelegate#launch(ILaunchConfiguration, String, ILaunch, IProgressMonitor)}
		 */
		public DSPConfig(ILaunchConfiguration configuration, String mode, ILaunch launch,
				IProgressMonitor monitor) {
			this.configuration = configuration;
			this.mode = mode;
			this.launch = launch;
			this.monitor = monitor;
			this.dspParameters = new HashMap<>();
		}

		@Override
		public String toString() {
			return "DSPLaunchDelegateLaunchBuilder [configuration=" + configuration + ", mode=" + mode + ", launch="
					+ launch + ", monitor=" + monitor + ", launchNotConnect=" + launchNotConnect + ", debugCmd="
			      + debugCmd + ", debugCmdArgs=" + debugCmdArgs //
			      + ", debugCmdEnvVars=" + (debugCmdEnvVars == null ? null : List.of(debugCmdEnvVars))
					+ ", monitorDebugAdapter=" + monitorDebugAdapter + ", server=" + server + ", port=" + port
					+ ", dspParameters=" + dspParameters + "]";
		}
	}
	
	public class JsonParserWithStringSubstitution {

		private final IStringVariableManager stringVariableManager;

		/**
		 * @param stringVariableManager that should be used to substitute variables in
		 *                              strings.
		 */
		public JsonParserWithStringSubstitution(IStringVariableManager stringVariableManager) {
			this.stringVariableManager = stringVariableManager;
		}

		/**
		 * Substitutes variables in all string values within the given json.
		 *
		 * @param json as {@link String}
		 * @return Returns json object as a {@link Map}. Keys are of type
		 *         {@link String}, values are of type {@link Object}.
		 * @throws IllegalStateException is thrown if top level element is not a
		 *                               {@link JsonObject}.
		 * @throws CoreException         is thrown if undefined variable was referenced
		 *                               in json.
		 */
		public Map<String, Object> parseJsonObject(final String json)
				throws IllegalStateException, CoreException {
			JsonElement jsonElement = JsonParser.parseString(json);
			JsonObject jsonObject = jsonElement.getAsJsonObject();
			return processJsonObject(jsonObject);
		}

		@SuppressWarnings("unchecked")
		public Map<String, Object> parseJsonObjectAndRemoveNulls(final String json)
				throws IllegalStateException, CoreException {
			Map<String, Object> map = parseJsonObject(json);
			map.values().removeIf(Objects::isNull);
			return (Map<String, Object>) (Map<String, ?>) map;
		}

		private Map<String, Object> processJsonObject(JsonObject jsonObject) throws CoreException {
			final var resultMap = new LinkedHashMap<String, Object>();
			for (Map.Entry<String, JsonElement> entry : jsonObject.entrySet()) {
				String key = entry.getKey();
				JsonElement value = entry.getValue();
				resultMap.put(key, handleJsonElement(value));
			}
			return resultMap;
		}

		private Object handleJsonElement(JsonElement value) throws CoreException {
			if (value.isJsonObject()) {
				return processJsonObject(value.getAsJsonObject());
			}

			if (value.isJsonArray()) {
				return processJsonArray(value.getAsJsonArray());
			}

			if (value.isJsonPrimitive()) {
				return handleJsonPrimitive(value.getAsJsonPrimitive());
			}

			// Must be null.
			return null;
		}

		private Object handleJsonPrimitive(JsonPrimitive primitive) throws CoreException {
			if (primitive.isString()) {
				return stringVariableManager.performStringSubstitution(primitive.getAsString());
			}
			return primitive;
		}

		private Object processJsonArray(JsonArray array) throws CoreException {
			final var resultArray = new ArrayList<Object>();
			for (JsonElement element : array) {
				resultArray.add(handleJsonElement(element));
			}
			return resultArray;
		}

	}
}
