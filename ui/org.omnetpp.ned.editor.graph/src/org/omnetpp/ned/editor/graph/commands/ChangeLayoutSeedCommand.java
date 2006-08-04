package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.INamedGraphNode;

/**
 * Change the size and location of a compound module (location cannot be changed)
 * @author rhornig
 *
 */
public class ChangeLayoutSeedCommand extends Command {
	private int oldSeed = 1;
    private INamedGraphNode module;

    public ChangeLayoutSeedCommand(CompoundModuleNodeEx newModule) {
    	super();
        module = newModule;
    }

    @Override
    public String getLabel() {
        return "Layout " + module.getName();
    }

    @Override
    public void execute() {
        oldSeed = module.getDisplayString().getAsIntDef(IDisplayString.Prop.MODULE_LAYOUT_SEED,1);
        redo();
    }

    @Override
    public void redo() {
        module.getDisplayString().set(IDisplayString.Prop.MODULE_LAYOUT_SEED, oldSeed+1);
    }

    @Override
    public void undo() {
    	if (oldSeed == 1)
            module.getDisplayString().set(IDisplayString.Prop.MODULE_LAYOUT_SEED, null);
    	else
            module.getDisplayString().set(IDisplayString.Prop.MODULE_LAYOUT_SEED, oldSeed);
    }

}