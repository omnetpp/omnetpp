package org.omnetpp.scave.editors.forms;

import static org.omnetpp.scave.computed.ComputedScalarEngine.FIELD_GROUPBY;
import static org.omnetpp.scave.computed.ComputedScalarEngine.FIELD_MODULE;
import static org.omnetpp.scave.computed.ComputedScalarEngine.FIELD_NAME;
import static org.omnetpp.scave.computed.ComputedScalarEngine.FIELD_VALUE;
import static org.omnetpp.scave.computed.ComputedScalarEngine.GROUPBY_PREDEFINED_VARIABLES;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.ui.HelpLink;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StatusUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.computed.ComputedScalarEngine;
import org.omnetpp.scave.computed.ExpressionEvaluator.Function;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.ComputeScalar;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Edit form for ComputeScalar operations.
 *
 * @author tomi
 */
public class ComputeScalarEditForm extends BaseScaveObjectEditForm {
    protected static final EStructuralFeature[] features = new EStructuralFeature[] {
        ScaveModelPackage.eINSTANCE.getComputeScalar_ScalarName(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_ValueExpr(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_ModuleExpr(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_GroupByExpr(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_AverageReplications(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_ComputeStddev(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_ComputeConfidenceInterval(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_ConfidenceLevel(),
        ScaveModelPackage.eINSTANCE.getComputeScalar_ComputeMinMax(),
    };

    private Set<String> statistics; // names of scalars, vectors, and histograms in the input dataset
    private Set<String> moduleHints; // module name hints; used only for content assist

    // text field with error/warning decoration
    private static class TextField {
        Text text;
        ControlDecoration decoration;

        private static final Image
            ERROR_IMAGE = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_DEC_FIELD_ERROR),
            WARNING_IMAGE = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_DEC_FIELD_WARNING);

        TextField(Text text) {
            this.text = text;
            decoration = new ControlDecoration(text, SWT.BOTTOM | SWT.LEFT);
            decoration.hide();
        }

        void setStatus(IStatus status) {
            if (status != null && status.matches(IStatus.ERROR)) {
                decoration.setImage(ERROR_IMAGE);
                decoration.setDescriptionText(StatusUtil.getFirstDescendantWithSeverity(status, IStatus.ERROR).getMessage());
                decoration.show();
            }
            else if (status != null && status.matches(IStatus.WARNING)) {
                decoration.setImage(WARNING_IMAGE);
                decoration.setDescriptionText(StatusUtil.getFirstDescendantWithSeverity(status, IStatus.WARNING).getMessage());
                decoration.show();
            }
            else
                decoration.hide();
        }
    }

    private TextField scalarNameField;
    private TextField valueField;
    private TextField moduleField;
    private TextField groupByField;
    private Button averageReplicationsButton;
    private Composite optionsPanel;
    private Button computeStddevButton;
    private Button computeConfIntervalButton;
    private Combo confidenceLevelCombo;
    private Button computeMinMaxButton;

    public ComputeScalarEditForm(ComputeScalar object, EObject parent, EStructuralFeature feature, int index)
    {
        super(object, parent);
        Assert.isNotNull(object);
        Assert.isNotNull(parent);

        ResultFileManager manager = ScaveModelUtil.getResultFileManagerFor(parent);
        Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
        if (manager != null && dataset != null) {
            IDList input;
            if (object.eContainer() == parent) {
                input = DatasetManager.getIDListFromDataset(manager, dataset, object, true, null);
                input = DatasetManager.select(input, object.getFilters(), manager, null);
            }
            else {
                if (feature == null)
                    feature = parent.eClass().getEStructuralFeature("items");
                @SuppressWarnings("unchecked")
                EObject prevSibling = feature != null && index >= 1 ? ((EList<EObject>)parent.eGet(feature)).get(index - 1) : null;
                Assert.isTrue(prevSibling == null || prevSibling instanceof DatasetItem);
                input = DatasetManager.getIDListFromDataset(manager, dataset, (DatasetItem)prevSibling, null);
            }

            StringVector names = manager.getUniqueNames(input).keys();
            statistics = new TreeSet<String>();
            for (int i = 0; i < names.size(); ++i) {
                statistics.add(names.get(i));
            }
            StringVector moduleNameHints = manager.getModuleFilterHints(input);
            moduleHints = new TreeSet<String>();
            for (int i = 0; i < moduleNameHints.size(); ++i) {
                moduleHints.add(moduleNameHints.get(i));
            }
        }
        if (statistics == null)
            this.statistics = Collections.emptySet();
        if (moduleHints == null)
            this.moduleHints = Collections.emptySet();
    }

    public void populatePanel(Composite panel) {
        panel.setLayout(new GridLayout());

        // measure the width of the longest label
        GC gc = new GC(panel);
        gc.setFont(panel.getFont());
        FontMetrics fontMetrics = gc.getFontMetrics();
        int maxLabelWidth = Dialog.convertWidthInCharsToPixels(fontMetrics, "Grouping:".length());
        gc.dispose();

        // Compute group
        Group group = new Group(panel, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        group.setText("Compute:");
        group.setLayout(new GridLayout(2, false));

        valueField = createField(FIELD_VALUE, "Value",
                                "Enter an arithmetic expression for the value of the generated scalars. <A>Click for details</A>",
                                VALUE_FIELD_HELP, group, maxLabelWidth);
        groupByField = createField(FIELD_GROUPBY, "Grouping",
                "Enter an expression for grouping scalars by module before applying aggregate functions (mean, sum, etc.). <A>Click for details</A>",
                GROUPBY_FIELD_HELP,
                group, maxLabelWidth);

        // 'Store as' group
        group = new Group(panel, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        group.setText("Store as:");
        group.setLayout(new GridLayout(2, false));
        scalarNameField = createField(FIELD_NAME, "Name",
                "Name for the scalar. May contain dollar variables or their expressions. <A>Click for details</A>",
                NAME_FIELD_HELP, group, maxLabelWidth);
        moduleField = createField(FIELD_MODULE, "Module",
                "Enter a module path. May contain dollar variables or their expressions. <A>Click for details</A>",
                MODULE_FIELD_HELP, group, maxLabelWidth);

        // Average replications
        group = new Group(panel, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        group.setText("Averaging:");
        group.setLayout(new GridLayout());
        HelpLink link = new HelpLink(group, SWT.NONE);
        link.getLink().setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        link.setText("Select this checkbox to compute average values across repetitions " +
                      "instead of values for each repetition. <A>Click for details</A>");
        link.setHoverText(AVERAGE_FIELD_HELP);

        averageReplicationsButton = new Button(group, SWT.CHECK);
        averageReplicationsButton.setText("Average replications");
        optionsPanel = new Composite(group, SWT.NONE);
        RowLayout optionsPanelLayout = new RowLayout(SWT.VERTICAL);
        optionsPanelLayout.marginLeft = 30;
        optionsPanel.setLayout(optionsPanelLayout);
        optionsPanel.setEnabled(false);
        Label label = new Label(optionsPanel, SWT.WRAP);
        label.setText("Generate additional scalars:");
        label.setEnabled(false);
        computeStddevButton = new Button(optionsPanel, SWT.CHECK);
        computeStddevButton.setText("standard deviation");
        computeStddevButton.setEnabled(false);
        Composite confIntPanel = new Composite(optionsPanel, SWT.NONE);
        RowLayout confIntPanelLayout = new RowLayout();
        confIntPanelLayout.center = true;
        confIntPanelLayout.marginLeft = 0;
        confIntPanel.setLayout(confIntPanelLayout);
        computeConfIntervalButton = new Button(confIntPanel, SWT.CHECK);
        computeConfIntervalButton.setText("confidence interval with confidence level ");
        computeConfIntervalButton.setEnabled(false);
        confidenceLevelCombo = new Combo(confIntPanel, SWT.BORDER);
        confidenceLevelCombo.setLayoutData(new RowData(120, SWT.DEFAULT));
        confidenceLevelCombo.setItems(new String[] {"90%", "95%", "99%", "99.9%"});
        confidenceLevelCombo.select(0);
        confidenceLevelCombo.setEnabled(false);
        computeMinMaxButton = new Button(optionsPanel, SWT.CHECK);
        computeMinMaxButton.setText("minimum and maximum");
        computeMinMaxButton.setEnabled(false);

        averageReplicationsButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setEnabled(averageReplicationsButton.getSelection(), optionsPanel, 2);
            }
        });
    }

    private static final String
        VALUE_FIELD_HELP = "<p>An arithmetic expression composed of constants, scalar values, vector or histogram fields, " +
                           "operators, and functions." +
                           "<h1>Constants</h1>"+
                           "<p>Numeric constants: <i>-1, 3.14159, 4.2e1</i>" +
                           "<p>In boolean expressions <i>0</i> represents <i>false</i>, any other value is <i>true</i>." +
                           "<h1>Scalars</h1>" +
                           "<p>Simply use the scalar name (e.g. <i>pkLoss</i>), or enclose it with apostrophes " +
                           "if the name of the scalar contains special characters (e.g. <i>'rcvdPk:count'</i>.) " +
                           "If there are several scalars with that name in the input dataset, usually recorded by different modules, then " +
                           "the computation is iterated over the modules. The scalar name cannot contain wildcards or dollar variables." +
                           "<p>Qualified names (i.e. <i>&lt;module&gt;.&lt;scalar&gt;</i>) can also be used to name scalars. The module " +
                           "part can be a pattern that is matched against the full paths of modules. The same pattern syntax is used as in ini files " +
                           "and in other parts of the Analysis Tool (Quick reminder: use <i>*</i> and <i>**</i> for wildcards, <i>{5..10}</i> for numeric range, " +
                           "<i>[5..10]</i> for module index range). If multiple scalars match " +
                           "the qualified name, they will be iterated, i.e. the expression will be computed for each value. This means that " +
                           "if there are several such pattern in the expression, then the computation is performed on their Cartesian product." +
                           "<p>If the expression mentions several unqualified scalars, they are expected to come from the same module. For example, " +
                           "if your expression is <i>foo+bar</i> but the <i>foo</i> and <i>bar</i> scalars have been recorded by different modules, " +
                           "the result will be empty." +
                           "<p>The iteration can be restricted by binding some part of the module name to variables, and use those variables " +
                           "in other patterns. The <i>${x=&lt;pattern&gt;}</i> syntax in a module name pattern binds the part of the module name " +
                           "matched to <i>pattern</i> to a variable named <i>x</i>. These variables can be referred as <i>${x}</i> in other " +
                           "patterns. The <i>${...}</i> syntax also allows you to embed expressions (e.g. <i>${x+1}</i>) into the pattern. " +
                           "For example, if there are 3 clients (<i>cli0,cli1,cli2</i>) and 3 servers (<i>srv0,srv1,srv2</i>), " +
                           "and each client sends packets to the corresponding server, then the packet losses can be computed from the number of " +
                           "sent and received packets as: " +
                           "<p><i>cli${i={0..2}}.pkSent - srv${i}.pkRcvd</i>." +
                           "<h1>Vectors and histograms</h1>" +
                           "<p>Use <i>count(&lt;name&gt;), mean(&lt;name&gt;), min(&lt;name&gt;), max(&lt;name&gt;), stddev(&lt;name&gt;), variance(&lt;name&gt;)</i> " +
                           "to access the fields of vectors and histograms. Here <i>&lt;name&gt;</i> is the name of the vector or histogram (quoted if necessary)." +
                           "<p>The same rules apply to qualified names and iterations over modules as in the case of scalars." +
                           "<h1>Functions</h1>" +
                           "<p>The following functions can be applied to scalars or to an expression that yields a scalar value: " +
                           "<i>count(), mean(), min(), max(), stddev(), variance()</i>. " +
                           "These aggregate functions produce one value for a group of scalars instead of one for each scalar. " +
                           "By default, each scalar belongs to the same group, but it is possible to group them by module name (see Grouping). " +
                           "Aggregate functions cannot cross simulation run boundaries, e.g they cannot be used to compute average over " +
                           "all replications of the same configuration; use the 'Average replications' checkbox for that." +
                           "<h1>Operators</h1>" +
                           "<p>The following operators can be used:" +
                           "<p>Arithmetic: <i>+ - * / ^ %</i>" +
                           "<p>Bitwise: | <i>&amp; # ~ &lt;&lt; &gt;&gt;</i>" +
                           "<p>Comparison: <i>== != &lt; &gt; &lt;= &gt;=</i>" +
                           "<p>Boolean: <i>!</i> || <i>&amp;&amp;</i>" +
                           "<p>Conditional: <i>?:</i>",
        MODULE_FIELD_HELP = "<p>Module to hold the newly computed scalars. You have the following options:" +
                            "<h1>1. Implicit names</h1>" +
                            "<p>If you leave this field empty, the new scalars will be assigned to the same module as their input scalars. " +
                            "This option is only open if the value expression contains a scalar which is not qualified with module path pattern; " +
                            "then the module of that scalar will be used. (That is, if you have both <i>**.mac.dropCount</i> and <i>thruput</i> " +
                            "in the value expression, then the module of <i>thruput</i> takes precedence.) However, if you use aggregate " +
                            "functions that derive a value from scalars possibly from multiple modules " +
                            "(e.g. <i>mean(dropCount)</i> which computes the mean over <i>dropCount</i> scalars of several modules), then " +
                            "you have to specify the module explicitly because it would be ambiguous." +
                            "<h2>Example</h2>" +
                            "<p>The dataset contains scalars <i>a</i> and <i>b</i> recorded for two modules <i>m1</i> and <i>m2</i>. " +
                            "The value expression <i>a+b</i> computes two scalars: one from <i>m1.a+m1.b</i> and one from <i>m2.a+m2.b</i>. " +
                            "The implicit module name is <i>m1</i> for the first, and <i>m2</i> for the second scalar."+
                            "<h2>2. Literal names</h2>" +
                            "<p>You can enter a qualified module name here, and this name will be used as the module of the new scalars. " +
                            "You can use any qualified name as a module name, it is no need to use the names of existing modules only. " +
                            "<h2>3. Variable substitution</h2>" +
                            "<p>Expressions can be used as part of the module name by the <i>${...}</i> syntax. " +
                            "These expressions are evaluated to strings and their values are substituted into the module name. " +
                            "Variables from the value expression and grouping expression can be referenced here. For example, if the " +
                            "value expression contains a <i>Net.node[${index=*}].sink.pkcount</i> scalar (and thus defines an " +
                            "<i>index</i> variable), you can write e.g. <i>Net.node[${2*index+1}]</i> in the module field." +
                            "<h2>Example</h2>" +
                            "If the grouping expression defines the <i>subnet</i> variable, and it has <i>A, B, C</i> values for the "+
                            "three groups, you can specify the module name of the computed scalars as <i>SkyNet.${subnet}</i> to yield " +
                            "names <i>SkyNet.A</i>, <i>SkyNet.B</i>, and <i>SkyNet.C</i>.",
        NAME_FIELD_HELP = "<p>Name of the newly computed scalars. You can enter any literal string here, and this name will be used as the name " +
                           "of the new scalars. Expressions can be embedded into the name by the <i>${...}</i> syntax. " +
                           "These expressions are evaluated to strings and their values are substituted into the module name. " +
                           "Variables from the value expression and grouping expression can be referenced here.",
        GROUPBY_FIELD_HELP = "<p>Scalars can be grouped by module before applying aggregate functions, and here you can enter " +
                             "the expression that denotes the group of each scalar." +
                             "<p>The aggregate functions (<i>mean,min,max,...</i>) compute one scalar for each run by default. " +
                             "It is possible to group the scalars, and compute an aggregate value for each group, by specifying a grouping expression. " +
                             "The grouping expression is evaluated for each statistic in the input dataset, and the resulting value " +
                             "denotes the statistic's group. Subsequent computations are performed on each group independently." +
                             "<p>The following variables can be referenced in the grouping expression:" +
                             "<ul><li><i>module</i>: the name of the module that recorded the statistic" +
                             "<li><i>name</i>: the name of the statistic" +
                             "<li><i>value</i>: the value of the statistic, if it is a scalar, NaN otherwise" +
                             "<li><i>run</i>: the run identifier" +
                             "<li>run attributes (<i>configname, experiment, measurement, replication,</i> etc.). " +
                             "However, note that you cannot join data from several simulation runs into one group this way." +
                             "</ul>" +
                             "<p>Usually you want to extract some part of the module name as the group identifier. " +
                             "The expression <i>&lt;str&gt; =~ &lt;pat&gt;</i> matches the string <i>str</i> with the pattern <i>pat</i>. " +
                             "If there is no match, then the value of the expression is <i>false</i>, otherwise that part of " +
                             "<i>str</i> that bound to a variable in <i>pat</i>." +
                             "<h2>Examples</h2>" +
                             "<p>If there are three subnets, (<i>Network.A, Network.B,Network.C</i>), each containing several modules," +
                             "you can compute the average of a scalar over subnets by specifying the following grouping expression:" +
                             "<p><i>(module =~ Network.${subnet=*}.**) ? subnet : 0</i>" +
                             "<p>The value of the <i>subnet</i> variable can be refered in the value and module name expressions.",
        AVERAGE_FIELD_HELP = "<p>The computation is performed in each run independently by default. If some run is a replication of the same " +
                             "measurement with different seeds, you may want to average the results. If the <i>Average replications</i> checkbox is " +
                             "selected, then only one scalar is added to the dataset for each measurement." +
                             "<p>A new run generated for the scalar which represents the set of replications which it is computed from. " +
                             "The attributes of this run are those that have the same value in all replications." +
                             "<p>In addition to <i>mean</i>, you can also add other statistical properties of the computed scalar to the dataset " +
                             "by selecting the corresponding checkboxes in the dialog. The names of these new scalars will be formed " +
                             "by adding the <i>:stddev</i>, <i>:confint</i>, <i>:min</i>, or <i>:max</i> suffix to the base name of the scalar.";

    private TextField createField(int field, String name, String description, String linkHoverText, Composite parent, int labelWidth)
    {
        Label label = new Label(parent, SWT.NONE);
        label.setText(name+":");
        label.setLayoutData(new GridData(labelWidth + 5, SWT.DEFAULT));
        final Text text = new Text(parent, SWT.BORDER);
        GridData gridData = new GridData(SWT.FILL, SWT.CENTER, true, false);
        gridData.horizontalIndent = 5; // space for decorations
        text.setLayoutData(gridData);
        if (description != null)
        {
            label = new Label(parent, SWT.NONE); // placeholder
            gridData = new GridData(SWT.FILL, SWT.CENTER, true, false);
            gridData.horizontalIndent = 5; // align with the Text
            if (linkHoverText == null) {
                label = new Label(parent, SWT.WRAP);
                label.setLayoutData(gridData);
                label.setText(description);
            }
            else {
                HelpLink link = new HelpLink(parent, SWT.NONE);
                link.getLink().setLayoutData(gridData);
                link.setText(description);
                link.setHoverText(linkHoverText);
            }
        }
        // content assists
        IContentProposalProvider proposalProvider = new ExprContentProposalProvider(field);
        new ContentAssistCommandAdapter(text,
                new TextContentAdapter(),
                proposalProvider,
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,
                ".".toCharArray() /*auto-activation*/,
                true /*installDecoration*/);
        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                fireChangeNotification();
            }
        });
        text.addFocusListener(new FocusAdapter() {
            public void focusGained(FocusEvent e) {
                text.selectAll();
                fireChangeNotification();
            }
        });
        return new TextField(text);
    }

    private static final Collection<String> FUNCTION_NAMES;
    static {
        FUNCTION_NAMES = new TreeSet<String>();
        for (Function fun : Function.class.getEnumConstants())
            FUNCTION_NAMES.add(fun.getName());
    }

    // propose statistic names, module names, function names
    class ExprContentProposalProvider extends ContentProposalProvider {
        int field;

        public ExprContentProposalProvider(int field) {
            super(false /*useWholePrefix*/);
            this.field = field;
        }

        @Override
        protected List<IContentProposal> getProposalCandidates(String prefix) {
            List<IContentProposal> proposals = new ArrayList<IContentProposal>();

            for (String variable : getVariablesForField(field)) {
                ContentProposal proposal = new ContentProposal(variable, variable, null);
                proposals.add(proposal);
            }

            if (field == FIELD_VALUE) {
                for (String name : statistics) {
                    String content = quoteStatisticsIfNeeded(name);
                    ContentProposal proposal = new ContentProposal(content, name, null);
                    proposals.add(proposal);
                }
            }

            for (String module : moduleHints) {
                ContentProposal proposal = new ContentProposal(module, module, null);
                proposals.add(proposal);
            }

            for (String fun : FUNCTION_NAMES) {
                ContentProposal proposal = new ContentProposal(fun, fun, null);
                proposal.setDecorators(ContentProposal.DEC_OP | ContentProposal.DEC_CP);
                proposals.add(proposal);
            }
            return proposals;
        }

        String quoteStatisticsIfNeeded(String name) {
            if ("[a-zA-Z_][a-zA-Z_0-9]*".matches(name))
                return name;
            if (name.indexOf('\'') == -1)
                return "'" + name + "'";
            if (name.indexOf('"') == -1)
                return "\"" + name + "\"";

            StringBuilder sb = new StringBuilder();
            sb.append("'");
            for (int i = 0; i < name.length(); ++i) {
                char ch = name.charAt(i);
                if (ch == '\'')
                    sb.append("\\");
                sb.append(ch);
            }
            sb.append("'");
            return sb.toString();
        }

        Set<String> getVariablesForField(int field) {
            switch (field) {
            case FIELD_GROUPBY:
                return GROUPBY_PREDEFINED_VARIABLES;
            case FIELD_MODULE:
            case FIELD_NAME:
            case FIELD_VALUE:
                Set<String> variables = new TreeSet<String>();
                if (!StringUtils.isEmpty(groupByField.text.getText()) && !groupByField.decoration.isVisible()) {
                    variables.add("group");
                }
                return variables;
            default:
                Assert.isTrue(false);
                return null;
            }
        }
    }

    private void setEnabled(boolean enabled, Control control, int depth)
    {
        control.setEnabled(enabled);
        if (!enabled && (control instanceof Button))
            ((Button)control).setSelection(false);
        if (depth == 0 || !(control instanceof Composite))
            return;

        for (Control child : ((Composite)control).getChildren())
                setEnabled(enabled, child, depth-1);
    }

    public EStructuralFeature[] getFeatures() {
        return features;
    }


    @Override
    public IStatus validate() {
        String valueText = valueField != null ? valueField.text.getText() : null;
        String groupByText = groupByField != null ? groupByField.text.getText() : null;
        String nameText = scalarNameField != null ? scalarNameField.text.getText() : null;
        String moduleText = moduleField != null ? moduleField.text.getText() : null;

        IStatus[] validationResult = null;
        if (StringUtils.isEmpty(valueText) && StringUtils.isEmpty(groupByText) && StringUtils.isEmpty(nameText) && StringUtils.isEmpty(moduleText)) {
            // special case: empty form is valid
        }
        else {
            ComputedScalarEngine engine = new ComputedScalarEngine(null);
            validationResult = engine.validate(valueText, nameText, moduleText, groupByText, statistics);
        }

        IStatus reportedStatus = null;
        // set decorations, and report errors/warnings of focused control in the title area
        for (int field : ALL_FIELDS) {
            TextField textField = getField(field);
            IStatus status = validationResult != null ? validationResult[field] : Status.OK_STATUS;
            textField.setStatus(status);
            if (textField.text.isFocusControl() && status != null && !status.isOK()) {
                reportedStatus = status;
            }
        }
        // if no error/warning in the focused control, then report the first error, or the first warning
        if (reportedStatus == null && validationResult != null) {
            for (int field : ALL_FIELDS) {
                if (reportedStatus == null || validationResult[field].getSeverity() > reportedStatus.getSeverity()) {
                    reportedStatus = validationResult[field];
                }
            }
        }

        return reportedStatus == null ? Status.OK_STATUS : reportedStatus;
    }

    private static final int[] ALL_FIELDS = new int[] {FIELD_VALUE, FIELD_GROUPBY, FIELD_NAME, FIELD_VALUE };

    private TextField getField(int field) {
        switch(field) {
        case FIELD_VALUE: return valueField;
        case FIELD_GROUPBY: return groupByField;
        case FIELD_NAME: return scalarNameField;
        case FIELD_MODULE: return moduleField;
        }
        Assert.isTrue(false);
        return null;
    }

    public Object getValue(EStructuralFeature feature) {
        switch (feature.getFeatureID()) {
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
                return scalarNameField.text.getText();
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
                return valueField.text.getText();
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
                return moduleField.text.getText();
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
                return groupByField.text.getText();
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
                return averageReplicationsButton.getSelection();
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
                return averageReplicationsButton.getSelection() && computeStddevButton.getSelection();
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
                return averageReplicationsButton.getSelection() && computeConfIntervalButton.getSelection();
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
                return averageReplicationsButton.getSelection() && computeConfIntervalButton.getSelection() ?
                        stringToConfidenceLevel(confidenceLevelCombo.getText()) : null;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                return averageReplicationsButton.getSelection() && computeMinMaxButton.getSelection();
        }
        return null;
    }

    public void setValue(EStructuralFeature feature, Object value) {
        switch (feature.getFeatureID()) {
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
                scalarNameField.text.setText((String)value);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
                valueField.text.setText((String)value);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
                moduleField.text.setText((String)value);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
                groupByField.text.setText((String)value);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
                averageReplicationsButton.setSelection((Boolean)value);
                setEnabled((Boolean)value, optionsPanel, 2);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
                computeStddevButton.setSelection((Boolean)value);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
                computeConfIntervalButton.setSelection((Boolean)value);
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
                confidenceLevelCombo.setText(confidenceLevelToString((Double)value));
                break;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                computeMinMaxButton.setSelection((Boolean)value);
                break;
        }
    }

    private Double stringToConfidenceLevel(String value)
    {
        boolean percent = value.endsWith("%");
        if (percent)
            value = value.substring(0, value.length() - 1);

        Double d = Converter.stringToDouble(value);
        if (percent || d > 1.0)
            d /= 100.0;

//        if (d < 0.5 || d >= 1.0)
//            throw new RuntimeException("Confidence level must be greater than 50% and smaller than 100%.");
        return d;
    }

    private String confidenceLevelToString(Double value)
    {
        String s = String.format("%.5f", 100.0*value);
        s = s.replaceAll("0+$", "");
        s = s.replaceAll("\\.$", "");
        return s+"%";
    }
}
