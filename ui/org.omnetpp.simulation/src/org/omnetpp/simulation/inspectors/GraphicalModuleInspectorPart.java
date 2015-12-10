package org.omnetpp.simulation.inspectors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.AbstractHintLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.ScalableFigure;
import org.eclipse.draw2d.ScalableLayeredPane;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.ui.HoverInfo;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.layout.CompoundModuleLayout;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.canvas.SelectionItem;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.figures.MessageFigure;
import org.omnetpp.simulation.figures.SubmoduleFigureEx;
import org.omnetpp.simulation.inspectors.actions.CloseAction;
import org.omnetpp.simulation.inspectors.actions.EnlargeIconsAction;
import org.omnetpp.simulation.inspectors.actions.InspectAsObjectAction;
import org.omnetpp.simulation.inspectors.actions.InspectComponentTypeAction;
import org.omnetpp.simulation.inspectors.actions.InspectParentAction;
import org.omnetpp.simulation.inspectors.actions.ModelInformationAction;
import org.omnetpp.simulation.inspectors.actions.ReduceIconsAction;
import org.omnetpp.simulation.inspectors.actions.RelayoutAction;
import org.omnetpp.simulation.inspectors.actions.RunLocalAction;
import org.omnetpp.simulation.inspectors.actions.RunLocalFastAction;
import org.omnetpp.simulation.inspectors.actions.ShowArrowheadsAction;
import org.omnetpp.simulation.inspectors.actions.ShowSubmoduleNamesAction;
import org.omnetpp.simulation.inspectors.actions.StopAction;
import org.omnetpp.simulation.inspectors.actions.ZoomInAction;
import org.omnetpp.simulation.inspectors.actions.ZoomOutAction;
import org.omnetpp.simulation.model.cChannel;
import org.omnetpp.simulation.model.cComponent;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectTreeHoverInfo;

/**
 * An inspector that displays a compound module graphically.
 * @author Andras
 */
public class GraphicalModuleInspectorPart extends AbstractInspectorPart {
    protected static final DisplayString EMPTY_DISPLAYSTRING = new DisplayString("");
    protected static final int IMAGESIZEPERCENTAGE_MIN = 10;
    protected static final int IMAGESIZEPERCENTAGE_MAX = 500;

    protected Label labelFigure;
    protected ScrollPane scrollPane; // child of the inspector figure
    protected ScalableFigure scalableFigure; // child (content) of scrollPane
    protected CompoundModuleFigure compoundModuleFigure; // child of scalableFigure
    protected float scale = 1.0f;  // zoom level

    protected Map<cModule,SubmoduleFigureEx> submodules = new HashMap<cModule,SubmoduleFigureEx>();
    protected Map<cGate,ConnectionFigure> connections = new HashMap<cGate, ConnectionFigure>();
    protected Map<cMessage,MessageFigure> messages = new HashMap<cMessage, MessageFigure>();  //XXX remove if not useful
    protected int imageSizePercentage = 100; // controls submodule icons; 100 means 1:1
    protected boolean showNameLabels = true;
    protected boolean showArrowHeads = true;

    protected IFigure nextEventMarkerFigure;

    /**
     * Constructor.
     * @param descriptor TODO
     */
    public GraphicalModuleInspectorPart(InspectorDescriptor descriptor, IInspectorContainer parent, cModule module) {
        super(descriptor, parent, module);

        // mouse handling
        compoundModuleFigure.addMouseListener(new MouseListener() {
            @Override
            public void mouseDoubleClicked(MouseEvent me) {
                handleMouseDoubleClick(me);
            }

            @Override
            public void mousePressed(MouseEvent me) {
                handleMousePressed(me);
            }

            @Override
            public void mouseReleased(MouseEvent me) {
                handleMouseReleased(me);
            }
        });

        figure.addLayoutListener(new LayoutListener.Stub() {
            @Override
            public void postLayout(IFigure container) {
                // when zooming out, ensure inspector shrinks accordingly (size obeys maxSize)
                boolean changed = false;
                Rectangle bounds = container.getBounds().getCopy();
                Dimension maxSize = container.getMaximumSize();
                if (bounds.width > maxSize.width) { bounds.width = maxSize.width; changed = true; }
                if (bounds.height > maxSize.height) { bounds.height = maxSize.height; changed = true; }
                if (changed)
                    container.getParent().setConstraint(container, bounds);
            }
        });
    }

