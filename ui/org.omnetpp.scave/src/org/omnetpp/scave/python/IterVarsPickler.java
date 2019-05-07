package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringPair;
import org.omnetpp.scave.engine.StringVector;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class IterVarsPickler implements IObjectPickler {

    public enum FilterMode {
        FILTER_ITERVARS,
        FILTER_RUNS,
        FILTER_RESULTS,
    };

    FilterMode filterMode;
    String filterExpression;

    public IterVarsPickler(String filterExpression, FilterMode filterMode) {
        this.filterExpression = filterExpression;
        this.filterMode = filterMode;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        if (filterExpression != null && !filterExpression.trim().isEmpty()) {
            RunList runs = null;
            if (filterMode == FilterMode.FILTER_ITERVARS) {
                OrderedKeyValueList itervars = resultManager.getMatchingItervars(filterExpression);

                for (int i = 0; i < itervars.size(); ++i) {
                    StringPair ri = itervars.get(i);

                    Run run = resultManager.getRunByName(ri.getFirst());
                    String ivName = ri.getSecond();

                    out.write(Opcodes.MARK);
                    {
                        pickler.save(run.getRunName());
                        pickler.save(ivName);
                        pickler.save(run.getIterationVariable(ivName));
                    }
                    out.write(Opcodes.TUPLE);
                }
            }
            else {
                if (filterMode == FilterMode.FILTER_RUNS) {
                    runs = resultManager.getRuns();
                    runs = resultManager.filterRunList(runs, filterExpression);
                    Debug.println("pickling itervars of " + runs.size() + " runs");
                }
                else {
                    IDList items = resultManager.getAllItems(false, false);
                    items = resultManager.filterIDList(items, filterExpression);
                    runs = resultManager.getUniqueRuns(items);
                    Debug.println("pickling itervars of " + runs.size() + " runs (for " + items.size() + " items)");
                }

                for (Run r : runs.toArray()) {
                    // Run attributes
                    StringMap itervars = r.getIterationVariables();
                    StringVector itervarKeys = itervars.keys();
                    for (int i = 0; i < itervarKeys.size(); ++i) {
                        String key = itervarKeys.get(i);

                        out.write(Opcodes.MARK);
                        {
                            pickler.save(r.getRunName());
                            pickler.save(key);
                            pickler.save(itervars.get(key));
                        }
                        out.write(Opcodes.TUPLE);
                    }
                }
            }
        }
        out.write(Opcodes.LIST);
    }
}
