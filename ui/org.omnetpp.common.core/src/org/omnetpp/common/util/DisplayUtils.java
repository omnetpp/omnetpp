/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.util.Set;
import java.util.concurrent.Callable;

import org.eclipse.e4.ui.css.swt.theme.IThemeEngine;
import org.eclipse.e4.ui.services.IStylingEngine;
import org.eclipse.jface.resource.ColorRegistry;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.themes.ITheme;
import org.eclipse.ui.themes.IThemeManager;
import org.omnetpp.common.color.ColorFactory;

public class DisplayUtils {
    public static void runNowOrAsyncInUIThread(Runnable runnable) {
        if (Display.getCurrent() == null)
            Display.getDefault().asyncExec(runnable);
        else
            runnable.run();
    }

    /**
     * Runs the given runnable in the UI thread, and waits for it to finish. If the
     * runnable throws an exception, it is re-thrown in the calling thread (wrapped
     * into a RuntimeException if it is a checked exception).
     *
     * Re-throwing is important: exceptions thrown from a Display.syncExec() runnable
     * do NOT necessarily reach the calling thread. Depending on how the UI thread
     * happens to pick up the runnable (e.g. via UILockListener.doPendingWork() when
     * it is itself blocked on a lock), the exception may escape into the SWT event
     * loop and only end up in the error log, leaving the caller under the impression
     * that everything went fine. Errors must never go unnoticed like that.
     */
    public static void runNowOrSyncInUIThread(Runnable runnable) {
        if (Display.getCurrent() != null) {
            runnable.run();
            return;
        }

        Throwable[] exception = new Throwable[1];
        Display.getDefault().syncExec(() -> {
            try {
                runnable.run();
            }
            catch (Throwable e) {
                exception[0] = e;
            }
        });

        if (exception[0] != null)
            throw toRuntimeException(exception[0]);
    }

    /**
     * Runs the given callable in the UI thread, waits for it to finish, and returns
     * its result. Exceptions are re-thrown in the calling thread, see
     * runNowOrSyncInUIThread().
     */
    public static <T> T callNowOrSyncInUIThread(Callable<T> callable) {
        Object[] result = new Object[1];
        runNowOrSyncInUIThread(() -> {
            try {
                result[0] = callable.call();
            }
            catch (Exception e) {
                throw toRuntimeException(e);
            }
        });

        @SuppressWarnings("unchecked")
        T typedResult = (T)result[0];
        return typedResult;
    }

    private static RuntimeException toRuntimeException(Throwable e) {
        if (e instanceof Error)
            throw (Error)e;
        if (e instanceof RuntimeException)
            return (RuntimeException)e;
        return new RuntimeException(e.getMessage(), e);
    }

    /**
     * Adds an event filter in front of existing filters. This is needed if you want
     * to receive all keypresses including hotkeys, because then you want to be in
     * front of the key binding service (which is also an event filter on Display,
     * and it translates keypress events that correspond to hotkeys to something else).
     */
    public static void addAsFirstFilter(int eventType, Listener listener) {
        // first, make room in the table (this adds the listener at the end of the table)
        Display display = Display.getDefault();
        display.addFilter(eventType, listener);

        // shift everything up, then add our listener to slot 0. Sorry about the reflection,
        // there seem to be no other way
        Object filterTable = ReflectionUtils.getFieldValue(display, "filterTable");
        int[] types = (int[]) ReflectionUtils.getFieldValue(filterTable, "types");
        Listener[] listeners = (Listener[]) ReflectionUtils.getFieldValue(filterTable, "listeners");
        System.arraycopy(types, 0, types, 1, types.length-1);
        System.arraycopy(listeners, 0, listeners, 1, listeners.length-1);
        types[0] = eventType;
        listeners[0] = listener;
        for (int i = 1; i < types.length; i++) {
            if (types[i] == eventType && listeners[i] == listener) {
                types[i] = 0; listeners[i] = null; break; // remove instance added first
            }
        }
    }

