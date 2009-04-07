package org.omnetpp.runtimeenv.editors;


public interface IInspectorPart {
    
    IInspectorFigure getFigure();
    
    boolean isSelected();

    void setSelected(boolean b);

}
