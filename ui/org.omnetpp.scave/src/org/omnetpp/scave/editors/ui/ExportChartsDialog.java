/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.SetChartPropertiesCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Export Charts dialog
 */
public class ExportChartsDialog extends TitleAreaDialog {
    // dialog setting keys
    private static final String KEY_CHARTS = "charts";
    private static final String KEY_EXPORT_IMAGES = "export_images";
    private static final String KEY_EXPORT_DATA = "export_data";
    private static final String KEY_IMAGE_TARGET_FOLDER = "image_target_folder";
    private static final String KEY_IMAGE_FORMAT = "image_format";
    private static final String KEY_IMAGE_DPI = "image_dpi";
    private static final String KEY_DATA_TARGET_FOLDER = "data_target_folder";
    private static final String KEY_STOP_ON_ERROR = "stop_on_error";
    private static final String KEY_NUM_CONCURRENT_PROCESSES = "num_concurrent_processes";

    // input
    private List<Chart> charts;
    private List<Chart> initialSelection; // =null means: take it from DialogSettings
    private boolean singleChart = false;
    private IFile anfFile;

    // widgets
    private Tree chartsTree;
    private Label numSelectedLabel;
    private Button setSizeButton;
    private Button exportImagesCheckbox;
    private Button exportDataCheckbox;
    private Text imageTargetFolderText;
    private Combo fileFormatCombo;
    private Combo dpiCombo;
    private Text dataTargetFolderText;
    private Spinner concurrencySpinner;
    private Button stopOnErrorCheckbox;

    private CommandStack commandStack;

    // result
    public static class Result {
        public List<Chart> selectedCharts;
        public boolean exportImages;
        public boolean exportData;
        public IContainer imageTargetFolder;
        public String imageFormat;
        public String imageDpi;
        public IContainer dataTargetFolder;
        public boolean stopOnError;
        public int numConcurrentProcesses;
    }

    private Result result = null;

    private static final String[] fileFormats = new String[] { //TODO get this list from Matplotlib
            // note: first word must be a recognized Matplotlib format name
            "png (Portable Network Graphics)",
            "jpg (Joint Photographic Experts Group)",
            "tif (Tagged Image File Format)",
            "svg (Scalable Vector Graphics)",
            "svgz (Scalable Vector Graphics)",
            "ps (Postscript)",
            "eps (Encapsulated Postscript)",
            "pdf (Portable Document Format)",
            "pgf (PGF code for LaTeX)",
            "raw (Raw RGBA bitmap)",
    };


    private static class SetSizeDialog extends TitleAreaDialog {
        private Text widthText;
        private Text heightText;
        private String width;
        private String height;
        private int numCharts;

        public SetSizeDialog(Shell parentShell, String initialWidth, String initialHeight, int numCharts) {
            super(parentShell);
            this.width = initialWidth;
            this.height = initialHeight;
            this.numCharts = numCharts;
        }

        @Override
        protected void configureShell(Shell newShell) {
            super.configureShell(newShell);
            newShell.setText("Set Chart Export Sizes");
        }

        @Override
        protected boolean isResizable() {
            return true;
        }

        @Override
        protected Control createDialogArea(Composite parent) {
            setTitle("Set Export Size for " + StringUtils.formatCounted(numCharts, "Highlighted Chart"));
            setMessage("Leave a field empty for no change");
            Composite container = (Composite) super.createDialogArea(parent);
            container.setLayout(new GridLayout(1, false));

            Group group = SWTFactory.createGroup(container, "Export size (inches):", 2, 1, SWTFactory.FILL_BOTH);

            SWTFactory.createLabel(group, "Width:", 1);
            widthText = SWTFactory.createSingleText(group, 1);
            if (width != null)
                widthText.setText(width);
            widthText.addModifyListener(e -> validate());

            SWTFactory.createLabel(group, "Height:", 1);
            heightText = SWTFactory.createSingleText(group, 1);
            if (height != null)
                heightText.setText(height);
            heightText.addModifyListener(e -> validate());

            // making it so that the last label doesn't increase the width of the dialog
            container.pack();
            int dialogWidthHint = container.getSize().x;
            Label l = SWTFactory.createWrapLabel(container, "HINT: The export size can also be edited for each chart in the Export tab of its Configuration dialog.", 2);
            ((GridData)l.getLayoutData()).widthHint = dialogWidthHint;

            return container;
        }


