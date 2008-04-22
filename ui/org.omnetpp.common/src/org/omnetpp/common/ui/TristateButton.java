package org.omnetpp.common.ui;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

//import org.eclipse.core.runtime.IStatus;
//import org.eclipse.core.runtime.Status;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

/**
 * A checkbox button with three states. To achieve the third state, this class
 * attempts to change the checkbox's colour to gray. This seems to work fine in
 * operating system's like linux, but in Windows due to the nature of the GDI,
 * this does not seem to work. However, Windows has native support for a
 * tri-state checkbox. Thus, in Windows this class attempts to hook into SWT's
 * lower level functions and make direct winapi calls to create the tri-state
 * button. If for any reason the api calls fail, this class will revert to its
 * default algorithm of colouring the button (which probably won't show up
 * properly... but at least there won't be crashes).
 * 
 * The win32 portions of this class are based off Klaus Wenger's code, which was
 * posted as a workaround for Eclipse bug #22261 
 * (https://bugs.eclipse.org/bugs/show_bug.cgi?id=22261). The code has been
 * reflected to ensure that this class will compile on non-Windows platforms.
 * 
 * @author emiranda, <a href="mailto:klaus.wenger@u14n.com">Klaus Wenger</a>
 */
@SuppressWarnings("unchecked")
public class TristateButton extends Button {
    private boolean grayed;        // The "gray" state flag
    private boolean eventEn;       // Used for disabling events so that we don't self trigger them
    private boolean allowUserGray; // Allows users to set the "gray" state by clicking on the checkbox
    private Class   win32os;       // Reference to the Windows OS class
    
    public static final int WIN32_TRISTATE_FAIL = 590;
    
    /**
     * Constructor.
     */
    public TristateButton(Composite parent, int style) {
        this(parent,style,false);
    }
    
    /**
     * Constructor.
     */
    public TristateButton(Composite parent,int style,boolean allowUserGray) {
        super(parent, style);
        
        try {
            // Try to grab the win32 OS module
            // This should always work in win32 and always fail on other platforms
            win32os = getClass().getClassLoader().loadClass("org.eclipse.swt.internal.win32.OS"); //$NON-NLS-1$
        }
        catch(ClassNotFoundException ex) {
            // Looks like we couldn't grab it, so this computer is probably not
            // running Windows
            win32os = null;
        }
        
        this.allowUserGray = allowUserGray;
        
        eventEn = true;
        
        setGrayed(grayed = false);
        this.addListener(SWT.Selection,new Listener () {
            public void handleEvent(Event e) {
                if (!eventEn)
                    return;
                
                if (grayed && win32os == null)
                    // Since Windows does this natively, we don't need to uncolour if
                    // the user is running windows
                    setGrayed(false);
                else if (TristateButton.this.allowUserGray && !grayed && getSelection())
                    setGrayed(true);
                
                grayed = getGrayed(); // Fix for win32 getting stuck on gray
            }
        });
    }

    protected void checkSubclass () {
        // Forcing permitted subclassing
    }
    
    protected void checkWidget() {
        super.checkWidget();
        
        if (win32os == null)
            return;
        
        try {
            Method getWindowLong = win32os.getMethod("GetWindowLong",int.class,int.class); //$NON-NLS-1$
            Method setWindowLong = win32os.getMethod("SetWindowLong",int.class,int.class,int.class); //$NON-NLS-1$
            Field  handle        = getClass().getField("handle"); //$NON-NLS-1$
            Field  gwl_style     = win32os.getField("GWL_STYLE"); //$NON-NLS-1$
            Field  bs_checkbox   = win32os.getField("BS_CHECKBOX"); //$NON-NLS-1$
            
            int bits = ((Integer)getWindowLong.invoke(null,handle.get(this),gwl_style.get(null))).intValue();
            
            bits |= BS_3STATE;
            bits &= ~bs_checkbox.getInt(null);
            
            setWindowLong.invoke(null,handle.get(this),gwl_style.get(this),new Integer(bits));
        }
        catch(Exception ex) {
            // Adds a warning to SKT UI's log
//            SKTUIPlugin.log(new Status(IStatus.WARNING, SKTUIPlugin.PLUGIN_SKTUI,
//                    WIN32_TRISTATE_FAIL,"Exception occurred calling native win32 tri-state button",ex)); //$NON-NLS-1$
            win32os = null;
        }
    }

