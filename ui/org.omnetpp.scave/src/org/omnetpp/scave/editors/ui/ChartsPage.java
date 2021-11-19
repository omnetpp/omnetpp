/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.IMemento;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.common.ui.IconGridViewer.ViewMode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorActions;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.SetChartNameCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Scave page which displays charts
 * @author Andras
 */
public class ChartsPage extends FormEditorPage {
    private IconGridViewer viewer;
    private Link pathLabel;
    protected CommandStack commandStack = new CommandStack("ChartsPage");


    /**
     * Constructor
     */
    public ChartsPage(Composite parent, ScaveEditor scaveEditor) {
        super(parent, SWT.NONE, scaveEditor);
        initialize();
    }

    private void initialize() {
        // set up UI
        setPageTitle("Charts");
        setFormTitle("Charts");
        getContent().setLayout(new GridLayout(2, false));

        pathLabel = new Link(getContent(), SWT.WRAP);
        pathLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false, 2, 1));

        pathLabel.addSelectionListener(SelectionListener.widgetSelectedAdapter(e -> {
            int i = Integer.parseInt(e.text); // e.text is the link in <a href>, contains in index into the path
            Folder folder = getCurrentFolder().getPathSegments().get(i);
            setCurrentFolder(folder);
        }));


        viewer = new IconGridViewer(getContent());
        viewer.getControl().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        viewer.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));
        setViewMode(ViewMode.ICONS);

        ScaveEditorActions actions = scaveEditor.getActions();

        addToToolbar(actions.newFolderAction);
        addToToolbar(actions.newChartFromTemplateGalleryAction);
        addToToolbar(actions.editAction);
        addToToolbar(actions.removeAction);

        addSeparatorToToolbar();

        addToToolbar(actions.viewIconsAction);
        addToToolbar(actions.viewMulticolumnListAction);
        addToToolbar(actions.viewListAction);
        addToToolbar(actions.setItemWidthAction);

        addSeparatorToToolbar();

        addToToolbar(actions.openAction);

        viewer.setFocus();

        // configure viewers
        configureViewer();

        // set contents
        setCurrentFolder(scaveEditor.getAnalysis().getRootFolder());

        // temporarily overwrite displayed path with a help text -- this will go away (be overwritten) once user starts to navigate across folders
        pathLabel.setText("Here you can edit chart definitions.");

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));
    }

    public IconGridViewer getViewer() {
        return viewer;
    }

    public Folder getCurrentFolder() {
        return (Folder)viewer.getInput();
    }

    public void setCurrentFolder(Folder folder) {
        if (folder != getCurrentFolder()) {
            viewer.setInput(folder);

            // display folder path, hyperlinking each folder
            String pathWithLinks = "";
            List<Folder> pathSegments = folder.getPathSegments();
            for (int i = 0; i < pathSegments.size(); i++) {
                Folder f = pathSegments.get(i);
                pathWithLinks += "<a href=\"" + i + "\">" + (i == 0 ? "Charts" : f.getName()) + "</a>" + " / ";
            }
            pathLabel.setText(pathWithLinks);
        }
    }

    public void setViewMode(ViewMode viewMode) {
        viewer.setViewMode(viewMode);
        switch (viewMode) { // note: it'd be prettier (and not leaky abstraction) if we turned these settings into some profile in IconGridViewer
        case ICONS:
            viewer.setIconHeight(64);
            viewer.setMargin(20);
            viewer.setSpacing(20, 20);
            break;
        case MULTICOLUMN_LIST:
            viewer.setIconHeight(64);
            viewer.setMargin(20);
            viewer.setSpacing(20, 20);
            break;
        case LIST:
            viewer.setIconHeight(16);
            viewer.setMargin(6);
            viewer.setSpacing(6, 6);
            break;
        }
    }

    public CommandStack getCommandStack() {
        return commandStack;
    }

    @Override
    public boolean gotoObject(Object object) {
        if (object instanceof ModelObject) {
            Folder containingFolder = ScaveModelUtil.findEnclosing((ModelObject)object, Folder.class);
            if (containingFolder != null) {
                setCurrentFolder(containingFolder);
                viewer.setSelection(new StructuredSelection(object), true);
                return true;
            }
        }
        return false;
    }

    public void modelChanged(ModelChangeEvent event) {
        viewer.refresh();
    }

    protected void configureViewer() {

        ILabelProvider labelProvider = new LabelProvider() {
            @Override
            public Image getImage(Object element) {
                if (element instanceof Chart) {
                    String iconPath = ((Chart)element).getIconPath();
                    if (iconPath == null || iconPath.isEmpty())
                        iconPath = ScaveImages.IMG_OBJ_CHART;
                    return ScavePlugin.getCachedImage(iconPath);
                }
                else if (element instanceof Folder)
                    return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ_FOLDER);
                else
                    return null;
            }

            @Override
            public String getText(Object element) {
                if (element instanceof AnalysisItem)
                    return StringUtils.defaultIfBlank(((AnalysisItem) element).getName(), "<unnamed>");
                return element == null ? "" : element.toString();
            }
        };

        viewer.setContentProvider(new IStructuredContentProvider() {
            @Override
            public Object[] getElements(Object inputElement) {
                if (inputElement instanceof Folder) {
                    Folder charts = (Folder)inputElement;
                    return charts.getItems().toArray();
                }
                return new Object[0];
            }
        });
        viewer.setLabelProvider(labelProvider);

        viewer.addDropListener(new IconGridViewer.IDropListener() {
            @Override
            public void drop(Object[] draggedElements, Point p) {
                Object insertionPoint = viewer.getElementAtOrAfter(p.x, p.y);
                if (insertionPoint == null ||!ArrayUtils.contains(draggedElements, insertionPoint)) {
                    Folder container = getCurrentFolder();
                    List<AnalysisItem> charts = container.getItems();
                    int index = insertionPoint == null ? charts.size() - 1 : charts.indexOf(insertionPoint);
                    ScaveModelUtil.moveElements(commandStack, container, draggedElements, index);
                    viewer.refresh();
                }

            }
        });

        viewer.setRenameAdapter(new IconGridViewer.IRenameAdapter() {
            @Override
            public boolean isRenameable(Object element) {
                return (element instanceof AnalysisItem);
            }

            @Override
            public boolean isNameValid(Object element, String name) {
                return true;
            }

            @Override
            public String getName(Object element) {
                return StringUtils.nullToEmpty(((AnalysisItem) element).getName());
            }

            @Override
            public void setName(Object element, String name) {
                commandStack.execute(new SetChartNameCommand((AnalysisItem)element, name));
            }
        });

        viewer.addSelectionChangedListener(scaveEditor.getSelectionChangedListener());

        IMenuListener menuListener = new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager menuManager) {
                boolean emptyAreaClicked = viewer.getSelection().isEmpty();
                scaveEditor.getActions().populateContextMenu(menuManager, emptyAreaClicked);
            }
        };

        UIUtils.createContextMenuFor(viewer.getCanvas(), true, menuListener);

        viewer.addDoubleClickListener(new IDoubleClickListener() {
            @Override
            public void doubleClick(DoubleClickEvent event) {
                ScaveEditorActions actions = scaveEditor.getActions();

                IStructuredSelection sel = viewer.getSelection();
                if (sel.getFirstElement() instanceof Folder)
                    setCurrentFolder((Folder)sel.getFirstElement());
                else if (actions.openAction.isEnabled())
                    actions.openAction.run();
                else if (actions.editAction.isEnabled())
                    actions.editAction.run();
            }
        });

        viewer.addKeyListener(KeyListener.keyPressedAdapter( ke -> {
            final int BACKSPACE = 8;
            if (ke.keyCode == BACKSPACE) {
                Folder parentFolder = getCurrentFolder().getParentFolder();
                if (parentFolder != null)
                    setCurrentFolder(parentFolder);
            }
        }));

    }

    @Override
    public void pageActivated() {
        super.pageActivated();
        scaveEditor.setSelection(viewer.getSelection());
    }

    @Override
    public void saveState(IMemento memento) {
        super.saveState(memento);
        memento.putInteger("currentFolderId", getCurrentFolder().getId());
        memento.putString("viewMode", viewer.getViewMode().name());
        memento.putInteger("numItemsPerRow", viewer.getNumItemsPerRow());
        memento.putInteger("itemWidth", viewer.getItemWidth());
        memento.putInteger("numColumns", viewer.getNumColumns());
        memento.putInteger("columnWidth", viewer.getColumnWidth());
    }

    @Override
    public void restoreState(IMemento memento) {
        Integer folderId = memento.getInteger("currentFolderId");
        if (folderId != null) {
            AnalysisItem item = scaveEditor.getAnalysis().getRootFolder().findRecursivelyById(folderId);
            if (item instanceof Folder)
                setCurrentFolder((Folder)item);
        }

        String mode = memento.getString("viewMode");
        if (mode != null)
            setViewMode(IconGridViewer.ViewMode.valueOf(mode));

        Integer value;
        if ((value = memento.getInteger("numItemsPerRow")) != null)
            viewer.setNumItemsPerRow(value);
        if ((value = memento.getInteger("itemWidth")) != null)
            viewer.setItemWidth(value);
        if ((value = memento.getInteger("numColumns")) != null)
            viewer.setNumColumns(value);
        if ((value = memento.getInteger("columnWidth")) != null)
            viewer.setColumnWidth(value);
    }

}


