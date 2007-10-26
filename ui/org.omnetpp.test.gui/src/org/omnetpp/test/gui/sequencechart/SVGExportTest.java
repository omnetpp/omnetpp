package org.omnetpp.test.gui.sequencechart;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class SVGExportTest
    extends EventLogFileTestCase
{
    public SVGExportTest() {
        super("stress.log");
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        openFileFromProjectExplorerViewInSequenceChartEditor();
    }

    public void testExportRangeOfTwoSelectedEvents() throws Exception {
        testExport("Range.*");
    }
    
    public void testExportVisibleArea() throws Exception {
        testExport("Visible area.*");
    }

    public void testExportWholeEventLog() throws Exception {
        testExport("Whole.*");
    }

    protected void testExport(String text) throws CoreException {
        WorkspaceUtils.ensureFileNotExists(projectName, "stress.svg");
        findSequenceChart().activateContextMenuWithMouseClick().activateMenuItemWithMouse("Export to SVG...");
        ShellAccess shell = Access.findShellWithTitle("Export to SVG");
        shell.findButtonWithLabel(text).selectWithMouseClick();
        shell.findButtonWithLabel("OK").selectWithMouseClick();
        Access.mustHaveActiveShell = false;
        shell.pressEnter();;
        Access.mustHaveActiveShell = true;
    }
}
