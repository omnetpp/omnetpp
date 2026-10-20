/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.Set;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.omnetpp.ned.editor.graph.parts.canvas.*;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.TypesElement;
import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

/**
 * Factory to create corresponding controller objects for the model objects.
 * Only model objects explicitly handled here will have a controller and a visual
 * counterpart in the editor
 *
 * @author rhornig
 */
public class NedEditPartFactory implements EditPartFactory {

    // @figure types for which createEditPart() creates a dedicated edit part; must be kept in sync with it
    private static final Set<String> FIGURE_TYPES_WITH_EDITPART = Set.of(
            FTYPE_RECTANGLE, FTYPE_OVAL, FTYPE_RING, FTYPE_PIESLICE, FTYPE_ARC, FTYPE_POLYLINE, FTYPE_LINE,
            FTYPE_POLYGON, FTYPE_TEXT, FTYPE_LABEL, FTYPE_GROUP, FTYPE_PANEL, FTYPE_IMAGE, FTYPE_PIXMAP, FTYPE_ICON, FTYPE_PATH);

    /**
     * Returns true if the given @figure type has a dedicated edit part. Figures of other types
     * (unknown or custom types, and known types that the editor cannot display) get a
     * PlaceholderEditPart.
     */
    public static boolean hasFigureEditPart(String type) {
        return FIGURE_TYPES_WITH_EDITPART.contains(type);
    }

    public EditPart createEditPart(EditPart context, Object model) {
        EditPart child = null;

        if (model == null)
            return null;

        if (model instanceof NedFileElementEx)
            child = new NedFileEditPart();
        else if (model instanceof CompoundModuleElementEx)
            child = new CompoundModuleEditPart();
        else if (model instanceof SubmoduleElementEx)
            child = new SubmoduleEditPart();
        else if (model instanceof ConnectionElementEx)
            child = new NedConnectionEditPart();
        else if (model instanceof INedTypeElement)
            child = new NedTypeEditPart();
        else if (model instanceof TypesElement)
            child = new TypesEditPart();
        else if (model instanceof PropertyElementEx) {
            PropertyElementEx property = (PropertyElementEx)model;
            if (property.getName().equals("figure")) {
                String type = property.getValue(PKEY_TYPE);

                if (type.equals(FTYPE_RECTANGLE)) {
                    child = new RectangleEditPart();
                } else if (type.equals(FTYPE_OVAL)) {
                    child = new OvalEditPart();
                } else if (type.equals(FTYPE_RING)) {
                    child = new RingEditPart();
                } else if (type.equals(FTYPE_PIESLICE)) {
                    child = new PiesliceEditPart();
                } else if (type.equals(FTYPE_ARC)) {
                    child = new ArcEditPart();
                } else if ((type.equals(FTYPE_POLYLINE)) || (type.equals(FTYPE_LINE))) {
                    child = new PolylineEditPart();
                } else if (type.equals(FTYPE_POLYGON)) {
                    child = new PolygonEditPart();
                } else if (type.equals(FTYPE_TEXT)) {
                    child = new TextEditPart();
                } else if (type.equals(FTYPE_LABEL)) {
                    child = new LabelEditPart();
                } else if (type.equals(FTYPE_GROUP)) {
                    child = new GroupEditPart();
                } else if (type.equals(FTYPE_PANEL)) {
                    child = new PanelEditPart();
                } else if (type.equals(FTYPE_IMAGE)) {
                    child = new ImageEditPart();
                } else if (type.equals(FTYPE_PIXMAP)) {
                    child = new PixmapEditPart();
                } else if (type.equals(FTYPE_ICON)) {
                    child = new IconEditPart();
                } else if (type.equals(FTYPE_PATH)) {
                    child = new PathEditPart();
                } else {
                    // unknown/custom figure type -- show a placeholder
                    child = new PlaceholderEditPart();
                }
            }
        } else
            throw new IllegalArgumentException("Unknown model element: " + model);

        child.setModel(model);
        return child;
    }

}
