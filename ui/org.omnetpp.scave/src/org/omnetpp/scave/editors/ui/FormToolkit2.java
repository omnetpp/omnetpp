package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.FormColors;
import org.eclipse.ui.forms.IFormColors;
import org.eclipse.ui.forms.widgets.FormToolkit;

/**
 * A FormToolkit which also configures the colors of CTabFolder tabs.
 * (By default, both selected and unselected tabs are white).
 * 
 * @author Andras
 */
public class FormToolkit2 extends FormToolkit {
    public FormToolkit2(Display display) {
        super(display);
    }
    
    public FormToolkit2(FormColors colors) {
        super(colors);
    }

    @Override
    public void adapt(Control control, boolean trackFocus, boolean trackKeyboard) {
        super.adapt(control, trackFocus, trackKeyboard);
        setColors(control);
    }

    @Override
    public void adapt(Composite composite) {
        super.adapt(composite);
        setColors(composite);
    }

    protected void setColors(Control control) {
        if (control instanceof CTabFolder) {
            // make the selected tab stand out (by default, everything is white)
            CTabFolder tabfolder = (CTabFolder)control;
            Color blue = getColors().getColor(IFormColors.TB_BORDER);
            Color lighterBlue = new Color(null, FormColors.blend(blue.getRGB(), new RGB(255,255,255), 30));
            tabfolder.setSelectionBackground(new Color[]{blue, lighterBlue}, new int[] {25}, true);

            // other color variations tried:
            //Color white = getDisplay().getSystemColor(SWT.COLOR_WHITE);
            //tabfolder.setSelectionBackground(lighterBlue);
            //tabfolder.setSelectionBackground(new Color[]{lighterBlue, white}, new int[] {25}, true);
            //Color gray = getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND);
            //tabfolder.setBackground(gray);
            //tabfolder.setSelectionBackground(gray);
        }
    }
}
