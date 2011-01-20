/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.CompoundCommand;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ImportElement;

/**
 * Sets the "type" or "like-type" field of a submodule or connection. 
 * Also takes care of adding/removing the necessary imports.
 *
 * @author andras
 */
public class SetTypeOrLikeTypeCommand extends CompoundCommand {
    private final ISubmoduleOrConnection submoduleOrConnection;
    private final String attribute; // "type" or "like-type" (SubmoduleElement.ATT_TYPE / SubmoduleElement.ATT_LIKE_TYPE)  
    private final String qname;

    /**
     * Creates the command.
     */
    public SetTypeOrLikeTypeCommand(ISubmoduleOrConnection submoduleOrConnection, String attribute, String qname) {
    	this.submoduleOrConnection = submoduleOrConnection;
    	this.attribute = attribute;
    	this.qname = qname;
    	setLabel("Set Type");
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
        StringBuffer modifiedName = new StringBuffer();
        ImportElement importElement = qname.equals("") ? null : NedElementUtilEx.createImportFor(submoduleOrConnection.getEnclosingLookupContext(), qname, modifiedName);

        // set the attribute
        add(new SetAttributeCommand(submoduleOrConnection, attribute, modifiedName.toString()));

        // insert the new import
        if (importElement != null) {
            NedFileElementEx nedFileElement = submoduleOrConnection.getContainingNedFileElement();
            INedElement importsInsertionPoint = NedElementUtilEx.findInsertionPointForNewImport(nedFileElement);

            add(new InsertCommand(nedFileElement, importElement, importsInsertionPoint));

            // ensure there are blank lines after the last import                
            if (!(importsInsertionPoint instanceof ImportElement))
                importElement.appendChild(NedElementUtilEx.createCommentElement("right", "\n\n\n"));
        }
        
    }

}
