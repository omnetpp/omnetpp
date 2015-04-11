/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import java.util.List;

import org.eclipse.gef.commands.CompoundCommand;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;

/**
 * Sets the "extends" or "like" list of a NED type. Also takes
 * care of adding/removing the necessary imports.
 *
 * @author andras
 */
public class SetInheritanceCommand extends CompoundCommand {
    private final INedTypeElement typeElement;
    private int tagcode;
    private final List<String> qnames;

    /**
     * Creates the command.
     * @param typeElement   the NED type to be modified
     * @param tagcode  must be INedElement.NED_EXTENDS or INedElement.NED_INTERFACE_NAME
     * @param qnames   the list of fully qualified NED type names
     */
    public SetInheritanceCommand(INedTypeElement typeElement, int tagcode, List<String> qnames) {
        if (tagcode != INedElement.NED_EXTENDS && tagcode != INedElement.NED_INTERFACE_NAME)
            throw new IllegalArgumentException();
        this.typeElement = typeElement;
        this.tagcode = tagcode;
        this.qnames = qnames;
        setLabel("Modify Inheritance");
    }

    @Override
    public boolean canExecute() {
        return true;
    }

    @Override
    public void execute() {
        createCommands();
        super.execute();
    }

    protected void createCommands() {
        // remove existing ExtendsElements / InterfaceNameElements
        for (INedElement e : typeElement.getChildrenWithTag(tagcode))
            add(new RemoveCommand(e));

        // add new elements
        NedFileElementEx nedFileElement = typeElement.getContainingNedFileElement();
        // TODO: this will always insert to the beginning of the imports, we could pass in the importElement
        // but that might leave extra new lines between two import elements...
        INedElement importsInsertionPoint = NedElementUtilEx.findInsertionPointForNewImport(nedFileElement, null);
        ImportElement lastNewImportElement = null;
        for (String qname : qnames) {
            StringBuffer modifiedName = new StringBuffer();
            ImportElement importElement = NedElementUtilEx.createImportFor(typeElement.getParentLookupContext(), qname, modifiedName);

            // insert the ExtendsElement or InterfaceNameElement
            INedElement e = NedElementFactoryEx.getInstance().createElement(tagcode);
            if (tagcode == INedElement.NED_EXTENDS)
                ((ExtendsElement)e).setName(modifiedName.toString());
            else
                ((InterfaceNameElement)e).setName(modifiedName.toString());
            add(new InsertCommand(typeElement, e));

            // insert the new import
            if (importElement != null) {
                add(new InsertCommand(nedFileElement, importElement, importsInsertionPoint));
                lastNewImportElement = importElement;
            }
        }

        // ensure there are blank lines after the last import
        if (lastNewImportElement != null && !(importsInsertionPoint instanceof ImportElement))
            lastNewImportElement.appendChild(NedElementUtilEx.createCommentElement("right", "\n\n\n"));
    }

}
