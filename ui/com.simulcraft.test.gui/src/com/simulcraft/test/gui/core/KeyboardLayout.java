package com.simulcraft.test.gui.core;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.jface.bindings.keys.ParseException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.access.Access;

/**
 * 
 * @author Andras
 */
public class KeyboardLayout {
    private Map<Character,KeyStroke> mapping = null;
    private char receivedChar; // used during testing the keyboard
    
    public KeyboardLayout() {
    }

    public KeyStroke getKeyFor(char character) {
        Assert.isTrue(mapping!=null, "keyboard layout not tested yet");
        Assert.isTrue(mapping.containsKey(character), "don't know how to produce '"+character+"' on the keyboard");
        return mapping.get(character);
    }

    public Map<Character,KeyStroke> getMapping() {
        return mapping;
    }
    
    public boolean isEmpty() {
        return mapping == null || mapping.isEmpty();
    }

    public void testKeyboard() {
        Assert.isTrue(Display.getCurrent() != null, "must be in the UI thread");
        
        final Shell shell = new Shell(SWT.TOOL | SWT.ON_TOP | SWT.APPLICATION_MODAL);
        shell.setSize(200, 20);
        Text text = new Text(shell, SWT.SINGLE | SWT.BORDER);
        shell.setLayout(new FillLayout());
        shell.layout();
        shell.open();
        text.setFocus();
        
        text.addListener(SWT.KeyDown, new Listener() {
            public void handleEvent(Event e) {
                if (receivedChar != 0)
                    MessageDialog.openWarning(shell, "Warning", "Please do NOT press that key again! Or any other key.\nThat's interfering with keyboard layout detection.");
                receivedChar = e.character;
            }
        });
        
        mapping = new HashMap<Character, KeyStroke>();
        for (char naturalKey = 32; naturalKey < 127; naturalKey++) {
            if (naturalKey>='A' && naturalKey<='Z') 
                continue;  // we test letter keys on the keyboard in lowercase only 
            testKey(naturalKey, 0);
            testKey(naturalKey, SWT.SHIFT);
        }
        shell.dispose();
    }

    protected void testKey(char naturalKey, int modifier) {
        new Access().pressKey(naturalKey, modifier);
        while (Display.getCurrent().readAndDispatch());
        registerKeyStroke(naturalKey, modifier, receivedChar);
        receivedChar = 0;
    }

    protected void registerKeyStroke(char naturalKey, int modifier, char ch) {
        KeyStroke keyStroke = KeyStroke.getInstance(modifier, naturalKey);
        //System.out.println(keyStroke.format() + " ==> " + receivedChar);
        if (!mapping.containsKey(ch))
            mapping.put(ch, keyStroke);
        else {
            // only overwrite if new one is better
            KeyStroke existingKeyStroke = mapping.get(ch);
            if (existingKeyStroke.getModifierKeys()==0 && existingKeyStroke.getNaturalKey() != ch) // already good enough
                if (modifier==0 || (naturalKey>='a' && naturalKey<='z') || (naturalKey>='0' && naturalKey<='9')) // prefer keys without SHIFT, and letter and number keys
                    mapping.put(ch, keyStroke);
        }
    }
    
    public void saveMapping(String filename) throws IOException {
            String contents = "";
            for (char ch : mapping.keySet())
                contents += ch + " " + mapping.get(ch).format() + "\n";
            FileUtils.copy(new ByteArrayInputStream(contents.getBytes()), new File(filename));
    }

    public void loadMapping(String filename) throws IOException, ParseException {
        mapping = null;
        HashMap<Character, KeyStroke> tmp = new HashMap<Character, KeyStroke>();
        String contents = FileUtils.readTextFile(filename);
        for (String line : StringUtils.splitToLines(contents)) {
            char ch = line.charAt(0);
            String key = line.substring(1);
            tmp.put(ch, KeyStroke.getInstance(key));
        }
        mapping = tmp; // only install if loaded without error
    }

    public void printMapping() {
        System.out.println("Keyboard mapping:");
        Character[] characters = mapping.keySet().toArray(new Character[]{});
        Arrays.sort(characters);
        for (Character character : characters)
            System.out.println(" for "+character+" press "+mapping.get(character));
    }
    
}
