package org.omnetpp.test.gui.nededitor.textual;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.TextEditorAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.eclipse.swt.SWT;

import org.omnetpp.test.gui.nededitor.NedFileTestCase;

public class ContentAssistTest extends NedFileTestCase {
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        createFileWithContent(
           "simple Simple1 {}\n" +
           "simple Simple2 {}\n" +
           "channel Channel1 {}\n" +
           "channel Channel2 {}\n" +
           "channelinterface ChannelIf1 {}\n" +
           "channelinterface ChannelIf2 {}\n" +
           "moduleinterface ModuleIf1 {}\n" +
           "moduleinterface ModuleIf2 {}\n" +
           "module Compound1\n" +
           "{\n" +
           "    parameters:\n" +
           "        string par;\n"+
           "    gates:\n" +
           "        inout cg;\n" +
           "    types:\n" +
           "        channel InnerChannel1 {}\n" +
           "        channel InnerChannel2 {}\n" +
           "        moduleinterface InnerModuleIf {}\n" +
           "        simple InnerSimple1\n" +
           "        {\n" +
           "            gates:\n" +
           "                inout g;\n" +
           "        }\n" +
           "        simple InnerSimple2 {}\n" +
           "        module InnerCompound {\n" +
           "            submodules:\n" +
           "                innersub1: InnerSimple1;\n" +
           "        }\n" +
           "    submodules:\n" +
           "        sub1: InnerSimple1;\n" +
           "        sub2: InnerSimple1;\n" +
           "    connections:\n" +
           "}\n" +
           "module Compound2 {\n"+
           "    gates: inout c2g;\n" +
           "    submodules: c2sub1 : Simple1;\n" +
           "}"
           );
        WorkspaceUtils.ensureFolderExists(projectName, "testpackage");
        WorkspaceUtils.createFileWithContent(projectName+"/testpackage/testfile2.ned",
                "package testpackage;\n"+
                "simple Simple1 {}\n" +
                "channel Channel1 {}\n" +
                "channelinterface ChannelIf1 {}\n" +
                "moduleinterface ModuleIf1 {}\n" +
                "module Compound1 {}\n");
       openFileFromProjectExplorerView();
       findNedEditor().ensureActiveTextEditor();
    }

    public void testSimpleExtends() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("simple Simple2");
        textualNedEditor.typeIn(" extends ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "Simple1 - simple module type",
                "Simple1 - testpackage - simple module type",
                "Simple2 - simple module type"});
    }

    public void testCompoundExtends() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("module Compound2");
        textualNedEditor.typeIn(" extends ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "Compound1 - compound module type",
                "Compound1 - testpackage - compound module type",
                "Compound2 - compound module type"});
    }

    public void testModuleInterfaceExtends() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("moduleinterface ModuleIf2");
        textualNedEditor.typeIn(" extends ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "IBidirectionalChannel - ned - module interface type",
                "IUnidirectionalChannel - ned - module interface type",
                "ModuleIf1 - module interface type",
                "ModuleIf1 - testpackage - module interface type",
                "ModuleIf2 - module interface type"});
    }

    public void testChannelExtends() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("channel Channel2");
        textualNedEditor.typeIn(" extends ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "DatarateChannel - ned - channel type",
                "Channel1 - channel type",
                "Channel1 - testpackage - channel type",
                "Channel2 - channel type",
                "IdealChannel - ned - channel type"});
    }

    public void testChannelInterfaceExtends() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("channelinterface ChannelIf2");
        textualNedEditor.typeIn(" extends ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "ChannelIf1 - channel interface type",
                "ChannelIf1 - testpackage - channel interface type",
                "ChannelIf2 - channel interface type"});
    }

    public void testSimpleLike() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("simple Simple2");
        textualNedEditor.typeIn(" like ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "IBidirectionalChannel - ned - module interface type",
                "IUnidirectionalChannel - ned - module interface type",
                "ModuleIf1 - module interface type",
                "ModuleIf1 - testpackage - module interface type",
                "ModuleIf2 - module interface type"});
    }

    public void testCompoundLike() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("module Compound2");
        textualNedEditor.typeIn(" like ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "IBidirectionalChannel - ned - module interface type",
                "IUnidirectionalChannel - ned - module interface type",
                "ModuleIf1 - module interface type",
                "ModuleIf1 - testpackage - module interface type",
                "ModuleIf2 - module interface type"});
    }

    public void testChannelLike() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("channel Channel2");
        textualNedEditor.typeIn(" like ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "ChannelIf1 - channel interface type",
                "ChannelIf1 - testpackage - channel interface type",
                "ChannelIf2 - channel interface type"});
    }

    public void testSubmoduleType() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("sub2: InnerSimple1;");
        textualNedEditor.typeIn("\n sub3: ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "InnerCompound - inner type",
                "InnerSimple1 - inner type",
                "InnerSimple2 - inner type",
                "Compound1 - compound module type",
                "Compound1 - testpackage - compound module type",
                "Compound2 - compound module type",
                "Simple1 - simple module type",
                "Simple1 - testpackage - simple module type",
                "Simple2 - simple module type"});
    }

    public void testSubmoduleLike() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("sub2: InnerSimple1;");
        textualNedEditor.typeIn("\n sub3: <");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "par - parameter"});
        textualNedEditor.typeIn("par>");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "like - keyword"});
        textualNedEditor.typeIn(" like ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "InnerModuleIf - inner type",
                "IBidirectionalChannel - ned - module interface type",
                "IUnidirectionalChannel - ned - module interface type",
                "ModuleIf1 - module interface type",
                "ModuleIf1 - testpackage - module interface type",
                "ModuleIf2 - module interface type"});
    }

    public void testConnection() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("connections:");
        textualNedEditor.typeIn("\n");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContentStartWith(new String[] {
                "sub1 - submodule",
                "sub2 - submodule",
                "cg - gate"
                }, null);
        textualNedEditor.typeIn("sub1.");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "g - gate"});
        textualNedEditor.typeIn("g <--> ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "sub1 - submodule",
                "sub2 - submodule",
                "cg - gate",
                "InnerChannel1 - inner type",
                "InnerChannel2 - inner type",
                "DatarateChannel - ned - channel type",
                "Channel1 - channel type",
                "Channel1 - testpackage - channel type",
                "Channel2 - channel type",
                "IdealChannel - ned - channel type"});
        textualNedEditor.typeIn("Channel2 <--> ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "sub1 - submodule",
                "sub2 - submodule",
                "cg - gate"
                });
        textualNedEditor.typeIn("sub2.");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "g - gate"});
    }

    public void testSubmoduleTypeInInnerCompound() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("innersub1: InnerSimple1;");
        textualNedEditor.typeIn("\n innersub1: ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "InnerCompound - inner type",
                "InnerSimple1 - inner type",
                "InnerSimple2 - inner type",
                "Compound1 - compound module type",
                "Compound1 - testpackage - compound module type",
                "Compound2 - compound module type",
                "Simple1 - simple module type",
                "Simple1 - testpackage - simple module type",
                "Simple2 - simple module type"});
    }

    public void testSubmoduleLikeInInnerCompound() throws Throwable {
        TextEditorAccess textualNedEditor = findNedEditor().getTextualNedEditor();
        textualNedEditor.moveCursorAfter("innersub1: InnerSimple1;");
        textualNedEditor.typeIn("\n sub3: <");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.assertHasNoContentAssistPopup();
        textualNedEditor.typeIn("par>");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "like - keyword"});
        textualNedEditor.typeIn(" like ");
        textualNedEditor.pressKey(' ', SWT.CTRL);
        Access.findContentAssistPopup().assertContent(new String[] {
                "InnerModuleIf - inner type",
                "IBidirectionalChannel - ned - module interface type",
                "IUnidirectionalChannel - ned - module interface type",
                "ModuleIf1 - module interface type",
                "ModuleIf1 - testpackage - module interface type",
                "ModuleIf2 - module interface type"});
    }

}
