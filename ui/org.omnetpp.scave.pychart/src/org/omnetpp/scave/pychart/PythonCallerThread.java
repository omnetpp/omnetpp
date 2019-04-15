package org.omnetpp.scave.pychart;

import java.util.concurrent.ConcurrentLinkedQueue;
import org.omnetpp.common.Debug;
import py4j.Py4JException;

public class PythonCallerThread extends Thread {

    public interface ExceptionHandler {
        void handle(Exception e);
    }

    public PythonCallerThread() {
        super("Python executor");
    }

    ConcurrentLinkedQueue<CustomRunnable> queue = new ConcurrentLinkedQueue<CustomRunnable>();

    class CustomRunnable implements Runnable {
        Runnable wrapped;
        boolean done = false;
        int eventStream;

        public CustomRunnable(Runnable wrapped, int eventStream) {
            this.wrapped = wrapped;
            this.eventStream = eventStream;
        }

        public boolean isDone() {
            return done;
        }

        public int getEventStream() {
            return eventStream;
        }

        @Override
        public void run() {
            try {
                wrapped.run();
            } catch (Exception e) {
                PyChartPlugin.logError(e);
            }
            finally {
                done = true;
            }
        }
    }

    // Note: There is no syncExec, and there shouldn't be.
    // We found that it was prone to deadlocking. When an SWT
    // event handler executed some Python code synchronously,
    // which then called back to SWT synchronously, it
    // obviously a deadlocked.
    // And in at least one case (MultiWidgetProvider.getWidget)
    // we currently have to use Display.syncExec() because we
    // return a reference to Python which gets created by SWT.
    // Also, since any user-supplied Python code (even event
    // handlers) can take arbitrarily long (doing computations,
    // or even just sleeping), it seems reasonable to never
    // wait for it to complete, allowing us (and the user) to
    // cancel it by (for example) killing the Python process.
    public void asyncExec(Runnable runnable) {
        asyncExec(runnable, -1);
    }

    /**
     * If eventStream is non-negative, all Runnables with the same eventStream are
     * removed from the queue before 'runnable' is submitted. This makes it possible
     * to "skip" the processing of previous MouseMove or Resize events if a newer
     * one arrived.
     */
    public void asyncExec(Runnable runnable, int eventStream) {
        CustomRunnable cr = new CustomRunnable(runnable, eventStream);

        synchronized (queue) {
            if (eventStream >= 0)
                queue.removeIf(icr -> icr.getEventStream() == eventStream);
            queue.add(cr);
            queue.notify();
        }
    }

    public void asyncExec(Runnable runnable, int eventStream, Runnable runAfterDone, ExceptionHandler errorHandler) {
        asyncExec(() -> {
            try {
                runnable.run();
            } catch (RuntimeException e) {
                if (errorHandler != null)
                    errorHandler.handle(e);
                else
                    System.out.println("NO ERROR HANDLER");
                return;
            }

            if (runAfterDone != null)
                runAfterDone.run();
        }, eventStream);
    }

    public void asyncExec(Runnable runnable, Runnable runAfterDone, ExceptionHandler errorHandler) {
        asyncExec(runnable, -1, runAfterDone, errorHandler);
    }

    @Override
    public void run() {
        outer: while (true) {
            synchronized (queue) {
                try {
                    // There is nothing to do, wait until a Runnable is submitted...
                    if (queue.isEmpty())
                        queue.wait();
                }
                catch (InterruptedException e) {
                    break; // rude...
                }
            }

            while (!queue.isEmpty()) {
                Runnable r = null;
                synchronized (queue) {
                    // double-check to avoid race condition if a submission to an event stream just emptied the queue
                    if (!queue.isEmpty())
                        r = queue.remove();
                }
                if (r != null) {
                    try {
                        r.run();
                    }
                    catch (Py4JException e) {
                        System.err.println("P4J Exception: " + e.getMessage());
                        // TODO signal this somehow? Via a listener?
                        break outer;
                    }
                }
            }
        }
        Debug.println("Python executor thread exiting.");
    }
}