    private class InspectorFigure extends RoundedRectangle implements IInspectorFigure {
        public InspectorFigure() {
            setForegroundColor(ColorFactory.GREY70);
        }

        public Dimension getMaximumSize() {
            return getPreferredSize();  // so that it cannot be enlarged more than the embedded CompoundModuleFigure permits
        }
    }

    // Note: this class looks actually quite reusable, for many inspectors
    protected static class TitleAndContentLayout extends AbstractHintLayout {
        private static final int MARGIN_HEIGHT = 5;
        private static final int MARGIN_WIDTH = 5;
        private static final int VERTICAL_SPACING = 5;

        @Override
        public void layout(IFigure container) {
            @SuppressWarnings("unchecked")
            List<IFigure> children = container.getChildren();
            Assert.isTrue(children.size()==2, "two children expected, a title bar and a content figure");
            IFigure title = children.get(0);
            IFigure content = children.get(1);
            Rectangle containerBounds = container.getBounds();

            title.setBounds(new Rectangle(MARGIN_WIDTH+containerBounds.x, MARGIN_HEIGHT+containerBounds.y, containerBounds.width - 2*MARGIN_WIDTH, title.getPreferredSize().height));
            int contentTop = title.getBounds().bottom() + VERTICAL_SPACING;
            Rectangle contentBounds = new Rectangle(MARGIN_WIDTH+containerBounds.x, contentTop, containerBounds.width - 2*MARGIN_WIDTH, containerBounds.bottom() - contentTop - MARGIN_HEIGHT);
            Dimension contentMaximumSize = content.getMaximumSize();
            if (contentBounds.width > contentMaximumSize.width)
                contentBounds.width = contentMaximumSize.width;
            if (contentBounds.height > contentMaximumSize.height)
                contentBounds.height = contentMaximumSize.height;
            content.setBounds(contentBounds);
        }

        @Override
        protected Dimension calculatePreferredSize(IFigure container, int wHint, int hHint) {
            @SuppressWarnings("unchecked")
            List<IFigure> children = container.getChildren();
            Dimension titleSize = children.get(0).getPreferredSize();
            Dimension contentSize = children.get(1).getPreferredSize();
            return new Dimension(2*MARGIN_WIDTH + contentSize.width, 2*MARGIN_HEIGHT + VERTICAL_SPACING + titleSize.height + contentSize.height); // note: we ignore title's width, as text in it tends to be too long
        }

        @Override
        protected Dimension calculateMinimumSize(IFigure container, int wHint, int hHint) {
            @SuppressWarnings("unchecked")
            List<IFigure> children = container.getChildren();
            Dimension titleSize = children.get(0).getMinimumSize();
            Dimension contentSize = children.get(1).getMinimumSize();
            return new Dimension(2*MARGIN_WIDTH + contentSize.width, 2*MARGIN_HEIGHT + VERTICAL_SPACING + titleSize.height + contentSize.height); // note: we ignore title's width, as text in it tends to be too long
        }
    }

    @Override
    protected IInspectorFigure createFigure() {
        InspectorFigure root = new InspectorFigure();
        root.setLayoutManager(new TitleAndContentLayout()); // note: GridLayout has problems (it does not observe getMaximumSize() of children, and its getMinimumSize() returns getPreferredSize() i.e. it cannot be shrunk)
        root.setMinimumSize(new Dimension(20, 20));

        labelFigure = new Label();
        labelFigure.setLabelAlignment(PositionConstants.LEFT);
        labelFigure.setForegroundColor(ColorFactory.BLACK); // otherwise it would inherit border's color from parent
        root.add(labelFigure);

        scrollPane = new ScrollPane();
        root.add(scrollPane);

        scalableFigure = new ScalableLayeredPane(); //XXX or something simpler
        scrollPane.setContents(scalableFigure);
        scrollPane.setMinimumSize(new Dimension(20,20));
        scalableFigure.setLayoutManager(new StackLayout()); // for lack of a FillLayout class...

        compoundModuleFigure = new CompoundModuleFigure();
        scalableFigure.add(compoundModuleFigure);

        // set an initial display string, otherwise bad things (NPE, red background, etc) will happen
        // due to poor CompoundModuleFigure defaults if a HTTP error occurs before we set the proper
        // display string in refreshVisuals()
        compoundModuleFigure.setDisplayString(new DisplayString(""), scale);

        // work around slightly odd default behavior of scrollPane (scrollbars don't appear immediately when you shrink the window)
        scrollPane.addLayoutListener(new LayoutListener.Stub() {
            public void postLayout(IFigure container) {
                scrollPane.setHorizontalScrollBarVisibility(scrollPane.getSize().width >= scrollPane.getContents().getSize().width ? ScrollPane.NEVER : ScrollPane.ALWAYS);
                scrollPane.setVerticalScrollBarVisibility(scrollPane.getSize().height >= scrollPane.getContents().getSize().height ? ScrollPane.NEVER : ScrollPane.ALWAYS);
            }
        });

        getContainer().addMoveResizeSupport(root); // for resize
        getContainer().addMoveResizeSupport(labelFigure);  // for drag

        return root;
    }

