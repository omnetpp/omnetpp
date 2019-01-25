package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class RunsPickler implements IObjectPickler {

    String filterExpression;
    boolean includeRunattrs;
    boolean includeItervars;

    public RunsPickler(String filterExpression, boolean includeRunattrs, boolean includeItervars) {
        this.filterExpression = filterExpression;
        this.includeRunattrs = includeRunattrs;
        this.includeItervars = includeItervars;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;
        out.write(Opcodes.MARK);
        {
            out.write(Opcodes.MARK);
            {
                RunList runs = resultManager.getRuns();
                runs = resultManager.filterRunList(runs, filterExpression);

                Debug.println("pickling " + runs.size() + " runs");

                for (Run r : runs.toArray())
                    pickler.save(r.getRunName());
            }
            out.write(Opcodes.LIST);

            if (includeRunattrs)
                new RunAttrsPickler(filterExpression, RunAttrsPickler.FilterMode.FILTER_RUNS).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);

            if (includeItervars)
                new IterVarsPickler(filterExpression, IterVarsPickler.FilterMode.FILTER_RUNS).pickle(resultManager, out, pickler);
            else
                out.write(Opcodes.NONE);
        }
        out.write(Opcodes.TUPLE);
    }
}
