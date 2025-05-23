/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_ABSTRACT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.ui.ActionContributionItem2;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.common.ui.ViewerDragAdapter;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.actions.AddInifileKeysAction;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.Timeout;

/**
 * Inifile editor page to manage the sections in the file.
 *
 * @author Andras
 */
public class SectionsPage extends FormPage {
    private static final String HINT_TEXT = "\nHINT: Drag sections to edit the hierarchy. Hierarchy defines the fallback order for lookups.";

    private Label label;
    private TreeViewer treeViewer;

    private IAction addAction = new Action("New...") {
        public void run() {
            createNewSection();
        }
    };
    private IAction editAction = new Action("Edit...") {
        public void run() {
            editSelectedSection();
        }
    };
    private IAction removeAction = new Action("Remove") {
        public void run() {
            removeSelectedSections();
        }
    };
    private IAction gotoParametersAction = new Action("Go To Parameters") {
        public void run() {
            gotoSectionParameters();
        }
    };
    private IAction addKeysAction = new AddInifileKeysAction();

    private IAction toggleAnalysisAction;

    private static class SectionData {
        String sectionName;
        int maxProblemSeverity;
        boolean isNonexistent;

        public SectionData(String sectionName, int maxProblemSeverity, boolean isNonexistent) {
            this.sectionName = sectionName;
            this.maxProblemSeverity = maxProblemSeverity;
            this.isNonexistent = isNonexistent;
        }
    }

    public SectionsPage(Composite parent, InifileEditor inifileEditor) {
        super(parent, inifileEditor);

        toggleAnalysisAction = inifileEditor.getToggleAnalysisAction();

        // layout: 2x2 grid: (label, dummy) / (tree, buttonGroup)
        setLayout(new GridLayout(2,false));
        ((GridLayout)getLayout()).marginRight = RIGHT_MARGIN;

        // create title
        Control titleArea = createTitleArea(this, "Sections");
        ((GridData)titleArea.getLayoutData()).horizontalSpan = 2;

        // create section chain label
        label = new Label(this, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        ((GridData)label.getLayoutData()).horizontalSpan = 2;
        label.setText(HINT_TEXT);

        // create treeviewer and buttons
        treeViewer = createAndConfigureTreeViewer();
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        Composite buttonGroup = createButtons();
        buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));

