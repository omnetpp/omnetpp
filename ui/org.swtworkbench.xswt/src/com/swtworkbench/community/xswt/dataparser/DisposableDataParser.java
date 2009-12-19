package com.swtworkbench.community.xswt.dataparser;

public abstract class DisposableDataParser implements IDataParser {
    public boolean isResourceDisposeRequired() {
        return false;
    }
}