        protected void validate() {
            String wText = widthText.getText();
            String hText = heightText.getText();

            boolean valid = true;

            if (!wText.isEmpty()) {
                try {
                    Double.parseDouble(wText);
                }
                catch (Exception e) {
                    valid = false;
                }
            }

            if (!hText.isEmpty()) {
                try {
                    Double.parseDouble(hText);
                }
                catch (Exception e) {
                    valid = false;
                }
            }

            getButton(OK).setEnabled(valid);
        }


        @Override
        protected void okPressed() {
            width = widthText.getText();
            height = heightText.getText();
            super.okPressed();
        }

        public String getWidth() {
            return width;
        }

        public String getHeight() {
            return height;
        }
    }

    public ExportChartsDialog(Shell parentShell, List<Chart> charts, IFile anfFile, CommandStack commandStack) { // take initialSelection from DialogSettings
        this(parentShell, charts, null, anfFile, commandStack);
    }

    public ExportChartsDialog(Shell parentShell, List<Chart> charts, List<Chart> initialSelection, IFile anfFile, CommandStack commandStack) { // initialSelection: should be an IStructuralSelection?
        super(parentShell);
        this.charts = charts;
        this.initialSelection = initialSelection;
        this.anfFile = anfFile;
        this.commandStack = commandStack;
    }

    public ExportChartsDialog(Shell parentShell, Chart chart, IFile anfFile, CommandStack commandStack) {
        super(parentShell);
        this.charts = this.initialSelection = Arrays.asList(new Chart[] {chart});
        this.singleChart = true;
        this.anfFile = anfFile;
        this.commandStack = commandStack;
    }

    public Result getResult() {
        return result;
    }

    @Override
    protected boolean isResizable() {
        return true;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return getDialogSettings();
    }

