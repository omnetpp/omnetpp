package org.omnetpp.test.gui.core;

import junit.framework.Assert;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkbenchUtils;

import org.eclipse.swt.SWT;



public class KeyboardTest extends GUITestCase
{
	public void testKeyboard() throws Throwable {
	    WorkbenchUtils.ensurePerspectiveActivated(".*OMN.*");
	    WorkbenchWindowAccess.getWorkbenchWindow().chooseFromMainMenu("File|New.*|.*Other.*");
	    TextAccess text = Access.findShellWithTitle("New").findTextAfterLabel("Wizards:");
	    for (char c=32; c<128; ++c) {
	        text.typeOver(""+c);
	        String result = text.getTextContent();
	        Assert.assertTrue("ASCII: "+(int)c+" expected: '"+c+"' result: '"+result+"'", result.equals(""+c));
	    }
	    text.pressKey(SWT.ESC);
	}
	
}