    @Override
    public HoverInfo getHoverFor(int x, int y) {
        SubmoduleFigureEx submoduleFigure = findSubmoduleAt(x,y);
        if (submoduleFigure != null) {
            cModule submodule = findSubmoduleFor(submoduleFigure);
            if (submodule != null)
                return new ObjectTreeHoverInfo(new Object[] { submodule }, getContainer());
        }
        else if (labelFigure.containsPoint(x, y)) {
            return new ObjectTreeHoverInfo(new Object[] { object }, getContainer());
        }
        return null;
    }

    @Override
    public int getDragOperation(IFigure figure, int x, int y) {
        if (figure == this.figure)
            return FigureUtils.getBorderResizeDragOperation(x, y, figure.getBounds());
        if (figure == labelFigure)
            return FigureUtils.getMoveOnlyDragOperation(x, y, figure.getBounds());
        return 0;
    }

    public CompoundModuleFigure getCompoundModuleFigure() {
        return compoundModuleFigure;
    }

    //@Override
    public boolean isMaximizable() {
        return false;
    }

    public double getZoomLevel() {
        return scalableFigure.getScale();
    }

    public void setZoomLevel(double zoom) {
        Assert.isTrue(zoom > 0);

        boolean haveScrollbar = scrollPane.getHorizontalScrollBar().isVisible() || scrollPane.getVerticalScrollBar().isVisible();

        scalableFigure.setScale(zoom);
        refresh();

        if (!haveScrollbar) {
            // grow inspector window so that scrollbars don't appear if they weren't present before setting the zoom
            figure.getParent().setConstraint(figure, figure.getBounds().getCopy().setSize(figure.getPreferredSize()));
        }

        getContainer().updateFloatingToolbarActions();
    }

    public void zoomOut() {
        double zoom = getZoomLevel();
        zoom /= 1.5;
        if (Math.abs(zoom-1.0) < 0.01)
            zoom = 1.0f; // prevent accumulation of rounding errors
        setZoomLevel(zoom);
    }

    public void zoomIn() {
        double zoom = getZoomLevel();
        zoom *= 1.5;
        if (Math.abs(zoom-1.0) < 0.01)
            zoom = 1.0f; // prevent accumulation of rounding errors
        setZoomLevel(zoom);
    }

    public boolean canZoomOut() {
        return getZoomLevel() > 0.1; // a pretty arbitrary limit
    }

    public boolean canZoomIn() {
        return getZoomLevel() < 10; // a pretty arbitrary limit
    }

    public int getImageSizePercentage() {
        return imageSizePercentage;
    }

    public void setImageSizePercentage(int imageSizePercentage) {
        Assert.isTrue(imageSizePercentage > 0);
        this.imageSizePercentage = imageSizePercentage;
        refresh();
        getContainer().updateFloatingToolbarActions();
    }

    /**
     * Adjusts imageSizePercentage.
     */
    public void enlargeIcons() {
        int imageSizePercentage = getImageSizePercentage();
        if (imageSizePercentage < IMAGESIZEPERCENTAGE_MAX) {
            imageSizePercentage *= 1.2;
            if (Math.abs(imageSizePercentage-100) < 15)
                imageSizePercentage = 100; // prevent accumulation of rounding errors
            setImageSizePercentage(imageSizePercentage);
        }
    }

    /**
     * Adjusts imageSizePercentage.
     */
    public void reduceIcons() {
        int imageSizePercentage = getImageSizePercentage();
        if (imageSizePercentage > IMAGESIZEPERCENTAGE_MIN) {
            imageSizePercentage /= 1.2;
            if (Math.abs(imageSizePercentage-100) < 15)
                imageSizePercentage = 100; // prevent accumulation of rounding errors
            setImageSizePercentage(imageSizePercentage);
        }
    }

