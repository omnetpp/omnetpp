package com.simulcraft.test.gui.core;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;

public class EventUtils {

    enum EventType {
        KeyDown(SWT.KeyDown), KeyUp(SWT.KeyUp), MouseDown(SWT.MouseDown), 
        MouseUp(SWT.MouseUp), MouseMove(SWT.MouseMove), MouseEnter(SWT.MouseEnter), 
        MouseExit(SWT.MouseExit), MouseDoubleClick(SWT.MouseDoubleClick), 
        Paint(SWT.Paint), Move(SWT.Move), Resize(SWT.Resize), Dispose(SWT.Dispose), 
        Selection(SWT.Selection), DefaultSelection(SWT.DefaultSelection), 
        FocusIn(SWT.FocusIn), FocusOut(SWT.FocusOut), Expand(SWT.Expand), 
        Collapse(SWT.Collapse), Iconify(SWT.Iconify), Deiconify(SWT.Deiconify), 
        Close(SWT.Close), Show(SWT.Show), Hide(SWT.Hide), Modify(SWT.Modify), 
        Verify(SWT.Verify), Activate(SWT.Activate), Deactivate(SWT.Deactivate), 
        Help(SWT.Help), DragDetect(SWT.DragDetect), Arm(SWT.Arm), 
        Traverse(SWT.Traverse), MouseHover(SWT.MouseHover), 
        HardKeyDown(SWT.HardKeyDown), HardKeyUp(SWT.HardKeyUp), 
        MenuDetect(SWT.MenuDetect), SetData(SWT.SetData), 
        MouseWheel(SWT.MouseWheel), Settings(SWT.Settings), 
        EraseItem(SWT.EraseItem), MeasureItem(SWT.MeasureItem), 
        PaintItem(SWT.PaintItem);
        private int typeCode;

        EventType(int typeCode) {
            this.typeCode = typeCode; 
        }
        
        public int getTypeCode() {
            return typeCode;
        }

        public static EventType[] getAllEventTypes() {
            return values();
        }

        public static EventType[] getKeyboardEventTypes() {
            return new EventType[] {
                EventType.KeyDown, EventType.KeyUp,  
                EventType.HardKeyDown, EventType.HardKeyUp  
            };
        }

        public static EventType[] getMouseEventTypes() {
            return new EventType[] {
                EventType.MouseDown, EventType.MouseUp, EventType.MouseMove, 
                EventType.MouseEnter, EventType.MouseExit, 
                EventType.MouseDoubleClick, EventType.MouseWheel, 
                EventType.MouseHover 
            };
        }
        
        public static EventType lookup(int typeCode) {
            for (EventType e : values())
                if (e.typeCode == typeCode)
                    return e;
            return null;
        }

    };
    
    public static String getEventDescription(Event event) {
        EventType eventType = EventType.lookup(event.type);
        String result = eventType.name()+" ";
        if (ArrayUtils.contains(EventType.getKeyboardEventTypes(), eventType))
            result +=  String.format("char: '%c' (0x%x) key: 0x%x  mask: 0x%x KeyStroke: %s", (32<=event.character && event.character<127) ? event.character : '?', (int)event.character, event.keyCode, event.stateMask, KeyStroke.getInstance(event.keyCode, (int)event.character).format());
        if (ArrayUtils.contains(EventType.getMouseEventTypes(), eventType))
            result +=  String.format("button: 0x%x  coord: (%d,%d)",event.button, event.x, event.y);
        result += "  widget: " + event.widget + " item: " + event.item;
        result += "  detail: " + event.detail;
        return result;
    }
    
}
