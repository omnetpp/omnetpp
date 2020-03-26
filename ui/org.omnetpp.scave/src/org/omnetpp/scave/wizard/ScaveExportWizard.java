/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.wizard;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.editors.text.EditorsUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.common.wizard.XSWTDataBinding;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.Exporter;
import org.omnetpp.scave.engine.ExporterFactory;
import org.omnetpp.scave.engine.ExporterType;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;

import com.swtworkbench.community.xswt.XSWT;

/**
 * A generic export wizard.
 *
 * @author andras
 */
public class ScaveExportWizard extends Wizard implements IExportWizard {
    protected IDList selectedIDs = IDList.EMPTY;
    protected int selectionItemTypes;
    protected ResultFileManager resultFileManager;
    protected Page page;
    protected String format;
    protected ExporterType exporterType;

    public ScaveExportWizard(String format, IDList selectedIDs, ResultFileManager resultFileManager) {
        this.format = format;
        setWindowTitle(format + " Export");

        this.selectedIDs = selectedIDs;
        this.selectionItemTypes = selectedIDs.getItemTypes();
        this.resultFileManager = resultFileManager;

        this.exporterType = ExporterFactory.getByFormat(format);
        if (exporterType == null)
            throw new RuntimeException("Unsupported export format: " + format);

        setDialogSettings(UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName()));
    }

    @Override
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void addPages() {
        page = new Page();
        addPage(page);
    }

    @Override
    public boolean canFinish() {
        return page != null && resultFileManager != null && super.canFinish();
    }

    private static double parseTime(String str) {
        String unit = UnitConversion.parseQuantityForUnit(str);
        return UnitConversion.parseQuantity(str, unit.isEmpty() ? "" : "s");
    }

    @Override
    public boolean performFinish() {
        if (page != null && resultFileManager != null) {
            try {
                Exporter exporter = ExporterFactory.createExporter(format);

                // set exporter options from the page
                for (String widgetName : page.widgetMap.keySet()) {
                    Control control = page.widgetMap.get(widgetName);
                    Object value = XSWTDataBinding.getValueFromControl(control, null);
                    try {
                        exporter.setOption(widgetName, value.toString());
                    }
                    catch (Exception e) {
                        // log but otherwise ignore the error
                        ScavePlugin.logError("Cannot set option " + widgetName + "=" + value + " on " + format + " exporter", e);
                    }
                }

                // set vector time limits if applicable
                if (page.startTimeText != null) {
                    String startText = page.startTimeText.getText();
                    if (!startText.trim().isEmpty())
                        exporter.setVectorStartTime(parseTime(startText));

                    String endText = page.endTimeText.getText();
                    if (!endText.trim().isEmpty())
                        exporter.setVectorEndTime(parseTime(endText));
                }

                final String filename = page.getFilePath().toOSString();
                boolean openAfterwards = page.openAfterwardsCheckbox.getSelection();

                // save the control values before the dialog gets closed
                saveDialogSettings();

                performExport(exporter, filename, openAfterwards);

                if (openAfterwards)
                    openFile(filename);

                return true;

            }
            catch (Exception e) {
                MessageDialog.openError(getShell(), "Error", "Error: " + e.toString());
            }
        }
        return false;
    }

    protected void performExport(Exporter exporter, final String filename, boolean openAfterwards) {
        try {
            ProgressMonitorDialog dialog = new ProgressMonitorDialog(getShell());
            dialog.run(true, true, (monitor) -> {
                monitor.beginTask("Exporting", 100);
                ResultFileManager.runWithReadLock(resultFileManager, () -> exporter.saveResults(filename, resultFileManager, selectedIDs, monitor));
            });
        }
        catch (InterruptedException e) {
            // void
        }
        catch (InvocationTargetException e) {
            Throwable ee = e.getCause();
            MessageDialog.openError(getShell(), "Error", "Error during export: " + ee.getMessage());
            ScavePlugin.logError("Error during exporting results:", ee);
        }
    }

    protected void saveDialogSettings() {
        if (page != null)
            page.saveDialogSettings();
    }

    protected void openFile(String filename) {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage workbenchPage = workbenchWindow == null ? null : workbenchWindow.getActivePage();
        if (workbenchPage == null)
            return;

        try {
            if (!StringUtils.endsWith(filename, ".vec") && !StringUtils.endsWith(filename, ".sca")) {
                long fileSize = new File(filename).length();
                long MiB = 1024*1024;
                if (fileSize >= 10*MiB) {
                    boolean ok = MessageDialog.openConfirm(getShell(), "Confirm", "Exported file is relatively large (~" + fileSize/MiB + "MB), still open it?");
                    if (!ok)
                        return;
                }
            }

            URI fileURI = new File(filename).toURI();
            IFile[] candidates = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocationURI(fileURI);
            IFile file = candidates.length == 0 ? null : candidates[0];
            if (file == null) {
                IDE.openEditor(workbenchPage, fileURI, EditorsUI.DEFAULT_TEXT_EDITOR_ID, true);
            }
            else {
                file.refreshLocal(IResource.DEPTH_ZERO, null);
                IDE.openEditor(workbenchPage, file, true);
            }
        }
        catch (Exception e) {
            String message = "Cannot open '" + filename + "'";
            if (!StringUtils.isEmpty(e.getMessage()))
                message += ": " + e.getMessage();
            MessageDialog.openError(getShell(), "Error", message);
        }
    }

    /**
     * Wizard page
     */
    class Page extends WizardPage
    {
        private Text filenameText;
        private Text startTimeText;
        private Text endTimeText;
        private Map<String,Control> widgetMap;
        private Button openAfterwardsCheckbox;

        public Page() {
            super("Export");
            String formatName = exporterType.getDisplayName();
            setTitle("Export " + getSelectionInfoString() + " to " + formatName + " (" + format + ")");
            setDescription(formatName + ": " + exporterType.getDescription().trim());
        }

        @Override
        public void createControl(Composite parent) {
            Composite panel = SWTFactory.createComposite(parent, 1, 1, SWTFactory.GRAB_AND_FILL_BOTH);
            addCommonFields(panel);
            addXswtForm(panel);
            addCommonBottomFields(panel);
            openAfterwardsCheckbox = SWTFactory.createCheckButton(panel, "Open with default editor afterwards", null, false, 1);
            ((GridData)openAfterwardsCheckbox.getLayoutData()).horizontalIndent = 8;
            restoreDialogSettings();
            setControl(panel);
            updatePageCompletion();
        }

        protected void addCommonFields(Composite parent) {
            Composite fileSel = SWTFactory.createComposite(parent, 3, 1, SWTFactory.GRAB_AND_FILL_HORIZONTAL);
            SWTFactory.createLabel(fileSel, "Target file:", 1);
            filenameText = SWTFactory.createText(fileSel, SWT.BORDER, 1, SWTFactory.GRAB_AND_FILL_HORIZONTAL);
            Button browseButton = SWTFactory.createPushButton(fileSel, "Browse..." , null);
            browseButton.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    handleBrowseButtonPressed();
                }
            });
            filenameText.addModifyListener(new ModifyListener() {
                @Override
                public void modifyText(ModifyEvent e) {
                    updatePageCompletion();
                }
            });
        }

        @SuppressWarnings("unchecked")
        protected void addXswtForm(Composite parent) {
            try {
                // instantiate XSWT form
                Composite xswtHolder = SWTFactory.createComposite(parent, 1, 1, SWTFactory.GRAB_AND_FILL_HORIZONTAL);
                String xswtDoc = exporterType.getXswtForm();
                widgetMap = new HashMap<>(); // prevent NPE later
                if (!xswtDoc.isEmpty())
                    widgetMap = XSWT.create(xswtHolder, new ByteArrayInputStream(xswtDoc.getBytes()));

                Display.getCurrent().asyncExec(() -> {  // when done synchronously, dialog height will be smaller than desired (?)
                        Shell shell = parent.getShell();
                        shell.setSize(shell.computeSize(SWT.DEFAULT, SWT.DEFAULT));
                });
            }
            catch (Exception e) {
                IStatus status = new Status(IStatus.ERROR, ScavePlugin.PLUGIN_ID, "Error showing the XSWT form of exporter '" + format + "'", e);
                ScavePlugin.log(status);
                ErrorDialog.openError(getShell(), "Error", "Cannot add exporter options to Export dialog.", status);
            }
        }

        protected void addCommonBottomFields(Composite parent) {
            if ((exporterType.getSupportedResultTypes() & ResultFileManager.VECTOR) > 0) {
                Group vectorLimit = SWTFactory.createGroup(parent, "Vector time limits", 2, 1, SWTFactory.GRAB_AND_FILL_HORIZONTAL);
                SWTFactory.createLabel(vectorLimit, "Start time:", 1);
                startTimeText = SWTFactory.createText(vectorLimit, SWT.BORDER, 1, SWTFactory.GRAB_AND_FILL_HORIZONTAL);
                SWTFactory.createLabel(vectorLimit, "End time:", 1);
                endTimeText = SWTFactory.createText(vectorLimit, SWT.BORDER, 1, SWTFactory.GRAB_AND_FILL_HORIZONTAL);
            }
        }

        protected String getSelectionInfoString() {
            ArrayList<String> items = new ArrayList<>();
            int numScalars = selectedIDs.countByTypes(ResultFileManager.SCALAR);
            int numVectors = selectedIDs.countByTypes(ResultFileManager.VECTOR);
            int numStatistics = selectedIDs.countByTypes(ResultFileManager.STATISTICS);
            int numHistograms = selectedIDs.countByTypes(ResultFileManager.HISTOGRAM);
            int numParameters = selectedIDs.countByTypes(ResultFileManager.PARAMETER);
            if (numScalars > 0)
                items.add(StringUtils.formatCounted(numScalars, "scalar"));
            if (numVectors > 0)
                items.add(StringUtils.formatCounted(numVectors, "vector"));
            if (numStatistics > 0)
                items.add(StringUtils.formatCounted(numStatistics, "statistics"));
            if (numHistograms > 0)
                items.add(StringUtils.formatCounted(numHistograms, "histogram"));
            if (numParameters > 0)
                items.add(StringUtils.formatCounted(numHistograms, "parameter"));
            if (selectedIDs.isEmpty())
                items.add("Empty set");
            return StringUtils.join(items, ", ", " and ");
        }

        private String itemTypesAsString(int types) {
            String result = "";
            if ((types & ResultFileManager.SCALAR) != 0)
                result += "scalar ";
            if ((types & ResultFileManager.VECTOR) != 0)
                result += "vector ";
            if ((types & ResultFileManager.STATISTICS) != 0)
                result += "statistics ";
            if ((types & ResultFileManager.HISTOGRAM) != 0)
                result += "histogram ";
            if ((types & ResultFileManager.PARAMETER) != 0)
                result += "parameter ";
            return StringUtils.join(result.split(" "), " and ");
        }

        protected void updatePageCompletion() {
            int supportedTypes = exporterType.getSupportedResultTypes();
            int unsupportedTypes = selectionItemTypes & ~supportedTypes;
            if (unsupportedTypes != 0) {
                setPageComplete(false);
                setErrorMessage("This exporter does not support exporting " + itemTypesAsString(unsupportedTypes) + " items");
                return;
            }

            IPath path = getFilePath();
            if (path.isEmpty()) {
                setErrorMessage(null);
                setPageComplete(false);
            }
            else if (path.toFile().isDirectory()) {
                setErrorMessage("Target is a directory");
                setPageComplete(false);
            }
            else if (path.segmentCount() > 1 && !path.removeLastSegments(1).toFile().isDirectory()) {
                setErrorMessage("Directory does not exist");
                setPageComplete(false);
            }
            else {
                setErrorMessage(null);
                setPageComplete(true);
            }
        }

        protected void handleBrowseButtonPressed() {
            String directory, fileName;
            IPath path = getFilePath();
            if (path.isEmpty()) {
                directory = getDefaultExportDirectory();
                fileName = "Untitled." + exporterType.getFileExtension();
            } else {
                directory = path.removeLastSegments(1).toOSString();
                fileName = path.lastSegment();
            }

            FileDialog dialog = new FileDialog(getShell(), SWT.SAVE);
            dialog.setText("Save to File");
            dialog.setFilterPath(directory);
            dialog.setFileName(fileName);
            dialog.setOverwrite(true);
            dialog.setFilterExtensions(new String[] {"*." + exporterType.getFileExtension(), "*.*"});
            String selectedFileName = dialog.open();
            if (selectedFileName != null)
                filenameText.setText(selectedFileName);
        }

        /**
         * Returns *absolute* file system (not workspace) path.
         */
        public IPath getFilePath() {
            String currentFilename = filenameText.getText();
            if (currentFilename.isEmpty())
                return new Path("");
            IPath path = new Path(currentFilename);
            if (!path.isAbsolute()) {
                String directory = getDefaultExportDirectory();
                path = new Path(directory).append(path);
            }
            if (path.getFileExtension() == null)
                path = path.addFileExtension(exporterType.getFileExtension());
            return path;
        }

        protected String getDefaultExportDirectory() {
            if (selectedIDs.isEmpty())
                return "";
            String firstResultFile = ResultFileManager.callWithReadLock(resultFileManager, () -> {
                return resultFileManager.getItem(selectedIDs.get(0)).getFileRun().getFile().getFileSystemFilePath();
            });
            return new Path(firstResultFile).removeLastSegments(1).toOSString(); // remove file name
        }

        //TODO use
        public Map<String,String> getOptions() {
            Map<String,String> options = new HashMap<String, String>();
            for (String widgetName : widgetMap.keySet()) {
                Control control = widgetMap.get(widgetName);
                Object value = XSWTDataBinding.getValueFromControl(control, null);
                options.put(widgetName, value.toString());
            }
            return options;
        }

        protected void saveDialogSettings() {
            IDialogSettings settings = getDialogSettings();
            if (settings != null) {
                for (String widgetName : page.widgetMap.keySet()) {
                    Control control = page.widgetMap.get(widgetName);
                    Object value = XSWTDataBinding.getValueFromControl(control, null);
                    settings.put(format + ":" + widgetName, value.toString());
                }
            }
        }

        protected void restoreDialogSettings() {
            IDialogSettings settings = getDialogSettings();
            if (settings != null) {
                for (String widgetName : page.widgetMap.keySet()) {
                    Control control = page.widgetMap.get(widgetName);
                    String value = settings.get(format + ":" + widgetName);
                    if (value != null) {
                        try {
                            XSWTDataBinding.putValueIntoControl(control, value, null);
                        }
                        catch (Exception e) {
                            ScavePlugin.logError("Cannot restore exporter preference " + format + ":" + widgetName, e);
                        }
                    }
                }
            }
        }
    }
}
