/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.jface.preference.ColorFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.PathEditor;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.jface.preference.PreferenceManager;
import org.eclipse.jface.preference.PreferenceNode;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.preference.PreferenceStore;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.charting.ChartSWTWrapper;

public class ChartPropertyDialog extends PreferenceDialog {

    public ChartPropertyDialog(Shell shell, ChartSWTWrapper chart) {
        super(shell, createPreferenceManager(chart));
        setPreferenceStore(new PreferenceStore());
    }

    protected static PreferenceManager createPreferenceManager(ChartSWTWrapper chart) {
        PreferenceManager manager = new PreferenceManager();
        manager.addToRoot(new PreferenceNode("Title", new TitlePreferencePage("Title", chart)));
        manager.addToRoot(new PreferenceNode("Axes", new AxesPreferencePage("Axes")));
        manager.addToRoot(new PreferenceNode("Bars", new BarsPreferencePage("Bars")));
        manager.addToRoot(new PreferenceNode("Legend", new LegendPreferencePage("Legend")));
        manager.addToRoot(new PreferenceNode("Plot", new PlotPreferencePage("Plot")));
        return manager;
    }

    static class EmptyPreferencePage extends PreferencePage
    {
        public EmptyPreferencePage(String title)
        {
            super(title);
        }

        @Override
        protected Control createContents(Composite parent) {
            return new Composite(parent, SWT.NULL);
        }
    }

    static class PlotPreferencePage extends FieldEditorPreferencePage
    {

        protected PlotPreferencePage(String title) {
            super(title, FieldEditorPreferencePage.GRID);
        }

        @Override
        protected void createFieldEditors() {
            addField(new ColorFieldEditor("BackgroundColor", "Background color", getFieldEditorParent()));
            addField(new ColorFieldEditor("OutlineColor", "Outline color", getFieldEditorParent()));
            //addField(new DoubleFieldEditor("ForegroundAlpha", "Foreground alpha", getFieldEditorParent()));
        }
    }
}
