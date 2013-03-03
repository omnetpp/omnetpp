package org.omnetpp.simulation.figures;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.FocusEvent;
import org.eclipse.draw2d.FocusListener;
import org.eclipse.draw2d.GridData;
import org.eclipse.draw2d.GridLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.KeyEvent;
import org.eclipse.draw2d.KeyListener;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.Panel;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.DelegatingStyledCellLabelProvider.IStyledLabelProvider;
import org.eclipse.jface.viewers.IInputSelectionProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.figures.TreeItemFigure.ToggleFigure;

// TODO styles: SINGLE, MULTI
public class TreeFigure extends ScrollPane implements IInputSelectionProvider {

    private static final Cursor CURSOR_ARROW = new Cursor(Display.getDefault(), SWT.CURSOR_ARROW);

    private static TreeFigureTheme theme;  // static because tree items have no back pointer

    private ListenerList selectionListeners = new ListenerList();
    private ListenerList selectionChangedListeners = new ListenerList();

    // panel containing TreeItemFigures
    class ContentsPanel extends Panel {
        @Override
        public void remove(IFigure figure) {
            handleItemRemoved(figure);
            super.remove(figure);
        }

        @Override
        protected boolean useLocalCoordinates() {
            return true;
        }
    }

    Object input;
    ITreeContentProvider contentProvider;
    IStyledLabelProvider labelProvider;
    TreeItemFigure selectionAnchorItem;  // one end of the range to be selected, the other end comes from the event
    TreeItemFigure activeItem;           // target of key events
    Panel contentsPane;

    MouseListener toggleMouseListener = new MouseListener.Stub() {
        @Override
        public void mouseReleased(MouseEvent event) {
            handleMouseReleasedOnToggle(event);
        }
    };

    MouseListener itemMouseClickListener = new MouseListener.Stub() {
        @Override
        public void mousePressed(MouseEvent event) {
            handleMousePressedOnItem(event);
        }
        @Override
        public void mouseDoubleClicked(MouseEvent event) {
            handleMouseDoubleClickedOnItem(event);
        }
    };

    KeyListener itemKeyPressedListener = new KeyListener.Stub() {
        @Override
        public void keyPressed(KeyEvent event) {
            handleKeyPressedEvent(event);
        }
    };

    FocusListener focusListener = new FocusListener() {
        @Override public void focusGained(FocusEvent event) {
            handleFocusGainedEvent(event);
        }

        @Override public void focusLost(FocusEvent event) {
            handleFocusLostEvent(event);
        }
    };

    public TreeFigure() {
        setRequestFocusEnabled(true);
        setFocusTraversable(true);
        setBorder(new LineBorder(Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_BORDER)));
        setCursor(CURSOR_ARROW); // XXX should not be necessary

        contentsPane = new ContentsPanel();
        GridLayout layout = new GridLayout(1, false);
        layout.marginWidth = 0;
        layout.marginHeight = 0;
        layout.verticalSpacing = 0;
        contentsPane.setLayoutManager(layout);
        setContents(contentsPane);

