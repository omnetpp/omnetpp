package org.omnetpp.test.gui.core;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

public class EventTracer {
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

	public static void start() {
		System.out.println("startDumpingEvents: hooking event filters");

		Listener eventFilter = new Listener() {
			public void handleEvent(Event event) {
				System.out.println(" - " + EventType.lookup(event.type) + ": " + event.toString());
			}
		};

		for (EventType eventType : EventType.getMouseEventTypes())
			Display.getDefault().addFilter(eventType.getTypeCode(), eventFilter);
		for (EventType eventType : EventType.getKeyboardEventTypes())
			Display.getDefault().addFilter(eventType.getTypeCode(), eventFilter);
	}

}
