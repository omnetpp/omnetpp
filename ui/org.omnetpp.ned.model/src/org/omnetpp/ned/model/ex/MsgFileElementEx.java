package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.INedFileElement;
import org.omnetpp.ned.model.pojo.MsgFileElement;

public class MsgFileElementEx extends MsgFileElement implements INedFileElement {
    protected MsgFileElementEx() {
    }

    protected MsgFileElementEx(INEDElement parent) {
        super(parent);
    }

    public List<IMsgTypeElement> getTopLevelTypeNodes() {
        List<IMsgTypeElement> result = new ArrayList<IMsgTypeElement>();
        for (INEDElement currChild : this)
            if (currChild instanceof IMsgTypeElement)
                result.add((IMsgTypeElement)currChild);

        return result;
    }
}
