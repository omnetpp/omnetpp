package org.omnetpp.test.gui.inifileeditor.configpages;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import junit.framework.Assert;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.form.FormPage;
import org.omnetpp.inifile.editor.form.GenericConfigPage;
import org.omnetpp.inifile.editor.form.InifileFormEditor;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

public class ConfigPagesCoverageTest extends InifileEditorTestCase {
    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    public void testConfigPagesCoverage() throws Exception {
        // checks that all config keys are covered by the forms.
        // see also InifileFormEditor.dumpForgottenConfigKeys()
        prepareTest("");
        InifileFormEditorAccess formEditor = findInifileEditor().ensureActiveFormEditor();
        InifileFormEditor formEditorPart = ((InifileEditor)findInifileEditor().getPart()).getFormEditor();

        List<String> categories = new ArrayList<String>();
        categories.add(InifileFormEditor.PARAMETERS_PAGE);
        categories.add(InifileFormEditor.SECTIONS_PAGE);
        categories.addAll(Arrays.asList(GenericConfigPage.getCategoryNames()));

        // collect keys supported by the editor forms
        Set<ConfigKey> supportedKeys = new HashSet<ConfigKey>();
        for (String category : categories) {
            formEditor.activateCategoryPage(category);
            FormPage page = formEditorPart.getActiveCategoryPage();
            for (ConfigKey key : page.getSupportedKeys())
                supportedKeys.add(key);
        }
        
        String expectedForgottenKeys = "seed-%-mt-p%  seed-%-lcg32  seed-%-mt"; // we know these are missing from the forms
        
        // see which keys are not supported anywhere
        Set<String> forgottenKeys = new HashSet<String>();
        for (ConfigKey key : ConfigRegistry.getEntries())
            if (!supportedKeys.contains(key) && !expectedForgottenKeys.contains(key.getKey()))
                forgottenKeys.add(key.getKey());
        Assert.assertTrue("forgotten config keys (do not occur in inifile form pages): "+StringUtils.join(forgottenKeys, ", "), forgottenKeys.isEmpty());
    }
}
