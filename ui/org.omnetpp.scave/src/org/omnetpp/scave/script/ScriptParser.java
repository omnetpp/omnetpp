/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.script;

import static org.omnetpp.scave.script.IScriptConstants.ADD;
import static org.omnetpp.scave.script.IScriptConstants.COMMAND_ANDS_CLAUSE_COMMANDS;
import static org.omnetpp.scave.script.IScriptConstants.WHERE;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.ResultType;

/**
 * Makeshift parser for analysis scripts.
 *
 * @author andras
 */
public class ScriptParser {

    public static ScriptCommand[] parseScript(String script) {
        int lineNo = 0;
        try {
            List<ScriptCommand> commands = new ArrayList<>();
            for (String line : script.split("\n")) {
                lineNo++;
                if (line.trim().isEmpty() || line.trim().startsWith("//"))
                    continue;
                String[] tokens = line.trim().split(" ");
                if (tokens.length == 0)
                    throw new RuntimeException("Missing command");
                String command = tokens[0];
                Map<String, String> clauses = parseClauses(tokens);

                if (command.equalsIgnoreCase(ADD)) {
                    // ADD scalars WHERE <filter>
                    checkClauses(clauses, new String[] {ADD, WHERE});
                    //TODO    throw new RuntimeException("Syntax error, expecting 'ADD <type> WHERE <filter>'");
                    ResultType resultType = parseResultTypeName(clauses.get(ADD));
                    String filter = clauses.get(WHERE);
                    AddCommand addCommand = new AddCommand();
                    addCommand.setResultType(resultType);
                    addCommand.setFilterExpression(filter);
                    commands.add(addCommand);
                }
                else {
                    throw new RuntimeException("Unknown command '" + StringUtils.abbreviate(command,15) + "'");
                }
            }
            return commands.toArray(new ScriptCommand[0]);
        }
        catch (Exception e) {
            throw new RuntimeException("<script>:" + lineNo + ": " + e.getMessage());
        }
    }

    private static ResultType parseResultTypeName(String resultTypeName) {
        ResultType resultType;
        switch (resultTypeName.toLowerCase()) {
            case "scalars": resultType = ResultType.SCALAR_LITERAL; break;
            case "vectors": resultType = ResultType.VECTOR_LITERAL; break;
            case "statistics": resultType = ResultType.STATISTICS_LITERAL; break; //TODO should imply the statistics part of histograms, too!
            case "histograms": resultType = ResultType.HISTOGRAM_LITERAL; break;
            default: throw new RuntimeException("Invalid result type '" + resultTypeName + "'");
        }
        return resultType;
    }

    private static void checkClauses(Map<String, String> clauses, String[] allowedClauseKeywords) {
        for (String clauseKeyword : clauses.keySet())
            if (!ArrayUtils.contains(allowedClauseKeywords, clauseKeyword))
                throw new RuntimeException("Clause '" + clauseKeyword + "' not allowed here");
    }

    private static Map<String, String> parseClauses(String[] tokens) {
        String[] keywords = COMMAND_ANDS_CLAUSE_COMMANDS;
        Map<String,String> clauses = new HashMap<>();
        String currentKeyword = null;
        String value = "";
        for (String token : tokens) {
            if (ArrayUtils.contains(keywords, token)) {
                if (currentKeyword != null)
                    clauses.put(currentKeyword.toUpperCase(), value.trim());
                currentKeyword = token;
                value = "";
            }
            else {
                value += " " + token;
            }
        }
        if (currentKeyword != null)
            clauses.put(currentKeyword.toUpperCase(), value.trim());
        return clauses;
    }
}