    public boolean canReduceIcons() {
        return getImageSizePercentage() > IMAGESIZEPERCENTAGE_MIN;
    }

    public boolean canEnlargeIcons() {
        return getImageSizePercentage() < IMAGESIZEPERCENTAGE_MAX;
    }

    public boolean getShowNameLabels() {
        return showNameLabels;
    }

    public void setShowNameLabels(boolean showNameLabels) {
        this.showNameLabels = showNameLabels;
        refresh();
    }

    public boolean getShowArrowHeads() {
        return showArrowHeads;
    }

    public void setShowArrowHeads(boolean showArrowHeads) {
        this.showArrowHeads = showArrowHeads;
        refresh();
    }

    public SubmoduleFigure getSubmoduleFigure(cModule submodule) {
        return submodules.get(submodule);
    }

    public ConnectionFigure getConnectionFigure(cGate srcGate) {
        return connections.get(srcGate);
    }

    public MessageFigure getMessageFigure(cMessage message) {
        return messages.get(message);
    }

    public void addMessageFigure(cMessage message, MessageFigure messageFigure) {
        getCompoundModuleFigure().getMessageLayer().add(messageFigure);
        messages.put(message, messageFigure);
    }

    public void removeMessageFigure(cMessage message) {
        MessageFigure messageFigure = messages.get(message);
        getCompoundModuleFigure().getMessageLayer().remove(messageFigure);
        messages.remove(message);
    }

    public void clearMessageFigures() {
        for (MessageFigure f : messages.values())
            f.getParent().remove(f);
        messages.clear();
    }

    public void relayout() {
        CompoundModuleLayout layouter = (CompoundModuleLayout) compoundModuleFigure.getSubmoduleLayer().getLayoutManager();
        layouter.requestFullLayout();
        //layouter.setSeed(layouter.getSeed()+1);  -- not needed
        compoundModuleFigure.getSubmoduleLayer().revalidate();
    }

    @Override
    public void refresh() {
        super.refresh();
        if (!isDisposed()) {
            try {
                cModule module = (cModule) object;
                preloadObjects(module);
                refreshLabel();
                refreshChildren();
                refreshConnections();
                refreshVisuals();
            }
            catch (CommunicationException e) {
                // try setting the label if it's empty, otherwise leave everything as it is
                if (labelFigure.getText().equals(""))
                    refreshLabel(); // this works even if module is unfilled
            }
        }
    }

    protected void preloadObjects(cModule module) throws CommunicationException {
        // load submodules, gates, channels in as few calls as possible:
        // module and submodules first
        module.loadIfUnfilled();
        module.getSimulation().loadUnfilledObjects(module.getSubmodules());

        // all gates (needed for connections)
        List<cGate> gateList = new ArrayList<cGate>();
        for (cGate gate : module.getGates())
            if (!gate.isFilledIn())
                gateList.add(gate);
        for (cModule submodule : module.getSubmodules())
            for (cGate gate : submodule.getGates())
                if (!gate.isFilledIn())
                    gateList.add(gate);
        module.getSimulation().loadObjects(gateList); // pre-filtered to unfilled objects

        // channel objects
        List<cChannel> channelList = new ArrayList<cChannel>();
        for (cGate gate : gateList)
            if (gate.getChannel() != null && !gate.getChannel().isFilledIn())
                channelList.add(gate.getChannel());
        module.getSimulation().loadObjects(channelList); // pre-filtered to unfilled objects
    }

    protected void refreshLabel() {
        cModule module = (cModule) getObject();
        if (module.isFilledIn())
            labelFigure.setText("(" + module.getShortTypeName() + ") " + module.getFullPath() + " [id=" + module.getId() + "]");
        else
            labelFigure.setText("(" + module.getClass().getSimpleName() + ") n/a"); //FIXME TODO ebbol egy utility fuggvenyt csinalni!!!!!!!
    }

