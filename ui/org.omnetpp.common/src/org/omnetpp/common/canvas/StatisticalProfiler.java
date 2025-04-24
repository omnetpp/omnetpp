/*--------------------------------------------------------------*
  Copyright (C) 2006-2025 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

import java.io.IOException;
import java.nio.file.Path;
import java.text.ParseException;
import java.util.HashMap;
import java.util.Map;

import jdk.jfr.Configuration;
import jdk.jfr.Recording;
import jdk.jfr.RecordingState;
import jdk.jfr.consumer.RecordedEvent;
import jdk.jfr.consumer.RecordedFrame;
import jdk.jfr.consumer.RecordingFile;

public class StatisticalProfiler {
    private Recording recording = null;
    private Path unfilteredOutputPath;
    private Path filteredOutputPath;
    private String filterThreadName;
    private String filterMethodName;

    public StatisticalProfiler(String outputPath, double seconds, boolean filterCurrentThread, String filterMethodName) {
        this.unfilteredOutputPath = Path.of(outputPath);
        this.filteredOutputPath = unfilteredOutputPath.resolveSibling(unfilteredOutputPath.getFileName().toString().replace(".jfr", "") + "-filtered.jfr");
        this.filterMethodName = filterMethodName;
        this.filterThreadName = filterCurrentThread ? Thread.currentThread().getName() : null;
        try {
            Map<String, String> settings = new HashMap<>(Configuration.getConfiguration("default").getSettings());
            settings.put("jdk.ExecutionSample#period", String.valueOf(seconds) + " s");
            recording = new Recording(settings);
            recording.setDumpOnExit(true);
            recording.setDestination(unfilteredOutputPath);
            recording.disable("jdk.ExecutionSample");
            recording.start();
            Runtime.getRuntime().addShutdownHook(new Thread(() -> { close(); }, "StaticticalProfiler-ShutdownHook"));
        }
        catch (IOException e) {
            throw new RuntimeException(e);
        }
        catch (ParseException e) {
            throw new RuntimeException(e);
        }
    }

    public void resume() {
        recording.enable("jdk.ExecutionSample");
    }

    public void suspend() {
        recording.disable("jdk.ExecutionSample");
    }

    public void close() {
        if (recording.getState() == RecordingState.RUNNING)
            recording.stop();
        if (recording.getState() == RecordingState.STOPPED)
            recording.close();
        try {
            try (RecordingFile in = new RecordingFile(unfilteredOutputPath)) {
                in.write(filteredOutputPath, (RecordedEvent e) -> {
                    if (filterThreadName != null) {
                        var thread = e.getThread();
                        if (thread == null || !filterThreadName.equals(thread.getJavaName()))
                            return false;
                    }
                    if (filterMethodName != null) {
                        var trace = e.getStackTrace();
                        if (trace == null)
                            return false;
                        boolean found = trace.getFrames().stream()
                            .map(RecordedFrame::getMethod)
                            .anyMatch(m -> m.getName().contains(filterMethodName));
                        if (!found)
                            return false;
                    }
                    return true;
                });
            }
        }
        catch (IOException e) {
            throw new RuntimeException("Failed to filter statistical profiler recording", e);
        }
    }
}
