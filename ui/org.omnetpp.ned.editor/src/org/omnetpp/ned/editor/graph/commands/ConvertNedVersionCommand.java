package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;

/**
 * This command converts the ned model to new format (if it is in old format currently)
 *
 * @author rhornig
 */
public class ConvertNedVersionCommand extends Command {
    protected String originalVersion;
    protected String newVersion = "2";
    protected NedFileElementEx fileElement;

    public ConvertNedVersionCommand(INEDElement element) {
        while (element != null && !(element instanceof NedFileElementEx)) {
            element = element.getParent();
        }
        fileElement = (NedFileElementEx)element;
        if (fileElement == null)
            return;

        originalVersion = fileElement.getVersion();
    }

    @Override
    public String getLabel() {
        return "Convert to New Format";
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        fileElement.setVersion(newVersion);
    }

    @Override
    public void undo() {
        fileElement.setVersion(originalVersion);
    }

    @Override
    public boolean canExecute() {
        return fileElement != null && !newVersion.equals(fileElement.getVersion());
    }
}
