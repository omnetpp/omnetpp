package org.omnetpp.simulation.model.essentials;

import static org.omnetpp.simulation.model.FieldNames.*;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPar;
import org.omnetpp.simulation.ui.IEssentialsProvider;

/**
 *
 * @author Andras
 */
public class cParEssentialsProvider implements IEssentialsProvider {

    @Override
    public boolean supports(cObject object) {
        return object instanceof cPar;
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
        cPar par = (cPar) object;
        List<Object> result = new ArrayList<Object>();
        result.add(object.getField(FLD_COBJECT_FULLNAME));
        result.add(object.getField(FLD_CPAR_VALUE));
        if (StringUtils.isNotEmpty(par.getUnit()))
            result.add(object.getField(FLD_CPAR_UNIT));

        //TODO merge the following two into one string: type = "volatile double"
        result.add(object.getField(FLD_CPAR_TYPE));
        if (par.isVolatile())
            result.add(object.getField(FLD_CPAR_ISVOLATILE));
        return result.toArray();
    }

}
