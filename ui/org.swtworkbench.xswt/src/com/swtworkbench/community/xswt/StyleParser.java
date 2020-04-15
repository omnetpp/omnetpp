package com.swtworkbench.community.xswt;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;

import org.eclipse.swt.SWT;

@SuppressWarnings({"rawtypes", "unchecked"})
public class StyleParser {
    private static final Map mapStyles = new TreeMap();

    static {
        try {
            registerClassConstants(SWT.class);
        }
        catch (XSWTException e) {
            e.printStackTrace();
        }
    }

    public static void registerClassConstants(Class constantsClass) throws XSWTException {
        TreeMap classStyles = new TreeMap();
        try {
            Field[] fields = constantsClass.getDeclaredFields();

            for (int i = 0; i < fields.length; ++i) {
                Field field = fields[i];

                if (Integer.TYPE.equals(field.getType())) {
                    int modifiers = field.getModifiers();

                    if ((!(Modifier.isPublic(modifiers))) || (!(Modifier.isStatic(modifiers)))
                            || (!(Modifier.isFinal(modifiers))))
                        continue;
                    try {
                        classStyles.put(field.getName(), field.get(null));
                    }
                    catch (IllegalAccessException localIllegalAccessException) {
                    }
                }
            }
        }
        catch (Throwable t) {
            throw new XSWTException(t);
        }

//        StringTokenizer stringTokenizer = new StringTokenizer(constantsClass.getCanonicalName(), ".");
//        int tokens = stringTokenizer.countTokens();
//        String simpleClassName = null;
//        for (int i = 0; i < tokens; ++i) {
//            simpleClassName = stringTokenizer.nextToken();
//        }
//        mapStyles.put(simpleClassName, classStyles);

        mapStyles.put(constantsClass.getSimpleName(), classStyles); // same code as above
    }

    public static int parse(String source) throws XSWTException {
        if ((source == null) || (source.length() == 0)) {
            return 0;
        }
        int style = 0;
        StringTokenizer stringTokenizer = new StringTokenizer(source, " |\t\r\n");

        while (stringTokenizer.hasMoreTokens()) {
            String token = stringTokenizer.nextToken();

            StringTokenizer idScanner = new StringTokenizer(token, ".");
            String identifier = idScanner.nextToken();
            String qualifier = "";
            while (idScanner.hasMoreTokens()) {
                qualifier = identifier;
                identifier = idScanner.nextToken();
            }
            int value = 0;
            try {
                if (identifier.startsWith("'")) {
                    value = identifier.charAt(1);
                }
                else {
                    if (qualifier == "")
                        qualifier = "SWT";
                    qualifier = XSWT.upperCaseFirstLetter(qualifier);

                    TreeMap classStyles = (TreeMap) mapStyles.get(qualifier);
                    value = ((Integer) classStyles.get(identifier)).intValue();
                }
                style |= value;
            }
            catch (Exception localException) {
                throw new XSWTException("Undefined constant: " + token);
            }
        }

        return style;
    }
}