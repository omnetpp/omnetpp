public class param {
    public static test(String fileNam) {
        try {
    	    int index = 0;
    	    Properties properties = new Properties();
    	    properties.load(new FileInputStream(fileName));

            for (Object key : CollectionUtils.toSorted((Set<String>)(Set)properties.keySet(), new DictionaryComparator())) {
                String paramName = (String)key;
                String runtimeParamValue = properties.getProperty(paramName);

                for (ParamResolution paramResolution : list) {
                    String paramPattern = paramResolution.key != null ? paramResolution.key : paramResolution.fullPath + "." + paramResolution.paramDeclaration.getName();
                    PatternMatcher m = new PatternMatcher(paramPattern, true, true, true);

                    if (m.matches(paramName)) {
                        String ideParamValue = null;

                        switch (paramResolution.type) {
                            case UNASSIGNED: 
                            case INI_ASK:
                                ideParamValue = "\"" + index + "\"";
                                index++;
                                break;
                            case NED:
                            case INI_DEFAULT:
                            case IMPLICITDEFAULT:
                                ideParamValue = paramResolution.paramAssignment.getValue();
                                break;
                            case INI:
                            case INI_OVERRIDE:
                            case INI_NEDDEFAULT:
                                ideParamValue = doc.getValue(paramResolution.section, paramResolution.key);
                                break;
                            default: 
                                throw new RuntimeException();
                        }

                        // System.out.println(paramName + " = " + ideParamValue);

                        if (!runtimeParamValue.equals(ideParamValue))
                            System.out.println("*** Mismatch *** for name: " + paramName + ", runtime value: " + runtimeParamValue + ", ide value: " + ideParamValue);
                        else
                            System.out.println("Match for name: " + paramName + ", value: " + runtimeParamValue);

                        break;
                    }
                }
            }
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}