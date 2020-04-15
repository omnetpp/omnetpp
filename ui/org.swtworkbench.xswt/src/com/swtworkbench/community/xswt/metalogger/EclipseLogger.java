package com.swtworkbench.community.xswt.metalogger;

import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class EclipseLogger extends AbstractLogger {
    private AbstractUIPlugin plugin;

    public EclipseLogger(AbstractUIPlugin plugin) {
        this.plugin = plugin;
    }

    public void error(Throwable t, String message) {
        this.plugin.getLog().log(new Status(4, this.plugin.getBundle().getSymbolicName(), 4, message, t));
    }

    public void message(String message) {
        this.plugin.getLog().log(new Status(2, this.plugin.getBundle().getSymbolicName(), 2, message, null));
    }

    @SuppressWarnings("rawtypes")
    public void debug(Class subject, String message) {
        if (isDebug(subject))
            this.plugin.getLog().log(
                    new Status(1, this.plugin.getBundle().getSymbolicName(), 1, subject.getName() + ": " + message, null));
    }
}