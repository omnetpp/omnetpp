package com.swtworkbench.community.xswt.metalogger;

import java.util.HashMap;

@SuppressWarnings({ "rawtypes", "unchecked" })
public abstract class AbstractLogger implements ILogger {
    private boolean debug = false;

    private HashMap subjects = new HashMap();

    public boolean isDebug() {
        return this.debug;
    }

    public void setDebug(boolean debugMode) {
        this.debug = debugMode;
    }

    public boolean isDebug(Class<?> subject) {
        if (subject == null) {
            throw new IllegalArgumentException("MetaLogger: Subject of debug message can't be null!");
        }
        if (this.debug)
            return true;
        return this.subjects.containsKey(subject);
    }

    public void setDebug(Class<?> subject, boolean enabled) {
        if (subject == null) {
            throw new IllegalArgumentException("MetaLogger: Subject of debug message can't be null!");
        }
        Object result = this.subjects.get(subject);
        if (enabled) {
            if (result == null) {
                this.subjects.put(subject, Boolean.TRUE);
            }
        }
        else if (result != null)
            this.subjects.remove(subject);
    }
}