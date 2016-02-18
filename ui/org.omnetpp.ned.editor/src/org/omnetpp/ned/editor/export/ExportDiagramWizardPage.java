/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.File;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ComboViewer;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
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
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.dialogs.WizardDataTransferPage;

public class ExportDiagramWizardPage extends WizardDataTransferPage
{
    private ComboViewer imageFormatComboViewer;
    private Combo outputFileNameCombo;
    private IPath diagramFilePath;
    private List<GraphicalEditPart> selectedEditParts;
    private ExportDiagramPreview diagramPreview;
    private List<DiagramExporter> diagramExporters;

    private static final String SETTINGS_OUTPUT_FILE_NAMES_ID = "ExportDiagramWizardPage.SETTINGS_OUTPUT_FILE_NAMES_ID";
    private static final String SETTINGS_IMAGE_FORMAT_ID = "ExportDiagramWizardPage.SETTINGS_IMAGE_FORMAT_ID";
    private static final String SETTINGS_ANTIALIAS_ID = "ExportDiagramWizardPage.SETTINGS_ANTIALIAS_ID";

    public ExportDiagramWizardPage(IPath diagramFilePath, List<GraphicalEditPart> selectedEditParts)
    {
        super("exportDiagramImageWizardPage");
        setTitle("Specify the image format.");
        setDescription("Select the format of the image.");
        this.diagramFilePath = diagramFilePath;
        this.selectedEditParts = selectedEditParts;
        this.diagramExporters = new ArrayList<DiagramExporter>();
        diagramExporters.add(new PNGDiagramExporter());
        diagramExporters.add(new SVGDiagramExporter());
    }

    @Override
    protected boolean allowNewContainerName()
    {
        return false;
    }

