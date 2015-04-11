/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.CompoundCommand;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.CommentElement;

/**
 * Sets the documentation of a NED type. This includes adding the "//" prefix to lines,
 * creating a CommentElement with type="banner" if needed, and putting the comment into it.
 *
 * @author andras
 */
public class SetDocumentationCommand extends CompoundCommand {
    private final INedTypeElement typeElement;
    private final String doc;

    /**
     * Creates the command.
     */
    public SetDocumentationCommand(INedTypeElement typeElement, String doc) {
        this.typeElement = typeElement;
        this.doc = doc;
        setLabel("Set Documentation");
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
        INedElement commentElement = typeElement.getFirstChildWithAttribute(INedElement.NED_COMMENT, CommentElement.ATT_LOCID, "banner"); //TODO add constant for "banner" to NedElementConstants
        if (StringUtils.isBlank(doc)) {
            // remove existing comment element
            if (commentElement != null)
                add(new RemoveCommand(commentElement));
        }
        else {
            // prefix lines with "//", plus one blank "//" line both at the top and bottom
            String comment = "//\n// " + doc.trim().replace("\n", "\n// ") + "\n//\n";

            // preserve private comments syntax ("//#")
            comment = comment.replace("\n// #", "\n//#");

            // overwrite or add comment element
            if (commentElement != null) {
                add(new SetAttributeCommand(commentElement, CommentElement.ATT_CONTENT, comment));
            }
            else {
                CommentElement newCommentElement = NedElementUtilEx.createCommentElement("banner", comment);
                add(new InsertCommand(typeElement, newCommentElement, typeElement.getFirstChild()));
            }
        }
    }

}
