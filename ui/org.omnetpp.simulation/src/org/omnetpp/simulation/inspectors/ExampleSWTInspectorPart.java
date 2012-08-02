package org.omnetpp.simulation.inspectors;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.List;
import org.omnetpp.simulation.model.Field;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;

public class ExampleSWTInspectorPart extends AbstractSWTInspectorPart {

    public ExampleSWTInspectorPart(IInspectorContainer parent, cObject object) {
        super(parent, object);
    }

    @Override
    protected Control createControl(Composite parent) {
        List listbox = new List(parent, SWT.BORDER | SWT.DOUBLE_BUFFERED | SWT.V_SCROLL);
        listbox.setItems("no content yet".split(" ")); //XXX

        listbox.setSize(300, 200);

        listbox.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                inspectorContainer.select(object, true); //XXX
            }
        });
        return listbox;
    }

    @Override
    public boolean isMaximizable() {
        return false;
    }

    @Override
    public void populateContextMenu(MenuManager contextMenuManager, Point p) {
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager manager) {
    }
    
    @Override
    public void refresh() {
        super.refresh();
        if (!isDisposed()) {
            List listbox = (List)getControl();
            listbox.setItems(new String[0]);  // clear listbox

            listbox.add("\"" + object.getFullName() + "\" (" + object.getClassName() + ")");

            cObject o = object;
            while (o instanceof cPacket) {
                addObjectDetails(listbox, o);
                o = ((cPacket)o).getEncapsulatedPacket();
            }
        }
    }

    private void addObjectDetails(List listbox, cObject object) {
        load(object);

        String[] builtinClasses = StringUtils.split("cObject cNamedObject cOwnedObject cMessage cPacket");

        listbox.add(object.getClassName() + " - 11.22.33.44 -> 99.88.77.66");

        for (Field field : object.getFields()) {
            if (!ArrayUtils.contains(builtinClasses, field.declaredOn)) {
//                String txt = "   " + field.declaredOn + "::" + field.name + " = ";
                String txt = "   " + field.name + " = ";
                if (field.values == null)
                    txt += field.value;  // prints "null" if value==null
                else
                    txt += "[ " + StringUtils.join(field.values, ", ") + " ]";
//                txt += " (" + field.type + ")";

                listbox.add(txt);
            }
        }
    }

    private void load(cObject object) {
        if (!object.isFilledIn())
            object.safeLoad();
        if (!object.isFieldsFilledIn())
            object.safeLoadFields();
    }


}
