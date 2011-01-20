/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.dialogs;

import static org.omnetpp.ned.model.interfaces.INedTypeResolver.CHANNELINTERFACE_FILTER;
import static org.omnetpp.ned.model.interfaces.INedTypeResolver.CHANNEL_FILTER;
import static org.omnetpp.ned.model.interfaces.INedTypeResolver.MODULEINTERFACE_FILTER;
import static org.omnetpp.ned.model.interfaces.INedTypeResolver.MODULE_EXCEPT_NETWORK_FILTER;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.FreeformLayer;
import org.eclipse.draw2d.FreeformLayeredPane;
import org.eclipse.draw2d.FreeformViewport;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.PuristicScrollPane;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TrayDialog;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.common.ui.TristateButton;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.graph.commands.ChangeDisplayPropertyCommand;
import org.omnetpp.ned.editor.graph.commands.RenameCommand;
import org.omnetpp.ned.editor.graph.commands.SetAttributeCommand;
import org.omnetpp.ned.editor.graph.commands.SetDocumentationCommand;
import org.omnetpp.ned.editor.graph.commands.SetInheritanceCommand;
import org.omnetpp.ned.editor.graph.commands.SetTypeOrLikeTypeCommand;
import org.omnetpp.ned.editor.graph.properties.util.SubmoduleNameValidator;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.IModuleKindTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;


/**
 * Dialog for editing (mostly graphical) properties of some item or items.
 *
 * @author andras
 */
//TODO type selection dialog does not show inner types
//TODO inner type-ot is beimportalja!!!!
//TODO shape editing ("" vs "rect") is not entirely correct... e.g. preview does not necessarily agree with final result...
//TODO warn for invalid colors, etc
//TODO factor out error/warning icons to UIUtils!!!!

//TODO words used in the Properties View and in the dialog should be consistent!! ("colorization" vs "tint", etc)
//TODO connection text position enum names are wrong ("left", "right", "top")
//TODO property nevekre nincs content assist! (@disp <ctrl+Space>  --> semmi)

