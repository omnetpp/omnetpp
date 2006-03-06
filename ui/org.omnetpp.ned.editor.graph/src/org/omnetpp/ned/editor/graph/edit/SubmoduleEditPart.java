package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.AccessibleAnchorProvider;
import org.omnetpp.ned.editor.graph.figures.SimpleModuleFigure;

/**
 * EditPart for holding gates in the Logic Example.
 */
public class SubmoduleEditPart extends NedNodeEditPart {

    /**
     * Returns a newly created Figure of this.
     * 
     * @return A new Figure of this.
     */
    protected IFigure createFigure() {
        
        return new SimpleModuleFigure();
    }

    /**
     * Returns the Figure for this as an SimpleModuleFigure.
     * 
     * @return Figure of this as a SimpleModuleFigure
     */
    protected SimpleModuleFigure getSimpleModuleFigure() {
        return (SimpleModuleFigure) getFigure();
    }

    public Object getAdapter(Class key) {
        if (key == AccessibleAnchorProvider.class) return new DefaultAccessibleAnchorProvider() {
            public List getSourceAnchorLocations() {
                List list = new ArrayList();
                Vector sourceAnchors = getNedFigure().getSourceConnectionAnchors();
                for (int i = 0; i < sourceAnchors.size(); i++) {
                    ConnectionAnchor anchor = (ConnectionAnchor) sourceAnchors.get(i);
                    list.add(anchor.getReferencePoint().getTranslated(0, -3));
                }
                return list;
            }

            public List getTargetAnchorLocations() {
                List list = new ArrayList();
                Vector targetAnchors = getNedFigure().getTargetConnectionAnchors();
                for (int i = 0; i < targetAnchors.size(); i++) {
                    ConnectionAnchor anchor = (ConnectionAnchor) targetAnchors.get(i);
                    list.add(anchor.getReferencePoint());
                }
                return list;
            }
        };

        // FIXME this is a duplicate. Should be deleted?
        if (key == AccessibleAnchorProvider.class) return new DefaultAccessibleAnchorProvider() {
            public List getSourceAnchorLocations() {
                List list = new ArrayList();
                Vector sourceAnchors = getNedFigure().getSourceConnectionAnchors();
                for (int i = 0; i < sourceAnchors.size(); i++) {
                    ConnectionAnchor anchor = (ConnectionAnchor) sourceAnchors.get(i);
                    list.add(anchor.getReferencePoint().getTranslated(0, -3));
                }
                return list;
            }

        };
        return super.getAdapter(key);
    }

}
