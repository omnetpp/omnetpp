package org.omnetpp.scave.python;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.ILock;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.OrderedKeyValueList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringPair;
import org.omnetpp.scave.engine.StringVector;

import net.razorvine.pickle.Opcodes;
import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;

public class ResultPicklingUtils {

    public static double[] bytesToDoubles(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        double[] doubles = new double[bytes.length / 8];
        byteBuffer.asDoubleBuffer().get(doubles);
        return doubles;
    }

    public static byte[] doublesToBytes(double[] doubles) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(doubles.length * 8);
        byteBuffer.asDoubleBuffer().put(doubles);
        return byteBuffer.array();
    }

    public static void pickleDoubleArray(double[] array, OutputStream out) throws IOException {
        // number of bytes
        int s = array.length * 8;

        out.write(Opcodes.BINBYTES);
        // 4 bytes length prefix
        out.write(s >> 0);
        out.write(s >> 8);
        out.write(s >> 16);
        out.write(s >> 24);
        // actual data
        out.write(doublesToBytes(array));
    }

    public static void pickleResultAttributes(ResultItem result, Pickler pickler, OutputStream out) throws IOException {
        StringMap attrs = result.getAttributes();
        StringVector attrKeys = attrs.keys();
        for (int i = 0; i < attrKeys.size(); ++i) {
            String key = attrKeys.get(i);

            out.write(Opcodes.MARK);
            {
                pickler.save(result.getRun().getRunName());
                pickler.save("attr");
                pickler.save(result.getModuleName());
                pickler.save(result.getName());
                pickler.save(key);
                pickler.save(attrs.get(key));
            }
            out.write(Opcodes.TUPLE);
        }
    }

    public static void pickleRunsOfResults(ResultFileManager resultManager, IDList results, Pickler pickler,
            OutputStream out) throws IOException {
        RunList runs = resultManager.getUniqueRuns(results);

        for (Run r : runs.toArray()) {

            // Run attributes
            StringMap attrs = r.getAttributes();
            StringVector attrKeys = attrs.keys();
            for (int i = 0; i < attrKeys.size(); ++i) {
                String key = attrKeys.get(i);

                out.write(Opcodes.MARK);
                {
                    pickler.save(r.getRunName());
                    pickler.save("runattr");
                    pickler.save(null); // module
                    pickler.save(null); // name
                    pickler.save(key);
                    pickler.save(attrs.get(key));
                }
                out.write(Opcodes.TUPLE);
            }

            // Iteration variables
            StringMap itervars = r.getIterationVariables();
            StringVector itervarKeys = itervars.keys();
            for (int i = 0; i < itervarKeys.size(); ++i) {
                String key = itervarKeys.get(i);

                out.write(Opcodes.MARK);
                {
                    pickler.save(r.getRunName());
                    pickler.save("itervar");
                    pickler.save(null); // module
                    pickler.save(null); // name
                    pickler.save(key);
                    pickler.save(itervars.get(key));
                }
                out.write(Opcodes.TUPLE);
            }

            // Parameter assignments
            OrderedKeyValueList params = r.getParamAssignments();
            for (int i = 0; i < params.size(); ++i) {
                StringPair pair = params.get(i);
                out.write(Opcodes.MARK);
                {
                    pickler.save(r.getRunName());
                    pickler.save("param");
                    pickler.save(null); // module
                    pickler.save(null); // name
                    pickler.save(pair.getFirst());
                    pickler.save(pair.getSecond());
                }
                out.write(Opcodes.TUPLE);
            }
        }

    }


    public static byte[] getScalarsPickle(ResultFileManager resultManager, String filter) throws PickleException, IOException {
        ILock l = resultManager.getReadLock();
        l.lock();

        Pickler p = new Pickler(true);
        Pickler.registerCustomPickler(ResultFileManager.class, new ScalarResultsPickler(filter));

        long startNanos = System.nanoTime();
        byte[] scalarsPickle = p.dumps(resultManager);
        long endNanos = System.nanoTime();

        Debug.println("pickling scalars took " + (endNanos - startNanos) / 1000000.0 + " ms, size is "
                + scalarsPickle.length + " bytes");

        l.unlock();

        return scalarsPickle;
    }

    public static byte[] getVectorsPickle(ResultFileManager resultManager, String filter) throws PickleException, IOException {
        ILock l = resultManager.getReadLock();
        l.lock();

        Pickler p = new Pickler(true);
        Pickler.registerCustomPickler(ResultFileManager.class, new VectorResultsPickler(filter));

        long startNanos = System.nanoTime();
        byte[] vectorsPickle = p.dumps(resultManager);
        long endNanos = System.nanoTime();

        Debug.println("pickling vectors took " + (endNanos - startNanos) / 1000000.0 + " ms, size is "
                + vectorsPickle.length + " bytes");

        l.unlock();

        return vectorsPickle;
    }

    public static byte[] getStatisticsPickle(ResultFileManager resultManager, String filter) throws PickleException, IOException {
        ILock l = resultManager.getReadLock();
        l.lock();

        Pickler p = new Pickler(true);
        Pickler.registerCustomPickler(ResultFileManager.class, new StatisticsResultsPickler(filter));

        long startNanos = System.nanoTime();
        byte[] vectorsPickle = p.dumps(resultManager);
        long endNanos = System.nanoTime();

        Debug.println("pickling statistics took " + (endNanos - startNanos) / 1000000.0 + " ms, size is "
                + vectorsPickle.length + " bytes");

        l.unlock();

        return vectorsPickle;
    }

    public static byte[] getHistogramsPickle(ResultFileManager resultManager, String filter) throws PickleException, IOException {
        ILock l = resultManager.getReadLock();
        l.lock();

        Pickler p = new Pickler(true);
        Pickler.registerCustomPickler(ResultFileManager.class, new HistogramResultsPickler(filter));

        long startNanos = System.nanoTime();
        byte[] vectorsPickle = p.dumps(resultManager);
        long endNanos = System.nanoTime();

        Debug.println("pickling histograms took " + (endNanos - startNanos) / 1000000.0 + " ms, size is "
                + vectorsPickle.length + " bytes");

        l.unlock();

        return vectorsPickle;
    }
}
