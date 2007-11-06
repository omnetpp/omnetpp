package com.simulcraft.test.gui.core;

/**
 * Marks the method as one "UI step". What happens is the following:
 * <ul>
 *   <li>1. The method annotated with @UIStep will be wrapped in a Runnable, 
 *          and gets executed in the UI thread with Display.syncExec().
 *   <li>2. If there was an exception, the step gets re-tried every 0.5s or so
 *          until it succeeds, or a timeout of about 5s is reached. If there
 *          was no successful execution, the exception is thrown on.
 *   <li>3. When execution was successful, UI events in queue get processed
 *          using Display.readAndDispatch() until the queue becomes empty.
 *          Then the method returns.       
 * </ul> 
 * @UIStep does NOT nest: If there are nested @UIStep method calls, 
 * only the toplevel one (entered first) will get special treatment, all nested
 * ones are treated like plain method calls.
 *
 * NOTE: no UI events (mouse clicks, keypresses) get processed while control is
 * in the body of the method annotated with @UIStep -- only after it finished.
 * So if you click a button or type something in the method, that will only
 * post the events, but nothing will actually happen until after the method returns.
 * 
 * If you need events to get processed during the method, it must NOT execute
 * within a @UIStep method, and you're recommended to use the @InBackgroundThread
 * annotation to assert that.
 * 
 * @author Andras
 */
public @interface UIStep {
}
