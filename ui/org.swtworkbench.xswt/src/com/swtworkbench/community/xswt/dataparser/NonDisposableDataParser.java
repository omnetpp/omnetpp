package com.swtworkbench.community.xswt.dataparser;

public abstract class NonDisposableDataParser implements IDataParser {
    public boolean isDisposable = true;

    public boolean isResourceDisposeRequired() {
        return this.isDisposable;
    }
}