    @SuppressWarnings("restriction")
    public static boolean isDarkTheme() {
        try {
            // Approach 1: Use Eclipse E4 Theme Engine to detect current theme
            IThemeEngine themeEngine = PlatformUI.getWorkbench().getService(IThemeEngine.class);
            String activeThemeId = themeEngine != null ? themeEngine.getActiveTheme().getId() : null;
            if (activeThemeId != null) {
                // known theme IDs: "org.eclipse.e4.ui.css.theme.e4_dark", "....e4_light", "....e4_default", "....e4_classic"
                // WTF is "default"?
                activeThemeId = activeThemeId.toLowerCase();
                if (activeThemeId.contains("dark"))
                    return true;
                if (activeThemeId.contains("light") || activeThemeId.contains("default") || activeThemeId.contains("classic"))
                    return false;
                // undecided, fall through
            }
        }
        catch (Exception e) {
            // Theming may be disabled, or theme engine may not be available right now (e.g. during startup)
        }

        try {
            // Approach 2: Use IStylingEngine with direct color query
            IStylingEngine stylingEngine = PlatformUI.getWorkbench().getService(IStylingEngine.class);
            if (stylingEngine != null) {
                // Create a temporary shell to query theme-aware colors
                Display display = Display.getCurrent();
                org.eclipse.swt.widgets.Shell tempShell = new org.eclipse.swt.widgets.Shell(display);
                try {
                    // Apply styling to get current theme's colors applied to the shell
                    stylingEngine.style(tempShell);

                    // Query the background color directly from the styled shell
                    Color backgroundColor = tempShell.getBackground();
                    if (backgroundColor != null) {
                        int brightness = (backgroundColor.getRed() + backgroundColor.getGreen() + backgroundColor.getBlue()) / 3;
                        return brightness < 128; // Threshold: values below 128 are considered dark
                    }
                }
                finally {
                    tempShell.dispose();
                }
            }
        } catch (Exception e) {
            // Styling engine might not be available or accessible
        }

        // Fallback: If both approaches fail, fall back to system theme as last resort
        return Display.isSystemDarkTheme();
    }

    private static final int[] SYSTEM_COLORS = {
        SWT.COLOR_INFO_BACKGROUND,
        SWT.COLOR_INFO_FOREGROUND,
        SWT.COLOR_LINK_FOREGROUND,
        SWT.COLOR_LIST_BACKGROUND,
        SWT.COLOR_LIST_FOREGROUND,
        SWT.COLOR_LIST_SELECTION,
        SWT.COLOR_LIST_SELECTION_TEXT,
        SWT.COLOR_TEXT_DISABLED_BACKGROUND,
        SWT.COLOR_TITLE_BACKGROUND,
        SWT.COLOR_TITLE_BACKGROUND_GRADIENT,
        SWT.COLOR_TITLE_FOREGROUND,
        SWT.COLOR_TITLE_INACTIVE_BACKGROUND,
        SWT.COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT,
        SWT.COLOR_TITLE_INACTIVE_FOREGROUND,
        SWT.COLOR_WIDGET_BACKGROUND,
        SWT.COLOR_WIDGET_BORDER,
        SWT.COLOR_WIDGET_DARK_SHADOW,
        SWT.COLOR_WIDGET_DISABLED_FOREGROUND,
        SWT.COLOR_WIDGET_FOREGROUND,
        SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW,
        SWT.COLOR_WIDGET_LIGHT_SHADOW,
        SWT.COLOR_WIDGET_NORMAL_SHADOW,
    };

