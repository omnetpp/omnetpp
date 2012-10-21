package org.omnetpp.simulation.model.essentials;

import static org.omnetpp.simulation.model.FieldNames.*;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.ui.IEssentialsProvider;

/**
 *
 * @author Andras
 */
public class cModuleEssentialsProvider implements IEssentialsProvider {

    @Override
    public boolean supports(cObject object) {
        return object instanceof cModule;
    }

    @Override
    public int getScore(cObject object) {
        return 10;
    }

    public boolean hasChildren(cObject object) {
        return true;
    }

    @Override
    public Object[] getChildren(cObject object) {
        cModule module = (cModule) object;
        List<Object> result = new ArrayList<Object>();
        try {
            module.loadIfUnfilled();
            module.loadFieldsIfUnfilled();
        }
        catch (CommunicationException e1) {
            // TODO Auto-generated catch block
            return result.toArray();
        }
        result.add(object.getField(FLD_COBJECT_FULLNAME));
        result.add("NED type = " + module.getNedTypeName());  //TODO implement synthetic fields!!!!
        //result.add(object.getField(FLD_CCOMPONENT_COMPONENTTYPE));
        result.add(object.getField(FLD_CMODULE_ISSIMPLE));  //TODO merge this into the "NED type" line!
        //TODO maybe the state (if not READY)? or "activity" if not handleMessage?

        cObject[] childObjects = module.getChildObjects();
        try {
            object.getSimulation().loadUnfilledObjects(childObjects);
        }
        catch (IOException e) {
            // FIXME proper exception handling!!!
            return result.toArray();
        }
        result.addAll(Arrays.asList(childObjects));
        return result.toArray();
    }

}
