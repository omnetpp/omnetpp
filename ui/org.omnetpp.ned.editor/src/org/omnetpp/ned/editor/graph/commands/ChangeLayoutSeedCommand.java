package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.interfaces.IConnectableNode;

/**
 * Change the size and location of a compound module (location cannot be changed)
 *
 * @author rhornig
 */
public class ChangeLayoutSeedCommand extends Command {
	private int oldSeed = 1;
    private IConnectableNode module;

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
        oldSeed = module.getDisplayString().getAsInt(IDisplayString.Prop.MODULE_LAYOUT_SEED,1);
        redo();
    }

    @Override
    public void redo() {
        module.getDisplayString().set(IDisplayString.Prop.MODULE_LAYOUT_SEED, String.valueOf(oldSeed+1));
    }

    @Override
    public void undo() {
    	if (oldSeed == 1)
            module.getDisplayString().set(IDisplayString.Prop.MODULE_LAYOUT_SEED, null);
    	else
            module.getDisplayString().set(IDisplayString.Prop.MODULE_LAYOUT_SEED, String.valueOf(oldSeed));
    }

}