        addKeyListener(itemKeyPressedListener);
        addFocusListener(focusListener);
    }

    public Dimension getMaximumSize() {
        return getPreferredSize();
    }

    public Object getInput() {
        return input;
    }

    public void setInput(Object input) {
        if (this.input != input) {
            this.input = input;
            rebuild();
            revalidate();
            repaint();
        }
    }

    public ITreeContentProvider getContentProvider() {
        return contentProvider;
    }

    public void setContentProvider(ITreeContentProvider contentProvider) {
        if (this.contentProvider != contentProvider) {
            this.contentProvider = contentProvider;
            rebuild();
            revalidate();
            repaint();
        }
    }

    public IStyledLabelProvider getLabelProvider() {
        return labelProvider;
    }

    public void setLabelProvider(IStyledLabelProvider labelProvider) {
        if (this.labelProvider != labelProvider) {
            this.labelProvider = labelProvider;
            revalidate();
            repaint();
        }
    }

    static TreeFigureTheme getTheme() {
        if (theme == null)
            theme = new TreeFigureTheme();
        return theme;
    }

    public void addSelectionListener(SelectionListener listener) { //TODO may need to replace SWT listener+event with own (draw2d) one
        selectionListeners.add(listener);
    }

    public void removeSelectionListener(SelectionListener listener) {
        selectionListeners.remove(listener);
    }

    protected void rebuild() {
        contentsPane.removeAll();
        if (contentProvider == null || labelProvider == null || input == null)
            return;

        Object roots[] = contentProvider.getElements(input);
        for (Object root : roots)
            addTreeItems(root, 0);
    }

    protected void addTreeItems(Object node, int level) {
        boolean hasChildren = contentProvider.hasChildren(node);
        TreeItemFigure item = addTreeItem(node, level, hasChildren);
        if (hasChildren && item.isExpanded()) {
            Object[] children = contentProvider.getChildren(node);
            for (Object child : children)
                addTreeItems(child, level+1);
        }
    }

    protected TreeItemFigure addTreeItem(Object node, int level, boolean hasChildren) {
        return addTreeItem(node, level, hasChildren, -1);
    }

    protected TreeItemFigure addTreeItem(Object node, int level, boolean hasChildren, int index) {
        TreeItemFigure item = createTreeItem(node, level, hasChildren);
        contentsPane.add(item, index);
        contentsPane.setConstraint(item, new GridData(GridData.FILL, GridData.CENTER, true, false));
        item.addMouseListener(itemMouseClickListener);
        if (item.getToggle() != null)
            item.getToggle().addMouseListener(toggleMouseListener);
        return item;
    }

    protected TreeItemFigure createTreeItem(Object node, int level, boolean hasChildren) {
        Image image = labelProvider.getImage(node);
        StyledString label = labelProvider.getStyledText(node);
        TreeItemFigure item = new TreeItemFigure(this, node, level, hasChildren);
        item.setImage(image);
        item.setLabel(label);
        return item;
    }

    protected void handleItemRemoved(IFigure item) {
        if (item instanceof TreeItemFigure) {
            item.removeMouseListener(itemMouseClickListener);
            IFigure toggle = ((TreeItemFigure)item).getToggle();
            if (toggle != null)
                toggle.removeMouseListener(toggleMouseListener);
            if (item == selectionAnchorItem)
                selectionAnchorItem = null;
            if (item == activeItem)
                activeItem = null;
        }
    }

    @SuppressWarnings("unchecked")
    protected List<IFigure> getItems() {
        return contentsPane.getChildren();
    }

    protected int getItemIndex(TreeItemFigure item) {
        return getItems().indexOf(item);
    }

    protected TreeItemFigure getNextItem(TreeItemFigure item) {
        List<IFigure> items = getItems();
        int index = items.indexOf(item);
        return index < items.size() - 1 ? (TreeItemFigure)items.get(index+1) : null;
    }

    protected TreeItemFigure getPreviousItem(TreeItemFigure item) {
        List<IFigure> items = getItems();
        int index = items.indexOf(item);
        return index > 0 ? (TreeItemFigure)items.get(index-1) : null;
    }

    protected void collapseItem(TreeItemFigure item, boolean refresh) {
        Assert.isTrue(item.getParent() == contentsPane && !item.isExpanded());
        int level = item.getLevel();
        List<IFigure> children = getItems();
        List<IFigure> descendants = new ArrayList<IFigure>();
        for (int i = children.indexOf(item) + 1; i < children.size(); ++i) {
            TreeItemFigure childItem = (TreeItemFigure)children.get(i);
            if (childItem.getLevel() > level)
                descendants.add(childItem);
            else
                break;
        }

        for (IFigure descendant : descendants)
            contentsPane.remove(descendant);

        if (refresh) {
            revalidate();
            repaint();
        }
    }

    protected void expandItem(TreeItemFigure item, boolean refresh) {
        Assert.isTrue(item.getParent() == contentsPane && item.isExpanded());
        Object content = item.getContent();
        int level = item.getLevel();
        Object[] children = contentProvider.getChildren(content);
        int index = getItems().indexOf(item);
        for (Object child : children) {
            boolean hasChildren = contentProvider.hasChildren(child);
            addTreeItem(child, level+1, hasChildren, ++index);
        }
        if (refresh) {
            revalidate();
            repaint();
        }
    }

    protected void toggleSelection(TreeItemFigure item) {
        selectionAnchorItem = item;
        item.setSelected(!item.isSelected());
        fireSelectionChanged();
    }

    protected void extendSelection(TreeItemFigure item) {
        if (selectionAnchorItem != null) {
            List<IFigure> items = getItems();
            int itemIndex = items.indexOf(item);
            int lastSelectedIndex = items.indexOf(selectionAnchorItem);
            if (itemIndex >= 0 && lastSelectedIndex >= 0) { // TODO assert?
                int start = Math.min(itemIndex, lastSelectedIndex);
                int end = Math.max(itemIndex, lastSelectedIndex);
                for (int i = 0; i < items.size(); ++i) {
                    TreeItemFigure ithItem = (TreeItemFigure)items.get(i);
                    ithItem.setSelected(start <= i && i <= end);
                }
                fireSelectionChanged();
            }
        }
        else {
            setSelection(item);
        }
    }

    protected void setSelection(TreeItemFigure item) {
        selectionAnchorItem = item;
        boolean changed = false;
        for (IFigure child : getItems()) {
            TreeItemFigure childItem = (TreeItemFigure)child;
            boolean selected = childItem == item;
            if (childItem.isSelected() != selected) {
                childItem.setSelected(selected);
                changed = true;
            }
        }
        if (changed)
            fireSelectionChanged();
    }

    protected void collapseCurrentItem() {
        if (activeItem != null && activeItem.isExpandable() && activeItem.isExpanded()) {
            activeItem.setExpanded(false);
            collapseItem(activeItem, true);
        }
    }

    protected void expandCurrentItem() {
        if (activeItem != null && activeItem.isExpandable() && !activeItem.isExpanded()) {
            activeItem.setExpanded(true);
            expandItem(activeItem, true);
        }
    }

    protected void toggleCurrentItem() {
        if (activeItem != null && activeItem.isExpandable()) {
            if (!activeItem.isExpanded())
                expandCurrentItem();
            else
                collapseCurrentItem();
        }
    }

    protected void showItem(TreeItemFigure item) {
        Point itemPos = item.getLocation();
        int itemHeight = item.getSize().height;
        Point scrollPos = getViewport().getViewLocation();
        int viewportHeight = getViewport().getClientArea(new Rectangle()).getSize().height;

        if (itemPos.y < scrollPos.y) {
            scrollVerticalTo(itemPos.y);
            repaint();
        }
        else if (itemPos.y + itemHeight > scrollPos.y + viewportHeight) {
            scrollVerticalTo(itemPos.y + itemHeight - viewportHeight);
            repaint();
        }
    }

    protected void handleMouseReleasedOnToggle(MouseEvent event) {
        if (event.getSource() instanceof ToggleFigure && event.button == 1) {
            TreeItemFigure item = (TreeItemFigure)((ToggleFigure)event.getSource()).getParent();
            if (item.isExpanded())
                expandItem(item, true);
            else
                collapseItem(item, true);
        }
    }

    protected void handleMousePressedOnItem(MouseEvent event) {
        requestFocus();
        if (event.getSource() instanceof TreeItemFigure) {
            TreeItemFigure item = (TreeItemFigure)event.getSource();
            int state = event.getState();
            if ((state & SWT.CTRL) != 0) {
                toggleSelection(item);
            }
            else if ((state & SWT.SHIFT) != 0) {
                extendSelection(item);
            }
            else if ((state & SWT.ALT) == 0) {
                setSelection(item);
            }
            else
                return;

            activeItem = item;
            fireWidgetSelected(item);
        }
    }

    // XXX should check button state and fire event for each selected item?
    protected void handleMouseDoubleClickedOnItem(MouseEvent event) {
        if (event.getSource() == activeItem)
            fireWidgetDefaultSelected(activeItem);
    }

    protected void handleKeyPressedEvent(KeyEvent event) {
        int key = event.keycode;
        int state = event.getState();
        boolean shift = (state & SWT.SHIFT) != 0;
        boolean ctrl = (state & SWT.CTRL) != 0;
        switch (key) {
        case SWT.ARROW_LEFT:
            collapseCurrentItem();  // Linux (both Gnome and KDE) require Shift for expand/collapse, but neither Windows not OS X does -- there's no harm in ignoring the Shift state
            break;
        case SWT.ARROW_RIGHT:
            expandCurrentItem();
            break;
        case SWT.ARROW_DOWN:
            handleKeyDown(shift, ctrl);
            break;
        case SWT.ARROW_UP:
            handleKeyUp(shift, ctrl);
            break;
        case SWT.SPACE:
            toggleCurrentItem();  // seems to be Linux only, but there's not harm doing it on all platforms
            fireWidgetSelected(activeItem);
            break;
        case SWT.CR:
            fireWidgetDefaultSelected(activeItem);
            break;
        }
    }

    protected void handleKeyUp(boolean shift, boolean ctrl) {
        if (activeItem == null) {
            List<IFigure> items = getItems();
            if (!items.isEmpty()) {
                activeItem = (TreeItemFigure)items.get(0);
                setSelection(activeItem);
            }
        }
        else if (shift) {
            TreeItemFigure prevItem = getPreviousItem(activeItem);
            if (prevItem != null) {
                activeItem = prevItem;
                extendSelection(prevItem);
                showItem(prevItem);
            }
        }
        else {
            TreeItemFigure prevItem = getPreviousItem(activeItem);
            if (prevItem != null) {
                activeItem = prevItem;
                setSelection(prevItem);
                showItem(prevItem);
            }
        }
    }

    protected void handleKeyDown(boolean shift, boolean ctrl) {
        if (activeItem == null) {
            List<IFigure> items = getItems();
            if (!items.isEmpty()) {
                activeItem = (TreeItemFigure)items.get(0);
                setSelection(activeItem);
            }
        }
        else if (shift) {
            TreeItemFigure nextItem = getNextItem(activeItem);
            if (nextItem != null) {
                activeItem = nextItem;
                extendSelection(nextItem);
                showItem(nextItem);
            }
        }
        else {
            TreeItemFigure nextItem = getNextItem(activeItem);
            if (nextItem != null) {
                activeItem = nextItem;
                setSelection(nextItem);
                showItem(nextItem);
            }
        }
    }

    protected void handleFocusGainedEvent(FocusEvent event) {
        activeItem = selectionAnchorItem;
        repaint();
    }

    protected void handleFocusLostEvent(FocusEvent event) {
        activeItem = null;
        repaint();
    }

    protected void fireWidgetSelected(TreeItemFigure item) {
        for (Object listener : selectionListeners.getListeners()) {
            try {
                ((SelectionListener)listener).widgetSelected(null /*FIXME*/);
            } catch (Exception e) {
                SimulationPlugin.logError(e);
            }
        }
    }

    protected void fireWidgetDefaultSelected(TreeItemFigure item) {
        for (Object listener : selectionListeners.getListeners()) {
            try {
                ((SelectionListener)listener).widgetDefaultSelected(null /*FIXME*/);
            } catch (Exception e) {
                SimulationPlugin.logError(e);
            }
        }
    }

    @Override
    public ISelection getSelection() {
        List<Object> selectedObjects = new ArrayList<Object>();
        for (IFigure child : getItems()) {
            TreeItemFigure item = (TreeItemFigure)child;
            if (item.isSelected())
                selectedObjects.add(item.getContent());
        }
        return new StructuredSelection(selectedObjects);
    }

    @Override
    public void setSelection(ISelection selection) {
        setSelection(selection, true);
    }

    // XXX if the same content appears several times in the tree, only the first one will be selected
    public void setSelection(ISelection selection, boolean refresh) {
        if (selection instanceof IStructuredSelection) {
            // clear selection
            for (IFigure child : getItems())
                ((TreeItemFigure)child).setSelected(false);
            // set selection of items
            boolean selectionChanged = false;
            List<?> selectedObjects = ((IStructuredSelection)selection).toList();
            for (Object object : selectedObjects) {
                TreeItemFigure item = findOrCreateItem(object);
                if (item != null && !item.isSelected()) {
                    item.setSelected(true);
                    selectionChanged = true;
                }
            }
            if (selectionChanged)
                fireSelectionChanged();
            if (refresh) {
                revalidate();
                repaint();
            }
        }
    }

    private TreeItemFigure findOrCreateItem(Object content) {
        List<Object> ancestors = new ArrayList<Object>();
        TreeItemFigure item = null;
        // find an ancestor whose item exists
        while (content != null) {
            item = findItem(content);
            if (item != null)
                break;
            ancestors.add(content);
            content = contentProvider.getParent(content);
        }
        // content not in this tree?
        if (item == null)
            return null;
        // create ancestors below item
        for (int i = ancestors.size() - 1; i >= 0; i--) {
            Object ancestor = ancestors.get(i);
            Assert.isTrue(item.isExpandable() && !item.isExpanded());
            item.setExpanded(true);
            expandItem(item, false);
            item = findItem(ancestor);
            if (item == null)
                break;
        }
        return item;
    }

    private TreeItemFigure findItem(Object content) {
        for (IFigure child : getItems()) {
            TreeItemFigure item = (TreeItemFigure)child;
            if (content.equals(item.getContent()))
                return item;
        }
        return null;
    }

    @Override
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    @Override
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    protected void fireSelectionChanged() {
        SelectionChangedEvent event = new SelectionChangedEvent(this, getSelection());
        for (Object listener : selectionChangedListeners.getListeners()) {
            try {
                ((ISelectionChangedListener)listener).selectionChanged(event);
            } catch (Exception e) {
                SimulationPlugin.logError(e);
            }
        }
    }
}