    /**
     * Allows or disallows the user from setting the "gray" state.
     */
    public void setAllowUserGray(boolean allowUserGray) {
        this.allowUserGray = allowUserGray;
    }
    
    /**
     * Sets the button into / out of the "gray" state.
     */
    public void setGrayed(boolean grayed) {
        if ((getStyle() & SWT.CHECK) == 0)
            return;
            
        if (win32os != null) {
            try {
                Method getWindowLong = win32os.getMethod("GetWindowLong",int.class,int.class); //$NON-NLS-1$
                Method setWindowLong = win32os.getMethod("SetWindowLong",int.class,int.class,int.class); //$NON-NLS-1$
                Method sendMessage   = win32os.getMethod("SendMessage",int.class,int.class,int.class,int.class); //$NON-NLS-1$
                Field  handle        = getClass().getField("handle"); //$NON-NLS-1$
                Field  bst_checked   = win32os.getField("BST_CHECKED"); //$NON-NLS-1$
                Field  bst_unchecked = win32os.getField("BST_UNCHECKED"); //$NON-NLS-1$
                Field  gwl_style     = win32os.getField("GWL_STYLE"); //$NON-NLS-1$
                Field  bm_setcheck   = win32os.getField("BM_SETCHECK"); //$NON-NLS-1$
            
                checkWidget();
                
                int flags = grayed ? BST_INDETERMINATE : getSelection()
                        ? bst_checked.getInt(null) : bst_unchecked.getInt(null);
                
                /*
                 * Feature in Windows. When BM_SETCHECK is used
                 * to set the checked state of a radio or check
                 * button, it sets the WM_TABSTOP style. This
                 * is undocumented and unwanted. The fix is
                 * to save and restore the window style bits.
                 */
                int bits = ((Integer)getWindowLong.invoke(null,handle.get(this),gwl_style.get(null))).intValue();
                sendMessage.invoke(null,handle.get(this),bm_setcheck.get(null),new Integer(flags),new Integer(0));
                setWindowLong.invoke(null,handle.get(this),gwl_style.get(this),new Integer(bits));
            }
            catch(Exception ex) {
                // Adds a warning to SKT UI's log
//                SKTUIPlugin.log(new Status(IStatus.WARNING, SKTUIPlugin.PLUGIN_SKTUI,
//                        WIN32_TRISTATE_FAIL,"Exception occurred calling native win32 tri-state button",ex)); //$NON-NLS-1$
                win32os = null;
                setGrayed(grayed);
            }
            return;         
        }
        
        Color c = grayed ? new Color(null,192,192,192) : new Color(null,0,0,0);
        setForeground(c);
        
        this.grayed = grayed;
        
        eventEn = false;
        // Prevent event from being handled locally so we don't get stuct in
        // a firing / handling loop
        setSelection(true);
        eventEn = true;
    }
    
    /**
     * Checks if the button is in the "gray" state.
     */
    public boolean getGrayed() {
        if ((getStyle() & SWT.CHECK) == 0)
            return false;
        
        if (win32os != null) {
            try {
                Method sendMessage = win32os.getMethod("SendMessage",int.class,int.class,int.class,int.class); //$NON-NLS-1$
                Field  handle      = getClass().getField("handle"); //$NON-NLS-1$
                Field  bm_getcheck = win32os.getField("BM_GETCHECK"); //$NON-NLS-1$
                
                checkWidget();
                
                int state = ((Integer)sendMessage.invoke(null,handle.get(this),bm_getcheck.get(null),new Integer(0),new Integer(0))).intValue();
                
                return (state & BST_INDETERMINATE) != 0;
            }
            catch(Exception ex) {
                // Adds a warning to SKT UI's log
//                SKTUIPlugin.log(new Status(IStatus.WARNING, SKTUIPlugin.PLUGIN_SKTUI,
//                        WIN32_TRISTATE_FAIL,"Exception occurred calling native win32 tri-state button",ex)); //$NON-NLS-1$
                win32os = null;
                setGrayed(false);
            }
        }
        
        return grayed;
    }
    
    private static final int BS_3STATE = 0x00000005;
    private static final int BST_INDETERMINATE = 0x0002;
}


