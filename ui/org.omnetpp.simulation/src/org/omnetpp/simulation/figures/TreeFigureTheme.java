package org.omnetpp.simulation.figures;

import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.jface.viewers.StyledString.Styler;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;

/**
 * Abstracts platform differences in tree item rendering.
 *
 * @author Andras
 */
public class TreeFigureTheme {
    enum Theme { Windows7Blue, UbuntuAmbiance, MacOSX, Generic };
    private static final Theme theme;

    private static final boolean isWindows = Platform.getOS().equals(Platform.OS_WIN32);
    private static final boolean isLinux = Platform.getOS().equals(Platform.OS_LINUX);
    private static final boolean isOSX = Platform.getOS().equals(Platform.OS_MACOSX);

    // Colors
    private static final Color win7Blue_selectionBorder = new Color(null, 132, 172, 221);
    private static final Color win7Blue_selectionFillTop = new Color(null, 242, 248, 255);
    private static final Color win7Blue_selectionFillBottom = new Color(null, 208, 229, 255);
    private static final Color win7Blue_selectionForeground = new Color(null, 0, 0, 0);

    private static final Color win7Blue_inactiveSelectionBorder = new Color(null, 217, 217, 217);
    private static final Color win7Blue_inactiveSelectionFillTop = new Color(null, 250, 250, 250);
    private static final Color win7Blue_inactiveSelectionFillBottom = new Color(null, 229, 229, 229);

    private static final Color win7Blue_mouseoverBorder = new Color(null, 184, 214, 251);
    private static final Color win7Blue_mouseoverFillTop = new Color(null, 252, 253, 254);
    private static final Color win7Blue_mouseoverFillBottom = new Color(null, 235, 243, 253);

    // Note: these linux colors (and images) are from the Ubuntu Ambiance (12.04) theme
    private static final Color ubuntuAmbiance_selectionBorder = new Color(null, 235, 110, 60);
    private static final Color ubuntuAmbiance_selectionFillTop = new Color(null, 244, 125, 76);
    private static final Color ubuntuAmbiance_selectionFillBottom = new Color(null, 235, 110, 60);

    private static final Color ubuntuAmbiance_inactiveSelectionBorder = new Color(null, 218, 216, 213);
    private static final Color ubuntuAmbiance_inactiveSelectionFillTop = new Color(null, 235, 234, 233);
    private static final Color ubuntuAmbiance_inactiveSelectionFillBottom = new Color(null, 218, 216, 213);