    public void createControl(Composite parent)
    {
        final Composite composite = new Composite(parent, SWT.NULL);
        composite.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        GridLayout layout = new GridLayout();
        layout.numColumns = 3;
        layout.verticalSpacing = 9;
        composite.setLayout(layout);

        diagramPreview = new ExportDiagramPreview(selectedEditParts);
        Control previewControl = diagramPreview.createControl(composite);
        GridData previewGridData = new GridData(SWT.FILL, SWT.FILL, true, true, layout.numColumns, 1);
        previewControl.setLayoutData(previewGridData);

        Label imageFormatLabel = new Label(composite, SWT.NULL);
        imageFormatLabel.setText("&Image format:");
        imageFormatLabel.setToolTipText("Choose the format of the diagram images.");

        imageFormatComboViewer = new ComboViewer(composite, SWT.READ_ONLY);
        imageFormatComboViewer.setContentProvider(new ArrayContentProvider());
        imageFormatComboViewer.setInput(diagramExporters);
        imageFormatComboViewer.setSelection(new StructuredSelection(diagramExporters.get(0)));
        imageFormatComboViewer.getControl().setFocus();
        imageFormatComboViewer.getControl().setToolTipText(imageFormatLabel.getToolTipText());
        imageFormatComboViewer.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                return ((DiagramExporter)element).getDescription();
            }
        });

        GridData imageFormatComboViewerGridData = new GridData(0, 0, false, false, layout.numColumns-1, 1);
        imageFormatComboViewer.getControl().setLayoutData(imageFormatComboViewerGridData);

        imageFormatComboViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            private DiagramExporter oldExporter = getImageExporter();
            @SuppressWarnings("synthetic-access")
            public void selectionChanged(SelectionChangedEvent event)
            {
                IPath outputPath = Path.fromOSString(getOutputFileName());
                DiagramExporter newExporter = getImageExporter();
                if (outputPath.getFileExtension()!=null && outputPath.getFileExtension().equals(oldExporter.getFileExtension())) {
                    IPath newOutputPath = outputPath.removeFileExtension().addFileExtension(newExporter.getFileExtension());
                    outputFileNameCombo.setText(newOutputPath.toOSString());
                }
                oldExporter = newExporter;
                updatePageComplete();
            }
        });

        Label outputFileNameLabel = new Label(composite, SWT.NULL);
        outputFileNameLabel.setText("&To file:");
        outputFileNameLabel.setToolTipText("Choose file where the exported image will be stored.");

        outputFileNameCombo = new Combo(composite, SWT.SINGLE | SWT.BORDER);
        if (diagramFilePath != null)
        {
            String defaultExtension = getImageExporter().getFileExtension();
            IPath defaultOutputPath = diagramFilePath.removeFileExtension().addFileExtension(defaultExtension);
            outputFileNameCombo.setText(defaultOutputPath.toOSString());
        }
        outputFileNameCombo.setToolTipText(outputFileNameLabel.getToolTipText());
        outputFileNameCombo.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        outputFileNameCombo.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e)
            {
                updatePageComplete();
            }
        });

        Button outputFileNameBrowseButton = new Button(composite, SWT.PUSH);
        outputFileNameBrowseButton.setText("Browse...");
        outputFileNameBrowseButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e)
            {
                handleBrowse();
            }
        });

        setControl(composite);
        restoreWidgetValues();
    }

    protected void handleBrowse()
    {
        FileDialog dialog = new FileDialog(getContainer().getShell(), SWT.SAVE);
        dialog.setText("Choose file for exported image");
        File file = new File(outputFileNameCombo.getText());
        if (file.getParent() != null)
            dialog.setFilterPath(file.getAbsolutePath());
        String selectedFileName = dialog.open();
        if (selectedFileName != null)
            outputFileNameCombo.setText(selectedFileName);
        updatePageComplete();
    }

    void updatePageComplete()
    {
        if (getOutputFileName().length() > 0) {
            File outputFile = new File(getOutputFileName());
            if (outputFile.isDirectory()) {
                String msg = "'{0}' is a directory.";
                this.setErrorMessage(MessageFormat.format(msg, outputFile));
                setPageComplete(false);
                return;
            }
            File parentDirectory = outputFile.getParentFile();
            if (parentDirectory != null) {
                if (!parentDirectory.exists()) {
                    String msg = "Directory '{0}' does not exist.";
                    setErrorMessage(MessageFormat.format(msg, parentDirectory));
                    setPageComplete(false);
                    return;
                }
                if (!parentDirectory.canWrite()) {
                    String msg = "Directory '{0}' is not writable.";
                    setErrorMessage(MessageFormat.format(msg, parentDirectory));
                    setPageComplete(false);
                    return;
                }
            }
        }
        setErrorMessage(null);
        setPageComplete(true);
    }

    public String getOutputFileName()
    {
        return outputFileNameCombo.getText();
    }

    protected void setOutputFileName(String outputFileName)
    {
        outputFileNameCombo.setText(outputFileName);
    }

    protected void addOutputFileNameHistoryItem(String outputFileName)
    {
        outputFileNameCombo.add(outputFileName);
    }

    public DiagramExporter getImageExporter()
    {
        assert imageFormatComboViewer.getSelection() instanceof IStructuredSelection;
        assert ((IStructuredSelection) imageFormatComboViewer.getSelection()).size() == 1;
        return (DiagramExporter)((IStructuredSelection)imageFormatComboViewer.getSelection()).getFirstElement();
    }

    public DiagramExporter getImageExporter(String name)
    {
        DiagramExporter diagramExporter = null;
        for (DiagramExporter d : diagramExporters) {
            if (d.getName().equals(name)) {
                diagramExporter = d;
                break;
            }
        }
        return diagramExporter;
    }

    protected void selectImageExporter(String name)
    {
        DiagramExporter imageExporter = getImageExporter(name);
        if (imageExporter != null)
            imageFormatComboViewer.setSelection(new StructuredSelection(imageExporter));
    }

    public List<GraphicalEditPart> getEditPartsToExport()
    {
        return diagramPreview.getEditPartsToExport();
    }

    public boolean isAntialias()
    {
        return diagramPreview.isAntialias();
    }

    public void handleEvent(Event event)
    {
        throw new RuntimeException("FIXME: does anything register this as a listener? event=" + event);
    }

    @Override
    protected void saveWidgetValues()
    {
        IDialogSettings settings = getDialogSettings();
        if (settings != null) {
            String[] outputFileNames = settings.getArray(SETTINGS_OUTPUT_FILE_NAMES_ID);
            if (outputFileNames == null)
                outputFileNames = new String[0];
            outputFileNames = addToHistory(outputFileNames, getOutputFileName());
            settings.put(SETTINGS_OUTPUT_FILE_NAMES_ID, outputFileNames);
            settings.put(SETTINGS_IMAGE_FORMAT_ID, getImageExporter().getName());
            settings.put(SETTINGS_ANTIALIAS_ID, isAntialias());
        }
    }

    protected void restoreWidgetValues()
    {
        IDialogSettings settings = getDialogSettings();
        if (settings != null) {
            String[] outputFileNames = settings.getArray(SETTINGS_OUTPUT_FILE_NAMES_ID);
            if (outputFileNames != null) {
                setOutputFileName(outputFileNames[0]);
                for (int i = 0; i < outputFileNames.length; i++)
                    addOutputFileNameHistoryItem(outputFileNames[i]);
                String imageExporterName = settings.get(SETTINGS_IMAGE_FORMAT_ID);
                selectImageExporter(imageExporterName);
            }
            diagramPreview.setAntialias(settings.getBoolean(SETTINGS_ANTIALIAS_ID));
        }
    }

    public void finish()
    {
        saveWidgetValues();
    }
}
