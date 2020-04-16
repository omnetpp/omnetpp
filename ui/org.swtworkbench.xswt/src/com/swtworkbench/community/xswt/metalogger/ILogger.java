package com.swtworkbench.community.xswt.metalogger;

public abstract interface ILogger {
    public abstract void error(Throwable paramThrowable, String paramString);

    public abstract void message(String paramString);

    public abstract void debug(Class<?> paramClass, String paramString);

    public abstract boolean isDebug();

    public abstract void setDebug(boolean paramBoolean);

    public abstract void setDebug(Class<?> paramClass, boolean paramBoolean);

    public abstract boolean isDebug(Class<?> paramClass);
}