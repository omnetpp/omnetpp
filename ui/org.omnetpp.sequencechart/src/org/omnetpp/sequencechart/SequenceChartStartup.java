/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart;

import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.ILog;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.ned.core.NedResources;
import org.omnetpp.sequencechart.editors.SequenceChartEditor;
import org.omnetpp.sequencechart.editors.SequenceChartSelfTestJob;
import org.omnetpp.sequencechart.widgets.SequenceChart;

// TODO: test with xvfb in-memory X11 display
// TODO: better name
public class SequenceChartStartup implements IStartup {
    public void earlyStartup() {
        String testFolderPaths = System.getenv().get("OPP_SEQUENCE_CHART_TEST_FOLDERS");
        if (testFolderPaths != null) {
            // run tests in a background job in order to not hold back IDE startup
            Job testEventLogFolderJob = new Job("SequenceChart Eventlog Folder Test") {
                @Override
                protected IStatus run(IProgressMonitor monitor) {
                    try {
                        // pre-load all NED files to avoid loading them while the first SequenceChart is being open
                        NedResources.getInstance().readMissingNedFiles();
                        ILog log = SequenceChartPlugin.getDefault().getLog();
                        IWorkbench workbench = PlatformUI.getWorkbench();
                        PersistentResourcePropertyManager persistentResourcePropertyManager = new PersistentResourcePropertyManager(SequenceChartPlugin.PLUGIN_ID, getClass().getClassLoader());
                        ArrayList<IStatus> statuses = new ArrayList<IStatus>();
                        for (String testFolderPath : testFolderPaths.split(";")) {
                            IFolder eventLogFolder = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(testFolderPath));
                            log.info("SequenceChart testing started, folder = " + eventLogFolder.getFullPath().toOSString());
                            ArrayList<IFile> eventLogFiles = collectEventLogFiles(eventLogFolder);
                            for (IFile eventLogFile : eventLogFiles) {
                                // clear all previously saved SequenceChart state
                                persistentResourcePropertyManager.removeProperty(eventLogFile, "SequenceChartState");
                                // must open SequenceChart from the UI thread
                                SequenceChartSelfTestJob[] testSequenceChartJob = new SequenceChartSelfTestJob[1];
                                workbench.getDisplay().syncExec(() -> {
                                    try {
                                        IWorkbenchPage page = workbench.getActiveWorkbenchWindow().getActivePage();
                                        SequenceChartEditor sequenceChartEditor = (SequenceChartEditor)IDE.openEditor(page, eventLogFile);
                                        SequenceChart sequenceChart = sequenceChartEditor.getSequenceChart();
                                        // run test in a background job and close editor when completed
                                        testSequenceChartJob[0] = new SequenceChartSelfTestJob(sequenceChart) {
                                            @Override
                                            protected IStatus run(IProgressMonitor monitor) {
                                                log.info("SequenceChart testing started, file = " + eventLogFile.getFullPath().toOSString());
                                                IStatus status = super.run(monitor);
                                                statuses.add(status);
                                                // close editor from the UI thread after the test
                                                workbench.getDisplay().syncExec(() -> {
                                                    page.closeEditor(sequenceChartEditor, true);
                                                    while (Display.getCurrent().readAndDispatch())
                                                        ; // finish painting
                                                });
                                                log.info("SequenceChart testing finished, result = " + status.toString() + ", file = " + eventLogFile.getFullPath().toOSString());
                                                return status;
                                            }
                                        };
                                        testSequenceChartJob[0].schedule();
                                    }
                                    catch (Exception e) {
                                        throw new RuntimeException(e);
                                    }
                                });
                                // wait for the test job to finish in the background thread
                                if (testSequenceChartJob[0] != null)
                                    testSequenceChartJob[0].join();
                               // Thread.sleep(3000);
                            }
                            log.info("SequenceChart testing finished, folder = " + eventLogFolder.getFullPath().toOSString());
                        }
                        int numOk = 0;
                        int numFail = 0;
                        for (IStatus status : statuses) {
                            if (status.isOK())
                                numOk++;
                            else
                                numFail++;
                        }
                        log.info("SequenceChart testing: " + numOk + " tests PASS, " + numFail + " tests FAIL");
                        // close workbench when all tests are completed from the UI thread
                        workbench.getDisplay().asyncExec(() -> workbench.close());
                        return numFail == 0 ? Status.OK_STATUS : new Status(Status.ERROR, SequenceChartPlugin.PLUGIN_ID, numFail + " tests failed");
                    }
                    catch (Exception e) {
                        throw new RuntimeException(e);
                    }
                }
            };
            testEventLogFolderJob.schedule();
        }
    }

    private ArrayList<IFile> collectEventLogFiles(IResource resource) {
        ArrayList<IFile> result = new ArrayList<IFile>();
        collectEventLogFiles(resource, result);
        return result;
    }

    private void collectEventLogFiles(IResource resource, ArrayList<IFile> files) {
        try {
            if (resource instanceof IFile) {
                IFile file = (IFile)resource;
                if (file.getFileExtension().equals("elog"))
                    files.add((IFile)resource);
            }
            else if (resource instanceof IFolder)
                for (IResource member : ((IFolder)resource).members())
                    collectEventLogFiles(member, files);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
