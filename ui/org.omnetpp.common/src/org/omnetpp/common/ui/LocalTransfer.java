/**
 * Copyright (c) 2002-2006 IBM Corporation and others.
 * All rights reserved.   This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   IBM - Initial API and implementation
 */
package org.omnetpp.common.ui;


import org.eclipse.swt.dnd.ByteArrayTransfer;
import org.eclipse.swt.dnd.TransferData;


/**
 * This derived implementation of a byte array transfer short circuits the transfer process
 * so that a local transfer does not serialize the object
 * and hence can and will return the original object, not just a clone.
 * You only really need ever know about {@link #getInstance LocalTransfer.getInstance()},
 * so that you can include it in when adding drag support to a viewer.
 * See {@link EditingDomainViewerDropAdapter} and {@link ViewerDragAdapter} for more details.
 * <p>
 * As an added guard, the time is recorded and serialized in javaToNative
 * to that native to java can ensure that it's returns the value that was really to have been transferred.
 *
 * THIS CLASS HAS BEEN COPIED FROM EMF
 */
public class LocalTransfer extends ByteArrayTransfer
{
  /**
   * This is the register transfer type name.
   */
  protected static final String TYPE_NAME = "local-transfer-format";

  /**
   * This is the ID that is registered to the name.
   */
  protected static final int TYPE_ID = registerType(TYPE_NAME);

  /**
   * This is initialized and returned by {@link #getInstance}.
   */
  protected static LocalTransfer instance;

  /**
   * This returns the one instance of this transfer agent.
   */
  public static LocalTransfer getInstance()
  {
    if (instance == null)
    {
      instance = new LocalTransfer();
    }

    return instance;
  }

  /**
   * This records the time at which the transfer data was recorded.
   */
  protected long startTime;

  /**
   * This records the data being transferred.
   */
  protected Object object;

  /**
   * This creates an instance; typically you get one from {@link #getInstance}.
   */
  protected LocalTransfer()
  {
    super();
  }

  /**
   * This returns the transfer IDs that this agent supports.
   */
  @Override
  protected int[] getTypeIds()
  {
    return new int[] { TYPE_ID };
  }

  /**
   * This returns the transfer names that this agent supports.
   */
  @Override
  public String[] getTypeNames()
  {
    return new String[] { TYPE_NAME };
  }

  /**
   * This records the object and current time and encodes only the current time into the transfer data.
   */
  @Override
  public void javaToNative(Object object, TransferData transferData)
  {
    startTime = System.currentTimeMillis();
    this.object = object;
    if (transferData != null)
    {
      super.javaToNative(String.valueOf(startTime).getBytes(), transferData);
    }
  }

  /**
   * This decodes the time of the transfer and returns the recorded the object if the recorded time and the decoded time match.
   */
  @Override
  public Object nativeToJava(TransferData transferData)
  {
    byte[] bytes = (byte[])super.nativeToJava(transferData);
    if (bytes == null) return null;

    try
    {
      long startTime = Long.parseLong(new String(bytes));
      return this.startTime == startTime ? object : null;
    }
    catch (NumberFormatException exception)
    {
     return null;
    }
  }
}