        reread();
    }

    private TreeViewer createAndConfigureTreeViewer() {
        final TreeViewer treeViewer = new TreeViewer(this, SWT.MULTI | SWT.BORDER);
        treeViewer.setLabelProvider(new GenericTreeLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                String section = getSectionNameFromTreeNode(element);
                IInifileDocument doc = getInifileDocument();
                String networkName = doc.getValue(section, CFGID_NETWORK.getName());
                boolean isAbstract = "true".equals(doc.getValue(section, CFGID_ABSTRACT.getName()));
                String description = doc.getValue(section, CFGID_DESCRIPTION.getName());
                String text = section;
                if (isAbstract)
                    text += " (abstract)";
                if (networkName != null)
                    text += " (network: " + networkName+")";
                if (description != null)
                    text += " -- " + description;
                return text;
            }
            @Override
            public Image getImage(Object element) {
                if (element instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                if (element instanceof SectionData) {
                    SectionData payload = (SectionData) element;
                    return InifileUtils.getSectionImage(payload.sectionName, getInifileAnalyzer());
                }
                return null;
            }
        }));
        treeViewer.setContentProvider(new GenericTreeContentProvider());

        // drag and drop support
        setupDragAndDropSupport(treeViewer);

        // on double-click, show section in the parameters page
        treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetDefaultSelected(SelectionEvent event) {
                gotoParametersAction.run();
            }
        });

        // set up hotkey support
        treeViewer.getTree().addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                if (e.character == SWT.DEL)
                    removeSelectedSections();
                if (e.keyCode == 'A' && (e.stateMask & SWT.MOD1) != 0)
                    treeViewer.getTree().selectAll(); //XXX this does not work, because text editor hotkey masks it
            }
        });

        // export the tree's selection as editor selection
        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateActions();
                IStructuredSelection sel = (IStructuredSelection) event.getSelection();
                String section = getSectionNameFromTreeNode(sel.getFirstElement());
                if (section != null)
                    setEditorSelection(section, null);
            }
        });

        // add tooltip support
        addTooltipSupport(treeViewer.getTree(), new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Item item = treeViewer.getTree().getItem(new Point(x,y));
                String section = getSectionNameFromTreeNode(item==null ? null : item.getData());
                return section==null ? null : new HtmlHoverInfo(InifileHoverUtils.getSectionHoverText(section, getInifileDocument(), getInifileAnalyzer(), true));
            }
        });

        // add context menu
        MenuManager menuManager = new MenuManager();
        menuManager.add(addAction);
        menuManager.add(editAction);
        menuManager.add(removeAction);
        menuManager.add(new Separator());
        menuManager.add(gotoParametersAction);
        menuManager.add(addKeysAction);
        menuManager.add(toggleAnalysisAction);
        treeViewer.getTree().setMenu(menuManager.createContextMenu(treeViewer.getTree()));

        return treeViewer;
    }

    protected void updateActions() {
        ISelection selection = treeViewer.getSelection();
        editAction.setEnabled(!selection.isEmpty());
        removeAction.setEnabled(!selection.isEmpty());
        gotoParametersAction.setEnabled(!selection.isEmpty());
        addKeysAction.setEnabled(!selection.isEmpty());
    }

    private void setupDragAndDropSupport(TreeViewer viewer) {
        int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
        Transfer[] transfers = new Transfer[] { LocalTransfer.getInstance() };
        viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
        viewer.addDropSupport(dndOperations, transfers, new DropTargetAdapter() {
            public void drop(DropTargetEvent event) {
                // note: in theory, the user can drag across different treeViewers
                // (i.e. from a different inifile editor, or even from a Scave editor!),
                // so we have to be careful when looking at the dragged data
                String[] draggedSections = getSectionNamesFromTreeSelection((IStructuredSelection) event.data);
                String targetSectionName = getSectionNameFromTreeNode(event.item==null ? null : event.item.getData());

                if (draggedSections.length != 0 && targetSectionName != null) {
                    sectionsDragged(draggedSections, targetSectionName);
                }
            }
        });
    }

    protected static String[] getSectionNamesFromTreeSelection(ISelection selection) {
        List<String> list = new ArrayList<String>();
        if (selection instanceof IStructuredSelection) {
            for (Object item : ((IStructuredSelection)selection).toArray()) {
                String payload = getSectionNameFromTreeNode(item);
                if (payload != null)
                    list.add(payload);
            }
        }
        return list.toArray(new String[]{});
    }

    protected static String getSectionNameFromTreeNode(Object data) {
        if (data instanceof GenericTreeNode)
            data = ((GenericTreeNode)data).getPayload();
        if (data instanceof SectionData)
            return ((SectionData) data).sectionName;
        return null;
    }

    protected void setSectionExtendsKey(String sectionName, String extendsSectionName) {
        IInifileDocument doc = getInifileDocument();
        if (extendsSectionName.equals(GENERAL)) {
            if (doc.containsKey(sectionName, EXTENDS))
                doc.removeKey(sectionName, EXTENDS);
        }
        else {
            String value = InifileUtils.removeSectionNamePrefix(extendsSectionName);
            if (doc.containsKey(sectionName, EXTENDS))
                doc.setRawValue(sectionName, EXTENDS, value);
            else
                InifileUtils.addEntry(doc, sectionName, EXTENDS, value, "");
        }
    }

    protected Composite createButtons() {
        Composite buttonGroup = new Composite(this, SWT.NONE);
        buttonGroup.setLayout(new GridLayout(1,false));
        addActionButton(buttonGroup, addAction);
        addActionButton(buttonGroup, editAction);
        addActionButton(buttonGroup, removeAction);
        return buttonGroup;
    }

    protected static void addActionButton(Composite buttonGroup, IAction action) {
        Button button = new ActionContributionItem2(action, ActionContributionItem.MODE_FORCE_TEXT).fill2(buttonGroup);
        button.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
    }

    /**
     * Invoked by clicking on the "New Section" button
     */
    protected void createNewSection() {
        // create and configure dialog
        IInifileDocument doc = getInifileDocument();
        SectionDialog dialog = new SectionDialog(getShell(), "New Section", "Create new section", doc, null);
        dialog.setSectionName("New");
        String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
        if (selection.length != 0)
            dialog.setBaseConfigNames(InifileUtils.sectionNameToConfigName(selection[0]));

        // open the dialog
        if (dialog.open()==Window.OK) {
            try {
                String sectionName = dialog.getSectionName();
                InifileUtils.addSection(doc, sectionName);

                String description = dialog.getDescription();
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, CFGID_DESCRIPTION.getName(), description.equals("") ? null : description);

                boolean isAbstract = dialog.getIsAbstract();
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, CFGID_ABSTRACT.getName(), isAbstract ? "true" : null);

                String baseConfigNames = StringUtils.defaultIfEmpty(dialog.getBaseConfigNames(), null);
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, EXTENDS, baseConfigNames);

                String networkName = dialog.getNetworkName();
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, CFGID_NETWORK.getName(), networkName.equals("") ? null : networkName);

                reread();
            }
            catch (RuntimeException e) {
                showErrorDialog(e);
            }
        }
    }

    /**
     * Invoked by clicking on the "Edit Section" button
     */
    protected void editSelectedSection() {
        // create and configure dialog
        String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
        if (selection.length == 0)
            return;
        String oldSectionName = selection[0];
        IInifileDocument doc = getInifileDocument();
        SectionDialog dialog = new SectionDialog(getShell(), "Rename/Edit Section", "Rename or edit section ["+oldSectionName+"]", doc, oldSectionName);

        // open the dialog
        if (dialog.open()==Window.OK) {
            try {
                String sectionName = dialog.getSectionName();
                if (!sectionName.equals(oldSectionName))
                    InifileUtils.renameSection(doc, oldSectionName, sectionName);

                String description = dialog.getDescription();
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, CFGID_DESCRIPTION.getName(), description.equals("") ? null : description);

                boolean isAbstract = dialog.getIsAbstract();
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, CFGID_ABSTRACT.getName(), isAbstract ? "true" : null);

                String baseConfigNames = StringUtils.defaultIfEmpty(dialog.getBaseConfigNames(), null);
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, EXTENDS, baseConfigNames);

                String networkName = dialog.getNetworkName();
                InifileUtils.addOrSetOrRemoveEntry(doc, sectionName, CFGID_NETWORK.getName(), networkName.equals("") ? null : networkName);

                reread();
            }
            catch (RuntimeException e) {
                showErrorDialog(e);
            }
        }
    }

    /**
     * Invoked by clicking on the "Remove Section" button
     */
    protected void removeSelectedSections() {
        try {
            String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
            if (selection.length != 0) {
                boolean ok = MessageDialog.openQuestion(getShell(), "Delete Sections", "Do you want to delete the selected "+(selection.length==1 ? "section" : selection.length+" sections")+"?");
                if (ok) {
                    for (String sectionName : selection)
                        getInifileDocument().removeSection(sectionName);
                    reread();
                }
            }
        }
        catch (RuntimeException e) {
            showErrorDialog(e);
        }
    }

    /**
     * Goes to the Parameters page of the inifile editor and chooses the selection section.
     * Invoked by double-clicking the tree viewer.
     */
    protected void gotoSectionParameters() {
        String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
        if (selection.length != 0) {
            String section = selection[0];
            InifileFormEditor formEditor = getEditorData().getInifileEditor().getFormEditor();
            formEditor.showCategoryPage(InifileFormEditor.PARAMETERS_PAGE);
            formEditor.getActiveCategoryPage().gotoSection(section);
        }
    }

    /**
     * Invoked when the user selects a few sections, and drags them to another section.
     */
    protected void sectionsDragged(String[] draggedSections, String targetSectionName) {
        try {
            //Debug.println(draggedSections.length + " items dropped to: "+targetSectionName);
            IReadonlyInifileDocument doc = getInifileDocument();
            for (String draggedSectionName : draggedSections)
                if (getInifileDocument().containsSection(draggedSectionName)) // might occur if it was dragged from a different editor's treeviewer...
                    if (!InifileUtils.sectionChainContains(doc, targetSectionName, draggedSectionName)) // avoid cycles
                        setSectionExtendsKey(draggedSectionName, targetSectionName);
            reread();
        }
        catch (RuntimeException e) {
            showErrorDialog(e);
        }
    }

    @Override
    public boolean setFocus() {
        return treeViewer.getTree().setFocus();
    }

    @Override
    public String getPageCategory() {
        return InifileFormEditor.SECTIONS_PAGE;
    }

    @Override
    public void reread() {
        IReadonlyInifileDocument doc = getInifileDocument();
        InifileAnalyzer analyzer = getInifileAnalyzer();

        try {
            analyzer.analyzeIfChanged(new Timeout(1000)); // refresh error markers
        } catch (ParamResolutionTimeoutException e) {
            // ignore?
        }

        // build tree
        HashMap<String,List<GenericTreeNode>> nodes = new HashMap<String, List<GenericTreeNode>>();
        GenericTreeNode rootNode = new GenericTreeNode("root");
        GenericTreeNode generalSectionNode = getOrCreateNode(nodes, GENERAL);
        rootNode.addChild(generalSectionNode);
        for (String sectionName : doc.getSectionNames()) {
            if (!sectionName.equals(GENERAL)) {
                List<String> baseSectionNames = InifileUtils.resolveBaseSectionsPretendingGeneralExists(doc, sectionName);
                if (doc.isCausingCycle(sectionName) || baseSectionNames.isEmpty()) {
                    // bogus section, put it under the root
                    GenericTreeNode childNode = getOrCreateNode(nodes, sectionName);
                    Assert.isTrue(childNode != null && childNode.getParent() == null);
                    rootNode.addChild(childNode);
                }
                else {
                    GenericTreeNode childNode = getOrCreateNode(nodes, sectionName);
                    Assert.isTrue(childNode != null && childNode.getParent() == null);
                    // need to clone child nodes here, to each copy has a different parent
                    List<GenericTreeNode> parentNodes = new ArrayList<GenericTreeNode>();
                    for (String baseSectionName : baseSectionNames)
                        parentNodes.addAll(getOrCreateNodes(nodes, baseSectionName));

                    List<GenericTreeNode> childNodes = getOrCreateNodes(nodes, sectionName, parentNodes.size());

                    for (int i = 0; i < parentNodes.size(); ++i) {
                        parentNodes.get(i).addChild(childNodes.get(i));
                    }
                }
            }
        }

        // reduce flicker: only overwrite existing tree input if it's not the same as this one
        if (treeViewer.getInput()==null || !GenericTreeUtils.treeEquals(rootNode, (GenericTreeNode)treeViewer.getInput())) {
            treeViewer.setInput(rootNode);
            treeViewer.expandAll();
        }

        treeViewer.refresh();  // refresh labels anyway
        updateActions();
    }

    @Override
    public void analysisCompleted() {
    }

    private GenericTreeNode getOrCreateNode(HashMap<String, List<GenericTreeNode>> nodes, String sectionName) {
        List<GenericTreeNode> sectionNodes = getOrCreateNodes(nodes, sectionName);
        Assert.isTrue(sectionNodes.size() == 1);
        return sectionNodes.get(0);
    }

    private List<GenericTreeNode> getOrCreateNodes(HashMap<String, List<GenericTreeNode>> nodes, String sectionName, int count) {
        List<GenericTreeNode> sectionNodes = getOrCreateNodes(nodes, sectionName);
        Assert.isTrue(sectionNodes.size() == 1);
        GenericTreeNode node = sectionNodes.get(0);
        for (int i = 1; i < count; ++i)
            sectionNodes.add(node.cloneTree());
        return sectionNodes;
    }

    private List<GenericTreeNode> getOrCreateNodes(HashMap<String, List<GenericTreeNode>> nodes, String sectionName) {
        List<GenericTreeNode> result = nodes.get(sectionName);
        if (result==null) {
            result = new ArrayList<GenericTreeNode>();
            result.add(createNode(sectionName));
            nodes.put(sectionName, result);
        }
        return result;
    }

    private GenericTreeNode createNode(String sectionName) {
        IReadonlyInifileDocument doc = getInifileDocument();
        if (doc.containsSection(sectionName)) {
            IMarker[] markers = InifileUtils.getProblemMarkersForWholeSection(sectionName, doc);
            int maxProblemSeverity = InifileUtils.getMaximumSeverity(markers);
            return new GenericTreeNode(new SectionData(sectionName, maxProblemSeverity, false));
        }
        else
            return new GenericTreeNode(new SectionData(sectionName, -1, true));
    }

    @Override
    public void gotoSection(final String section) {
        // we must find the node in the tree manually. equals() cannot be used, because it
        // compares the maxNumErrors and isNonexistent fields as well (this is required for
        // treeEquals() in refresh())
        GenericTreeNode root = (GenericTreeNode)treeViewer.getInput();
        Object sectionData = GenericTreeUtils.findFirstMatchingNode(root, new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof SectionData && ((SectionData)object).sectionName.equals(section);
            }
        });

        if (sectionData!=null)
            treeViewer.setSelection(new StructuredSelection(new GenericTreeNode(sectionData)), true);
        else
            treeViewer.setSelection(new StructuredSelection(), true);
    }

    @Override
    public void gotoEntry(String section, String key) {
        gotoSection(section);
    }

    @Override
    public List<ConfigOption> getSupportedKeys() {
        List<ConfigOption> result = new ArrayList<ConfigOption>();
        result.add(ConfigRegistry.CFGID_EXTENDS);
        result.add(ConfigRegistry.CFGID_DESCRIPTION);
        result.add(ConfigRegistry.CFGID_NETWORK);
        return result;
    }

}
