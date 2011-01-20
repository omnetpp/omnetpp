package org.omnetpp.ned.editor.graph.dialogs;

import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;

/**
 * Interface for controls in the Properties dialog.
 * 
 * Field editors support a textual get/set interface, because everything in display strings 
 * and NED items are text. (Even numeric display string args like width/height are text 
 * because they may contain $PARAM, "-" the antivalue, etc.) They must be able to display
 * and return an empty string (this is a challenge e.g. with a Spinner).
 * 
 * Field editors also support a "grayed" state that indicates that different items in 
 * the selection have different values for the given property. When the user edits a grayed
 * field, it loses the grayed state, meaning that the edited field value (even if the field
 * is empty!) will be written back into the selected NED objects.
 * 
 * The grayed state can also be used for values that cannot be displayed or edited by the 
 * underlying SWT widget (e.g $PARAM cannot be edited in a Spinner that only supports numeric 
 * values.)
 * 
 * @author Andras
 */
public interface IFieldEditor {
    /**
     * Recommended background color for the "grayed" state.
     */
    public static final Color GREY = ColorFactory.asColor("#E0E0FF"); // should be distinguishable from the "disabled" color both on all platforms (Win,Linux,OSX)

    /**
     * Returns whether the control is in the "grayed" state, usually represented
     * with grey background.
     * 
     * The "grayed" state is used when (a) different items in the selection have
     * different values for the edited property, or (b) the control cannot
     * properly display the value (for example, Spinners can only display
     * numbers but the display string may also contain parameter references with
     * the "$paramname" syntax.)
     * 
     * Grayed controls are editable, but lose the "grayed" state on editing.
     */
    boolean isGrayed();

    /**
     * Turns "grayed" state on/off
     */
    void setGrayed(boolean grayed);

    /**
     * Return the value of the control
     */
    String getText();
    
    /**
     * Sets the control's value
     */
    void setText(String text);
    
    /**
     * Returns whether the control is enabled
     */
    boolean isEnabled();
    
    /**
     * Enable/disable the control
     */
    void setEnabled(boolean enabled);

    /**
     * Sets the validation message for the field editor. The severity parameter is one of the
     * IMarker.SEVERITY_xxx constants, and affect the displayed icon. There is only a single
     * message, not one for each severity. Pass text=null (with any severity) to clear the message. 
     */
    void setMessage(int severity, String text);
    
    /**
     * Get the SWT control to be used for layouting (i.e. setLayoutData()) 
     */
    Control getControl(); // for layouting

    /**
     * Add modification listener
     */
    void addModifyListener(ModifyListener listener);
    
    /**
     * Remove modification listener
     */
    void removeModifyListener(ModifyListener listener);
}