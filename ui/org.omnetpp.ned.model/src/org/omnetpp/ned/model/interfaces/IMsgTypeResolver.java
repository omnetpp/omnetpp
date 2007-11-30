package org.omnetpp.ned.model.interfaces;

public interface IMsgTypeResolver {
    public IMsgTypeElement lookupMsgType(String name);

    /**
     * INTERNAL Factory method, to be called from INedTypeElement constructors.
     */
    public IMsgTypeInfo createTypeInfoFor(IMsgTypeElement node);
}
