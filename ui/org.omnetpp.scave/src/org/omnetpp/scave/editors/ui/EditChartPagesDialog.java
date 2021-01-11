/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.ui.StyledTextUndoRedoManager;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;

/**
 * Dialog for editing the XSWT form pages of a chart.
 *
 * @author andras
 */
public class EditChartPagesDialog extends TitleAreaDialog {
    private String title;
    private SashForm mainSash;
    private TableViewer tableViewer;
    private SashForm contentSash;
    private StyledText styledText;
    private Composite previewHolder;

    private List<DialogPage> pages;
    private DialogPage editedPage;

    private DelayedJob updatePreviewJob = new DelayedJob(200) {
        @Override
        public void run() {
            if (!getShell().isDisposed())
                updatePreview();
        }
    };

    public EditChartPagesDialog(Shell parentShell, Chart chart) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Edit Chart Dialog Pages";
        this.pages = clone(chart.getDialogPages());
    }

    private List<DialogPage> clone(List<DialogPage> dialogPages) {
        List<DialogPage> clone = new ArrayList<>();
        for (DialogPage page : dialogPages)
            clone.add(new DialogPage(page));
        return clone;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
            shell.setText(title);
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        label.setText(text);
        return label;
    }

    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("Add/Remove/Edit Chart Dialog Pages");
        setMessage("Dialog pages prompt for data that serve as input for the chart script");

        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout());

        mainSash = new SashForm(composite, SWT.NONE);
        mainSash.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        Composite listPane = createCompactComposite(mainSash, 1, false, 0);
        contentSash = new SashForm(mainSash, SWT.NONE);

        createLabel(listPane, "Pages:");
        tableViewer = new TableViewer(listPane, SWT.BORDER);
        tableViewer.getTable().setLayoutData(new GridData(GridData.FILL_BOTH));
        tableViewer.setContentProvider(new ArrayContentProvider());
        tableViewer.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                return ((DialogPage)element).label;
            }
        });
        tableViewer.setInput(pages);

        ToolBar buttonBar = SWTFactory.createToolbar(listPane, SWT.HORIZONTAL, 1);
        ToolItem addButton = createToolButton(buttonBar, ScavePlugin.getCachedImage(ScaveImages.IMG_ETOOL16_NEWPAGE), "Add");
        ToolItem removeButton = createToolButton(buttonBar, ScavePlugin.getCachedImage(ScaveImages.IMG_ETOOL16_TRASHCAN), "Remove");
        ToolItem moveUpButton = createToolButton(buttonBar, ScavePlugin.getCachedImage(ScaveImages.IMG_ETOOL16_MOVE_UP), "Up");
        ToolItem moveDownButton = createToolButton(buttonBar, ScavePlugin.getCachedImage(ScaveImages.IMG_ETOOL16_MOVE_DOWN), "Down");
        ToolItem renameButton = createToolButton(buttonBar, ScavePlugin.getCachedImage(ScaveImages.IMG_ETOOL16_RENAME), "Rename");

        addButton.addSelectionListener(SelectionListener.widgetSelectedAdapter((e) -> addPage()));
        removeButton.addSelectionListener(SelectionListener.widgetSelectedAdapter((e) -> removePage()));
        moveUpButton.addSelectionListener(SelectionListener.widgetSelectedAdapter((e) -> moveUp()));
        moveDownButton.addSelectionListener(SelectionListener.widgetSelectedAdapter((e) -> moveDown()));
        renameButton.addSelectionListener(SelectionListener.widgetSelectedAdapter((e) -> renamePage()));

        String XSWT_HELP = "<p>XSWT is an XML-based UI description language for SWT, the widget toolkit of Eclipse on which the OMNeT++ IDE is based. "
                + "It is based on reflection, and thus, the content of XSWT files closely mirror SWT widget trees.</p>"
                + "<p>When used for requesting chart input from the user, <code>x:id</code> attributes serve as names "
                + "for properties that can be queried from the Python chart script, using <code>chart.get_properties()</code>.\n"
                + "For example, the property edited by a text input field defined as</p>"
                +"<pre>"
                + "&lt;text x:id=\"title\"&gt;"
                + "</pre>"
                + "<p>can be accessed from Python code as</p>"
                + "<pre>"
                + "props = chart.get_properties()\n"
                + "title = props[\"title\"]"
                + "</pre>";

        Composite contentPane = createCompactComposite(contentSash, 1, false, 0);
        SWTFactory.createInfoLink(contentPane, "XSWT source (<a>help</a>):", XSWT_HELP, 1);
        styledText = new StyledText(contentPane, SWT.BORDER|SWT.V_SCROLL|SWT.H_SCROLL);
        styledText.setLayoutData(new GridData(GridData.FILL_BOTH));
        new StyledTextUndoRedoManager(styledText);

        Composite previewPane = createCompactComposite(contentSash, 1, false, 0);
        createLabel(previewPane, "Preview:");
        previewHolder = new Composite(previewPane, SWT.BORDER);
        previewHolder.setLayoutData(new GridData(GridData.FILL_BOTH));
        previewHolder.setLayout(new GridLayout());
        previewHolder.setBackground(ColorFactory.PALE_GREEN2);

        tableViewer.addPostSelectionChangedListener(new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                DialogPage page = getTableSelection();
                if (page != editedPage) {
                    if (editedPage != null)
                        editedPage.xswtForm = styledText.getText();
                    if (page != null)
                        styledText.setText(page.xswtForm);
                    editedPage = page;
                    updatePreviewJob.cancel();
                    updatePreview();
                }
            }
        });

        styledText.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent e) {
                updatePreviewJob.restartTimer();
            }
        });

        Dialog.applyDialogFont(mainSash);

        tableViewer.refresh();

        mainSash.setWeights(new int[] {1,4}); // default
        UIUtils.restoreSashWeights(mainSash, getDialogBoundsSettings(), "mainSash");
        UIUtils.restoreSashWeights(contentSash, getDialogBoundsSettings(), "contentSash");

        return composite;
    }

    protected ToolItem createToolButton(ToolBar toolbar, Image image, String tooltip) {
        ToolItem item = new ToolItem(toolbar, SWT.PUSH);
        item.setImage(image);
        item.setToolTipText(tooltip);
        return item;
    }

    private Composite createCompactComposite(Composite parent, int columns, boolean makeEqualWidth, int gridDataStyle) {
        Composite composite = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(columns, makeEqualWidth);
        layout.marginWidth = layout.marginHeight = 0;
        composite.setLayout(layout);
        composite.setLayoutData(new GridData(gridDataStyle));
        return composite;
    }

    // note: currently unused
    public void toggleOrientation() {
        contentSash.setOrientation(contentSash.getOrientation()==SWT.HORIZONTAL ? SWT.VERTICAL : SWT.HORIZONTAL);
    }

    protected DialogPage getTableSelection() {
        Object firstElement = ((IStructuredSelection)tableViewer.getSelection()).getFirstElement();
        if (firstElement instanceof Chart.DialogPage)
            return (DialogPage)firstElement;
        return null;
    }

    protected void addPage() {
        NewChartPageDialog dialog = new NewChartPageDialog(getShell());
        if (dialog.open() == Dialog.OK) {
            DialogPage page = dialog.getResult();
            page.id = "page" + System.nanoTime(); //TODO better ID
            DialogPage beforePage = getTableSelection();
            if (beforePage == null)
                pages.add(page);
            else
                pages.add(pages.indexOf(beforePage), page);
            tableViewer.refresh();
            tableViewer.setSelection(new StructuredSelection(page));
        }
    }

    protected void removePage() {
        DialogPage page = getTableSelection();
        if (page != null) {
            int pos = pages.indexOf(page);
            pages.remove(page);
            tableViewer.refresh();

            if (pages.isEmpty())
                styledText.setText("");
            else {
                DialogPage newPage = pages.get(pos==pages.size() ? pos-1 : pos);
                tableViewer.setSelection(new StructuredSelection(newPage));
            }
        }
   }

    protected void moveUp() {
        DialogPage page = getTableSelection();
        if (page != null) {
            int oldPos = pages.indexOf(page);
            if (oldPos > 0) {
                pages.remove(oldPos);
                pages.add(oldPos-1, page);
                tableViewer.refresh();
            }
        }
    }

    protected void moveDown() {
        DialogPage page = getTableSelection();
        if (page != null) {
            int oldPos = pages.indexOf(page);
            if (oldPos < pages.size()-1) {
                pages.remove(oldPos);
                pages.add(oldPos+1, page);
                tableViewer.refresh();
            }
        }
    }

    protected void renamePage() {
        DialogPage page = getTableSelection();
        if (page != null) {
            InputDialog dialog = new InputDialog(getShell(), "Rename Page", "Label:", page.label, null);
            if (dialog.open() == Dialog.OK) {
                page.label = dialog.getValue();
                tableViewer.refresh();
            }
        }
    }

    @SuppressWarnings("unchecked")
    protected void updatePreview() {
        try {
            String xswtForm = styledText.getText();
            for (Control c : previewHolder.getChildren())
                c.dispose();
            Composite xswtHolder = SWTFactory.createComposite(previewHolder, 1, 1, GridData.FILL_BOTH);
            validateXml(xswtForm); // because XSWT is not very good at it
            Map<String,Control> xswtWidgetMap = XSWT.create(xswtHolder, new ByteArrayInputStream(xswtForm.getBytes()));
            setTooltips(xswtWidgetMap);
            setErrorMessage(null);
            previewHolder.requestLayout();
        }
        catch (SAXParseException e) {
            setErrorMessage("XML parse error",  e.getMessage(), e.getLineNumber(), e.getColumnNumber());
        }
        catch (XSWTException e) {
            setErrorMessage("XSWT error", e.getMessage(), e.row, e.column);
        }
        catch (Exception e) {
            setErrorMessage("Error: " + e.toString());
        }
    }

    protected void setTooltips(Map<String, Control> xswtWidgetMap) {
        for (String id : xswtWidgetMap.keySet())
            setTooltip(xswtWidgetMap.get(id), "x:id=\"" + id + "\"");
    }

    protected void setTooltip(Control control, String tooltip) {
        control.setToolTipText(tooltip);
        if (control instanceof Composite)
            for (Control c : ((Composite)control).getChildren())
                setTooltip(c, tooltip);
    }

    protected void validateXml(String xswtForm) throws SAXException, IOException, ParserConfigurationException {
        DocumentBuilder db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
        db.parse(new ByteArrayInputStream(xswtForm.getBytes()));
    }

    private int clamp(int x, int a, int b) {
        return Math.max(b, Math.min(a, x));
    }

    protected void setErrorMessage(String what, String message, int line, int col) {
        setErrorMessage(what + " at " + line + ":" + col + ": " + message);

        // highlight the line containing the error
        line = clamp(line-1, 0, styledText.getLineCount()-1); // convert to 0-based, and ensure it's valid (sometimes we can get line=-1)
        int start = styledText.getOffsetAtLine(line);
        boolean isLastLine = line >= styledText.getLineCount()-1;
        int end = isLastLine ? styledText.getCharCount() : styledText.getOffsetAtLine(line+1);
        StyleRange range = new StyleRange(start, end-start, null, ColorFactory.PINK);
        styledText.setStyleRange(null); // clear existing style
        styledText.setStyleRange(range);
    }

    public void setErrorMessage(String message) {
        super.setErrorMessage(message);
        styledText.setStyleRange(null); // clear existing style
    }

    @Override
    public boolean close() {
        UIUtils.saveSashWeights(mainSash, getDialogBoundsSettings(), "mainSash");
        UIUtils.saveSashWeights(contentSash, getDialogBoundsSettings(), "contentSash");
        return super.close();
    }

    protected void okPressed() {
        if (editedPage != null)
            editedPage.xswtForm = styledText.getText();
        super.okPressed();
    }

    public List<DialogPage> getResult() {
        return pages;
    }
}
