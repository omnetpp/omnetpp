package org.omnetpp.simulation.ui;

import org.eclipse.jface.viewers.DelegatingStyledCellLabelProvider.IStyledLabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.jface.viewers.StyledString.Styler;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.model.Field;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectFieldsTreeContentProvider.FieldArrayElement;
import org.omnetpp.simulation.ui.ObjectFieldsTreeContentProvider.FieldGroup;
import org.omnetpp.simulation.ui.ObjectFieldsTreeContentProvider.Notice;

/**
 *
 * @author Andras
 */
public class ObjectFieldsTreeLabelProvider implements IStyledLabelProvider {
    private static final Image IMG_FIELD = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_FIELD);

    private static class ColorStyler extends Styler {
        Color color;
        public ColorStyler(Color color) { this.color = color; }
        @Override public void applyStyles(TextStyle textStyle) { textStyle.foreground = color; }
    };

    private Styler blueStyle = new ColorStyler(ColorFactory.BLUE4);
    private Styler greyStyle = new ColorStyler(ColorFactory.GREY60);
    private Styler brownStyle = new ColorStyler(ColorFactory.BURLYWOOD4);

    @Override
    public StyledString getStyledText(Object element) {
            StyledString result = new StyledString();
            if (element instanceof cObject) {
                cObject object = (cObject) element;
                try {
                    object.loadIfUnfilled();
                    result.append(object.getFullName());
                    result.append(" (" + object.getShortTypeName() + ")", greyStyle);
                    String infoTxt = object.getInfo();
                    if (!infoTxt.equals(""))
                        result.append("  " + infoTxt, brownStyle);
                    return result;
                }
                catch (CommunicationException e) {
                    result.append("(" + object.getClass().getSimpleName() + "?)", greyStyle);
                    result.append(" Error loading object info, try Refresh");
                    return result;
                }
            }
            else if (element instanceof Field || element instanceof FieldArrayElement) {
                boolean isArrayElement = element instanceof FieldArrayElement;
                Field field = !isArrayElement ? (Field)element : ((FieldArrayElement)element).field;
                Object value = !isArrayElement ? field.getValue() : field.getValues()[((FieldArrayElement)element).index];

                // the @label property can be used to override the field name
                String name = field.getLabelProperty()!=null ? field.getLabelProperty() : field.getName();

                // if it's an unexpanded array, return "name[size]" immediately
                if (field.isArray() && !isArrayElement) {
                    result.append(name + "[" + field.getValues().length + "]");
                    result.append(" (" + field.getType() + ")", greyStyle);
                    return result;
                }

                // when showing array elements, omit name and just show "[index]" instead
                if (isArrayElement)
                    name = "[" + ((FieldArrayElement)element).index + "]";

                // we'll want to print the field type, except for expanded array elements
                // (no need to repeat it, as it's printed in the "name[size]" node already)
                String typeNameText = isArrayElement ? "" : " (" + field.getType() + ")";

                if (field.isCompound()) {
                    // if it's an object, try to say something about it...
                    if (field.isCObject()) {
                        result.append(name + " = ");
                        if (value == null)
                            result.append("NULL", blueStyle);
                        else {
                            cObject valueObject = (cObject) value;
                            try {
                                valueObject.loadIfUnfilled();
                                valueObject.loadFieldsIfUnfilled();
                                result.append("(" + valueObject.getShortTypeName() + ") ", brownStyle);
                                String fullNameOrPath = (valueObject.getOwner() == field.getOwner()) ?
                                        valueObject.getFullName() : valueObject.getFullPath();
                                result.append(fullNameOrPath, blueStyle);
                                String infoTxt = valueObject.getInfo();
                                if (!infoTxt.equals(""))
                                    result.append(": " + infoTxt, brownStyle);
                            }
                            catch (CommunicationException e) {
                                result.append("(" + valueObject.getClass().getSimpleName() + "?)", blueStyle);
                                result.append(" Error loading object info, try Refresh");
                            }
                        }
                        result.append(typeNameText, greyStyle);
                        return result;
                    }
                    else {
                        // a value can be generated via operator<<
                        result.append(name);
                        if (!value.toString().equals("")) {
                            result.append(" = ");
                            result.append(value.toString(), blueStyle);
                        }
                        result.append(typeNameText, greyStyle);
                        return result;
                    }
                }
                else {
                    // plain field, return "name = value" text
                    if (field.getEnumProperty() != null) {
                        typeNameText += " - enum " + field.getEnumProperty();
                        value = field.getValueSymbolicName() + " (" + value.toString() + ")";
                    }
                    if (field.getType().equals("string"))
                        value = "\"" + value.toString() + "\"";

                    result.append(name);
                    if (!value.toString().equals("")) {
                        result.append(" = ");
                        result.append(value.toString(), blueStyle);
                    }
                    result.append(typeNameText, greyStyle);
                    return result;
                }
            }
            else if (element instanceof FieldGroup) {
                FieldGroup group = (FieldGroup) element;
                return result.append(group.groupName);
            }
            else if (element instanceof Notice) {
                return result.append(((Notice) element).message);
            }
            else {
                return result.append(element.toString());
            }
    }

    @Override
    public Image getImage(Object element) {
        try {
            if (element instanceof FieldGroup)
                return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJ_FOLDER);
            if (element instanceof Notice)
                return ((Notice)element).image;
            if (element instanceof String)
                return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_INFO_TSK);

            Object value = element;
            if (element instanceof Field)
                value = ((Field) element).getValue();
            if (element instanceof FieldArrayElement) {
                FieldArrayElement fieldArrayElement = (FieldArrayElement)element;
                value = fieldArrayElement.field.getValues()[fieldArrayElement.index];
            }
            if (value instanceof cObject) {
                cObject object = (cObject)value;
                object.loadIfUnfilled();
                object.loadFieldsIfUnfilled();
                String icon = object.getIcon(); // note: may be empty
                return SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_DIR + icon + ".png", SimulationUIConstants.IMG_OBJ_COGWHEEL);
            }
            return IMG_FIELD;
        }
        catch (CommunicationException e) {
            return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK);
        }
    }

    @Override
    public void dispose() {
    }

    @Override
    public boolean isLabelProperty(Object element, String property) {
        return false;
    }

    @Override
    public void addListener(ILabelProviderListener listener) {
    }

    @Override
    public void removeListener(ILabelProviderListener listener) {
    }

}
