package com.simulcraft.test.gui.core;

/**
 * Marks methods that should be executed in the UI thread.
 * This annotation is typically placed on methods of 
 * org.omnetpp.test.gui.access.* classes, and the UIThread
 * aspect will add code containing Display.syncExec() around them.  
 * 
 * @author Andras
 *
 */
public @interface InUIThread {

}
