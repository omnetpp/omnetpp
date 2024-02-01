package org.omnetpp.ide.installer;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.OmnetppUtils;

public class InstallSimulationModelsHandler extends AbstractHandler {
    @Override
    public Object execute(ExecutionEvent event) throws ExecutionException {
    	
		if (OmnetppUtils.isInsideOppEnv()) {
    		boolean wantExit = MessageDialog.openQuestion(null, "Model Installation", 
    				"We detected that the IDE and OMNeT++ were installed via opp_env. "
    				+ "For consistency and optimal performance, it is recommended to install "
    				+ "simulation models via opp_env, not from the IDE. \n"
    				+ "\n"
    				+ "Do you want to exit the IDE and install models using opp_env?");	    	
    		if (wantExit) {
    			if (MessageDialog.openConfirm(null, "Confirm", "Confirm exiting the IDE."))
    				PlatformUI.getWorkbench().close();
    			
    			return null;
    		}
    	}
		
		new InstallSimulationModelsDialog(null).open();
        return null;
    }

}
