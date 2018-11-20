package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class ScalarResultsPickler implements IObjectPickler {

    static void pickleScalarResult(ResultFileManager resultManager, long ID, Pickler pickler, OutputStream out)
            throws PickleException, IOException {
        ScalarResult result = resultManager.getScalar(ID);

        out.write(Opcodes.MARK);
        {
            pickler.save(result.getRun().getRunName());
            pickler.save("scalar");
            pickler.save(result.getModuleName());
            pickler.save(result.getName());
            pickler.save(null); // attrname
            pickler.save(null); // attrvalue
            pickler.save(result.getValue());
        }
        out.write(Opcodes.TUPLE);

        ResultPicklingUtils.pickleResultAttributes(result, pickler, out);
    }

    public static void pickleScalarsFiltered(ResultFileManager resultManager, String filter, Pickler pickler,
            OutputStream out) throws IOException {

        out.write(Opcodes.MARK);
        {
            IDList scalars = resultManager.getAllScalars();
            scalars = resultManager.filterIDList(scalars, filter);

            Debug.println("pickling " + scalars.size() + " scalars");

            ResultPicklingUtils.pickleRunsOfResults(resultManager, scalars, pickler, out);

            for (int i = 0; i < scalars.size(); ++i)
                pickleScalarResult(resultManager, scalars.get(i), pickler, out);
        }
        out.write(Opcodes.LIST);
    }

    String filter;

    public ScalarResultsPickler(String filter) {
        this.filter = filter;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        pickleScalarsFiltered((ResultFileManager) obj, filter, pickler, out);
    }
}
