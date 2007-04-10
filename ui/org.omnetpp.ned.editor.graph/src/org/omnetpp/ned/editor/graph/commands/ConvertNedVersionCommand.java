package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.NedFileNodeEx;

/**
 * This command converts the ned model to new format (if it is in old format currently)
 * 
 * @author rhornig
 */
public class ConvertNedVersionCommand extends Command {
    protected String originalVersion;
    protected String newVersion = "2";
    protected NedFileNodeEx fileElement;
    
    public ConvertNedVersionCommand(NEDElement element) {
        while (element != null && !(element instanceof NedFileNodeEx)) {
            element = element.getParent();
        }
        fileElement = (NedFileNodeEx)element;
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
