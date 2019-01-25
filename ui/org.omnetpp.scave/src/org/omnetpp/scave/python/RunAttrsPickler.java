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
import org.omnetpp.scave.python.IterVarsPickler.FilterMode;

import net.razorvine.pickle.IObjectPickler;
import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class RunAttrsPickler implements IObjectPickler {

    public enum FilterMode {
        FILTER_RUNATTRS,
        FILTER_RUNS,
        FILTER_RESULTS
    };

    FilterMode filterMode;
    String filterExpression;

    public RunAttrsPickler(String filterExpression, FilterMode filterMode) {
        this.filterExpression = filterExpression;
        this.filterMode = filterMode;
    }

    @Override
    public void pickle(Object obj, OutputStream out, Pickler pickler) throws PickleException, IOException {
        ResultFileManager resultManager = (ResultFileManager)obj;

        out.write(Opcodes.MARK);
        {
            RunList runs = null;
            if (filterMode == FilterMode.FILTER_RUNATTRS) {
                OrderedKeyValueList runattrs = resultManager.getMatchingRunattrs(filterExpression);

                for (int i = 0; i < runattrs.size(); ++i) {
                    StringPair ra = runattrs.get(i);

                    Run run = resultManager.getRunByName(ra.getFirst());
                    String raName = ra.getSecond();

                    out.write(Opcodes.MARK);
                    {
                        pickler.save(run.getRunName());
                        pickler.save(raName);
                        pickler.save(run.getAttribute(raName));
                    }
                    out.write(Opcodes.TUPLE);
                }
            }
            else {

                if (filterMode == FilterMode.FILTER_RUNS) {
                    runs = resultManager.getRuns();
                    runs = resultManager.filterRunList(runs, filterExpression);
                    Debug.println("pickling attrs of " + runs.size() + " runs");
                }
                else {
                    IDList items = resultManager.getAllItems(false, false);
                    items = resultManager.filterIDList(items, filterExpression);
                    runs = resultManager.getUniqueRuns(items);
                    Debug.println("pickling attrs of " + runs.size() + " runs (for " + items.size() + " items)");
                }

                for (Run r : runs.toArray()) {
                    // Run attributes
                    StringMap attrs = r.getAttributes();
                    StringVector attrKeys = attrs.keys();
                    for (int i = 0; i < attrKeys.size(); ++i) {
                        String key = attrKeys.get(i);

                        out.write(Opcodes.MARK);
                        {
                            pickler.save(r.getRunName());
                            pickler.save(key);
                            pickler.save(attrs.get(key));
                        }
                        out.write(Opcodes.TUPLE);
                    }
                }
            }
        }
        out.write(Opcodes.LIST);
    }
}