    protected void refreshChildren() {
        //TODO only call this function if there were any moduleCreated/moduleDeleted notifications from the simkernel
        List<cModule> toBeRemoved = null;
        List<cModule> toBeAdded = null;

        // find submodule figures whose module has been deleted
        for (cModule submodule : submodules.keySet()) {
            if (submodule.isDisposed() || submodule.getParentModule() != object) {
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<cModule>();
                toBeRemoved.add(submodule);
            }
        }

        // find submodules that not yet have a figure
        for (cModule submodule : ((cModule)object).getSubmodules()) {
            if (!submodules.containsKey(submodule)) {
                if (toBeAdded == null)
                    toBeAdded = new ArrayList<cModule>();
                toBeAdded.add(submodule);
            }
        }

        // do the removals and additions
        if (toBeRemoved != null) {
            for (cModule submodule : toBeRemoved) {
                compoundModuleFigure.getSubmoduleLayer().remove(submodules.get(submodule));
                submodules.remove(submodule);
            }
        }
        if (toBeAdded != null) {
            for (cModule submodule : toBeAdded) {
                // create figure
                SubmoduleFigureEx submoduleFigure = new SubmoduleFigureEx();
                submoduleFigure.setFont(getContainer().getControl().getFont()); // to speed up figure's getFont()
                submoduleFigure.setPinVisible(false);
                compoundModuleFigure.getSubmoduleLayer().add(submoduleFigure);
                submoduleFigure.setRangeFigureLayer(compoundModuleFigure.getBackgroundDecorationLayer());
                submodules.put(submodule, submoduleFigure);
            }
        }
    }

    protected void refreshConnections() throws CommunicationException {
        ArrayList<cGate> toBeRemoved = null;
        ArrayList<cGate> toBeAdded = null;

        // find connection figures whose connection has been deleted
        for (cGate gate : connections.keySet()) {
            if (gate.isDisposed() || gate.getNextGate() == null) {
                if (toBeRemoved == null)
                    toBeRemoved = new ArrayList<cGate>();
                toBeRemoved.add(gate);
            }
        }

        // find connections that not yet have a figure.
        // loop through all submodules and enclosing module, and find their connections
        boolean atParent = false;
        cModule parentModule = (cModule)object;
        cModule[] submodules2 = parentModule.getSubmodules();
        for (int i = 0; i <= submodules2.length; i++) {
            cModule module = i < submodules2.length ? submodules2[i] : parentModule;
            if (module==parentModule)
                atParent = true;
            module.getSimulation().loadUnfilledObjects(module.getGates());
            for (cGate gate : module.getGates()) {
                if (gate.getType()==(atParent ? cGate.Type.INPUT : cGate.Type.OUTPUT) && gate.getNextGate() != null) {
                    if (!connections.containsKey(gate)) {
                        if (toBeAdded == null)
                            toBeAdded = new ArrayList<cGate>();
                        toBeAdded.add(gate);
                    }
                }
            }
        }

        // do the removals and additions
        if (toBeRemoved != null) {
            for (cGate gate : toBeRemoved) {
                compoundModuleFigure.getConnectionLayer().remove(connections.get(gate));
                connections.remove(gate);
            }
        }
        if (toBeAdded != null) {
            for (cGate gate : toBeAdded) {
                // create figure
                cGate targetGate = gate.getNextGate();
                ConnectionFigure connectionFigure = new ConnectionFigure();
                GateAnchor sourceAnchor = (gate.getOwnerModule().equals(parentModule) ?
                        new CompoundModuleGateAnchor(compoundModuleFigure) : new GateAnchor(submodules.get(gate.getOwnerModule())));
                GateAnchor targetAnchor = (targetGate.getOwnerModule().equals(parentModule) ?
                        new CompoundModuleGateAnchor(compoundModuleFigure) : new GateAnchor(submodules.get(targetGate.getOwnerModule())));
                connectionFigure.setSourceAnchor(sourceAnchor);
                connectionFigure.setTargetAnchor(targetAnchor);
                compoundModuleFigure.getConnectionLayer().add(connectionFigure);
                connections.put(gate, connectionFigure);
            }
        }
    }

