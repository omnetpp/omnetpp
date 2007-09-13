package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleConnectionEditPart;
import org.omnetpp.ned.editor.graph.edit.NedTypeEditPart;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;


public class GraphicalNedEditorAccess
	extends EditorPartAccess
{
	public GraphicalNedEditorAccess(GraphicalNedEditor editorPart) {
		super(editorPart);
	}
	
	public GraphicalNedEditor getGraphicalNedEditor() {
		return (GraphicalNedEditor)workbenchPart;
	}
	
	public FlyoutPaletteCompositeAccess getFlyoutPaletteComposite() {
		return new FlyoutPaletteCompositeAccess((FlyoutPaletteComposite)ReflectionUtils.getFieldValue(getGraphicalNedEditor(), "splitter"));
	}

	@NotInUIThread
	public NedTypeEditPart createSimpleModuleWithPalette(String name) {
		clickPaletteItem(".*Simple.*module.*");
		clickBackground();
		renameModule("Unnamed.*", name);

		// TODO:
		return null;
	}

	@NotInUIThread
	public void renameModule(String oldName, String newName) {
		cliclLabelFigure(oldName);
		typeInNewName(newName);
	}

	@InUIThread
	private void typeInNewName(String name) {
		FigureCanvas figureCanvas = getGraphicalNedEditor().getFigureCanvas();
		Text text = (Text)Access.findDescendantControl(figureCanvas, Text.class);
		TextAccess textAccess = new TextAccess(text);
		textAccess.pressKey(SWT.F6);
		textAccess.typeIn(name);
		textAccess.pressEnter();
	}

	@InUIThread
	public void cliclLabelFigure(final String name) {
		IFigure labelFigure = findDescendantFigure(getRootFigure(), new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Label && ((Label)object).getText().matches(name);
			}
		});

		new FigureAccess(labelFigure).click(LEFT_MOUSE_BUTTON);
	}

    @SuppressWarnings("unchecked")
    @InUIThread
    public void cliclConnectionFigure(final String label1, final String label2) {
        final IPredicate predicate = new IPredicate() {
            public boolean matches(Object object) {
                if (object instanceof ModuleConnectionEditPart) {
                    ModuleConnectionEditPart connectionEditPart = (ModuleConnectionEditPart)object;
                    String sourceName = ((INEDElement)connectionEditPart.getSource().getModel()).getAttribute("name");
                    String targetName = ((INEDElement)connectionEditPart.getTarget().getModel()).getAttribute("name");
                    
                    return sourceName.matches(label1) && targetName.matches(label2);
                }

                return false;
            }
        };
        GraphicalEditPart graphicalEditPart = (GraphicalEditPart)findDescendantEditPart(getRootEditPart(), new IPredicate() {
            @SuppressWarnings("unchecked")
            public boolean matches(Object object) {
                if (object instanceof GraphicalEditPart) {
                    GraphicalEditPart graphicalEditPart = (GraphicalEditPart)object;
                    return hasObject(graphicalEditPart.getSourceConnections(), predicate);
                }
                
                return false;
            }
        });
        
        ConnectionEditPart connectionEditPart = (ConnectionEditPart)findObject(graphicalEditPart.getSourceConnections(), predicate);
        ConnectionFigure connectionFigure = (ConnectionFigure)connectionEditPart.getFigure();
        new FigureAccess(connectionFigure).click(LEFT_MOUSE_BUTTON);
    }

	@InUIThread
	public void clickBackground() {
		getComposite().click(LEFT_MOUSE_BUTTON, getCompositeInternal().toDisplay(1, 1));
	}

	@InUIThread
	public void clickPaletteItem(String label) {
		getFlyoutPaletteComposite().clickButtonWithLabel(label);
	}

    @InUIThread
	public CompoundModuleEditPartAccess createCompoundModuleWithPalette(String name) {
		clickPaletteItem(".*Compound.*Module.*");
		clickBackground();

		return new CompoundModuleEditPartAccess((CompoundModuleEditPart)findDescendantEditPart(getRootEditPart(), CompoundModuleEditPart.class));
	}

    private GraphicalViewer getGraphicalViewer() {
        GraphicalViewer graphicalViewer = (GraphicalViewer)ReflectionUtils.invokeMethod(getGraphicalNedEditor(), "getGraphicalViewer");
        return graphicalViewer;
    }

    private RootEditPart getRootEditPart() {
        return getGraphicalViewer().getRootEditPart();
    }
    
    private IFigure getRootFigure() {
        FigureCanvas figureCanvas = getGraphicalNedEditor().getFigureCanvas();
        IFigure rootFigure = figureCanvas.getLightweightSystem().getRootFigure();
        return rootFigure;
    }
}
