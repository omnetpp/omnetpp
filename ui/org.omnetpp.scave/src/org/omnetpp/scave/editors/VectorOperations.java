package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.collections.list.UnmodifiableList;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;

/**
 * Handles querying the list of available vector operations from Python processes used to run chart scripts.
 * Both the built-in operations, that are not specific to any project, analysis, or chart (from the omnetpp.scave.vectorops module);
 * and the custom operations added and registered by user code in a given process (using the @vector_operation decorator).
 */
public class VectorOperations {

    // These are the "built-in" operations, queried once, and assumed to never change.
    // Used as fallback for the specific list.
    protected static List<VectorOp> builtinVectorOps = null;

    // These are the actual, chart-specific operations, if not null.
    // Usually a superset of the builtin ones (unless the user did something naughty in their chart script)
    protected List<VectorOp> vectorOps = null;

    /**
     * Describes a vector operation as registered in a Python process.
     */
    public static class VectorOp {
        // properties of the Python function:
        public final String module;
        public final String name;
        public final String signature; // does not include the "internal" first parameter (for the df/row)
        public final String docstring; // shown as content assist details

        // additional info about the operation:
        public final String label; // shown on the GUI, in the context menu and such
        public final String example; // the default string to be inserted into the "recipe" (similar to name+signature, but usually with concrete parameter values)

        public final boolean hasArgs;

        public VectorOp(String module, String name, String signature, String docstring, String label, String example) {
            this.module = module;
            this.name = name;
            this.signature = signature;
            this.docstring = docstring;
            this.label = label;
            this.example = example;
            this.hasArgs = !signature.equals("()");
        }
    }

    public VectorOperations() {
    }

    @SuppressWarnings("unchecked")
    public static List<VectorOp> getBuiltinVectorOpData() {
        if (builtinVectorOps == null) {
            try {
                builtinVectorOps = gatherBuiltInVectorOpData();
            } catch (Exception e) {
                ScavePlugin.logError("Could not query list of built-in vector operations from Python", e);
                // setting it to nun-null, to not try again
                builtinVectorOps = new ArrayList<VectorOp>();
            }
        }
        return (List<VectorOp>)UnmodifiableList.decorate(builtinVectorOps);
    }

    /// prefers the specific list, but falls back to built-in ones if the specific list was not updated yet
    @SuppressWarnings("unchecked")
    public List<VectorOp> getVectorOpData() {
        return (List<VectorOp>)UnmodifiableList.decorate(vectorOps == null ? getBuiltinVectorOpData() : vectorOps);
    }

    public void updateVectorOpDataFromProcess(PythonProcess proc) {
        vectorOps = gatherVectorOpData(proc);
    }

    protected static List<VectorOp> gatherBuiltInVectorOpData() {
        PythonProcessPool processPool = new PythonProcessPool(1);
        PythonProcess proc = processPool.getProcess();
        List<VectorOp> result = gatherVectorOpData(proc);
        proc.kill();
        processPool.dispose();
        return result;
    }

    protected static List<VectorOp> gatherVectorOpData(PythonProcess proc) {
        List<Object> e = proc.getEntryPoint().getVectorOps();
        ArrayList<VectorOp> result = new ArrayList<>();

        if (e != null)
            for (Object op : e)
                result.add((VectorOp)op);

        return result;
    }
}
