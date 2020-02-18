package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.treeproviders.ScaveModelLabelProvider;
import org.omnetpp.scave.export.FileFormats;
import org.omnetpp.scave.export.IChartExport;
import org.omnetpp.scave.export.IGraphicalExportFileFormat;
import org.omnetpp.scave.export.NativeChartExport;
import org.omnetpp.scave.model.Chart;

/**
 * Export Charts dialog
 */
public class ExportChartsDialog extends Dialog {

    private static final String
        KEY_TARGET_FOLDER = "folder",
        KEY_RENDERER = "renderer",
        KEY_FORMAT = "format";

    private List<Chart> charts;
    private List<Chart> initialSelection;
    private ScaveEditor editor;

    private Tree chartsTree;
    private Text folderText;
    private List<Button> rendererRadios = new ArrayList<Button>();
    private Combo fileFormatCombo;

    private List<Chart> selectedCharts;
    private IContainer targetFolder;
    private List<IChartExport> renderers;
    private IChartExport selectedRenderer;
    private List<IGraphicalExportFileFormat> formats;
    private IGraphicalExportFileFormat selectedFormat;

    public ExportChartsDialog(Shell parentShell, List<Chart> charts, List<Chart> initialSelection, ScaveEditor editor) {
        super(parentShell);
        this.charts = charts;
        this.initialSelection = initialSelection;
        this.editor = editor;
        renderers = new ArrayList<IChartExport>();
        renderers.add(new NativeChartExport(editor.getResultFileManager()));
        // renderers.add(new RChartExport(editor.getResultFileManager()));
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
        newShell.setText("Export Chart Graphics");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        final Composite composite = (Composite)super.createDialogArea(parent);

        composite.setLayout(new GridLayout());

        Composite panel = new Composite(composite, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        panel.setLayout(new GridLayout(2, false));
        Label label = new Label(panel, SWT.NONE);
        label.setText("Select charts to export:");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
        chartsTree = new Tree(panel, SWT.CHECK | SWT.BORDER);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 320;
        gridData.heightHint = 200;
        chartsTree.setLayoutData(gridData);
        ILabelProvider labelProvider = new ScaveModelLabelProvider();
        for (Chart chart : charts) {
            TreeItem item = new TreeItem(chartsTree, SWT.NONE);
            item.setText(labelProvider.getText(chart));
            item.setImage(labelProvider.getImage(chart));
            if (initialSelection.contains(chart))
                item.setChecked(true);
        }
        chartsTree.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                validateDialogContents();
            }
        });
        panel = new Composite(panel, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.CENTER, SWT.TOP, false, false));
        panel.setLayout(new GridLayout());
        Button button = new Button(panel, SWT.NONE);
        button.setText("Select All");
        button.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (TreeItem item : chartsTree.getItems())
                    item.setChecked(true);
            }
        });
        button = new Button(panel, SWT.NONE);
        button.setText("Deselect All");
        button.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                for (TreeItem item : chartsTree.getItems())
                    item.setChecked(false);
            }
        });

        panel = new Composite(composite, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        panel.setLayout(new GridLayout(2, false));

        label = new Label(panel, SWT.NONE);
        label.setText("Folder to save into:");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
        folderText = new Text(panel, SWT.BORDER);
        folderText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        button = new Button(panel, SWT.PUSH);
        button.setText("Browse...");
        button.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                IResourceSelectionDialog dialog = new IResourceSelectionDialog(composite.getShell(), IResource.PROJECT | IResource.FOLDER);
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

        panel = new Composite(composite, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        panel.setLayout(new GridLayout());

        label = new Label(panel, SWT.NONE);
        label.setText("Render using:");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        for (IChartExport renderer : renderers) {
            button = new Button(panel, SWT.RADIO);
            button.setText(renderer.getName());
            button.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
            button.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    updateFileFormats();
                }
            });
            rendererRadios.add(button);
        }

        panel = new Composite(composite, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        panel.setLayout(new GridLayout(2, false));
        label = new Label(panel, SWT.NONE);
        label.setText("File format:");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
        fileFormatCombo = new Combo(panel, SWT.NONE);
        fileFormatCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        button = new Button(panel, SWT.NONE);
        button.setText("Options...");
        button.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                IGraphicalExportFileFormat format = getSelectedFileFormat();
                if (format != null)
                    format.openOptionsDialog(composite.getShell());
            }
        });

        selectedRenderer = renderers.get(0);
        updateFileFormats();

        restoreDialogSettings();

        return composite;
    }

    private void updateFileFormats() {
        List<IGraphicalExportFileFormat> newFormats = new ArrayList<IGraphicalExportFileFormat>();
        IChartExport renderer = getSelectedRenderer();
        if (renderer != null) {
            for (IGraphicalExportFileFormat format : FileFormats.getAll()) {
                if (renderer.isSupportedFormat(format))
                    newFormats.add(format.clone());
            }
        }
        formats = newFormats;
        String items[] = new String[formats.size()];
        for (int i = 0; i < formats.size(); ++i)
            items[i] = formats.get(i).getDescription();
        fileFormatCombo.setItems(items);
        if (items.length > 0) {
            fileFormatCombo.select(0);
        }
    }

    protected void validateDialogContents() {
        boolean ok = true;

        IContainer folder = getTargetFolderFromControl();
        if (folder == null || !folder.isAccessible())
            ok = false;

        if (getSelectedChartsFromTree().isEmpty())
            ok = false;

        if (getButton(OK) != null) // it is null during dialog creation
            getButton(OK).setEnabled(ok);
    }

    @Override
    protected void okPressed() {
        selectedCharts = getSelectedChartsFromTree();
        targetFolder = getTargetFolderFromControl();
        selectedRenderer = getSelectedRenderer();
        selectedFormat = getSelectedFileFormat();

        saveDialogSettings();

        super.okPressed();
    }

    protected List<Chart> getSelectedChartsFromTree() {
        List<Chart> result = new ArrayList<Chart>();
        for (int i = 0; i < charts.size(); ++i) {
            TreeItem item = chartsTree.getItem(i);
            if (item.getChecked())
                result.add(charts.get(i));
        }
        return result;
    }

    protected IContainer getTargetFolderFromControl() {
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

    private IDialogSettings getDialogSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    private void saveDialogSettings() {
        IDialogSettings settings = getDialogSettings();

        String folder = folderText.getText();
        settings.put(KEY_TARGET_FOLDER, folder);

        IChartExport renderer = getSelectedRenderer();
        settings.put(KEY_RENDERER, renderer != null ? renderer.getName() : null);

        IGraphicalExportFileFormat format = getSelectedFileFormat();
        settings.put(KEY_FORMAT, format != null ? format.getName() : null);
    }

    private void restoreDialogSettings() {
        IDialogSettings settings = getDialogSettings();

        String folder = settings.get(KEY_TARGET_FOLDER);
        if (folder != null)
            folderText.setText(folder);

        String rendererName = settings.get(KEY_RENDERER);
        IChartExport renderer = getRenderer(rendererName);
        if (renderer == null && !renderers.isEmpty())
            renderer = renderers.get(0);
        setSelectedRenderer(renderer);

        String format = settings.get(KEY_FORMAT);
        IGraphicalExportFileFormat selectedFormat = getFileFormat(format);
        if (selectedFormat == null && !formats.isEmpty())
            selectedFormat = formats.get(0);
        setSelectedFileFormat(selectedFormat);
    }

    private IChartExport getSelectedRenderer() {
        return selectedRenderer;
//        int index = -1;
//        for (int i = 0; i < rendererRadios.size(); ++i)
//            if (rendererRadios.get(i).getSelection()) {
//                index = i;
//                break;
//            }
//        return index >= 0 && index < renderers.size() ? renderers.get(index) : null;
    }

    private void setSelectedRenderer(IChartExport renderer) {
//        for (Button radio : rendererRadios)
//            radio.setSelection(renderer != null && radio.getText().equals(renderer.getName()));
//        updateFileFormats();
    }

    private IChartExport getRenderer(String name) {
        if (name != null) {
            for (IChartExport renderer : renderers)
                if (name.equals(renderer.getName()))
                    return renderer;
        }
        return null;
    }

    private void setSelectedFileFormat(IGraphicalExportFileFormat format) {
        int index = -1;
        for (int i = 0; i < fileFormatCombo.getItemCount(); ++i)
            if (format.getDescription().equals(fileFormatCombo.getItem(i))) {
                index = i;
                break;
            }

        if (index >= 0)
            fileFormatCombo.select(index);
        else
            fileFormatCombo.clearSelection();
    }

    private IGraphicalExportFileFormat getFileFormat(String name) {
        if (name != null) {
            for (IGraphicalExportFileFormat format : formats)
                if (name.equals(format.getName()))
                    return format;
        }
        return null;
    }

    private IGraphicalExportFileFormat getSelectedFileFormat() {
        int index = fileFormatCombo.getSelectionIndex();
        return index >= 0 && index < formats.size() ? formats.get(index) : null;
    }

    public IChartExport getChartExporter() {
        return selectedRenderer;
    }

    public IGraphicalExportFileFormat getFileFormat() {
        return selectedFormat;
    }

    public IContainer getTargetFolder() {
        return targetFolder;
    }

    public List<Chart> getSelectedCharts() {
        return selectedCharts;
    }
}
