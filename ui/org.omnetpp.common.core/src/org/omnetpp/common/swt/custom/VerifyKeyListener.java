/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.common.swt.custom;

import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.internal.SWTEventListener;

/**
 * Classes which implement this interface provide a method
 * that deals with the event that is generated when a
 * key is pressed.
 *
 * @see VerifyEvent
 * @see <a href="http://www.eclipse.org/swt/">Sample code and further information</a>
 */
public interface VerifyKeyListener extends SWTEventListener {
/**
 * The following event fields are used:<ul>
 * <li>event.character is the character that was typed (input)</li>
 * <li>event.keyCode is the key code that was typed (input)</li>
 * <li>event.stateMask is the state of the keyboard (input)</li>
 * <li>event.doit is processed or not (output)</li>
 * </ul>
 * @param event the verify event
 * @see VerifyEvent
 */
public void verifyKey (VerifyEvent event);
}
