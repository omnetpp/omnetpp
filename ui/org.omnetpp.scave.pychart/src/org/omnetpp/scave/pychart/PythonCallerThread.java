/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.concurrent.ConcurrentLinkedQueue;

import org.omnetpp.common.Debug;

import py4j.Py4JException;

/**
 * A thread that queues and executes blocking function calls to a
 * PythonProcess (running Python code), in an asynchronous way. This makes it
 * impossible to halt the main IDE thread if something gets stuck, or takes too
 * long on the Python side.
 *
 * It also has support for so-called "event streams". This is useful for passing
 * down UI event notifications to our matplotlib backend - or for any other task
 * that might be triggered quite frequently, might take some time to execute,
 * but only the latest execution is important.
 * Each event stream is identified by an integer (of arbitrary value).
 * When a Runnable is submitted for async execution in an event stream, any
 * previously submitted Runnables in the queue that belong to the same event
 * stream will be cancelled - removed from the queue; effectively replaced by
 * the newly submitted runnable (which is still put at the end of the queue).
 *
 * For each submitted Runnable, an optional "done" notification callback and an
 * also optional error handler can be attached. If the Runnable throws an
 * exception, it will be passed to the error handler. The "done" notification
 * runnable is called after the Runnable finished, or if it threw an exception,
 * after the errorHandler is executed.
 */
public class PythonCallerThread extends Thread {

    private PythonProcess proc;
    private ConcurrentLinkedQueue<CustomRunnable> queue = new ConcurrentLinkedQueue<CustomRunnable>();

    public interface ExceptionHandler {
        void handle(PythonProcess proc, Exception e);
    }

    public PythonCallerThread(PythonProcess proc) {
        super("Python executor");
        this.proc = proc;
    }

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
                    errorHandler.handle(proc, e);
                else
                    Debug.println("NO ERROR HANDLER");
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
        outer: while (proc.isAlive()) {
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

            while (!queue.isEmpty() && proc.isAlive()) {
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
                        PyChartPlugin.logError(e);
                        break outer;
                    }
                }
            }
        }
        Debug.println("Python executor thread exiting.");
    }
}