    private static final String[] COLOR_NAMES = {
        "COLOR_INFO_BACKGROUND",
        "COLOR_INFO_FOREGROUND",
        "COLOR_LINK_FOREGROUND",
        "COLOR_LIST_BACKGROUND",
        "COLOR_LIST_FOREGROUND",
        "COLOR_LIST_SELECTION",
        "COLOR_LIST_SELECTION_TEXT",
        "COLOR_TEXT_DISABLED_BACKGROUND",
        "COLOR_TITLE_BACKGROUND",
        "COLOR_TITLE_BACKGROUND_GRADIENT",
        "COLOR_TITLE_FOREGROUND",
        "COLOR_TITLE_INACTIVE_BACKGROUND",
        "COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT",
        "COLOR_TITLE_INACTIVE_FOREGROUND",
        "COLOR_WIDGET_BACKGROUND",
        "COLOR_WIDGET_BORDER",
        "COLOR_WIDGET_DARK_SHADOW",
        "COLOR_WIDGET_DISABLED_FOREGROUND",
        "COLOR_WIDGET_FOREGROUND",
        "COLOR_WIDGET_HIGHLIGHT_SHADOW",
        "COLOR_WIDGET_LIGHT_SHADOW",
        "COLOR_WIDGET_NORMAL_SHADOW",
    };

    /**
     * Dumps SWT system colors to the console.
     */
    public static void dumpSwtSystemColors() {
        var display = Display.getCurrent();
        // Print the name and RGB values of the system colors
        for (int i = 0; i < SYSTEM_COLORS.length; ++i) {
            String colorName = COLOR_NAMES[i];
            Color color = display.getSystemColor(SYSTEM_COLORS[i]);
            System.out.println("Color Name: " + colorName + ", RGB: " 
                               + color.getRed() + "," + color.getGreen() + "," + color.getBlue());
        }
    }

    /**
     * Dumps Eclipse theme colors to the console.
     */
    public static void dumpEclipseThemeColors() {
        IThemeManager tm = PlatformUI.getWorkbench().getThemeManager();
        ITheme theme = tm.getCurrentTheme();
        ColorRegistry reg = theme.getColorRegistry();

        Set<String> keys = reg.getKeySet();

        for (String k : keys) {
            System.out.println(k);
            System.out.println(reg.get(k));
        }
    }

    /**
     * Shows a window with colored squares of all SWT and Eclipse colors.
     */
    public static void openSwatch() {
        Shell shell = new Shell(SWT.CLOSE | SWT.RESIZE);
        shell.setText("Color Swatch");
        shell.setLayout(new FillLayout());

        ScrolledComposite scroll = new ScrolledComposite(shell, SWT.V_SCROLL);
        scroll.setExpandHorizontal(true);
        scroll.setExpandVertical(true);

        Composite content = new Composite(scroll, SWT.NONE);

        content.setLayout(new GridLayout(2, false));

        Device device = shell.getDisplay();

        for (int i = 0; i < SYSTEM_COLORS.length; i++) {
            Color color = device.getSystemColor(SYSTEM_COLORS[i]);

            Label colorBox = new Label(content, SWT.NONE);
            colorBox.setLayoutData(new GridData(15, 15));
            colorBox.setImage(ColorFactory.createColorImage(color.getRGB()));

            Label label = new Label(content, SWT.NONE);
            label.setText(COLOR_NAMES[i]);
        }

        Label sep = new Label(content, SWT.NONE);
        sep.setText("Eclipse theme colors:");
        GridData gd = new GridData();
        gd.horizontalSpan = 2;
        sep.setLayoutData(gd);

        IThemeManager tm = PlatformUI.getWorkbench().getThemeManager();
        ITheme theme = tm.getCurrentTheme();
        ColorRegistry reg = theme.getColorRegistry();

        Set<String> keys = reg.getKeySet();

        for (String k : keys) {
            Color color = reg.get(k);

             Label colorBox = new Label(content, SWT.NONE);
             colorBox.setLayoutData(new GridData(15, 15));
             colorBox.setImage(ColorFactory.createColorImage(color.getRGB()));

             Label label = new Label(content, SWT.NONE);
             label.setText(k);
        }

        // Tell ScrolledComposite which control to scroll
        scroll.setContent(content);
        scroll.setMinSize(content.computeSize(SWT.DEFAULT, SWT.DEFAULT));

        shell.setSize(500, 400);
        shell.open();
    }
}
