package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ImportElement;

/**
 * Adds a newly created Submodule element to a compound module.
 *
 * @author rhornig, andras
 */
public class CreateSubmoduleCommand extends org.eclipse.gef.commands.Command {
	private String fullyQualifiedTypeName;
    private SubmoduleElementEx child;
    private Rectangle rect;
    private CompoundModuleElementEx parent;
    private ImportElement importElement;


    public CreateSubmoduleCommand(CompoundModuleElementEx parent, SubmoduleElementEx child) {
        Assert.isNotNull(parent);
        Assert.isNotNull(child);
    	this.child = child;
    	this.parent = parent;
    	this.fullyQualifiedTypeName = child.getType();  // redo() destructively modifies child's type
    }

    @Override
    public boolean canExecute() {
        // check that the type exists
    	return NEDElement.getDefaultTypeResolver().lookupNedType(fullyQualifiedTypeName, parent) != null;
    }

    @Override
    public void execute() {
        setLabel("Create " + child.getName());

        if (rect != null) {
            // get the scaling factor from the container module
            float scale = parent.getDisplayString().getScale();
            child.getDisplayString().setConstraint(rect.getLocation(), rect.getSize(), scale);
        }
        redo();
    }

    @Override
    public void redo() {
        // make the submodule name unique if needed before inserting into the model
        child.setName(NEDElementUtilEx.getUniqueNameFor(child, parent.getSubmodules()));
        
        // replace fully qualified name with import + simple name if needed/possible
    	importElement = null;
		if (fullyQualifiedTypeName.contains(".")) {
        	String simpleTypeName = StringUtils.substringAfterLast(fullyQualifiedTypeName, ".");
        	INEDTypeResolver resolver = NEDElement.getDefaultTypeResolver();
			INEDTypeInfo existingSimilarType = resolver.lookupNedType(simpleTypeName, parent);
			if (existingSimilarType == null) {
				// add import
				importElement = parent.getContainingNedFileElement().addImport(fullyQualifiedTypeName);
				child.setType(simpleTypeName);
			}
			else if (existingSimilarType.getFullyQualifiedName().equals(fullyQualifiedTypeName)) {
				// import not needed, this type is already visible 
				child.setType(simpleTypeName);
			}
			else {
				// another module with the same simple name already imported -- must use fully qualified name
				child.setType(fullyQualifiedTypeName);
			}
        }

		parent.insertSubmodule(null, child);
    }

    @Override
    public void undo() {
        parent.removeSubmodule(child);
        if (importElement != null)
        	importElement.removeFromParent();
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }
}