public class PropertiesDialog extends TrayDialog {
    protected static final Image ICON_ERROR = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_ERROR);
    protected static final Image ICON_WARNING = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_WARN_TSK); //ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING);
    public static final Image ICON_ERROR_SMALL = NedEditorPlugin.getCachedImage("icons/ovr16/error.gif");
    public static final Image ICON_WARNING_SMALL = NedEditorPlugin.getCachedImage("icons/ovr16/warning.gif");

    private static final String STR_PARENTMODULE = "<parent>";  // for connection src/dest module selection combo
    private static final String STR_GATEPLUSPLUS = "++";    // for connection gate index selection combo

    private INedElement[] elements;
    private Command resultCommand;
    
    private boolean initializing = true;  // while dialog is being set up and created

    private TabFolder tabfolder;
    private Composite previewArea;

    private Composite positionPage;
    private Composite appearancePage;
    private Composite backgroundPage;
    private Button fixedTypeRadioButton;
    private Button parametricTypeRadioButton;

    private Composite previewComposite;
    private IFigure previewFigure[];  // actually SubmoduleFigure, ConnectionFigure, etc; max 10 figures
    private ScrollPane previewScrollPane;

    private Composite bgPreviewComposite;
    private CompoundModuleFigure bgPreviewFigure;
    private ScrollPane bgPreviewScrollPane;

    private Label errorImageLabel;
    private Label errorMessageLabel;

    // General page
    private TextFieldEditor nameField;
    private NedTypeFieldEditor extendsField;
    private NedTypeFieldEditor interfacesField;
    private NedTypeFieldEditor typeField;
    private NedTypeFieldEditor likeTypeField;
    private TextFieldEditor likeParamField;
    private TextFieldEditor vectorSizeField;
    private TristateButton isBidirectionalField;
    private TextFieldEditor documentationField;

    // Endpoints page (for connection)
    private ComboFieldEditor connSrcModuleField;
    private TextFieldEditor connSrcModuleIndexField;
    private ComboFieldEditor connSrcGateField;
    private TextFieldEditor connSrcGateIndexField;
    private ComboFieldEditor connDestModuleField;
    private TextFieldEditor connDestModuleIndexField;
    private ComboFieldEditor connDestGateField;
    private TextFieldEditor connDestGateIndexField;
    private boolean modifyInProgress;

    // Graphical properties
    private TextFieldEditor xField;
    private TextFieldEditor yField;
    private ComboFieldEditor layoutField;
    private Label layoutPar1Label;
    private Label layoutPar2Label;
    private Label layoutPar3Label;
    private TextFieldEditor layoutPar1Field;
    private TextFieldEditor layoutPar2Field;
    private TextFieldEditor layoutPar3Field;
    private TextFieldEditor shapeWidthField;
    private TextFieldEditor shapeHeightField;
    private ComboFieldEditor shapeField;
    private ColorFieldEditor shapeFillColorField;
    private ColorFieldEditor shapeBorderColorField;
    private SpinnerFieldEditor shapeBorderWidthField;
    private ImageFieldEditor imageField;
    private ColorFieldEditor imageColorField;
    private SpinnerFieldEditor imageColorPercentageField;
    private ComboFieldEditor imageSizeField;
    private ImageFieldEditor image2Field;
    private ColorFieldEditor image2ColorField;
    private SpinnerFieldEditor image2ColorPercentageField;
    private TextFieldEditor rangeField;
    private ColorFieldEditor rangeFillColorField;
    private ColorFieldEditor rangeBorderColorField;
    private SpinnerFieldEditor rangeBorderWidthField;
    private TextFieldEditor queueNameField;
    private TextFieldEditor textField;
    private ComboFieldEditor textPosField;
    private ColorFieldEditor textColorField;
    private TextFieldEditor tooltipField;
    private ColorFieldEditor connectionColorField;
    private SpinnerFieldEditor connectionWidthField;
    private ComboFieldEditor connectionStyleField;

    private TextFieldEditor bgScaleField;
    private Label bgScaleLabel;
    private TextFieldEditor bgUnitField;
    private Label bgNoteLabel;
    private TextFieldEditor bgWidthField;
    private TextFieldEditor bgHeightField;
    private ColorFieldEditor bgFillColorField;
    private ColorFieldEditor bgBorderColorField;
    private SpinnerFieldEditor bgBorderWidthField;
    private TextFieldEditor bgTooltipField;
    private ImageFieldEditor bgImageField;
    private ComboFieldEditor bgImageArrangement;
    private TextFieldEditor bgGridDistanceField;
    private SpinnerFieldEditor bgGridSubdivisionField;
    private ColorFieldEditor bgGridColorField;
    private TextFieldEditor bgLayoutSeedField;
    //private ComboFieldEditor bgLayoutAlgorithmField;
    private List<Label> unitLabels = new ArrayList<Label>();

    private Map<IFieldEditor, String> validationErrors = new LinkedHashMap<IFieldEditor, String>();
    
    private ModifyListener modifyListener = new ModifyListener() {
        public void modifyText(ModifyEvent e) {
            if (initializing || modifyInProgress)
                return;  // reduce excessive updates during startup, and prevent recursive calls

            try {
                modifyInProgress = true;
                updateControlStates();
                updatePreview();
                validateDialogContents();
            } 
            finally {
                modifyInProgress = false;
            }
        }
    };
    
    /**
     * Adapter interface for all properties (graphical and non-graphical) editable in this dialog
     */
    interface IPropertyAccess {
        /**
         * Returns the property value, which is never return null (missing value should be represented 
         * with some object such as ""). Throws exception if e does not have this property.
         * (Implementors may use a more specific return type that corresponds to the actual objects returned.)
         */
        Object getValue(INedElement e);
        
        /**
         * Returns a command that the property value (never null) on e. Throws exception if e does 
         * not have this property.
         */
        Command makeChangeCommand(INedElement e, Object value);
    }

    static class NamePropertyAccess implements IPropertyAccess {
        public String getValue(INedElement e) {
            return StringUtils.nullToEmpty(((IHasName)e).getName());
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new RenameCommand((IHasName)e, ((String)value).trim());
        }
    }

    static class VectorSizePropertyAccess implements IPropertyAccess {
        public String getValue(INedElement e) {
            return StringUtils.nullToEmpty(((SubmoduleElement)e).getVectorSize());
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetAttributeCommand((SubmoduleElement)e, SubmoduleElement.ATT_VECTOR_SIZE, ((String)value).trim());
        }
    }

    static class TypePropertyAccess implements IPropertyAccess {
        public String getValue(INedElement e) {
            ISubmoduleOrConnection submoduleOrConnection = (ISubmoduleOrConnection)e;
            String type = submoduleOrConnection.getType();
            return resolveType(type, submoduleOrConnection.getCompoundModule());
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetTypeOrLikeTypeCommand((ISubmoduleOrConnection)e, SubmoduleElement.ATT_TYPE, ((String)value).trim());
        }
    }

    static class LikeTypePropertyAccess implements IPropertyAccess {
        public String getValue(INedElement e) {
            ISubmoduleOrConnection submoduleOrConnection = (ISubmoduleOrConnection)e;
            String likeType = submoduleOrConnection.getLikeType();
            return resolveType(likeType, submoduleOrConnection.getCompoundModule());
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetTypeOrLikeTypeCommand((ISubmoduleOrConnection)e, SubmoduleElement.ATT_LIKE_TYPE, ((String)value).trim());
        }
    }

    static class LikeParamPropertyAccess implements IPropertyAccess {
        public String getValue(INedElement e) {
            return StringUtils.nullToEmpty(((ISubmoduleOrConnection)e).getLikeParam());
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetAttributeCommand((ISubmoduleOrConnection)e, SubmoduleElement.ATT_LIKE_PARAM, ((String)value).trim());
        }
    }

    static class ExtendsPropertyAccess implements IPropertyAccess {
        public List<String> getValue(INedElement e) {
            return extractInheritanceTypeList((INedTypeElement)e, INedElement.NED_EXTENDS);
        }

        @SuppressWarnings("unchecked")
        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetInheritanceCommand((INedTypeElement)e, INedElement.NED_EXTENDS, (List<String>)value);
        }
    }

    static class InterfacesPropertyAccess implements IPropertyAccess {
        public List<String> getValue(INedElement e) {
            return extractInheritanceTypeList((INedTypeElement)e, INedElement.NED_INTERFACE_NAME);
        }

        @SuppressWarnings("unchecked")
        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetInheritanceCommand((INedTypeElement)e, INedElement.NED_INTERFACE_NAME, (List<String>)value);
        }
    }

    static class ElementAttributePropertyAccess implements IPropertyAccess {
        private String attributeName;
        public ElementAttributePropertyAccess(String attributeName) {
            this.attributeName = attributeName;
        }

        public String getValue(INedElement e) {
            return e.getAttribute(attributeName);
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetAttributeCommand(e, attributeName, (String)value);
        }
    }

    static class DocumentationPropertyAccess implements IPropertyAccess {
        public String getValue(INedElement e) {
            // return the banner comment of the type element (note: getComment() is not good, because it can return the "right" comment)
            INedTypeElement typeElement = (INedTypeElement)e;
            CommentElement commentElement = (CommentElement) typeElement.getFirstChildWithAttribute(INedElement.NED_COMMENT, CommentElement.ATT_LOCID, "banner");
            String comment = commentElement == null ? "" : StringUtils.nullToEmpty(commentElement.getContent());
            String doc = comment.replaceAll("(?m)^\\s*// ?", "").trim() + "\n";
            return doc;
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            return new SetDocumentationCommand((INedTypeElement)e, (String)value);
        }
    }

    static class DisplayPropertyAccess implements IPropertyAccess {
        private Prop displayProperty;

        public DisplayPropertyAccess(IDisplayString.Prop displayProperty) {
            this.displayProperty = displayProperty;
        }

        public String getValue(INedElement e) {
            String value = StringUtils.nullToEmpty(((IHasDisplayString)e).getDisplayString().getAsString(displayProperty, false, false)); 
            if (!value.equals("") && displayProperty.getEnumSpec() != null) {
                String expandedValue = displayProperty.getEnumSpec().getNameFor(value); // expand abbreviation
                if (expandedValue != null)
                    value = expandedValue;  
            }
            return value;
        }

        public Command makeChangeCommand(INedElement e, Object value) {
            // exceptionally, this method accepts value=null, to mean "remove this display property"
            if (value == null)
                return new ChangeDisplayPropertyCommand((IHasDisplayString)e, displayProperty, "");

            DisplayString displayString = ((IHasDisplayString)e).getDisplayString();
            if (!value.equals("") && displayProperty.getEnumSpec() != null) {
                String shorthand = displayProperty.getEnumSpec().getShorthandFor((String)value);
                if (shorthand != null)
                    value = shorthand;  // use shorthand, e.g. replace "left" with "l" 
            }
            String newValue = getLocalDisplayPropertyChange(displayString, displayProperty, (String)value);
            if (newValue == null)
                return null; // no change needed
            return new ChangeDisplayPropertyCommand((IHasDisplayString)e, displayProperty, newValue);
        }
    }

    /**
     * Creates the dialog.
     */
    public PropertiesDialog(Shell parentShell, INedElement[] elements) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.elements = elements;

        if (!supportsEditing(elements))
            throw new IllegalArgumentException("Properties dialog does not support heterogenous selections");
    }

    /**
     * Returns whether this dialog can handle the given elements. Elements must be
     * of the same type, and, for submodules and connections, they must be 
     * under the same compound module. (Reason for the latter is that otherwise
     * we cannot display the measurement unit, e.g. "m" or "km".)
     */
    public static boolean supportsEditing(INedElement[] elements) {
        return elements.length > 0 && areSameType(elements) && areInSameCompoundModule(elements);
    }

    protected static boolean areSameType(INedElement[] elements) {
        if (elements.length == 0) 
            return true;
        int firstType = elements[0].getTagCode();
        for (INedElement element : elements)
            if (element.getTagCode() != firstType)
                return false;
        return true;
    }
    
    protected static boolean areInSameCompoundModule(INedElement[] elements) {
        if (elements.length == 0) 
            return true;
        CompoundModuleElementEx firstCompoundModule = compoundModuleOf(elements[0]);
        for (INedElement e : elements)
            if (compoundModuleOf(e) != firstCompoundModule)
                return false;
        return true;
    }

    protected static CompoundModuleElementEx compoundModuleOf(INedElement e) {
        return (e instanceof ISubmoduleOrConnection) ? ((ISubmoduleOrConnection)e).getCompoundModule() : null;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return getDialogSettings();
    }

    protected IDialogSettings getDialogSettings() {
        // return a different settings object for each class we edit (simple module, compound module, etc)
        return UIUtils.getDialogSettings(NedEditorPlugin.getDefault(), getClass().getName()+":"+elements[0].getTagName()); 
    }

	@Override
    protected void configureShell(Shell shell) {
	    super.configureShell(shell);

	    String what;
	    if (elements.length == 1)
	        what = new NedModelLabelProvider().getText(elements[0]);
	    else 
	        what = elements.length + " " + elements[0].getReadableTagName() + "s";
	    shell.setText("Properties of " + what);
	}

    @Override
    protected Control createDialogArea(Composite parent) {
        // dialog area
        Composite dialogArea = (Composite)super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

        // tabfolder and pages
        tabfolder = new TabFolder(composite, SWT.TOP);
        tabfolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        createGeneralPage(tabfolder);
        if (elements[0] instanceof ConnectionElement)
            createEndpointsPage(tabfolder);
        if (elements[0] instanceof SubmoduleElement)
            positionPage = createPositionPage(tabfolder);
        if (!(elements[0] instanceof IInterfaceTypeElement))
            appearancePage = createAppearancePage(tabfolder);
        if (elements[0] instanceof CompoundModuleElement)
            backgroundPage = createBackgroundPage(tabfolder);

        // preview area at the bottom
        previewArea = new Composite(composite, SWT.NONE);
        previewArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        previewArea.setLayout(new StackLayout());
        previewComposite = createPreview(previewArea);
        if (elements[0] instanceof CompoundModuleElement)
            bgPreviewComposite = createModuleBackgroundPreview(previewArea);
        ((GridData)previewArea.getLayoutData()).heightHint = bgPreviewComposite==null ? 120 : 250;

        // error display
        Composite errorArea = createComposite(composite, 2);
        errorImageLabel = new Label(errorArea, SWT.NONE); 
        errorImageLabel.setLayoutData(new GridData(ICON_ERROR.getBounds().width, SWT.DEFAULT));
        errorMessageLabel = new Label(errorArea, SWT.NONE);
        errorMessageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        
        // when user flips to the Background page, show background preview
        tabfolder.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                tabfolderPageChanged();
            }
        });

        Dialog.applyDialogFont(composite);
        
        populateDialog();

        initializing = false; // done
        
        updateControlStates();
        updatePreview();
        updateLabelsWithUnit();
        
        // restore active page
        try {
            int pageIndex = getDialogSettings().getInt("activePage");
            if (pageIndex > 0 && pageIndex < tabfolder.getItemCount())
                tabfolder.setSelection(pageIndex);
        } 
        catch (NumberFormatException e) { /*not saved, or wrong*/ }

        tabfolderPageChanged();

        // set initial focus (without asyncExec() it apparently has no effect [Windows])
        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                if (tabfolder.getSelection().length>0)
                    tabfolder.getSelection()[0].getControl().setFocus();
                validateDialogContents();
            }
        });
        
        return composite;
    }

    protected static int computeDialogFontAverageCharWidth(Control window) {
        // note: we could also use Dialog.convertWidthInCharsToPixels() instead of this
        GC gc = new GC(window);
        Font f = JFaceResources.getFontRegistry().get(JFaceResources.DIALOG_FONT);
        gc.setFont(f);
        int averageWidth = gc.getFontMetrics().getAverageCharWidth();
        gc.dispose();
        return averageWidth;
    }
   
    protected Composite createGeneralPage(TabFolder tabfolder) {
        INedElement e = elements[0];  // all elements are of the same class (i.e. same tag code)
        Composite page = createTabPage(tabfolder, "General");
        page.setLayout(new GridLayout(2,false));
        Group group;
        
        if (e instanceof INedTypeElement) {
            // Types
            createLabel(page, "Name:", false); 
            nameField = createText(page, 20); 
            nameField.setEnabled(elements.length == 1); // makes no sense to give the same name to set multiple elements
            
            createLabel(page, "Extends:", false); 
            boolean areInterfaces = e instanceof IInterfaceTypeElement;
            boolean areModuleKind = e instanceof IModuleKindTypeElement;
            extendsField = createNedTypeSelector(page, areInterfaces, new INedResources.InstanceofPredicate(((INedTypeElement)e).getClass()));
            if (!areInterfaces) {
                createLabel(page, "Interfaces:", false); 
                interfacesField = createNedTypeSelector(page, true, areModuleKind ? MODULEINTERFACE_FILTER : CHANNELINTERFACE_FILTER);
            }
            
            createLabel(page, "", false, 2);
            createLabel(page, "Documentation: ", false, 2);
            documentationField = createMultilineText(page, 8);
            ((GridData)documentationField.getControl().getLayoutData()).horizontalSpan = 2;
        }
        else {
            // Submodules or connections
            boolean areSubmodules = e instanceof SubmoduleElement;
            if (areSubmodules) {
                group = createGroup(page, "Name and Size", 2, 2);
                createLabel(group, "Name:", false); 
                nameField = createText(group, 20); 
                nameField.setEnabled(elements.length == 1); // makes no sense to give the same name to set multiple elements

                createLabel(group, "Vector size:*", false); 
                vectorSizeField = createText(group, 10);
                createWrappingLabel(group, "* Number of elements in this submodule vector. Leave empty for scalar submodule", false, 2);
            }

            group = createGroup(page, "Type", 2, 2);
            fixedTypeRadioButton = createRadioButton(group, "Fixed type:", 2);
            createLabel(group, "Type:", true); 
            typeField = createNedTypeSelector(group, false, areSubmodules ? MODULE_EXCEPT_NETWORK_FILTER : CHANNEL_FILTER); 
            parametricTypeRadioButton = createRadioButton(group, "Parametric type:", 2);
            createLabel(group, "Interface:", true); 
            likeTypeField = createNedTypeSelector(group, false, areSubmodules ? MODULEINTERFACE_FILTER : CHANNELINTERFACE_FILTER); 
            createLabel(group, "Type expression:*", true); 
            likeParamField = createText(group, 20);
            createWrappingLabel(group, "* Leave empty to use **.type-name= syntax in ini files", true, 2); 

            if (!areSubmodules) {
                createLabel(group, "", false, 2);  // separator
                isBidirectionalField = createCheckbox(group, "Bidirectional connection", 2);
            }

            SelectionListener radioListener = new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    updateTypeRadioButtons();
                }
            };
            fixedTypeRadioButton.addSelectionListener(radioListener);
            parametricTypeRadioButton.addSelectionListener(radioListener);
        }
        
        return page;
    }

    protected Composite createEndpointsPage(TabFolder tabfolder) {
        Assert.isTrue(elements[0] instanceof ConnectionElement);
        
        Composite page = createTabPage(tabfolder, "Endpoints");
        page.setLayout(new GridLayout(1,true));
        Group group;
        
        // "Source" group
        group = createGroup(page, "Source", 8);
        createLabel(group, "Module:", false); 
        connSrcModuleField = createCombo(group, 16);
        createLabel(group, "Index:", true); 
        connSrcModuleIndexField = createText(group, 5); 
        createLabel(group, "Gate:", true); 
        connSrcGateField = createCombo(group, 16);
        createLabel(group, "Index:", true); 
        connSrcGateIndexField = createText(group, 5);

        // "Destination" group
        group = createGroup(page, "Destination", 8);
        createLabel(group, "Module:", false); 
        connDestModuleField = createCombo(group, 16);
        createLabel(group, "Index:", true); 
        connDestModuleIndexField = createText(group, 5);
        createLabel(group, "Gate:", true); 
        connDestGateField = createCombo(group, 16);
        createLabel(group, "Index:", true); 
        connDestGateIndexField = createText(group, 5);

        Map<String,String> gateIndexProposals = new LinkedHashMap<String, String>();
        gateIndexProposals.put("++", "++ (next free gate index)");
        gateIndexProposals.put("i+1", "(any integer constant or expression)");
        connSrcGateIndexField.setContentProposals(gateIndexProposals, null);
        connDestGateIndexField.setContentProposals(gateIndexProposals, null);

        return page;
    }
    
    protected Composite createPositionPage(TabFolder tabfolder) {
        Assert.isTrue(elements[0] instanceof SubmoduleElement);
        
        Composite page = createTabPage(tabfolder, "Position");
        page.setLayout(new GridLayout(1,false));
        Group group;
        Composite comp;

        group = createGroup(page, "Position", 6);
        // P tag
        createLabel(group, "X:", false); 
        xField = createText(group, 10);
        createUnitLabel(group); 
        createLabel(group, "Y:", true); 
        yField = createText(group, 10);
        createUnitLabel(group); 

        group = createGroup(page, "Transmission Range Indicator", 1);
        // R tag
        comp = createComposite(group, 4);
        createLabel(comp, "Radius:", false); 
        rangeField = createText(comp, 10); 
        createUnitLabel(comp); 
        comp = createComposite(group, 7);
        createLabel(comp, "Line width:", false); 
        rangeBorderWidthField = createSpinner(comp, 0, 0, 100, 1); 
        createLabel(comp, "px", false); 
        createLabel(comp, "Line color:", true); 
        rangeBorderColorField = createColorSelector(comp);
        createLabel(comp, "Fill color:", true); 
        rangeFillColorField = createColorSelector(comp);

        group = createGroup(page, "Module Vector", 8);
        createLabel(group, "Arrangement:", false); 
        layoutField = createCombo(group, IDisplayString.Prop.LAYOUT.getEnumSpec().getNames());
        layoutPar1Label = createLabel(group, "", true);  // temporary labels 
        layoutPar1Field = createText(group, 5); 
        layoutPar2Label = createLabel(group, "", true); 
        layoutPar2Field = createText(group, 5); 
        layoutPar3Label = createLabel(group, "", true); 
        layoutPar3Field = createText(group, 5); 
        layoutField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateLayoutParLabels();
            }
        });

        String parentType = ((SubmoduleElementEx) elements[0]).getCompoundModule().isNetwork() ? "network" : "compound module"; 
        createWrappingLabel(page, "Note: Coordinates and sizes are understood in the " + parentType + "'s units, not pixels.", false, 1);

        return page;
    }
    
    protected Composite createAppearancePage(TabFolder tabfolder) {
        INedElement e = elements[0];  // all elements are of the same class (i.e. same tag code)
        Composite page = createTabPage(tabfolder, "Appearance");
        page.setLayout(new GridLayout(1,false));
        Group group;
        Composite comp;
        
        if (e instanceof SimpleModuleElement || e instanceof CompoundModuleElement || e instanceof ModuleInterfaceElement) {
            group = createGroup(page, "Icon", 1);
            // I tag
            comp = createComposite(group, 7);
            createLabel(comp, "Image:", false); 
            imageField = createImageSelector(comp); 
            imageSizeField = createCombo(comp, IDisplayString.Prop.IMAGE_SIZE.getEnumSpec().getNames());
            createLabel(comp, " Tint:", true); 
            imageColorPercentageField = createSpinner(comp, 0, 0, 100, 1); 
            createLabel(comp, "%", false); 
            imageColorField = createColorSelector(comp);

            group = createGroup(page, "Decorations", 1);

            comp = createComposite(group, 7);
            // I2 tag
            createLabel(comp, "Status image:", false); 
            image2Field = createImageSelector(comp, "status/");
            createLabel(comp, "", false); 
            createLabel(comp, " Tint:", true); 
            image2ColorPercentageField = createSpinner(comp, 0, 0, 100, 1); 
            createLabel(comp, "%", false); 
            image2ColorField = createColorSelector(comp);

            // notes
            if (e instanceof CompoundModuleElementEx && !((CompoundModuleElementEx)e).isNetwork())
                createWrappingLabel(page, "Note: The above icon is used when the module appears in another compound module as a submodule.", false, 1);
            createWrappingLabel(page, "Note: The display string (@display) may contain other graphical properties as well, only they are not editable in this dialog.", false, 1);
        }
        if (e instanceof ChannelElement || e instanceof ChannelInterfaceElement) {
            group = createGroup(page, "Line Style", 7);

            // LS tag
            createLabel(group, "Line color:", false); 
            connectionColorField = createColorSelector(group); 
            createLabel(group, "Line width:", true); 
            connectionWidthField = createSpinner(group, 0, 0, 100, 1); 
            createLabel(group, "px", false); 
            createLabel(group, "Line style:", true); 
            connectionStyleField = createCombo(group, IDisplayString.Prop.CONNECTION_STYLE.getEnumSpec().getNames());

            createWrappingLabel(page, "Note: The display string (@display) may contain other graphical properties as well, only they are not editable in this dialog.", false, 1);
        }
        else if (e instanceof SubmoduleElement) {
            group = createGroup(page, "Icon", 1);
            // I tag
            comp = createComposite(group, 7);
            createLabel(comp, "Image:", false); 
            imageField = createImageSelector(comp); 
            imageSizeField = createCombo(comp, IDisplayString.Prop.IMAGE_SIZE.getEnumSpec().getNames());
            createLabel(comp, " Tint:", true); 
            imageColorPercentageField = createSpinner(comp, 0, 0, 100, 1); 
            createLabel(comp, "%", false); 
            imageColorField = createColorSelector(comp);

            group = createGroup(page, "Shape", 1);

            // B tag
            comp = createComposite(group, 8);
            createLabel(comp, "Shape:", false); 
            shapeField = createCombo(comp, IDisplayString.Prop.SHAPE.getEnumSpec().getNames());
            createLabel(comp, "Width:", true); 
            shapeWidthField = createText(comp, 10); 
            createUnitLabel(comp); 
            createLabel(comp, "Height:", true); 
            shapeHeightField = createText(comp, 10); 
            createUnitLabel(comp); 
            comp = createComposite(group, 7);
            createLabel(comp, "Border width:", false); 
            shapeBorderWidthField = createSpinner(comp, 0, 0, 100, 1); 
            createLabel(comp, "px", false); 
            createLabel(comp, "Border color:", true); 
            shapeBorderColorField = createColorSelector(comp); 
            createLabel(comp, "Fill color:", true); 
            shapeFillColorField = createColorSelector(comp); 
            createWrappingLabel(group, "Note: Shape is displayed behind the icon if both are present", false, 1); 

            group = createGroup(page, "Decorations", 1);

            // T and TT tags
            comp = createComposite(group, 6);
            createLabel(comp, "Text:", false); 
            textField = createText(comp, 30);
            createLabel(comp, "Position:", true); 
            textPosField = createCombo(comp, IDisplayString.Prop.TEXT_POS.getEnumSpec().getNames());
            createLabel(comp, "Color:", true); 
            textColorField = createColorSelector(comp);

            comp = createComposite(group, 2);
            createLabel(comp, "Tooltip:", false); 
            tooltipField = createText(comp, 70); 

            comp = createComposite(group, 7);
            // I2 tag
            createLabel(comp, "Status image:", false); 
            image2Field = createImageSelector(comp, "status/");
            createLabel(comp, "", false); 
            createLabel(comp, " Tint:", true); 
            image2ColorPercentageField = createSpinner(comp, 0, 0, 100, 1); 
            createLabel(comp, "%", false); 
            image2ColorField = createColorSelector(comp);
            
            // Q tag
            comp = createComposite(group, 2);
            createLabel(comp, "Queue object's name for queue length indicator: ", false); 
            queueNameField = createText(comp, 20); 
        }
        else if (e instanceof ConnectionElement) {
            group = createGroup(page, "Line Style", 7);
            // LS tag
            createLabel(group, "Line color:", false); 
            connectionColorField = createColorSelector(group);
            createLabel(group, "Line width:", true); 
            connectionWidthField = createSpinner(group, 0, 0, 100, 1); 
            createLabel(group, "px", false); 
            createLabel(group, "Line style:", true); 
            connectionStyleField = createCombo(group, IDisplayString.Prop.CONNECTION_STYLE.getEnumSpec().getNames());

            group = createGroup(page, "Text Label", 6);
            // T tag
            createLabel(group, "Text:", false); 
            textField = createText(group, 30); 
            createLabel(group, "Position:", true); 
            textPosField = createCombo(group, IDisplayString.Prop.TEXT_POS.getEnumSpec().getNames());
            createLabel(group, "Text color:", true); 
            textColorField = createColorSelector(group);

            group = createGroup(page, "Tooltip", 2);
            // TT tag
            createLabel(group, "Tooltip:", false); 
            tooltipField = createText(group, 70); 
        }
        
        return page;
    }

    protected Composite createBackgroundPage(TabFolder tabfolder) {
        Composite page = createTabPage(tabfolder, "Background");
        page.setLayout(new GridLayout(2,false));
        Group group;
        Composite comp;

        // NOTE: we do not edit the MODULE_X and MODULE_Y properties, because their semantics is unclear 
        // (coordinate offset??) and are ignored by the graphical editor anyway
        
        group = createGroup(page, "Area", 3);
        // BGP tag
        createLabel(group, "Width:", false); 
        bgWidthField = createText(group, 10); 
        createUnitLabel(group); 
        createLabel(group, "Height:", false); 
        bgHeightField = createText(group, 10); 
        createUnitLabel(group); 

        group = createGroup(page, "Unit and Scale", 1);
        comp = createComposite(group, 2);
        createLabel(comp, "Measurement unit:", false); 
        bgUnitField = createText(comp, 10);
        comp = createComposite(group, 3);
        bgScaleLabel = createLabel(comp, "One unit corresponds to:", false); // text is updated from updateLabelsWithUnit()
        bgScaleField = createText(comp, 10); 
        createLabel(comp, "pixel(s)", false);
        
        bgNoteLabel = createLabel(page, "Note: ...", false); // text will be set in updateUnit()
        ((GridData)bgNoteLabel.getLayoutData()).horizontalSpan = 2;
        bgUnitField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateLabelsWithUnit();
            }
        });
        
        group = createGroup(page, "Background", 1, 2);
        // BGB tag
        comp = createComposite(group, 7);
        createLabel(comp, "Color:", false); 
        bgFillColorField = createColorSelector(comp); 
        createLabel(comp, "Border color:", true); 
        bgBorderColorField = createColorSelector(comp); 
        createLabel(comp, "Border width:", true); 
        bgBorderWidthField = createSpinner(comp, 0, 0, 100, 1); 
        createLabel(comp, "px", false); 

        // BGI tag
        comp = createComposite(group, 4);
        createLabel(comp, "Image:", false); 
        bgImageField = createImageSelector(comp, "background/");
        createLabel(comp, "Arrangement:", true); 
        bgImageArrangement = createCombo(comp, IDisplayString.Prop.MODULE_IMAGE_ARRANGEMENT.getEnumSpec().getNames()); 

        comp = createComposite(group, 2);
        createLabel(comp, "Tooltip:", false); 
        bgTooltipField = createText(comp, 70); 

        group = createGroup(page, "Grid", 7, 2);
        // BGG tag
        createLabel(group, "Major gridline distance:", false); 
        bgGridDistanceField = createText(group, 10); 
        createUnitLabel(group); 
        createLabel(group, "Subdivision:", true); 
        bgGridSubdivisionField = createSpinner(group, 0, 0, 100, 1); 
        createLabel(group, "Color:", true); 
        bgGridColorField = createColorSelector(group); 

        group = createGroup(page, "Submodule Layouting", 2, 2);
        // BGL tag 
        createLabel(group, "RNG seed:", false); 
        bgLayoutSeedField = createText(group, 10); 
        //createLabel(group, "Layouting algorithm:"); 
        //bgLayoutAlgorithmField = createCombo(group, IDisplayString.Prop.MODULE_LAYOUT_ALGORITHM.getEnumSpec().getNames()); 

        return page;
    }

    protected Composite createPreview(Composite parent) {
        Composite previewGroup = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(1, false);
        layout.marginHeight = layout.marginWidth = 0;
        previewGroup.setLayout(layout);
        previewGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(previewGroup, "Preview", false);
        
        Canvas canvas = new Canvas(previewGroup, SWT.BORDER | SWT.DOUBLE_BUFFERED);
        canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        canvas.setBackground(ColorFactory.WHITE);

        LightweightSystem lws = new LightweightSystem(canvas);
        previewScrollPane = new PuristicScrollPane();
        lws.setContents(previewScrollPane);
        FreeformViewport viewport= new FreeformViewport();
        previewScrollPane.setViewport(viewport);
        LayeredPane layeredPane = new FreeformLayeredPane() {
            @Override
            public void paint(Graphics graphics) {
                // set antialiasing on content and child/derived figures
                graphics.setAntialias(SWT.ON);
                super.paint(graphics);
            }
            
            @Override
            public Rectangle getFreeformExtent() {
                return super.getFreeformExtent().getCopy().expand(10, 10);
            }
        };
        viewport.setContents(layeredPane);

        FreeformLayer previewFigureLayer = new FreeformLayer();
        FreeformLayer rangeFigureLayer = new FreeformLayer();
        layeredPane.add(rangeFigureLayer);
        layeredPane.add(previewFigureLayer);

        INedElement e = elements[0];
        int n = Math.min(elements.length, 10);
        previewFigure = new IFigure[n]; 
        for (int i=0; i<n; i++) {
            if (e instanceof SubmoduleElement || e instanceof IModuleKindTypeElement) {
                SubmoduleFigure submoduleFigure = new SubmoduleFigure();
                submoduleFigure.setRangeFigureLayer(rangeFigureLayer);
                previewFigureLayer.add(submoduleFigure);
                previewFigure[i] = submoduleFigure;
            } 
            else {
                ConnectionFigure connectionFigure = new ConnectionFigure(); 
                previewFigureLayer.add(connectionFigure);
                connectionFigure.setPoints(new PointList(new int[] {10,10,50,50}));
                connectionFigure.setLineWidth(1);
                connectionFigure.setArrowHeadEnabled(true);
                previewFigure[i] = connectionFigure;
            }
        }

        return previewGroup;
    }

    protected Composite createModuleBackgroundPreview(Composite parent) {
        Composite previewGroup = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(1, false);
        layout.marginHeight = layout.marginWidth = 0;
        previewGroup.setLayout(layout);
        previewGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        createLabel(previewGroup, "Preview", false);
        Canvas canvas = new Canvas(previewGroup, SWT.BORDER | SWT.DOUBLE_BUFFERED);
        canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        canvas.setBackground(ColorFactory.WHITE);

        LightweightSystem lws = new LightweightSystem(canvas);
        bgPreviewScrollPane = new PuristicScrollPane();
        lws.setContents(bgPreviewScrollPane);
        FreeformViewport viewport= new FreeformViewport();
        bgPreviewScrollPane.setViewport(viewport);
        Layer layerFigure = new FreeformLayer() {
            @Override
            public void paint(Graphics graphics) {
                // set antialiasing on content and child/derived figures
                graphics.setAntialias(SWT.ON);
                super.paint(graphics);
            }

            @Override
            public Rectangle getFreeformExtent() {
                return super.getFreeformExtent().getCopy().expand(10, 10);
            }
        };
        viewport.setContents(layerFigure);

        CompoundModuleFigure compoundModuleFigure = new CompoundModuleFigure();
        layerFigure.add(compoundModuleFigure);
        compoundModuleFigure.setLocation(new Point(10,10));
        bgPreviewFigure = compoundModuleFigure;
        
        return previewGroup;
    }

    protected void tabfolderPageChanged() {
        // there are two previews: one for Appearance, and one for Background; make sure the correct one is visible
        StackLayout stackLayout = (StackLayout)previewArea.getLayout();
        if (stackLayout.topControl == null)
            stackLayout.topControl = previewComposite; // initialize

        // if user goes to the Background page, display the background preview; 
        // if user goes to Appearance or Position page, display the normal preview; 
        // otherwise keep currently shown preview.
        TabItem[] items = tabfolder.getSelection();
        if (items.length != 0 && items[0] != null) {
            Composite previewPageToShow = null;
            if (items[0].getControl() == backgroundPage)
                previewPageToShow = bgPreviewComposite;
            else if (items[0].getControl() == positionPage || items[0].getControl() == appearancePage)
                previewPageToShow = previewComposite;

            if (previewPageToShow != null && stackLayout.topControl != previewPageToShow) {
                stackLayout.topControl = previewPageToShow;
                previewArea.layout(true);
            }
        }
    }
    
    protected Label createLabel(Composite parent, String text, boolean indented) {
        return createLabel(parent, text, indented, 1);
    }
    
    protected Label createLabel(Composite parent, String text, boolean indented, int hspan) {
        Label label = new Label(parent, SWT.NONE);
        GridData gridData = new GridData(SWT.FILL, SWT.CENTER, false, false);
        if (indented)
            gridData.horizontalIndent = 10; // note: indent usually serves as separator between adjacent controls
        gridData.horizontalSpan = hspan;
        label.setLayoutData(gridData);
        label.setText(text);
        return label;
    }

    protected Label createWrappingLabel(Composite parent, String text, boolean indented, int hspan) {
        Label label = new Label(parent, SWT.WRAP);
        GridData gridData = new GridData(SWT.FILL, SWT.CENTER, true, false);
        gridData.widthHint = 200; // cannot be too small, otherwise label will request a large height 
        if (indented)
            gridData.horizontalIndent = 10; // note: indent usually serves as separator between adjacent controls
        gridData.horizontalSpan = hspan;
        label.setLayoutData(gridData);
        label.setText(text);
        return label;
    }

    protected void createUnitLabel(Composite parent) {
        Label label = new Label(parent, SWT.NONE);
        GridData gridData = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        gridData.widthHint = 4 * computeDialogFontAverageCharWidth(parent);  // we use a fixed width, and e.g. "nm." is already 4 chars wide) 
        label.setLayoutData(gridData);
        unitLabels.add(label);
    }
    
    protected Button createRadioButton(Composite parent, String text) {
        return createRadioButton(parent, text, 1);
    }
    
    protected Button createRadioButton(Composite parent, String text, int hspan) {
        Button radio = new Button(parent, SWT.RADIO);
        GridData gridData = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        gridData.horizontalSpan = hspan;
        radio.setLayoutData(gridData);
        radio.setText(text);
        radio.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                modifyListener.modifyText(null);
            }
        });
        return radio;
    }

    protected TextFieldEditor createText(Composite parent, int widthInChars) {
        TextFieldEditor fieldEditor = new TextFieldEditor(parent, SWT.BORDER);
        fieldEditor.addModifyListener(modifyListener);
        Text text = fieldEditor.getControl();
        text.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
        if (widthInChars != SWT.DEFAULT)
            ((GridData)text.getLayoutData()).widthHint = convertWidthInCharsToPixels(widthInChars);
        return fieldEditor;
    }

    protected TextFieldEditor createMultilineText(Composite parent, int heightInLines) {
        TextFieldEditor fieldEditor = new TextFieldEditor(parent, SWT.BORDER | SWT.MULTI | SWT.V_SCROLL);
        fieldEditor.addModifyListener(modifyListener);
        Text text = fieldEditor.getControl();
        text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        if (heightInLines != SWT.DEFAULT)
            ((GridData)text.getLayoutData()).heightHint = convertHeightInCharsToPixels(heightInLines);
        return fieldEditor;
    }

    protected SpinnerFieldEditor createSpinner(Composite parent, int digitsAfterPoint, int minimum, int maximum, int increment) {
        SpinnerFieldEditor fieldEditor = new SpinnerFieldEditor(parent, SWT.BORDER);
        fieldEditor.addModifyListener(modifyListener);
        Spinner spinner = fieldEditor.getControl();
        spinner.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
        spinner.setDigits(digitsAfterPoint);
        spinner.setMinimum(minimum);
        spinner.setMaximum(maximum);
        spinner.setIncrement(increment);
        return fieldEditor;
    }

    protected ComboFieldEditor createCombo(Composite parent, String[] items) {
        return createCombo(parent, items, SWT.DEFAULT);
    }

    protected ComboFieldEditor createCombo(Composite parent, int widthInChars) {
        return createCombo(parent, ArrayUtils.EMPTY_STRING_ARRAY, widthInChars);
    }

    protected TristateButton createCheckbox(Composite parent, String label, int hSpan) {
        TristateButton checkbox = new TristateButton(parent, SWT.CHECK);
        checkbox.setText(label);
        checkbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        ((GridData)checkbox.getLayoutData()).horizontalSpan = hSpan;
        checkbox.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                modifyListener.modifyText(null);
            }
        });
        return checkbox;
    }
    
    protected ComboFieldEditor createCombo(Composite parent, String[] items, int widthInChars) {
        ComboFieldEditor fieldEditor = new ComboFieldEditor(parent, SWT.BORDER);
        fieldEditor.addModifyListener(modifyListener);
        Combo combo = fieldEditor.getControl();
        combo.setItems(items);
        combo.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
        if (widthInChars != SWT.DEFAULT)
            ((GridData)combo.getLayoutData()).widthHint = convertWidthInCharsToPixels(widthInChars);
        return fieldEditor;
    }

    protected ColorFieldEditor createColorSelector(Composite parent) {
        ColorFieldEditor fieldEditor = new ColorFieldEditor(parent, SWT.BORDER);
        fieldEditor.addModifyListener(modifyListener);
        Composite composite = fieldEditor.getControl();
        composite.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
        return fieldEditor;
    }

    protected ImageFieldEditor createImageSelector(Composite parent) {
        return createImageSelector(parent, "");
    }
    
    protected ImageFieldEditor createImageSelector(Composite parent, String defaultFilter) {
        ImageFieldEditor fieldEditor = new ImageFieldEditor(parent, SWT.BORDER);
        fieldEditor.addModifyListener(modifyListener);
        fieldEditor.setDefaultFilter(defaultFilter);
        Composite composite = fieldEditor.getControl();
        composite.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
        return fieldEditor;
    }

    protected NedTypeFieldEditor createNedTypeSelector(Composite parent, boolean multiple, INedTypeResolver.IPredicate typeFilter) {
        IProject contextProject = NedResourcesPlugin.getNedResources().getNedFile(elements[0].getContainingNedFileElement()).getProject();
        NedTypeFieldEditor fieldEditor = new NedTypeFieldEditor(parent, multiple, contextProject, typeFilter);
        fieldEditor.addModifyListener(modifyListener);
        Control control = fieldEditor.getControl();
        control.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
        ((GridData)control.getLayoutData()).widthHint = (multiple ? 70 : 50) * computeDialogFontAverageCharWidth(getShell());
        
        return fieldEditor;
    }

    protected Composite createTabPage(TabFolder tabfolder, String text) {
        TabItem item = new TabItem(tabfolder, SWT.NONE);
        item.setText(text);
        Composite composite = new Composite(tabfolder, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        item.setControl(composite);
        return composite;
    }

    protected Composite createComposite(Composite parent, int numColumns) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        GridLayout layout = new GridLayout(numColumns,false);
        layout.marginHeight = layout.marginWidth = 0;
        composite.setLayout(layout); 
        return composite;
    }
    
    protected Group createGroup(Composite parent, String text, int numColumns) {
        return createGroup(parent, text, numColumns, 1);
    }
    
    protected Group createGroup(Composite parent, String text, int numColumns, int horizSpanInParent) {
        Group group = new Group(parent, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        ((GridData)group.getLayoutData()).horizontalSpan = horizSpanInParent;
        group.setLayout(new GridLayout(numColumns,false));
        return group;
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
    }
    
    @SuppressWarnings("unchecked")
    protected void populateDialog() {
        // "General" page
        if (elements[0] instanceof INedTypeElement) {
            // for types: name, inheritance, documentation
            populateField(nameField, (String)getCommonProperty(new NamePropertyAccess()));
            populateField(extendsField, qnameListToFriendlyString((List<String>)getCommonProperty(new ExtendsPropertyAccess())));
            populateField(interfacesField, qnameListToFriendlyString((List<String>)getCommonProperty(new InterfacesPropertyAccess())));
            populateField(documentationField, (String)getCommonProperty(new DocumentationPropertyAccess()));
        }
        else if (elements[0] instanceof ISubmoduleOrConnection) {
            // for submodules and connections: name (submodule only), type, and "vectorness" (submodule only)
            if (elements[0] instanceof SubmoduleElement) {
                populateField(nameField, (String)getCommonProperty(new NamePropertyAccess()));
                populateField(vectorSizeField, (String)getCommonProperty(new VectorSizePropertyAccess()));
            }
            
            // type, likeType
            populateField(typeField, NedElementUtilEx.qnameToFriendlyTypeName((String)getCommonProperty(new TypePropertyAccess())));
            populateField(likeTypeField, NedElementUtilEx.qnameToFriendlyTypeName((String)getCommonProperty(new LikeTypePropertyAccess())));
            populateField(likeParamField, (String)getCommonProperty(new LikeParamPropertyAccess()));
            
            if (!likeTypeField.isGrayed() && !likeParamField.isGrayed() && StringUtils.isEmpty(likeTypeField.getText()) && StringUtils.isEmpty(likeParamField.getText())) {
                fixedTypeRadioButton.setSelection(true);
                updateTypeRadioButtons();
            }
            else if (!typeField.isGrayed() && StringUtils.isEmpty(typeField.getText())) {
                parametricTypeRadioButton.setSelection(true);
                updateTypeRadioButtons();
            }
            else { 
                // leave both radio buttons unselected (let user deal with the mess)
            }
        }

        // "Endpoints" page
        if (elements[0] instanceof ConnectionElement) {
            String isBidir = (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_IS_BIDIRECTIONAL));
            if (isBidir == null)
                isBidirectionalField.setGrayed(true);
            else
                isBidirectionalField.setSelection(isBidir.equals("true"));

            // fill in module combo item lists
            CompoundModuleElementEx compoundModule = (CompoundModuleElementEx) ((ConnectionElement)elements[0]).getEnclosingTypeElement();
            INedTypeInfo compoundModuleType = compoundModule.getNedTypeInfo();
            Map<String, SubmoduleElementEx> submodules = compoundModuleType.getSubmodules();
            String[] submoduleNames = submodules.keySet().toArray(new String[]{});
            Arrays.sort(submoduleNames);
            String[] submoduleLabels = new String[submoduleNames.length];
            for (int i=0; i<submoduleLabels.length; i++)
                submoduleLabels[i] = NedTreeUtil.getNedModelLabelProvider().getText(submodules.get(submoduleNames[i]));
            submoduleNames = (String[]) ArrayUtils.add(submoduleNames, 0, STR_PARENTMODULE);
            submoduleLabels = (String[]) ArrayUtils.add(submoduleLabels, 0, STR_PARENTMODULE);
            connSrcModuleField.setItems(submoduleNames, submoduleLabels);
            connDestModuleField.setItems(submoduleNames, submoduleLabels);

            // populate fields
            populateField(connSrcModuleField, (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_MODULE)));
            populateField(connSrcModuleIndexField, (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_MODULE_INDEX))); 
            populateField(connSrcGateField, (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_GATE))); 
            String srcGateIndex = (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_GATE_INDEX));
            String srcGatePlusPlus = (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_GATE_PLUSPLUS));
            populateField(connSrcGateIndexField, (srcGateIndex==null || srcGatePlusPlus==null) ? null : srcGateIndex + (srcGatePlusPlus.equals("true")?STR_GATEPLUSPLUS:""));

            populateField(connDestModuleField, (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_MODULE))); 
            populateField(connDestModuleIndexField, (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_MODULE_INDEX))); 
            populateField(connDestGateField, (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_GATE)));  
            String destGateIndex = (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_GATE_INDEX));
            String destGatePlusPlus = (String)getCommonProperty(new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_GATE_PLUSPLUS));
            populateField(connDestGateIndexField, (destGateIndex==null || destGatePlusPlus==null) ? null : destGateIndex + (destGatePlusPlus.equals("true")?STR_GATEPLUSPLUS:""));
        }

        //============== SUBMODULE ================
        // P tag
        populateField(xField, IDisplayString.Prop.X);
        populateField(yField, IDisplayString.Prop.Y); 
        populateField(layoutField, IDisplayString.Prop.LAYOUT); 
        populateField(layoutPar1Field, IDisplayString.Prop.LAYOUT_PAR1); 
        populateField(layoutPar2Field, IDisplayString.Prop.LAYOUT_PAR2); 
        populateField(layoutPar3Field, IDisplayString.Prop.LAYOUT_PAR3); 

        // B tag
        populateField(shapeField, IDisplayString.Prop.SHAPE); 
        populateField(shapeWidthField, IDisplayString.Prop.SHAPE_WIDTH); 
        populateField(shapeHeightField, IDisplayString.Prop.SHAPE_HEIGHT); 
        populateField(shapeFillColorField, IDisplayString.Prop.SHAPE_FILL_COLOR); 
        populateField(shapeBorderColorField, IDisplayString.Prop.SHAPE_BORDER_COLOR); 
        populateField(shapeBorderWidthField, IDisplayString.Prop.SHAPE_BORDER_WIDTH);
