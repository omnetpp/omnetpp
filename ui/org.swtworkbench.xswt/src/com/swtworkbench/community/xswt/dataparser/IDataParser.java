package com.swtworkbench.community.xswt.dataparser;

import com.swtworkbench.community.xswt.XSWTException;

public abstract interface IDataParser {
    public abstract Object parse(String paramString) throws XSWTException;

    public abstract boolean isResourceDisposeRequired();
}