    @Override
    protected void configureShell(Shell newShell) {
        super.configureShell(newShell);
        newShell.setText("Export Chart(s)");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        final Composite dialogArea = (Composite)super.createDialogArea(parent);

        setTitle("Export Chart Graphics and/or Data");
        setMessage("Runs chart script(s) in the background for exporting.");

        dialogArea.setLayout(new GridLayout());

        Composite chartsPanel = SWTFactory.createComposite(dialogArea, 2, new GridData(SWT.FILL, SWT.FILL, true, true));
        SWTFactory.createLabel(chartsPanel, "Check charts to export:", 2);
        chartsTree = new Tree(chartsPanel, SWT.CHECK | SWT.BORDER | SWT.MULTI);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 320;
        gridData.heightHint = 200;
        chartsTree.setLayoutData(gridData);
        for (int i = 0; i < charts.size(); ++i)
            new TreeItem(chartsTree, SWT.NONE);

        SelectionListener dialogValidator = new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                validateDialogContents();
            }
        };

        chartsTree.addSelectionListener(dialogValidator);

        Composite chartsButtonPanel = SWTFactory.createComposite(chartsPanel, 1, new GridData(SWT.CENTER, SWT.TOP, false, false));
        if (!singleChart) {
            Button button = SWTFactory.createPushButton(chartsButtonPanel, "Check All", null, new GridData(SWT.FILL, SWT.CENTER, true, false));
            button.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    for (TreeItem item : chartsTree.getItems())
                        item.setChecked(true);
                    validateDialogContents();
                }
            });

            button = SWTFactory.createPushButton(chartsButtonPanel, "Uncheck All", null, new GridData(SWT.FILL, SWT.CENTER, true, false));
            button.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    for (TreeItem item : chartsTree.getItems())
                        item.setChecked(false);
                    validateDialogContents();
                }
            });

            numSelectedLabel = SWTFactory.createLabel(chartsButtonPanel, "n/a", 1);
            numSelectedLabel.setAlignment(SWT.RIGHT);

            new Label(chartsButtonPanel, SWT.NONE);
        }

        setSizeButton = SWTFactory.createPushButton(chartsButtonPanel, "Set Export Size...", null, new GridData(SWT.FILL, SWT.CENTER, true, false));
        setSizeButton.addSelectionListener(SelectionListener.widgetSelectedAdapter((e)-> openSetSizeDialog()));

        boolean hasNativeChart = charts.stream().anyMatch(chart -> chart.getType() != ChartType.MATPLOTLIB);
        if (hasNativeChart) {
            Label l = SWTFactory.createWrapLabel(dialogArea, "* For native charts, image export is emulated using Matplotlib. As a result, the exported images may differ from their on-screen rendering in certain visual details.", 1);
            SWTFactory.setIndent(l, 15);
        }

        Group imageGroup = SWTFactory.createGroup(dialogArea, "Image Export", 1, 1, new GridData(SWT.FILL, SWT.FILL, true, false));

        exportImagesCheckbox = SWTFactory.createCheckButton(imageGroup, "Export images", null, true, 1);
        exportImagesCheckbox.addSelectionListener(dialogValidator);

        Composite folderPanel = SWTFactory.createComposite(imageGroup, 3, new GridData(SWT.FILL, SWT.CENTER, true, false));
        SWTFactory.createLabel(folderPanel, "Target folder:", 1);
        imageTargetFolderText = createFolderInputTextAndButton(folderPanel);

        Composite imageOptionsPanel = SWTFactory.createComposite(imageGroup, 4, new GridData(SWT.FILL, SWT.CENTER, true, false));
        SWTFactory.createLabel(imageOptionsPanel, "File format:", 1);
        fileFormatCombo = SWTFactory.createCombo(imageOptionsPanel, SWT.READ_ONLY, 1, fileFormats);
        fileFormatCombo.addModifyListener((e) -> validateDialogContents());

        SWTFactory.createLabel(imageOptionsPanel, "DPI:", 1);
        dpiCombo = SWTFactory.createCombo(imageOptionsPanel, SWT.BORDER, 1, "72 150 300 600 720".split(" "));
        dpiCombo.select(2);
        dpiCombo.addModifyListener((e) -> validateDialogContents());
        dpiCombo.addModifyListener((e) -> updateChartLabels());

        SWTFactory.configureEnablerCheckbox(exportImagesCheckbox, imageGroup, false);

        Group dataGroup = SWTFactory.createGroup(dialogArea, "Data Export", 1, 1, new GridData(SWT.FILL, SWT.FILL, true, false));

        exportDataCheckbox = SWTFactory.createCheckButton(dataGroup, "Export data", null, true, 1);
        exportDataCheckbox.addSelectionListener(dialogValidator);

        Composite dataFolderPanel = SWTFactory.createComposite(dataGroup, 3, new GridData(SWT.FILL, SWT.CENTER, true, false));
        SWTFactory.createLabel(dataFolderPanel, "Target folder:", 1);
        dataTargetFolderText = createFolderInputTextAndButton(dataFolderPanel);

        SWTFactory.configureEnablerCheckbox(exportDataCheckbox, dataGroup, false);

        Group jobGroup = SWTFactory.createGroup(dialogArea, "Job Control", 3, 1, new GridData(SWT.FILL, SWT.FILL, true, false));
        SWTFactory.createLabel(jobGroup, "Concurrent processes:", 1);
        concurrencySpinner = SWTFactory.createSpinner(jobGroup, SWT.BORDER, 1);
        concurrencySpinner.setMinimum(1);
        stopOnErrorCheckbox = SWTFactory.createCheckButton(jobGroup, "Stop on first error", null, false, 1);
        SWTFactory.setIndent(stopOnErrorCheckbox, 30);

        restoreDialogSettings();
        updateChartLabels();

        if (singleChart) {
            chartsTree.setEnabled(false);
            SWTFactory.setEnabled(jobGroup, false, null);
            // The text is more readable in disabled widgets if the item is not selected
            chartsTree.deselectAll();
        }

        Display.getCurrent().asyncExec(() -> {
            if (!getShell().isDisposed())
                setErrorMessage(null); // don't open with an error message displayed
        });

        return dialogArea;
    }

    protected Text createFolderInputTextAndButton(Composite parent) {
        Text folderText = SWTFactory.createSingleText(parent, 1);
        Button browseButton = SWTFactory.createPushButton(parent, "Browse...", null);
        browseButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                IResourceSelectionDialog dialog = new IResourceSelectionDialog(dialogArea.getShell(), IResource.PROJECT | IResource.FOLDER, true);
                dialog.setInitialSelection(folderText.getText().trim());
                int result = dialog.open();
                if (result == Window.OK) {
                    IResource folder = dialog.getSelectedResource();
                    if (folder instanceof IFolder || folder instanceof IProject)
                        folderText.setText(folder.getFullPath().toString());
                }
            }
        });
        folderText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                validateDialogContents();
            }
        });
        return folderText;
    }

    protected void validateDialogContents() {
        Result tmp = readControls();
        String err = doValidate(tmp);
        if (numSelectedLabel != null)
            numSelectedLabel.setText("" + tmp.selectedCharts.size() + " selected for export");

        if (getButton(OK) != null) { // it is null during dialog creation
            setErrorMessage(err);
            getButton(OK).setEnabled(err == null);
        }

        setSizeButton.setEnabled(singleChart || chartsTree.getSelectionCount() > 0);
    }

    protected void updateChartLabels() {
        int dpi = Integer.parseInt(dpiCombo.getText());
        for (int i = 0; i < charts.size(); i++) {
            String label = getChartLabelForImageExport(charts.get(i), dpi);
            chartsTree.getItem(i).setText(label);
        }
    }

    protected String getChartLabelForImageExport(Chart chart, int dpi) {
        String chartName = ScaveModelUtil.getDisplayFullPath(chart, " / ");

        String widthStr = StringUtils.defaultIfBlank(chart.getPropertyValue("image_export_width"), "6");
        String heightStr = StringUtils.defaultIfBlank(chart.getPropertyValue("image_export_height"), "4");
        chartName += "  (" + widthStr + "″x" + heightStr +  "″";
        try {
            double width = Double.parseDouble(widthStr);
            double height = Double.parseDouble(heightStr);
            int widthInPixels = (int)(width * dpi);
            int heightInPixels = (int)(height * dpi);
            chartName += ", " + widthInPixels + "x" + heightInPixels + " px at " + dpi + " dpi";
        } catch (NumberFormatException e) {
            // don't care
        }

        if (chart.getType() != ChartType.MATPLOTLIB)
            chartName += ", emulated*";

        chartName += ")";

        return chartName;
    }

    protected String doValidate(Result result) {
        if (result.selectedCharts.isEmpty())
            return "Check at least one chart";
        if (result.exportImages) {
            if (result.imageTargetFolder == null || !result.imageTargetFolder.isAccessible())
                return "Invalid or inaccessible target folder for image export";
            if (result.imageFormat.isEmpty())
                return "Image format not selected";
            if (!result.imageDpi.trim().matches("\\d+"))
                return "Invalid DPI entered";
        }

        if (result.exportData) {
            if (result.dataTargetFolder == null || !result.dataTargetFolder.isAccessible())
                return "Invalid or inaccessible target folder for data export";
        }
        return null;
    }

    @Override
    protected void okPressed() {
        result = readControls();
        super.okPressed();
    }

    @Override
    public boolean close() {
        saveDialogSettings();
        return super.close();
    }

    protected Result readControls() {
        Result result = new Result();
        result.selectedCharts = getCheckedChartsFromTree();
        result.exportImages = exportImagesCheckbox.getSelection();
        result.exportData = exportDataCheckbox.getSelection();
        result.imageTargetFolder = getTargetFolderFromControl(imageTargetFolderText);
        result.dataTargetFolder = getTargetFolderFromControl(dataTargetFolderText);
        result.imageFormat = StringUtils.substringBefore(getSelectedFileFormat(), " ");
        result.imageDpi = dpiCombo.getText();
        result.stopOnError = stopOnErrorCheckbox.getSelection();
        result.numConcurrentProcesses = concurrencySpinner.getSelection();
        return result;
    }

    protected List<Chart> getCheckedChartsFromTree() {
        List<Chart> result = new ArrayList<Chart>();
        for (int i = 0; i < charts.size(); i++) {
            TreeItem item = chartsTree.getItem(i);
            if (item.getChecked())
                result.add(charts.get(i));
        }
        return result;
    }

    protected IContainer getTargetFolderFromControl(Text folderText) {
        try {
            String text = folderText.getText().trim();
            Path path = new Path(text);
            if (path.segmentCount() < 2)
                return ResourcesPlugin.getWorkspace().getRoot().getProject(text);
            else
                return ResourcesPlugin.getWorkspace().getRoot().getFolder(path);
        }
        catch (Exception e) {
            return null;
        }
    }

    protected String getSelectedFileFormat() {
        int index = fileFormatCombo.getSelectionIndex();
        return fileFormats[index == -1 ? 0 : index];
    }

    protected IDialogSettings getDialogSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    protected void saveDialogSettings() {
        IDialogSettings settings = getDialogSettings();

        String selectedChartIds = "";
        for (Chart chart : getCheckedChartsFromTree())
                selectedChartIds = selectedChartIds  + " " + chart.getId();

        settings.put(KEY_CHARTS, selectedChartIds.trim());
        settings.put(KEY_EXPORT_IMAGES, exportImagesCheckbox.getSelection());
        settings.put(perAnf(KEY_IMAGE_TARGET_FOLDER), imageTargetFolderText.getText());
        settings.put(KEY_IMAGE_FORMAT, getSelectedFileFormat());
        settings.put(KEY_IMAGE_DPI, dpiCombo.getText());
        settings.put(KEY_EXPORT_DATA, exportDataCheckbox.getSelection());
        settings.put(perAnf(KEY_DATA_TARGET_FOLDER), dataTargetFolderText.getText());
        settings.put(KEY_STOP_ON_ERROR, stopOnErrorCheckbox.getSelection());
        settings.put(KEY_NUM_CONCURRENT_PROCESSES, concurrencySpinner.getSelection());
    }

    private String perAnf(String key) {
        return key + "%" + anfFile.getFullPath().toString();
    }

    protected void restoreDialogSettings() {
        IDialogSettings settings = getDialogSettings();

        if (initialSelection != null) {
            // initial selection was specified in the ctor
            for (int i = 0; i < charts.size(); ++i) {
                boolean shouldCheck = initialSelection.contains(charts.get(i));
                chartsTree.getItem(i).setChecked(shouldCheck);
            }
        }
        else {
            // take initial selection from DialogSettings
            String selectedChartIdsSetting = settings.get(KEY_CHARTS);
            if (selectedChartIdsSetting != null) {
                Set<String> selectedChartIds = new HashSet<String>(Arrays.asList(selectedChartIdsSetting.split(" ")));
                for (int i = 0; i < charts.size(); ++i) {
                    boolean shouldCheck = selectedChartIds.contains(Integer.toString(charts.get(i).getId()));
                    chartsTree.getItem(i).setChecked(shouldCheck);
                }
            }
        }

        // reveal first selected item
        for (TreeItem item: chartsTree.getItems()) {
            if (item.getChecked()) {
                chartsTree.showItem(item);
                break;
            }
        }

        String exportImages = settings.get(KEY_EXPORT_IMAGES);
        exportImagesCheckbox.setSelection(exportImages == null || exportImages.equals("true"));
        exportImagesCheckbox.notifyListeners(SWT.Selection, new Event()); // update enablements

        String imageFolder = settings.get(perAnf(KEY_IMAGE_TARGET_FOLDER));
        if (imageFolder != null)
            imageTargetFolderText.setText(imageFolder);
        else
            imageTargetFolderText.setText(anfFile.getParent().getFullPath().toString());

        String format = settings.get(KEY_IMAGE_FORMAT);
        if (ArrayUtils.contains(fileFormats, format))
            fileFormatCombo.setText(format);

        String dpi = settings.get(KEY_IMAGE_DPI);
        if (dpi != null)
            dpiCombo.setText(dpi);

        String exportData = settings.get(KEY_EXPORT_DATA);
        exportDataCheckbox.setSelection(exportData == null || exportData.equals("true"));
        exportDataCheckbox.notifyListeners(SWT.Selection, new Event());  // update enablements

        String dataFolder = settings.get(perAnf(KEY_DATA_TARGET_FOLDER));
        if (dataFolder != null)
            dataTargetFolderText.setText(dataFolder);
        else
            dataTargetFolderText.setText(anfFile.getParent().getFullPath().toString());

        if (settings.get(KEY_STOP_ON_ERROR) != null)
            stopOnErrorCheckbox.setSelection(settings.getBoolean(KEY_STOP_ON_ERROR));
        if (settings.get(KEY_NUM_CONCURRENT_PROCESSES) != null)
            concurrencySpinner.setSelection(settings.getInt(KEY_NUM_CONCURRENT_PROCESSES));
    }

    private void openSetSizeDialog() {

        // If the export dialog is opened for a single chart (from within its own editor),
        // the tree widget is disabled, and the single entry in it is kept unselected, to make it more readable.
        TreeItem[] selectedItems;
        if (singleChart) {
            selectedItems = new TreeItem[1];
            selectedItems[0] = chartsTree.getItem(0);
        }
        else {
            selectedItems = chartsTree.getSelection();
        }

        Set<String> widthValues = new HashSet<String>();
        Set<String> heightValues = new HashSet<String>();
        for (TreeItem ti : selectedItems) {
            int i = chartsTree.indexOf(ti);
            Chart chart = charts.get(i);
            String widthProp = chart.getPropertyValue("image_export_width");
            String heightProp = chart.getPropertyValue("image_export_height");
            widthValues.add(widthProp);
            heightValues.add(heightProp);
        }

        String initialWidth = widthValues.size() == 1 ? widthValues.iterator().next() : null;
        String initialHeight = heightValues.size() == 1 ? heightValues.iterator().next() : null;
        SetSizeDialog dialog = new SetSizeDialog(Display.getCurrent().getActiveShell(), initialWidth, initialHeight, selectedItems.length);
        if (dialog.open() == Window.OK) {
            String width = dialog.getWidth();
            String height = dialog.getHeight();
            if (StringUtils.isBlank(width) && StringUtils.isBlank(height))
                return;
            CompoundCommand command = new CompoundCommand("Set chart image export size");
            for (TreeItem ti : selectedItems) {
                int i = chartsTree.indexOf(ti);
                Chart chart = charts.get(i);

                Map<String, String> properties = new HashMap<String, String>();
                if (!StringUtils.isBlank(width) && !width.equals(chart.getPropertyValue("image_export_width")))
                    properties.put("image_export_width", width);
                if (!StringUtils.isBlank(height) && !height.equals(chart.getPropertyValue("image_export_height")))
                    properties.put("image_export_height", height);

                if (!properties.isEmpty())
                    command.append(new SetChartPropertiesCommand(chart, properties));
            }
            if (!command.isEmpty())
                commandStack.execute(command);
            updateChartLabels();
        }
    }

}
