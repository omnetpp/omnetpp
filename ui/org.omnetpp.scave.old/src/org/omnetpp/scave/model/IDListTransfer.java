/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import org.eclipse.swt.dnd.ByteArrayTransfer;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.TransferData;

import org.omnetpp.scave.engine.IDList;

/**
 * Drag & drop transfer type for an IDList.
 *
 * TODO IDs don't make sense outside this app instance! so it'd be nice to
 * find a way to prevent dragging it out of the app.
 */
public class IDListTransfer extends ByteArrayTransfer {

    private static final String TYPENAME = "SCAVE_IDLIST";

    private static final int TYPEID = registerType(TYPENAME);

    private static IDListTransfer _instance = new IDListTransfer();

    public static IDListTransfer getInstance() {
        return _instance;
    }

    public void javaToNative(Object object, TransferData transferData) {
        if (!validate(object) || !isSupportedType(transferData)) {
            DND.error(DND.ERROR_INVALID_DATA);
        }
        // original ByteArrayOutputStream code was too slow, using native code
        IDList idlist = (IDList) object;
        byte[] buffer = new byte[8*(int)idlist.size()];
        idlist.toByteArray(buffer);
        super.javaToNative(buffer, transferData);
    }

    public Object nativeToJava(TransferData transferData) {
        if (isSupportedType(transferData)) {
            byte[] buffer = (byte[]) super.nativeToJava(transferData);
            if (buffer == null)
                return null;
            // original ByteArrayInputStream code was too slow, using native code
            IDList idlist = new IDList();
            idlist.fromByteArray(buffer);
            return idlist;
        }

        return null;
    }

    protected String[] getTypeNames() {
        return new String[] { TYPENAME };
    }

    protected int[] getTypeIds() {
        return new int[] { TYPEID };
    }

    protected boolean validate(Object object) {
        if (object == null || !(object instanceof IDList))
            return false;
        return true;
    }
}
