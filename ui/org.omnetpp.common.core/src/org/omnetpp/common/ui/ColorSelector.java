/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.common.ui;

import org.eclipse.core.commands.common.EventManager;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.ColorDialog;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.color.ColorFactory;

/**
 * This is a modified version of <code>org.eclipse.jface.preference.ColorSelector</code>.
 * The reasons to copy and modify it are (1) to make the color image a small rectangle,
 * and (2) to let it support a "grayed" state when the color image displays an X.
 * 
 * <code>ColorSelector</code> is a wrapper for a button that displays a
 * selected <code>Color</code> and allows the user to change the selection.
 */
public class ColorSelector extends EventManager {
    /**
     * Property name that signifies the selected color of this
     * <code>ColorSelector</code> has changed.
     */
    public static final String PROP_COLORCHANGE = "colorValue"; 

    private Button fButton;
    private RGB fColorValue;

    /**
     * Create a new instance of the receiver and the button that it wraps in
     * the supplied parent <code>Composite</code>.
     */
    public ColorSelector(Composite parent) {
        fButton = new Button(parent, SWT.PUSH);
        fButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent event) {
                open();
            }
        });
        fButton.addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent event) {
                Image image = fButton.getImage();
                if (image != null)
                    image.dispose();
            }
        });
    }

    /**
     * Adds a property change listener to this <code>ColorSelector</code>.
     * Events are fired when the color in the control changes via the user
     * clicking an selecting a new one in the color dialog. No event is fired in
     * the case where <code>setColorValue(RGB)</code> is invoked.
     */
    public void addListener(IPropertyChangeListener listener) {
        addListenerObject(listener);
    }

    /**
     * Removes the given listener from this <code>ColorSelector</code>. Has
     * no affect if the listener is not registered.
     */
    public void removeListener(IPropertyChangeListener listener) {
        removeListenerObject(listener);
    }

    /**
     * Get the button control being wrapped by the selector.
     */
    public Button getButton() {
        return fButton;
    }

    /**
     * Return the currently displayed color, or null if no color was selected 
     * (the button displays an X).
     */
    public RGB getColorValue() {
        return fColorValue;
    }

    /**
     * Set the current color value and update the control. A null value is also accepted, 
     * and will display an X on the button. 
     */
    public void setColorValue(RGB rgb) {
        fColorValue = rgb;
        updateColorImage();
    }

    /**
     * Set whether or not the button is enabled.
     */
    public void setEnabled(boolean state) {
        getButton().setEnabled(state);
    }

    /**
     * Update the image being displayed on the button using the current color
     * setting.
     */
    protected void updateColorImage() {
        Image oldImage = fButton.getImage();
        if (oldImage != null)
            oldImage.dispose();
        fButton.setImage(ColorFactory.createColorImage(fColorValue));
    }

    /**
     * Activate the editor for this selector. This causes the color selection
     * dialog to appear and wait for user input.
     */
    public void open() {
        ColorDialog colorDialog = new ColorDialog(fButton.getShell());
        colorDialog.setRGB(fColorValue);
        RGB newColor = colorDialog.open();
        if (newColor != null) {
            RGB oldValue = fColorValue;
            fColorValue = newColor;
            final Object[] finalListeners = getListeners();
            if (finalListeners.length > 0) {
                PropertyChangeEvent pEvent = new PropertyChangeEvent(this, PROP_COLORCHANGE, oldValue, newColor);
                for (int i = 0; i < finalListeners.length; ++i) {
                    IPropertyChangeListener listener = (IPropertyChangeListener) finalListeners[i];
                    listener.propertyChange(pEvent);
                }
            }
            updateColorImage();
        }
    }
}