    protected void refreshVisuals() throws CommunicationException {
        cModule parentModule = (cModule) object;
        compoundModuleFigure.setDisplayString(getDisplayStringFrom(parentModule), scale);

        // refresh submodules
        for (cModule submodule : submodules.keySet()) {
            submodule.loadIfUnfilled();
            SubmoduleFigureEx submoduleFigure = submodules.get(submodule);
            submoduleFigure.setDisplayString(getDisplayStringFrom(submodule), scale, null);
            submoduleFigure.setName(showNameLabels ? submodule.getFullName() : null);
            submoduleFigure.setSubmoduleVectorIndex(submodule.getName(), submodule.getVectorSize(), submodule.getIndex());
            submoduleFigure.setImageSizePercentage(imageSizePercentage);
        }

        // refresh connections
        //FIXME this is very slow!!!! even when there are no display strings at all. ConnectionFigure is not caching the displaystring?
        for (cGate gate : connections.keySet()) {
            gate.loadIfUnfilled();
            ConnectionFigure connectionFigure = connections.get(gate);
            connectionFigure.setDisplayString(getDisplayStringFrom(gate.getChannel())); // note: gate.getDisplayString() would implicitly create a cIdealChannel!
            connectionFigure.setArrowHeadEnabled(showArrowHeads);
        }

        //Debug.debug = true;
        //FigureUtils.debugPrintRootFigureHierarchy(figure);
    }

    protected IDisplayString getDisplayStringFrom(cComponent component) throws CommunicationException {
        if (component == null)
            return EMPTY_DISPLAYSTRING;
        component.loadIfUnfilled();  //XXX maybe not here...
        return component.getDisplayString();
    }

    protected cModule findSubmoduleFor(SubmoduleFigureEx submoduleFigure) {
        for (cModule submodule : submodules.keySet())
            if (submodules.get(submodule) == submoduleFigure)
                return submodule;
        return null;
    }

    @Override
    public IFigure findFigureContaining(cObject object) {
        cObject submoduleOrConnection = null;
        for (cObject o = object; o != null; o = o.getOwner()) {
            if (o.getOwner() == this.object) {
                submoduleOrConnection = o;
                break;
            }
        }

        if (submoduleOrConnection instanceof cModule) {
            System.out.println("FOR " + object + " FIGURE IS " + submoduleOrConnection);
            return submodules.get((cModule)submoduleOrConnection);
        }
        else if (submoduleOrConnection instanceof cChannel) {
            return null; //FIXME find and return connection figure
        }
        else if (submoduleOrConnection instanceof cChannel) {
            return null; //FIXME find and return connection figure
        }
        else {
            return figure; // whatever else it is, we cannot point to a figure that represents it
        }
    }

    protected void handleMouseDoubleClick(MouseEvent me) {
        SubmoduleFigureEx submoduleFigure = findSubmoduleAt(me.x,me.y);
        System.out.println("clicked submodule: " + submoduleFigure);
        if (submoduleFigure != null) {
            cModule submodule = findSubmoduleFor(submoduleFigure);
            if (submodule != null)
                inspectorContainer.inspect(submodule);
        }
    }

    protected void handleMousePressed(MouseEvent me) {
        //TODO also allow selecting connections!!!
        SubmoduleFigureEx submoduleFigure = findSubmoduleAt(me.x,me.y);
        System.out.println("clicked: " + submoduleFigure);

        // make selection item
        Object selectionItem;
        if (submoduleFigure == null)
            selectionItem = null;  //XXX currently this only happens when one clicks on a connection -- when clicking on the background, MoveResizeSupport handles in somehow (?)
        else {
            cModule submodule = findSubmoduleFor(submoduleFigure);
            selectionItem = new SelectionItem(this, submodule);
        }

        // modify selection
        if (selectionItem == null) {
            if (me.button == 1 && (me.getState()&SWT.CONTROL) == 0)
                inspectorContainer.deselectAll();
        }
        else {
            if (me.button == 1) {
                if ((me.getState()&SWT.CONTROL) != 0)
                    inspectorContainer.toggleSelection(selectionItem);
                else
                    inspectorContainer.select(selectionItem, true);
            }
            else if (me.button == 3) { // right-click
                if ((me.getState()&SWT.CONTROL) != 0)
                    inspectorContainer.select(selectionItem, false);
                else if (SelectionUtils.contains(inspectorContainer.getSelection(), selectionItem))
                    /* no selection change*/;
                else
                    inspectorContainer.select(selectionItem, true);
            }
        }
        //note: no me.consume()! it would kill the move/resize listener
    }

    protected void handleMouseReleased(MouseEvent me) {
    }

    public SubmoduleFigureEx findSubmoduleAt(int x, int y) {
        IFigure target = figure.findFigureAt(x, y);
        while (target != null && !(target instanceof SubmoduleFigureEx))
            target = target.getParent();
        return (SubmoduleFigureEx)target;
    }