//XXX        
//        if (shapeField != null && !shapeField.isGrayed() && shapeField.getText().length()==0) {
//            // if any other shape-related field is filled in, adjust shapeField to show "rectangle"
//            if (shapeWidthField.getText().length()>0 || shapeHeightField.getText().length()>0 || shapeFillColorField.getText().length()>0 || shapeBorderColorField.getText().length()>0 || shapeBorderWidthField.getText().length()>0)
//                shapeField.setText("rectangle");
//            
//        }

        // I tag
        populateField(imageField, IDisplayString.Prop.IMAGE); 
        populateField(imageColorField, IDisplayString.Prop.IMAGE_COLOR); 
        populateField(imageColorPercentageField, IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE); 

        // IS tag
        populateField(imageSizeField, IDisplayString.Prop.IMAGE_SIZE); 

        // I2 tag
        populateField(image2Field, IDisplayString.Prop.IMAGE2); 
        populateField(image2ColorField, IDisplayString.Prop.IMAGE2_COLOR); 
        populateField(image2ColorPercentageField, IDisplayString.Prop.IMAGE2_COLOR_PERCENTAGE); 

        // R tag
        populateField(rangeField, IDisplayString.Prop.RANGE); 
        populateField(rangeFillColorField, IDisplayString.Prop.RANGE_FILL_COLOR); 
        populateField(rangeBorderColorField, IDisplayString.Prop.RANGE_BORDER_COLOR); 
        populateField(rangeBorderWidthField, IDisplayString.Prop.RANGE_BORDER_WIDTH); 

        // Q tag
        populateField(queueNameField, IDisplayString.Prop.QUEUE_NAME); 

        //============== CONNECTION ================
        // LS tag
        populateField(connectionColorField, IDisplayString.Prop.CONNECTION_COLOR); 
        populateField(connectionWidthField, IDisplayString.Prop.CONNECTION_WIDTH); 
        populateField(connectionStyleField, IDisplayString.Prop.CONNECTION_STYLE); 

        //============== SUBMODULE, CONNECTION ================
        // T tag
        populateField(textField, IDisplayString.Prop.TEXT); 
        populateField(textPosField, IDisplayString.Prop.TEXT_POS); 
        populateField(textColorField, IDisplayString.Prop.TEXT_COLOR); 

        // TT tag
        populateField(tooltipField, IDisplayString.Prop.TOOLTIP); 


        //============== COMPOUND MODULE ================
        // BGP tag: not edited because its semantics is unclear and it is ignored by the graphical editor 
        //populateField(bgXField, IDisplayString.Prop.MODULE_X); 
        //populateField(bgYField, IDisplayString.Prop.MODULE_Y); 

        // BGB tag
        populateField(bgWidthField, IDisplayString.Prop.MODULE_WIDTH); 
        populateField(bgHeightField, IDisplayString.Prop.MODULE_HEIGHT); 
        populateField(bgFillColorField, IDisplayString.Prop.MODULE_FILL_COLOR); 
        populateField(bgBorderColorField, IDisplayString.Prop.MODULE_BORDER_COLOR); 
        populateField(bgBorderWidthField, IDisplayString.Prop.MODULE_BORDER_WIDTH); 

        // BGTT tag
        populateField(bgTooltipField, IDisplayString.Prop.MODULE_TOOLTIP); 

        // BGI tag
        populateField(bgImageField, IDisplayString.Prop.MODULE_IMAGE); 
        populateField(bgImageArrangement, IDisplayString.Prop.MODULE_IMAGE_ARRANGEMENT); 

        // BGG tag
        populateField(bgGridDistanceField, IDisplayString.Prop.MODULE_GRID_DISTANCE); 
        populateField(bgGridSubdivisionField, IDisplayString.Prop.MODULE_GRID_SUBDIVISION); 
        populateField(bgGridColorField, IDisplayString.Prop.MODULE_GRID_COLOR); 

        // module layouting 
        populateField(bgLayoutSeedField, IDisplayString.Prop.MODULE_LAYOUT_SEED); 
        //populateField(bgLayoutAlgorithmField, IDisplayString.Prop.MODULE_LAYOUT_ALGORITHM); 
        populateField(bgScaleField, IDisplayString.Prop.MODULE_SCALE); 
        populateField(bgUnitField, IDisplayString.Prop.MODULE_UNIT);
        
        updatePreview();
    }

    protected void populateField(IFieldEditor field, IDisplayString.Prop displayProperty) {
        populateField(field, (String)getCommonProperty(new DisplayPropertyAccess(displayProperty)));
    }
    
    protected void populateField(IFieldEditor field, String value) {
        if (field != null) {
            if (value == null)
                field.setGrayed(true);
            else
                field.setText(value);
        }
    }

    /**
     * If all elements share the same value, returns that value; otherwise returns null.
     * null always means that differing values are present, because IProperty never returns null.
     */
    protected Object getCommonProperty(IPropertyAccess property) {
        Object firstValue = property.getValue(elements[0]);
        Assert.isNotNull(firstValue);
        for (INedElement e : elements)
            if (!firstValue.equals(property.getValue(e)))
                return null;
        return firstValue;
    }

    protected static List<String> extractInheritanceTypeList(INedTypeElement nedTypeElement, int tagcode) {
        Assert.isTrue(tagcode == INedElement.NED_EXTENDS || tagcode == INedElement.NED_INTERFACE_NAME);

        List<String> result = new ArrayList<String>();
        for (INedElement e : nedTypeElement.getChildrenWithTag(tagcode)) {
            String value = (tagcode == INedElement.NED_EXTENDS) ? ((ExtendsElement)e).getName() : ((InterfaceNameElement)e).getName();
            value = resolveType(value, nedTypeElement.getParentLookupContext()); // convert to fully qualified name
            result.add(value);
        }
        return result;
    }

    protected static String resolveType(String value, INedTypeLookupContext lookupContext) {
        if (StringUtils.isEmpty(value))
            return "";
        
        // if we can resolve this type, convert to fully qualified name
        INedTypeInfo typeInfo = NedResourcesPlugin.getNedResources().lookupNedType(value, lookupContext);

        if (typeInfo != null) {
            value = typeInfo.getFullyQualifiedName();
        }
        else if (value.indexOf('.') == -1) {
            // if the referred type used to exist and had been properly imported, we can recover
            // the fully qualified type name from the imports. E.g. if we have a "tcp: TCP" submodule
            // which is unresolved (typeInfo==null), but there is an "import inet.transport.tcp.TCP"
            // line in the file, we can conclude that the "tcp" module's type is "inet...", even though 
            // such type currently does not exist.
            for (String importSpec : lookupContext.getContainingNedFileElement().getImports()) {
                if (importSpec.endsWith("."+value) && importSpec.indexOf('*') == -1) {
                    value = importSpec;
                    break;
                }
            }
        }
        return value;
    }

    protected String qnameListToFriendlyString(List<String> qnames) {
        if (qnames ==  null)
            return null;
        StringBuilder result = new StringBuilder();
        for (String qname : qnames) {
            if (result.length() != 0)
                result.append(", ");
            result.append(NedElementUtilEx.qnameToFriendlyTypeName(qname));
        }
        return result.toString();
    }

    protected void updateTypeRadioButtons() {
        boolean isFixed = fixedTypeRadioButton.getSelection();
        typeField.setEnabled(isFixed);
        likeTypeField.setEnabled(!isFixed);
        likeParamField.setEnabled(!isFixed);
    }

    protected void updateLabelsWithUnit() {
        String unit;
        if (elements[0] instanceof CompoundModuleElement) {
            // compound module: get unit from dialog field
            unit = bgUnitField.getText();

            // update texts with embedded units
            bgScaleLabel.setText("One " + StringUtils.defaultIfEmpty(unit, "unit")+ " corresponds to:");
            bgScaleLabel.getParent().layout(true);
            
            bgNoteLabel.setText("Note: all sizes and distances in the compound module are understood in units" 
                    + (unit.equals("") ? "" : " (" + unit + ")")
                    + ", not pixels");
        }

        else if (elements[0] instanceof ISubmoduleOrConnection) {
            // submodule or connection: get unit from compound module (note: dialog does not allow editing submodules different compound modules)
            CompoundModuleElementEx compoundModule = compoundModuleOf(elements[0]);
            unit = compoundModule.getDisplayString().getAsString(IDisplayString.Prop.MODULE_UNIT);
            unit = StringUtils.nullToEmpty(unit);
        }
        else {
            // no units in simple module, channel, module interface, connection interface dialog pages
            unit = "";
        }
        
        // update labels
        String abbreviatedUnit = unit.length() <= 2 ? unit : unit.substring(0,2)+".";
        String unitTooltip = unit.length() <= 2 ? null : unit;
        for (Label label : unitLabels) {
            label.setText(abbreviatedUnit);
            label.setToolTipText(unitTooltip);
        }
    }
    
    protected void updateLayoutParLabels() {
        String value = layoutField.getText();
        
        // normalize value (replace enum value with its standard abbreviation)
        if (!value.equals(""))
            value = StringUtils.nullToEmpty(IDisplayString.Prop.LAYOUT.getEnumSpec().getShorthandFor(value));

        // values: r/c/m/ri/x (enum spec: "row=r.*,r; column=c.*,c; matrix=m.*,m; ring=ri.*,ri; exact=e.*|x.*,x")
        String par1Label = null, par2Label = null, par3Label = null;
        if (value.equals("r")) {
            par1Label = "Spacing:";
        }
        else if (value.equals("c")) {
            par1Label = "Spacing:";
        }
        else if (value.equals("m")) {
            par1Label = "Num.cols:";
            par2Label = "X spacing:";
            par3Label = "Y spacing:";
        }
        else if (value.equals("ri")) {
            par1Label = "X radius:";
            par2Label = "Y radius:";
        }
        else if (value.equals("x") || value.equals("")) {
            // leave all three hidden
        }
        else {
            // cannot happen
            NedEditorPlugin.logError("Invalid module vector layout: " + value, new Exception());
        }
        
        layoutPar1Label.setVisible(par1Label != null);
        layoutPar1Field.getControl().setVisible(par1Label != null);
        layoutPar2Label.setVisible(par2Label != null);
        layoutPar2Field.getControl().setVisible(par2Label != null);
        layoutPar3Label.setVisible(par3Label != null);
        layoutPar3Field.getControl().setVisible(par3Label != null);

        layoutPar1Label.setText(StringUtils.nullToEmpty(par1Label));
        layoutPar2Label.setText(StringUtils.nullToEmpty(par2Label));
        layoutPar3Label.setText(StringUtils.nullToEmpty(par3Label));
        
        layoutPar1Label.getParent().layout(true);
    }

    protected void updateControlStates() {
        if (connSrcModuleField != null) {
            updateConnectionEndpoint(true, connSrcModuleField, connSrcModuleIndexField, connSrcGateField, connSrcGateIndexField);
            updateConnectionEndpoint(false, connDestModuleField, connDestModuleIndexField, connDestGateField, connDestGateIndexField);
        } 
        if (vectorSizeField != null) {
            boolean isVector = vectorSizeField.getText().trim().length() > 0;
            layoutField.setEnabled(isVector);
            layoutPar1Field.setEnabled(isVector);
            layoutPar2Field.setEnabled(isVector);
            layoutPar3Field.setEnabled(isVector);
        }
        if (imageField != null) {
            boolean hasImage = imageField.getText().trim().length() > 0;
            imageSizeField.setEnabled(hasImage);
            imageColorPercentageField.setEnabled(hasImage);
            imageColorField.setEnabled(hasImage);
        }
        if (image2Field != null) {
            boolean hasImage2 = image2Field.getText().trim().length() > 0;
            image2ColorPercentageField.setEnabled(hasImage2);
            image2ColorField.setEnabled(hasImage2);
        }
        if (shapeField != null) {
            boolean hasShape = shapeField.getText().trim().length() > 0;
            shapeHeightField.setEnabled(hasShape);
            shapeWidthField.setEnabled(hasShape);
            shapeFillColorField.setEnabled(hasShape);
            shapeBorderColorField.setEnabled(hasShape);
            shapeBorderWidthField.setEnabled(hasShape);
        }
        if (textField != null) {
            boolean hasText = textField.getText().trim().length() > 0;
            textColorField.setEnabled(hasText);
            textPosField.setEnabled(hasText);
        }
        if (rangeField != null) {
            boolean hasRange = rangeField.getText().trim().length() > 0;
            rangeFillColorField.setEnabled(hasRange);
            rangeBorderColorField.setEnabled(hasRange);
            rangeBorderWidthField.setEnabled(hasRange);
        }
        if (bgGridDistanceField != null) {
            boolean hasGrid = bgGridDistanceField.getText().trim().length() > 0;
            bgGridColorField.setEnabled(hasGrid);
            bgGridSubdivisionField.setEnabled(hasGrid);
        }
        if (bgImageField != null) {
            boolean hasBgImage = bgImageField.getText().trim().length() > 0;
            bgImageArrangement.setEnabled(hasBgImage);
        }

    }

    protected void updateConnectionEndpoint(boolean isSrcGate, ComboFieldEditor moduleField, TextFieldEditor moduleIndexField, ComboFieldEditor gateField, TextFieldEditor gateIndexField) {
        // resolve module
        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx) ((ConnectionElement)elements[0]).getEnclosingTypeElement();
        INedTypeInfo compoundModuleType = compoundModule.getNedTypeInfo();
        IConnectableElement module = null;
        if (!moduleField.isGrayed()) {
            String moduleName = moduleField.getText().trim();
            module = moduleName.equals(STR_PARENTMODULE) ? compoundModule : compoundModuleType.getSubmodules().get(moduleName);
        }
        if (module == null) {
            // cannot resolve module: clear gate combo's list
            moduleIndexField.setEnabled(true);
            gateField.setItems(ArrayUtils.EMPTY_STRING_ARRAY);
            gateIndexField.setEnabled(true);
            return;
        }
        
        // update moduleIndexField
        boolean isParent = (module instanceof CompoundModuleElementEx);  
        if (isParent) {  
            moduleIndexField.setEnabled(false);
        }
        else {
            SubmoduleElementEx submodule = (SubmoduleElementEx) module;
            moduleIndexField.setEnabled(StringUtils.isNotBlank(submodule.getVectorSize()));
        }

        // update gateField
        Boolean areBidirConns = isBidirectionalField.getGrayed() ? null : isBidirectionalField.getSelection();
        Map<String, GateElementEx> gateDecls = module.getGateDeclarations();
        String[] gateNames = makeGateNameList(areBidirConns, isSrcGate, isParent, gateDecls.values());
        Arrays.sort(gateNames);
        String[] gateLabels = new String[gateNames.length];
        for (int i=0; i<gateLabels.length; i++)
            gateLabels[i] = NedTreeUtil.getNedModelLabelProvider().getText(gateDecls.get(gateNames[i])); //TODO gatesize too!
        gateField.setItems(gateNames, gateLabels);

        // update gateIndexField
        if (module.getNedTypeInfo() == null) {
            gateIndexField.setEnabled(true);
        }
        else {
            // try resolve gate
            String gateName = gateField.getText().trim();
            GateElementEx gateDeclElement = gateDecls.get(gateName);
            gateIndexField.setEnabled(gateDeclElement == null || gateDeclElement.getIsVector());
        }
    }

    protected static String[] makeGateNameList(Boolean isBidirConn, boolean isSrcGate, boolean isParent, Collection<GateElementEx> gates) {
        List<String> result = new ArrayList<String>();
        for (GateElementEx gate : gates)
            if (isBidirConn == null ? gate.getType()!=(isSrcGate!=isParent ? INedElement.NED_GATETYPE_INPUT : INedElement.NED_GATETYPE_OUTPUT) : 
                isBidirConn ? gate.getType()==INedElement.NED_GATETYPE_INOUT : 
                    gate.getType()==(isSrcGate!=isParent ? INedElement.NED_GATETYPE_OUTPUT : INedElement.NED_GATETYPE_INPUT))
                result.add(gate.getName());
        return result.toArray(new String[]{});
    }

    protected void updatePreview() {
        // background preview
        if (bgPreviewFigure != null) {
            INedElement e = elements[elements.length-1]; // the primary selection in the editor (black handles)
            // set fallback display string
            DisplayString fallbackDisplayString = getFallbackDisplayString(e);
            
            DisplayString origDisplayString = ((IHasDisplayString)e).getDisplayString();
            DisplayString displayString = new DisplayString(origDisplayString.toString());
            displayString.setFallbackDisplayString(fallbackDisplayString); 
            
            updatePreviewDisplayString(displayString); 
            
            bgPreviewFigure.setDisplayString(displayString);
            Dimension compoundModuleSize = displayString.getCompoundSize(displayString.getScale());
            if (compoundModuleSize.height == -1)
                compoundModuleSize.height = 200;
            if (compoundModuleSize.width == -1)
                compoundModuleSize.width = 300;
            bgPreviewFigure.setSize(compoundModuleSize);
            bgPreviewFigure.validate();
        }

        // submodule / connection / type preview
        int x = 10;
        for (int i=0; i<previewFigure.length; ++i) {
            INedElement e = elements[i];
            // set fallback display string
            DisplayString fallbackDisplayString = getFallbackDisplayString(e);
            
            DisplayString origDisplayString = ((IHasDisplayString)e).getDisplayString();
            DisplayString displayString = new DisplayString(origDisplayString.toString());
            displayString.setFallbackDisplayString(fallbackDisplayString); 
            
            updatePreviewDisplayString(displayString); 
            
            // normal preview
            if (previewFigure != null) {
                if (e instanceof SubmoduleElement || e instanceof IModuleKindTypeElement) {
                    SubmoduleFigure submoduleFigure = (SubmoduleFigure)previewFigure[i];
                    float scale = 1.0f;
                    String name = (nameField.isEnabled() && !nameField.isGrayed()) ? nameField.getText() : ((IHasName)e).getName();
                    
                    if (e instanceof SubmoduleElement) {
                        SubmoduleElementEx submodule = (SubmoduleElementEx)e;
                        String vectorSize = (vectorSizeField.isEnabled() && !vectorSizeField.isGrayed()) ? vectorSizeField.getText() : ((SubmoduleElement)e).getVectorSize();
                        if (StringUtils.isNotBlank(vectorSize))
                            name = name + "[" + vectorSize + "]";
                        DisplayString parentDisplayString = submodule.getCompoundModule().getDisplayString();
                        scale = parentDisplayString.getScale();
                    }
                    
                    submoduleFigure.setName(name);
                    submoduleFigure.setDisplayString(scale, displayString);
                    int width = submoduleFigure.getSize().width;
                    submoduleFigure.setCenterLocation(new Point(x+width/2, 50));
                    x += width + 10; 
                }
                else {
                    ConnectionFigure connectionFigure = (ConnectionFigure)previewFigure[i]; 
                    connectionFigure.setDisplayString(displayString);
                    connectionFigure.setPoints(new PointList(new int[] {x, 10, x+40, 50}));
                    if (isBidirectionalField != null) {
                        if (isBidirectionalField.getGrayed())
                            connectionFigure.setArrowHeadEnabled(!((ConnectionElement)e).getIsBidirectional());
                        else
                            connectionFigure.setArrowHeadEnabled(!isBidirectionalField.getSelection());
                    }
                    x += 50;
                }
            }
        }
    }

    protected DisplayString getFallbackDisplayString(INedElement e) {
        IProject project = NedResourcesPlugin.getNedResources().getNedFile(elements[0].getContainingNedFileElement()).getProject();

        if (e instanceof ISubmoduleOrConnection) {
            if (typeField.isEnabled() && !typeField.isGrayed()) {
                String type = NedElementUtilEx.friendlyTypeNameToQName(typeField.getText());
                INedTypeInfo toplevelNedType = NedResourcesPlugin.getNedResources().getToplevelNedType(type, project); // FIXME add inner types too
                if (toplevelNedType != null)
                    return toplevelNedType.getNedElement().getDisplayString();
                else 
                    return new DisplayString("");
            }
        }
        if (e instanceof INedTypeElement) {
            if (extendsField.isEnabled() && !extendsField.isGrayed()) {
                String superType = NedElementUtilEx.friendlyTypeNameToQName(extendsField.getText());
                INedTypeInfo toplevelNedType = NedResourcesPlugin.getNedResources().getToplevelNedType(superType, project); // FIXME add inner types too
                if (toplevelNedType != null)
                    return toplevelNedType.getNedElement().getDisplayString();
                else 
                    return new DisplayString("");
            }
        }

        return ((IHasDisplayString)e).getDisplayString().getFallbackDisplayString();
    }

    protected void updatePreviewDisplayString(DisplayString displayString) {
        // update display string
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.X, xField);
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.Y, yField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.LAYOUT, layoutField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.LAYOUT_PAR1, layoutPar1Field); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.LAYOUT_PAR2, layoutPar2Field); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.LAYOUT_PAR3, layoutPar3Field); 

        // B tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.SHAPE, shapeField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.SHAPE_WIDTH, shapeWidthField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.SHAPE_HEIGHT, shapeHeightField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.SHAPE_FILL_COLOR, shapeFillColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.SHAPE_BORDER_COLOR, shapeBorderColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.SHAPE_BORDER_WIDTH, shapeBorderWidthField); 

        // I tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE, imageField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE_COLOR, imageColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE, imageColorPercentageField); 

        // IS tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE_SIZE, imageSizeField); 

        // I2 tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE2, image2Field); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE2_COLOR, image2ColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.IMAGE2_COLOR_PERCENTAGE, image2ColorPercentageField); 

        // R tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.RANGE, rangeField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.RANGE_FILL_COLOR, rangeFillColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.RANGE_BORDER_COLOR, rangeBorderColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.RANGE_BORDER_WIDTH, rangeBorderWidthField); 

        // Q tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.QUEUE_NAME, queueNameField); 

        // T tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.TEXT, textField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.TEXT_POS, textPosField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.TEXT_COLOR, textColorField); 

        // TT tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.TOOLTIP, tooltipField); 

        //============== CONNECTION ================
        // LS tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.CONNECTION_COLOR, connectionColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.CONNECTION_WIDTH, connectionWidthField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.CONNECTION_STYLE, connectionStyleField); 

        //============== COMPOUND MODULE ================
        // BGP tag: not edited because its semantics is unclear and it is ignored by the graphical editor 
        //updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_X, bgXField); 
        //updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_Y, bgYField); 

        // BGB tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_WIDTH, bgWidthField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_HEIGHT, bgHeightField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_FILL_COLOR, bgFillColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_BORDER_COLOR, bgBorderColorField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_BORDER_WIDTH, bgBorderWidthField); 

        // BGTT tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_TOOLTIP, bgTooltipField); 

        // BGI tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_IMAGE, bgImageField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_IMAGE_ARRANGEMENT, bgImageArrangement); 

        // BGG tag
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_GRID_DISTANCE, bgGridDistanceField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_GRID_SUBDIVISION, bgGridSubdivisionField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_GRID_COLOR, bgGridColorField); 

        // module layouting 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_LAYOUT_SEED, bgLayoutSeedField); 
        //updatePreviewDisplayProperty(IDisplayString.Prop.MODULE_LAYOUT_ALGORITHM, bgLayoutAlgorithmField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_SCALE, bgScaleField); 
        updatePreviewDisplayProperty(displayString, IDisplayString.Prop.MODULE_UNIT, bgUnitField);
    }

    protected void updatePreviewDisplayProperty(DisplayString displayString, IDisplayString.Prop property, IFieldEditor fieldEditor) {
        if (fieldEditor != null && !fieldEditor.isGrayed()) {
            String value = fieldEditor.getText();

            // normalize value (replace enum value with its standard abbreviation)
            if (!value.equals("") && property.getEnumSpec() != null) {
                String shorthand = property.getEnumSpec().getShorthandFor(value);
                if (shorthand != null)
                    value = shorthand;
            }

            Assert.isNotNull(value);
            String newValue = getLocalDisplayPropertyChange(displayString, property, value);
            if (newValue != null)
                displayString.set(property, newValue);
        }
    }
    
    @Override
	protected void okPressed() {
        // creating the result command
        CompoundCommand compoundCommand = new CompoundCommand("Edit properties");

        // name, inheritance, etc.
        if (nameField != null && nameField.isEnabled() && !nameField.isGrayed())
            addPropertyChangeCommands(compoundCommand, new NamePropertyAccess(), nameField.getText());

        // inheritance
        if (elements[0] instanceof INedTypeElement) {
            if (!extendsField.isGrayed()) {
                List<String> extendsList = NedTypeFieldEditor.parseNedTypeList(extendsField.getText());
                Assert.isTrue(extendsList.size() <= 1 || elements[0] instanceof IInterfaceTypeElement, "Multiple 'extends' types for module/channel"); // validation should have filtered this out
                addPropertyChangeCommands(compoundCommand, new ExtendsPropertyAccess(), extendsList);
            }

            if (!(elements[0] instanceof IInterfaceTypeElement) && !interfacesField.isGrayed()) {
                List<String> interfacesList = NedTypeFieldEditor.parseNedTypeList(interfacesField.getText());
                addPropertyChangeCommands(compoundCommand, new InterfacesPropertyAccess(), interfacesList);
            }
            
            if (!documentationField.isGrayed())
                addPropertyChangeCommands(compoundCommand, new DocumentationPropertyAccess(), documentationField.getText());
        }

        // submodule or connection 
        if (elements[0] instanceof ISubmoduleOrConnection) {
            // vector size
            if (elements[0] instanceof SubmoduleElement && !vectorSizeField.isGrayed())
                addPropertyChangeCommands(compoundCommand, new VectorSizePropertyAccess(), vectorSizeField.getText().trim());

            if (!typeField.isEnabled() || !typeField.isGrayed()) {
                String type = typeField.isEnabled() ? NedElementUtilEx.friendlyTypeNameToQName(typeField.getText()) : ""; // if disabled, clear the attribute 
                addPropertyChangeCommands(compoundCommand, new TypePropertyAccess(), type);
            }
            
            if (!likeTypeField.isEnabled() || !likeTypeField.isGrayed()) {
                String likeType = likeTypeField.isEnabled() ? NedElementUtilEx.friendlyTypeNameToQName(likeTypeField.getText()) : ""; // if disabled, clear the attribute
                addPropertyChangeCommands(compoundCommand, new LikeTypePropertyAccess(), likeType);
            }

            addPropertyChangeCommands(compoundCommand, new LikeParamPropertyAccess(), likeParamField);
        }

        // "Endpoints" page
        if (elements[0] instanceof ConnectionElement) {
            // "bidirectional" checkbox
            if (!isBidirectionalField.getGrayed()) {
                String value = isBidirectionalField.getSelection() ? "true" : "false";
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_IS_BIDIRECTIONAL), value);
            }
            
            // source gate
            if (!connSrcModuleField.isGrayed()) {
                String value = connSrcModuleField.getText().trim();
                if (value.equals(STR_PARENTMODULE)) 
                    value = "";
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_MODULE), value);
            }
            addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_MODULE_INDEX), connSrcModuleIndexField);
            addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_GATE), connSrcGateField);
            if (!connSrcGateIndexField.isEnabled() || !connSrcGateIndexField.isGrayed()) {
                String value = connSrcGateIndexField.isEnabled() ? connSrcGateIndexField.getText().trim() : "";
                boolean isPlusPlus = value.equals(STR_GATEPLUSPLUS);
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_GATE_INDEX), isPlusPlus ? "" : value);
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_SRC_GATE_PLUSPLUS), isPlusPlus ? "true" : "false");
            }

            // destination gate
            if (!connDestModuleField.isGrayed()) {
                String value = connDestModuleField.getText().trim();
                if (value.equals(STR_PARENTMODULE)) 
                    value = "";
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_MODULE), value);
            }
            addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_MODULE_INDEX), connDestModuleIndexField);
            addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_GATE), connDestGateField);
            if (!connDestGateIndexField.isEnabled() || !connDestGateIndexField.isGrayed()) {
                String value = connDestGateIndexField.isEnabled() ? connDestGateIndexField.getText().trim() : "";
                boolean isPlusPlus = value.equals(STR_GATEPLUSPLUS);
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_GATE_INDEX), isPlusPlus ? "" : value);
                addPropertyChangeCommands(compoundCommand, new ElementAttributePropertyAccess(ConnectionElement.ATT_DEST_GATE_PLUSPLUS), isPlusPlus ? "true" : "false");
            }
        }
        
        // display string properties
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.X, xField);
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.Y, yField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.LAYOUT, layoutField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.LAYOUT_PAR1, layoutPar1Field); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.LAYOUT_PAR2, layoutPar2Field); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.LAYOUT_PAR3, layoutPar3Field); 

        // B tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.SHAPE, shapeField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.SHAPE_WIDTH, shapeWidthField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.SHAPE_HEIGHT, shapeHeightField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.SHAPE_FILL_COLOR, shapeFillColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.SHAPE_BORDER_COLOR, shapeBorderColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.SHAPE_BORDER_WIDTH, shapeBorderWidthField); 

        // I tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE, imageField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE_COLOR, imageColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE_COLOR_PERCENTAGE, imageColorPercentageField); 

        // IS tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE_SIZE, imageSizeField); 

        // I2 tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE2, image2Field); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE2_COLOR, image2ColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.IMAGE2_COLOR_PERCENTAGE, image2ColorPercentageField); 

        // R tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.RANGE, rangeField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.RANGE_FILL_COLOR, rangeFillColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.RANGE_BORDER_COLOR, rangeBorderColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.RANGE_BORDER_WIDTH, rangeBorderWidthField); 

        // Q tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.QUEUE_NAME, queueNameField); 

        // T tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.TEXT, textField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.TEXT_POS, textPosField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.TEXT_COLOR, textColorField); 

        // TT tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.TOOLTIP, tooltipField); 

        //============== CONNECTION ================
        // LS tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.CONNECTION_COLOR, connectionColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.CONNECTION_WIDTH, connectionWidthField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.CONNECTION_STYLE, connectionStyleField); 

        //============== COMPOUND MODULE ================
        // BGP tag: not edited because its semantics is unclear and it is ignored by the graphical editor 
        //addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_X, bgXField); 
        //addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_Y, bgYField); 

        // BGB tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_WIDTH, bgWidthField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_HEIGHT, bgHeightField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_FILL_COLOR, bgFillColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_BORDER_COLOR, bgBorderColorField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_BORDER_WIDTH, bgBorderWidthField); 

        // BGTT tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_TOOLTIP, bgTooltipField); 

        // BGI tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_IMAGE, bgImageField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_IMAGE_ARRANGEMENT, bgImageArrangement); 

        // BGG tag
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_GRID_DISTANCE, bgGridDistanceField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_GRID_SUBDIVISION, bgGridSubdivisionField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_GRID_COLOR, bgGridColorField); 

        // module layouting 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_LAYOUT_SEED, bgLayoutSeedField); 
        //addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_LAYOUT_ALGORITHM, bgLayoutAlgorithmField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_SCALE, bgScaleField); 
        addDisplayPropertyChangeCommands(compoundCommand, IDisplayString.Prop.MODULE_UNIT, bgUnitField); 

        resultCommand = compoundCommand;
        
        super.okPressed();
    }

    protected void addDisplayPropertyChangeCommands(CompoundCommand compoundCommand, IDisplayString.Prop displayProperty, IFieldEditor fieldEditor) {
        if (fieldEditor != null && !fieldEditor.isGrayed()) {
            // if field is disabled, remove its value from the display string
            String value = fieldEditor.isEnabled() ? fieldEditor.getText() : null;
            addPropertyChangeCommands(compoundCommand, new DisplayPropertyAccess(displayProperty), value);
        }
    }
    
    /**
     * Sets the given property to the value taken from the field editor. If the field editor is grayed, 
     * does nothing; if it is disabled, sets the property to the empty string.   
     */
    protected void addPropertyChangeCommands(CompoundCommand compoundCommand, IPropertyAccess property, IFieldEditor fieldEditor) {
        if (!fieldEditor.isEnabled() || !fieldEditor.isGrayed()) {
            String value = fieldEditor.isEnabled() ? fieldEditor.getText().trim() : ""; // if disabled, clear the attribute
            addPropertyChangeCommands(compoundCommand, property, value);
        }
    }

    protected void addPropertyChangeCommands(CompoundCommand compoundCommand, IPropertyAccess property, Object value) {
        for (INedElement e : elements)
            if (!ObjectUtils.equals(value, property.getValue(e)))
                compoundCommand.add(property.makeChangeCommand(e, value));
    }

    /**
     * Tells what to change a local display string property to, so that it yields the given value 
     * when considering fallback display strings, default values, etc.
     *  
     * It returns the new value, or null if no change is required.
     * 
     * This same function is used by both the preview and the dialog commit function so we can be
     * sure the preview is accurate.
     *
     * Strategy:
     *   1. if existing display string already results in the desired value, whichever way  
     *       (explicitly, via inherited value or default value), leave it unchanged
     *   2. if not, try to achieve that with the shortest possible display string:
     *      2a. if the inherited value is the same as the desired one, remove local
     *          value so that it doesn't shadow inherited value
     *      2b. otherwise explicitly set the desired value (but if it's an empty string,
     *          we may use "-" (the antivalue) to neutralize inherited value
     */
    protected static String getLocalDisplayPropertyChange(DisplayString displayString, IDisplayString.Prop property, String value) {
        String existingValue = displayString.getAsString(property, false, false);
        if (ObjectUtils.equals(value, existingValue)) {
            return null; // already OK, no change needed
        }
        else {
            String newValue;
            DisplayString fallbackDisplayString = displayString.getFallbackDisplayString();
            String inheritedValue = fallbackDisplayString==null ? "" : StringUtils.nullToEmpty(fallbackDisplayString.getAsString(property, false, false));
            if (value.equals(inheritedValue)) {
                newValue = "";  // remove local value to let inherited value through
            }
            else if (value.equals("")) {
                // to create an empty value, we may need to use "-" (the antivalue) to kill off the inherited one  
                newValue = inheritedValue.equals("") ? "" : "-";
            } 
            else {
                newValue = value;
            }
            return newValue;
        }
    }
    
    @Override
    public boolean close() {
        // save which page is shown
        getDialogSettings().put("activePage", tabfolder.getSelectionIndex());
       
        return super.close();
    }

    public Command getResultCommand() {
        return resultCommand;
    }

    protected void validateDialogContents() {
        Button okButton = getButton(IDialogConstants.OK_ID);
        if (okButton == null)
            return;  // we are still within createDialogArea(), when createButtonsForButtonBar() was not yet called

        // GUIDELINE: only add "error" for issues that will make NED unparseable in text mode; the rest (no such gate, etc) are only warnings!
        
        Map<IFieldEditor, String> errors = new LinkedHashMap<IFieldEditor, String>();
        
        // name
        if (elements[0] instanceof INedTypeElement && nameField.isEnabled() && !nameField.isGrayed())
            addErrorIfNotNull(errors, nameField, "Name", new TypeNameValidator((INedTypeElement)elements[0]).isValid(nameField.getText()));
        else if (elements[0] instanceof SubmoduleElementEx && nameField.isEnabled() && !nameField.isGrayed())
            addErrorIfNotNull(errors, nameField, "Name", new SubmoduleNameValidator((SubmoduleElementEx)elements[0]).isValid(nameField.getText()));

        // submodule size, like param expression
        if (vectorSizeField != null && vectorSizeField.isEnabled() && !vectorSizeField.isGrayed())
            validateExpressionField(errors, vectorSizeField, "Vector size", true, null);

        if (likeParamField != null)
            validateExpressionField(errors, likeParamField, "Submodule type name expression", true, null);

        // validate type and type list fields
        if (elements[0] instanceof INedTypeElement) {
            validateNedTypeField(errors, "Extends", extendsField, true);
            validateNedTypeField(errors, "Interfaces", interfacesField, true);
        }
        if (elements[0] instanceof ISubmoduleOrConnection) {
            if (fixedTypeRadioButton.getSelection())
                validateNedTypeField(errors, "Type", typeField, elements[0] instanceof ConnectionElement);
            else
                validateNedTypeField(errors, "Interface", likeTypeField, false);
        }
        
        // validate connection stuff
        if (elements[0] instanceof ConnectionElement) {
            validateConnectionGate(errors, true, connSrcModuleField, connSrcModuleIndexField, connSrcGateField, connSrcGateIndexField);
            validateConnectionGate(errors, false, connDestModuleField, connDestModuleIndexField, connDestGateField, connDestGateIndexField);
        }

        // graphical properties
        if (bgWidthField != null && bgHeightField != null && !bgWidthField.isGrayed() && !bgHeightField.isGrayed()) {
            // our layouter cannot handle it if only width or height is given 
            if (StringUtils.isBlank(bgWidthField.getText()) != StringUtils.isBlank(bgHeightField.getText())) {
                IFieldEditor bogusField = StringUtils.isBlank(bgWidthField.getText()) ? bgWidthField : bgHeightField;
                addErrorIfNotNull(errors, bogusField, "Background", "Width and height must be specified together, or both of them should be left empty");
            }
        }
        
        boolean hasError = synchronizeErrors(errors);

        okButton.setEnabled(!hasError);
    }

    protected void validateNedTypeField(Map<IFieldEditor, String> errors, String fieldName, NedTypeFieldEditor fieldEditor, boolean canBeEmpty) {
        if (fieldEditor.isEnabled() && !fieldEditor.isGrayed()) {
            String value = fieldEditor.getText();
            if (canBeEmpty && StringUtils.isBlank(value))
                return;
            
            // check syntax
            String msg = isValidTypeName(value, fieldEditor.getMultiple(), canBeEmpty);
            if (msg != null) {
                addErrorIfNotNull(errors, fieldEditor, fieldName, msg);
                return;
            }
            
            // check whether such type really exists
            INedTypeResolver resolver = NedElement.getDefaultNedTypeResolver();
            IProject project = NedResourcesPlugin.getNedResources().getNedFile(elements[0].getContainingNedFileElement()).getProject();
            for (String v : value.split(",")) {
                String qname = NedElementUtilEx.friendlyTypeNameToQName(v);
                INedTypeInfo nedType = resolver.getToplevelNedType(qname, project);
                if (nedType == null)
                    addWarningIfNotNull(errors, fieldEditor, fieldName, "No such NED type");
                else if (!fieldEditor.getTypeFilter().matches(nedType)) {               
                    String what = nedType.getNedElement().getReadableTagName();
                    addWarningIfNotNull(errors, fieldEditor, fieldName, StringUtils.indefiniteArticle(what, true) + " " + what + " is not allowed here");
                }
            }
        }
    }

    protected void validateConnectionGate(Map<IFieldEditor, String> errors, boolean isSrcGate, IFieldEditor moduleField, IFieldEditor moduleIndexField, IFieldEditor gateField, IFieldEditor gateIndexField) {
        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx) ((ConnectionElement)elements[0]).getEnclosingTypeElement();
        INedTypeInfo compoundModuleType = compoundModule.getNedTypeInfo();
        String prefix = isSrcGate ? "Source" : "Destination";

        // validate module
        IConnectableElement module = null;
        if (!moduleField.isGrayed()) {
            String moduleName = moduleField.getText().trim();
            if (!moduleName.equals(STR_PARENTMODULE))
                addErrorIfNotNull(errors, moduleField, prefix+" module name", isValidIdentifier(moduleName, false));

            module = moduleName.equals(STR_PARENTMODULE) ? compoundModule : compoundModuleType.getSubmodules().get(moduleName);
            if (module == null)
                addErrorIfNotNull(errors, moduleField, prefix+" module name", "No such submodule"); // cannot be a warning: committing the dialog would destroy the connection in the graphical editor!
        }

        // validate module index
        if (moduleIndexField.isEnabled() && !moduleIndexField.isGrayed()) {
            validateExpressionField(errors, moduleIndexField, prefix+" module index", module==null, null);
        }
        
        // validate gate name
        GateElementEx gateDecl = null;
        if (!gateField.isGrayed()) {
            String gateName = gateField.getText().trim();
            addErrorIfNotNull(errors, gateField, prefix+" gate name", isValidIdentifier(gateName, false));
            if (module != null) {
                INedTypeInfo type = module.getNedTypeInfo();
                if (type != null) {
                    gateDecl = type.getGateDeclarations().get(gateName);
                    if (gateDecl == null)
                        addWarningIfNotNull(errors, gateField, prefix+" gate name", NedElementUtilEx.qnameToFriendlyTypeName(type.getFullyQualifiedName()) + " does not have such gate");
                }
            }
        }

        // validate gate index
        if (gateIndexField.isEnabled() && !gateIndexField.isGrayed()) {
            validateExpressionField(errors, gateIndexField, prefix+" gate index", gateDecl==null, STR_GATEPLUSPLUS);
        }
    }

    protected void validateExpressionField(Map<IFieldEditor, String> errors, IFieldEditor fieldEditor, String fieldName, boolean canBeEmpty, String alsoAllowedValue) {
        if (fieldEditor.isEnabled() && !fieldEditor.isGrayed()) {
            String value = fieldEditor.getText().trim();
            if (StringUtils.isEmpty(value)) {
                if (!canBeEmpty)
                    addErrorIfNotNull(errors, fieldEditor, fieldName, "Field cannot be empty");
            }
            else if (alsoAllowedValue != null && value.equals(alsoAllowedValue)) {
                // OK
            }
            else if (!NedTreeUtil.isExpressionValid(value)) {
                addErrorIfNotNull(errors, fieldEditor, fieldName, "Syntax error in expression");
            }
        }
    }

    protected String isValidIdentifier(String text, boolean canBeEmpty) {
        if (StringUtils.isBlank(text))
            return canBeEmpty ? null : "Field cannot be empty";
        if (!NedElementUtilEx.isValidIdentifier(text))
            return "Not a valid identifier";
        return null;
    }

    protected String isValidTypeName(String text, boolean commaSeparatedList, boolean canBeEmpty) {
        if (StringUtils.isBlank(text))
            return canBeEmpty ? null : "Field cannot be empty";
        if (!commaSeparatedList && text.indexOf(',') != -1)
            return "Only one type name is allowed";
        for (String value : text.split(","))
            if (!NedElementUtilEx.isValidFriendlyQualifiedIdentifier(value.trim()))
                return "Syntax error in type name";
        return null;
    }

    /**
     * Adds an error message to the given errors list. If the message is null or if the given 
     * field editor already has an error/warning message assigned, the method does nothing. 
     * (One field can only display one message at a time.) 
     */
    protected void addErrorIfNotNull(Map<IFieldEditor, String> errors, IFieldEditor fieldEditor, String fieldName, String message) {
        Assert.isTrue(fieldName.indexOf(':') == -1); // prefix cannot contain colon, otherwise we cannot strip it off in synchronizeErrors()
        if (message != null && !errors.containsKey(fieldEditor)) // only one message is added per field
            errors.put(fieldEditor, "E " + fieldName + ": " + message);
    }
    
    /**
     * Adds a warning message to the given errors list. If the message is null or if the given 
     * field editor already has an error/warning message assigned, the method does nothing. 
     * (One field can only display one message at a time.) 
     */
    protected void addWarningIfNotNull(Map<IFieldEditor, String> errors, IFieldEditor fieldEditor, String fieldName, String message) {
        Assert.isTrue(fieldName.indexOf(':') == -1); // prefix cannot contain colon, otherwise we cannot strip it off in synchronizeErrors()
        if (message != null && !errors.containsKey(fieldEditor)) // only one message is added per field
            errors.put(fieldEditor, "W " + fieldName + ": " + message);
    }

    /**
     * Displays the given errors in the dialog. Returns true if there are errors in the list, 
     * false if there are none (it is empty or all warnings). 
     */
    protected boolean synchronizeErrors(Map<IFieldEditor, String> newErrors) {
        // update field error markers that changed (we mustn't touch them if they haven't changed,
        // otherwise the whole dialog contents will flicker at each keypress)
        for (IFieldEditor fieldEditor : newErrors.keySet()) {
            if (!newErrors.get(fieldEditor).equals(validationErrors.get(fieldEditor))) {
                String str = newErrors.get(fieldEditor);
                Assert.isTrue(str.charAt(0)=='E' || str.charAt(0)=='W');
                int severity = str.charAt(0)=='E' ? IMarker.SEVERITY_ERROR : IMarker.SEVERITY_WARNING;
                fieldEditor.setMessage(severity, StringUtils.substringAfter(str, ":").trim());
            }
        }
        for (IFieldEditor fieldEditor : validationErrors.keySet())
            if (!newErrors.containsKey(fieldEditor))
                fieldEditor.setMessage(IMarker.SEVERITY_ERROR, null); // clear

        // set message label at the bottom of the dialog
        String[] messages = newErrors.values().toArray(new String[]{});
        // find first error
        String firstError = null;
        for (String msg : messages)
            if (msg.charAt(0)=='E') {firstError = msg; break;}
        errorImageLabel.setImage(messages.length == 0 ? null : firstError==null ? ICON_WARNING : ICON_ERROR);
        errorMessageLabel.setText(messages.length == 0 ? "" : firstError==null ? messages[0].substring(2) : firstError.substring(2));
        errorMessageLabel.setForeground(firstError==null ? ColorFactory.YELLOW4 : ColorFactory.RED2);

        validationErrors = newErrors;
        return firstError != null;
    }

    //TODO move to some UIUtils or so, together with similar stuff in InifileEditor field editors
    protected static void updateProblemDecoration(ControlDecoration problemDecoration, int severity, String text) {
        problemDecoration.setImage(severity==IMarker.SEVERITY_ERROR ? ICON_ERROR_SMALL : ICON_WARNING_SMALL);
        problemDecoration.setDescriptionText(text);
        if (text != null) 
            problemDecoration.show();
        else { 
            problemDecoration.hide();
            problemDecoration.hideHover();
        }
    }
}