    private static final Color listSelectionBackground = Display.getDefault().getSystemColor(SWT.COLOR_LIST_SELECTION);
    private static final Color listSelectionForeground = Display.getDefault().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT);
    private static final Color genericInactiveSelectionBackground = new Color(null, 229, 229, 229);

    private static final Color win7Blue_listSelectionBackground = new Color(null, 51, 153, 255); // for recognizing whether Windows is set to the default blue theme
    private static final Color ubuntuAmbiance_listSelectionBackground = new Color(null, 240, 119, 70); // for recognizing whether Linux uses the Ubuntu Ambiance theme

    // Tree plus/minus images.
    // note: load images unconditionally, so we always detect if something goes wrong with them, regardless of development platform
    private static final Image win7Blue_toggleClosed = SimulationPlugin.getCachedImage("icons/tree/toggle_win7blue_closed.png");
    private static final Image win7Blue_toggleMouseoverClosed = SimulationPlugin.getCachedImage("icons/tree/toggle_win7blue_closed_mouseover.png");
    private static final Image win7Blue_toggleOpen = SimulationPlugin.getCachedImage("icons/tree/toggle_win7blue_open.png");
    private static final Image win7Blue_toggleMouseoverOpen = SimulationPlugin.getCachedImage("icons/tree/toggle_win7blue_open_mouseover.png");

    private static final Image linux_toggleClosed = SimulationPlugin.getCachedImage("icons/tree/toggle_linux_closed.png");
    private static final Image linux_toggleMouseoverClosed = SimulationPlugin.getCachedImage("icons/tree/toggle_linux_closed_mouseover.png");
    private static final Image linux_toggleOpen = SimulationPlugin.getCachedImage("icons/tree/toggle_linux_open.png");
    private static final Image linux_toggleMouseoverOpen = SimulationPlugin.getCachedImage("icons/tree/toggle_linux_open_mouseover.png");

    private static final Image osx_toggleClosed = SimulationPlugin.getCachedImage("icons/tree/toggle_osx_closed.png");
    private static final Image osx_toggleSelectedClosed = SimulationPlugin.getCachedImage("icons/tree/toggle_osx_closed_selected.png");
    private static final Image osx_toggleOpen = SimulationPlugin.getCachedImage("icons/tree/toggle_osx_open.png");
    private static final Image osx_toggleSelectedOpen = SimulationPlugin.getCachedImage("icons/tree/toggle_osx_open_selected.png");

    static {
        if (isWindows && closeEnough(listSelectionBackground, win7Blue_listSelectionBackground, 20))
            theme = Theme.Windows7Blue;
        else if (isLinux && closeEnough(listSelectionBackground, ubuntuAmbiance_listSelectionBackground, 20))
            theme = Theme.UbuntuAmbiance;
        else if (isOSX)
            theme = Theme.MacOSX;
        else
            theme = Theme.Generic;
}

    private static boolean closeEnough(Color a, Color b, int tolerance) {
        return Math.abs(a.getRed()-b.getRed()) <= tolerance &&
               Math.abs(a.getGreen()-b.getGreen()) <= tolerance &&
               Math.abs(a.getBlue()-b.getBlue()) <= tolerance;
    }


    public TreeFigureTheme() {
    }

    /**
     * "active" = tree is focused or not
     */
    public void paintBackground(Graphics graphics, Rectangle r, int imageIndent, int contentWidth, boolean selected, boolean mouseOver, boolean treeActive, boolean mouseOverTree) {
        if (selected || mouseOver) {
            boolean shouldIndent = (theme == Theme.Windows7Blue); // other systems seem to draw full-width selection bar
            r = shouldIndent ? new Rectangle(r.x + imageIndent - 2, r.y, contentWidth + 4, r.height) : r.getCopy();
            r.height--; r.width--;

            if (theme == Theme.Windows7Blue) {
                if (selected && (treeActive || mouseOver))
                    drawGradientRoundedRect(graphics, r, win7Blue_selectionBorder, win7Blue_selectionFillTop, win7Blue_selectionFillBottom, 4);
                else if (selected && !treeActive)
                    drawGradientRoundedRect(graphics, r, win7Blue_inactiveSelectionBorder, win7Blue_inactiveSelectionFillTop, win7Blue_inactiveSelectionFillBottom, 4);
                else if (mouseOver)
                    drawGradientRoundedRect(graphics, r, win7Blue_mouseoverBorder, win7Blue_mouseoverFillTop, win7Blue_mouseoverFillBottom, 4);
            }
            else if (theme == Theme.UbuntuAmbiance) {
                if (selected && treeActive)
                    drawGradientRoundedRect(graphics, r, ubuntuAmbiance_selectionBorder, ubuntuAmbiance_selectionFillTop, ubuntuAmbiance_selectionFillBottom, 0);
                else if (selected && !treeActive)
                    drawGradientRoundedRect(graphics, r, ubuntuAmbiance_inactiveSelectionBorder, ubuntuAmbiance_inactiveSelectionFillTop, ubuntuAmbiance_inactiveSelectionFillBottom, 0);
            }
            else /* MacOSX and Generic */ {
                if (selected) {
                    graphics.setBackgroundColor(treeActive ? listSelectionBackground : genericInactiveSelectionBackground);
                    graphics.fillRectangle(r);
                }
            }
        }
    }

    protected void drawGradientRoundedRect(Graphics graphics, Rectangle r, Color border, Color fillTop, Color fillBottom, int cornerRadius) {
        graphics.setForegroundColor(fillTop);
        graphics.setBackgroundColor(fillBottom);
        graphics.fillGradient(r, true);
        graphics.setForegroundColor(border);
        graphics.drawRoundRectangle(r, cornerRadius, cornerRadius);
    }

    public void paintToggle(Graphics graphics, Point centerLoc, boolean expanded, boolean selected, boolean mouseOver, boolean treeActive, boolean mouseOverTree) {
        Image image = null;
        if (theme == Theme.Windows7Blue) {
            if (!treeActive && !mouseOverTree)
                image = null;
            else if (expanded)
                image = mouseOver ? win7Blue_toggleMouseoverOpen : win7Blue_toggleOpen;
            else
                image = mouseOver ? win7Blue_toggleMouseoverClosed : win7Blue_toggleClosed;
        }
        else if (theme == Theme.UbuntuAmbiance) {
            if (expanded)
                image = mouseOver ? linux_toggleMouseoverOpen : linux_toggleOpen;
            else
                image = mouseOver ? linux_toggleMouseoverClosed : linux_toggleClosed;
        }
        else /* MacOSX and Generic */ {
            if (selected && treeActive)
                image = expanded ? osx_toggleSelectedOpen : osx_toggleSelectedClosed;
            else
                image = expanded ? osx_toggleOpen : osx_toggleClosed;
        }

        if (image != null) {
            org.eclipse.swt.graphics.Rectangle size = image.getBounds();
            graphics.drawImage(image, centerLoc.x - size.width/2, centerLoc.y - size.height/2); //XXX or: mandate 16x16 images, and then we can pass simply the top-left corner as (x,y)
        }
    }

    public Color getSelectionForeground(boolean mouseOver, boolean active) { //XXX currently only used in the following method
        return theme == Theme.Windows7Blue ? win7Blue_selectionForeground : listSelectionForeground;
    }

    public StyledString getSelectedItemLabel(StyledString normalLabel, final boolean mouseOver, final boolean active) {
        return new StyledString(normalLabel.getString(), new Styler() {
            @Override public void applyStyles(TextStyle textStyle) {
                textStyle.foreground = getSelectionForeground(mouseOver, active);
            }
        });
    }
}