    @Override
    public void selectionChanged(IStructuredSelection selection) {
        super.selectionChanged(selection);

        // update selection border around submodules
        List<cModule> selectedLocalObjects = SelectionUtils.getObjects(selection, this, cModule.class);
        for (cModule submodule : submodules.keySet()) {
            SubmoduleFigureEx submoduleFigure = submodules.get(submodule);
            submoduleFigure.setBorder(selectedLocalObjects.contains(submodule) ? new LineBorder(3) : null);
        }
    }

    public void populateContextMenu(MenuManager menu, int x, int y) {
        System.out.println(this + ": populateContextMenu invoked");
        SubmoduleFigureEx submoduleFigure = findSubmoduleAt(x, y);
        if (submoduleFigure == null)
            populateBackgroundContextMenu(menu);
        else
            getContainer().populateContextMenu(menu, getContainer().getSelection()); // submodule is already selected at this point
    }

    protected void populateBackgroundContextMenu(MenuManager menu) {
        menu.add(my(new RelayoutAction()));
        menu.add(new Separator());
        menu.add(my(new ZoomInAction()));
        menu.add(my(new ZoomOutAction()));
        menu.add(my(new EnlargeIconsAction()));
        menu.add(my(new ReduceIconsAction()));
        menu.add(new Separator());
        menu.add(my(new ShowSubmoduleNamesAction()));
        menu.add(my(new ShowArrowheadsAction()));
        menu.add(new Separator());
        menu.add(my(new InspectParentAction()));
        menu.add(new Separator());
        menu.add(my(new CloseAction()));
    }

    @Override
    public void populateFloatingToolbar(ToolBarManager toolbar) {
        //XXX the following buttons are from Tkenv -- revise which ones we really need
        toolbar.add(my(new InspectParentAction()));
        toolbar.add(my(new InspectAsObjectAction()));
        toolbar.add(my(new InspectComponentTypeAction()));
        toolbar.add(my(new ModelInformationAction()));
//        ToolItem search = addToolItem(toolbar, "Find and inspect messages, queues, watched variables, statistics, etc (Ctrl+S)", SimulationUIConstants.IMG_TOOL_FINDOBJ);
        toolbar.add(new Separator());
        toolbar.add(my(new RunLocalAction()));
        toolbar.add(my(new RunLocalFastAction()));
        toolbar.add(my(new StopAction()));
        toolbar.add(new Separator());
//        ToolItem moduleOutput = addToolItem(toolbar, "See module output", SimulationUIConstants.IMG_TOOL_ASOUTPUT);
        toolbar.add(my(new RelayoutAction()));
        toolbar.add(new Separator());
        toolbar.add(my(new ShowSubmoduleNamesAction()));
        toolbar.add(my(new ShowArrowheadsAction()));
        toolbar.add(my(new EnlargeIconsAction()));
        toolbar.add(my(new ReduceIconsAction()));
        toolbar.add(my(new ZoomInAction()));
        toolbar.add(my(new ZoomOutAction()));
        toolbar.update(false);
    }

    public void setNextEventMarker(cModule submodule, boolean primary) {
        if (nextEventMarkerFigure != null)
            removeNextEventMarker();

        RoundedRectangle roundedRectangle = new RoundedRectangle() {
            public boolean containsPoint(int x, int y) { return false; }  // otherwise module icon under it becomes non-clickable
        };
        roundedRectangle.setCornerDimensions(new Dimension(16,16));
        roundedRectangle.setAlpha(140);
        roundedRectangle.setFill(false);
        int borderWidth = primary ? 4 : 2;
        roundedRectangle.setLineWidth(borderWidth);
        roundedRectangle.setForegroundColor(ColorFactory.RED);

        nextEventMarkerFigure = roundedRectangle;

        getCompoundModuleFigure().getForegroundDecorationLayer().add(nextEventMarkerFigure);
        SubmoduleFigureEx submoduleFigure = submodules.get(submodule);
        nextEventMarkerFigure.setBounds(submoduleFigure.getShapeBounds().getShrinked(-(borderWidth+2), -(borderWidth+2)));
    }

    public void removeNextEventMarker() {
        if (nextEventMarkerFigure != null) {
            nextEventMarkerFigure.getParent().remove(nextEventMarkerFigure);
            nextEventMarkerFigure = null;
        }
    }

}
