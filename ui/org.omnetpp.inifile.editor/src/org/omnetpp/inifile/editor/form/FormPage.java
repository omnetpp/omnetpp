/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.List;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.IAnalysisListener;

/**
 * Base class for inifile form editor pages.
 *
 * Note: no commit() method: all fields or controls on the page
 *should auto-commit, latest when they lose the focus.
 *
 * @author Andras
 */
public abstract class FormPage extends Composite {
    private static final Image BANNER_IMAGE = InifileEditorPlugin.getCachedImage("icons/full/misc/formpage_banner2.png");
    protected static final int RIGHT_MARGIN = 20; // >= scrollbar width
    private static Font titleFont = JFaceResources.getBannerFont();
    private InifileEditor inifileEditor;
    private HoverSupport hoverSupport = new HoverSupport();

    // IMPLEMENTATION NOTE: HOW TO KEEP FORM PAGES UP-TO-DATE.
    //
    // Form pages should keep themselves up-to-date (e.g. by invoking their
    // reread() method) whenever they directly change the document. For changes
    // caused by other events (e.g. toolbar actions like AddMissingKeys),
    // there is an InifileListener which rereads the form after 1000ms
    // if that wasn't done until that time automatically.
    //
    protected IInifileChangeListener inifileListener = new IInifileChangeListener() {
        public void modelChanged() {
            if (!FormPage.this.isDisposed() && inifileEditor.isFormPageDisplayed() && inifileEditor.getFormEditor().getActiveCategoryPage()==FormPage.this)
                reread();
        }
    };

    protected IAnalysisListener analysisListener = new IAnalysisListener() {
        @Override
        public void analysisCompleted(InifileAnalyzer analyzer) {
            DisplayUtils.runNowOrAsyncInUIThread(() -> {
                if (!FormPage.this.isDisposed() && inifileEditor.isFormPageDisplayed() && inifileEditor.getFormEditor().getActiveCategoryPage()==FormPage.this)
                    FormPage.this.analysisCompleted();
            });
        }
    };

    public FormPage(Composite parent, InifileEditor inifileEditor) {
        super(parent, SWT.NONE);
        this.inifileEditor = inifileEditor;
        getInifileDocument().addInifileChangeListener(inifileListener);
        getInifileAnalyzer().addAnalysisListener(analysisListener);
    }

    @Override
    public void dispose() {
        getInifileDocument().removeInifileChangeListener(inifileListener);
        getInifileAnalyzer().removeAnalysisListener(analysisListener);
        super.dispose();
    }

    protected InifileEditorData getEditorData() {
        return inifileEditor.getEditorData();
    }

    protected IInifileDocument getInifileDocument() {
        return inifileEditor.getEditorData().getInifileDocument();
    }

    protected InifileAnalyzer getInifileAnalyzer() {
        return inifileEditor.getEditorData().getInifileAnalyzer();
    }

    // used by field editors as well
    public void setEditorSelection(String section, String key) {
        inifileEditor.setSelection(section, key);
    }

    /**
     * String that identifies the form page, and also gets displayed
     * in the form editor's tree.
     */
    public abstract String getPageCategory();

    /**
     * Reads data from the document into the current page.
     * FormPage's implementation does nothing -- subclasses must override and actually implement the rereading!
     */
    public abstract void reread();

    /**
     * Reflect analysis results on the form.
     */
    public abstract void analysisCompleted();

    /**
     * Set the focus to an appropriate control in the form page.
     */
    @Override
    public abstract boolean setFocus();

    /**
     * Returns the list of keys that this page can edit.
     *
     * This method is currently only used for code maintenance,
     * to check that every registered config key appears on
     * some form page.
     */
    public abstract List<ConfigOption> getSupportedKeys();

    /**
     * Utility method for subclasses: add a title to the form
     */
    protected Composite createTitleArea(Composite parent, String category) {
        Composite titleArea = new Composite(parent, SWT.BORDER);
        GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1);
        gridData.heightHint = 44;
        titleArea.setLayoutData(gridData);
        titleArea.setBackground(ColorFactory.WHITE);
        titleArea.setLayout(new GridLayout(3, false));
        ((GridLayout)titleArea.getLayout()).horizontalSpacing = 12;
        ((GridLayout)titleArea.getLayout()).marginWidth = 10;

        Label imageLabel = new Label(titleArea, SWT.NONE);
        imageLabel.setImage(BANNER_IMAGE);
        imageLabel.setBackground(ColorFactory.WHITE);

        Label title = new Label(titleArea, SWT.NONE);
        title.setText(category);
        title.setFont(titleFont);
        title.setBackground(ColorFactory.WHITE);
        title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
        return titleArea;
    }

    /**
     * Should be called whenever for content changes. layout(true,true) is not enough
     * because ScrolledFormPage has extra things to do here.
     */
    public void layoutForm() {
        layout(true);
    }

    /**
     * Add tooltip (hover) support to the given control. Call this instead of instantiating
     * HoverSupport for each control or field editor, otherwise "F2 to focus" will only
     * work when hovering the control that owns the focus.
     */
    protected void addTooltipSupport(Control control, IHoverInfoProvider hoverTextProvider) {
        hoverSupport.adapt(control, hoverTextProvider);
    }

    /**
     * Position the cursor on the given section inside the form page (as far as possible or makes sense)
     */
    public void gotoSection(String section) {
        // do nothing by default (subclasses are advised to override)
    }

    /**
     * Position the cursor on the given entry inside the form page (as far as possible or makes sense)
     */
    public void gotoEntry(String section, String key) {
        // do nothing by default (subclasses are advised to override)
    }

    protected void showErrorDialog(RuntimeException e) {
        reread(); // restore "legal" form contents
        MessageDialog.openError(getShell(), "Error", e.getMessage()+".");
    }

}
