package org.omnetpp.sequencechart.editors;

import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.omnetpp.sequencechart.widgets.SequenceChart;


public class SequenceChartContributor extends EditorActionBarContributor {
	protected SequenceChart sequenceChart;
	
	public SequenceChartContributor() {
	}
	
	public SequenceChartContributor(SequenceChart sequenceChart) {
		this.sequenceChart = sequenceChart;
	}

	public void contributeToPopupMenu(IMenuManager menuManager) {
	}
	@Override
	public void contributeToToolBar(IToolBarManager toolBarManager) {
	}
	
	@Override
	public void setActiveEditor(IEditorPart targetEditor) {
		sequenceChart = ((SequenceChartEditor)targetEditor).getSequenceChart();
	}

	private void displayPopupMenu(MouseEvent e) {
/* FIXME: resurrect this code
		final int x = e.x;
		Menu popupMenu = new Menu(seqChart);
		ArrayList<IEvent> events = new ArrayList<IEvent>();
		ArrayList<MessageEntry> msgs = new ArrayList<MessageEntry>();
		Point p = seqChart.toControl(seqChart.getDisplay().getCursorLocation());
		seqChart.collectStuffUnderMouse(p.x, p.y, events, msgs);

		// center menu item
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Center");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.gotoSimulationTimeWithCenter(seqChart.pixelToSimulationTime(x));
			}
		});

		// axis submenu
		MenuItem cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
		cascadeItem.setText("Axis");
		Menu subMenu = new Menu(popupMenu);
		cascadeItem.setMenu(subMenu);

		subMenuItem = new MenuItem(subMenu, SWT.PUSH);
		subMenuItem.setText("Dense");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setAxisSpacing(20);
			}
		});
		
		subMenuItem = new MenuItem(subMenu, SWT.PUSH);
		subMenuItem.setText("Evenly");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setAxisSpacing(-1);
			}
		});
		
		new MenuItem(popupMenu, SWT.SEPARATOR);
		
		// events submenu
		for (final IEvent event : events) {
			cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
			cascadeItem.setText(seqChart.getEventText(event));
			subMenu = new Menu(popupMenu);
			cascadeItem.setMenu(subMenu);

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Center");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoEvent(event);
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Select");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					// TODO:
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Filter to");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					showSequenceChartForEvent(event.getEventNumber());
				}
			});
		}
		
		new MenuItem(popupMenu, SWT.SEPARATOR);

		// messages submenu
		for (final MessageEntry msg : msgs) {
			cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
			cascadeItem.setText(seqChart.getMessageText(msg));
			subMenu = new Menu(popupMenu);
			cascadeItem.setMenu(subMenu);

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Zoom to message");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoSimulationTimeRange(msg.getSource().getSimulationTime(), msg.getTarget().getSimulationTime(), (int)(seqChart.getWidth() * 0.1));
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Go to source event");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoEvent(msg.getSource());
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Go to target event");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoEvent(msg.getTarget());
				}
			});
		}
		
		// axis submenu
		final ModuleTreeItem axisModule = seqChart.findAxisAt(p.y);
		if (axisModule != null) {
			cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
			cascadeItem.setText(seqChart.getAxisText(axisModule));
			subMenu = new Menu(popupMenu);
			cascadeItem.setMenu(subMenu);

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Center");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoAxisModule(axisModule);
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Zoom to value");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoAxisValue(axisModule, seqChart.pixelToSimulationTime(x));
				}
			});			
		}
		
		seqChart.setMenu(popupMenu);
*/
	}
}
