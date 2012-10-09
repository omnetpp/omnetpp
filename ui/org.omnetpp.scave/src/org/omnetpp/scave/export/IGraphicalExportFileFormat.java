package org.omnetpp.scave.export;

import org.eclipse.swt.widgets.Shell;

/**
 * Interface for graphical export file formats
 *
 * @author tomi
 */
public interface IGraphicalExportFileFormat {
    String getName();
    String getFileExtension();
    String getDescription();
    void openOptionsDialog(Shell parentShell);
    IGraphicalExportFileFormat clone();
}
