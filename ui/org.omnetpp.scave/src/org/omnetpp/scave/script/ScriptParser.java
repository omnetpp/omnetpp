/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.script;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.script.IScriptConstants.ADD;
import static org.omnetpp.scave.script.IScriptConstants.APPLY;
import static org.omnetpp.scave.script.IScriptConstants.AVERAGEREPLICATIONS;
import static org.omnetpp.scave.script.IScriptConstants.COMMAND_ANDS_CLAUSE_COMMANDS;
import static org.omnetpp.scave.script.IScriptConstants.COMPUTE;
import static org.omnetpp.scave.script.IScriptConstants.COMPUTECONFIDENCEINTERVAL;
import static org.omnetpp.scave.script.IScriptConstants.COMPUTEMINMAX;
import static org.omnetpp.scave.script.IScriptConstants.COMPUTESTDDEV;
import static org.omnetpp.scave.script.IScriptConstants.COMPUTE_CLAUSE_KEYWORDS;
import static org.omnetpp.scave.script.IScriptConstants.GROUPING;
import static org.omnetpp.scave.script.IScriptConstants.NAMED;
import static org.omnetpp.scave.script.IScriptConstants.OPTIONS;
import static org.omnetpp.scave.script.IScriptConstants.VALUE;
import static org.omnetpp.scave.script.IScriptConstants.WHERE;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.IStatus;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.computed.ComputedScalarEngine;
import org.omnetpp.scave.computed.VectorOperation;
import org.omnetpp.scave.computed.VectorOperations;
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
                else if (command.equalsIgnoreCase(APPLY)) {
                    // APPLY <operation> WHERE <filter>
                    checkClauses(clauses, new String[] {APPLY, WHERE});
                    String operation = clauses.get(APPLY);
                    String filter = clauses.get(WHERE);

                    int openParenPos = operation.indexOf('(');
                    String name = openParenPos == -1 ? operation : operation.substring(0, openParenPos);
                    VectorOperation op = VectorOperations.get(name);
                    if (op == null)
                        throw new RuntimeException("No such vector operation: " + name);
                    Object[] args = parseArgs(operation, op);

                    ApplyCommand applyCommand = new ApplyCommand();
                    applyCommand.setOperation(op);
                    applyCommand.setParameters(args);
                    applyCommand.setFilterExpression(filter);
                    commands.add(applyCommand);
                }
                else if (command.equalsIgnoreCase(COMPUTE)) {
                    // COMPUTE scalars NAMED <scalarName> AS <valueExpr> MODULE <moduleExpr> GROUPING <groupByExpr> OPTIONS averageReplications, computeStddev, computeConfidenceInterval, confidenceLevel, computeMinMax
                    checkClauses(clauses, ArrayUtils.add(COMPUTE_CLAUSE_KEYWORDS, COMPUTE));
                    ComputeScalarCommand computeCommand = new ComputeScalarCommand();
                    computeCommand.setScalarName(clauses.get(NAMED));
                    computeCommand.setValueExpr(clauses.get(VALUE));
                    computeCommand.setModuleExpr(clauses.get(MODULE));
                    computeCommand.setGroupByExpr(clauses.get(GROUPING));
                    computeCommand.setFilterExpression(clauses.get(WHERE));
                    String[] options = StringUtils.nullToEmpty(clauses.get(OPTIONS)).split(" *, *");
                    computeCommand.setAverageReplications(ArrayUtils.contains(options, AVERAGEREPLICATIONS));
                    computeCommand.setComputeMinMax(ArrayUtils.contains(options, COMPUTEMINMAX));
                    computeCommand.setComputeStddev(ArrayUtils.contains(options, COMPUTESTDDEV));
                    computeCommand.setComputeConfidenceInterval(ArrayUtils.contains(options, COMPUTECONFIDENCEINTERVAL));
                    computeCommand.setConfidenceLevel(0.95); //TODO
                    commands.add(computeCommand);
                    
                    //IStatus[] status = new ComputedScalarEngine(null).validate(computeCommand.getValueExpr(), computeCommand.getScalarName(), computeCommand.getModuleExpr(), computeCommand.getGroupByExpr(), new HashSet<String>());
                    //if (status.length > 0)
                    //    throw new RuntimeException(status[0].getMessage()); //TODO

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

    private static Object[] parseArgs(String operation, VectorOperation op) {
        if (!operation.contains("("))
            operation = "()";

        String argsString = StringUtils.removeEnd(StringUtils.substringAfter(operation, "("), ")");
        String[] argTokens = StringUtils.isBlank(argsString) ? new String[0] : argsString.split(",");
        int argCount = op.getArgTypes().length;
        if (argTokens.length != argCount)
            throw new RuntimeException(op.getName() + " expects " + StringUtils.formatCounted(argCount, "arg"));

        try {
            Object[] args = new Object[argCount];
            for (int i = 0; i < argCount; i++) {
                Class<?> argType = op.getArgTypes()[i];
                if (argType == double.class)
                    args[i] = Double.valueOf(argTokens[i]); //TODO better error handling
                else if (argType == int.class)
                    args[i] = Integer.valueOf(argTokens[i]); //TODO better error handling
                else
                    throw new RuntimeException(op.getName() + ": unsupported argument type " + argType.getName());
            }
            return args;
        }
        catch (NumberFormatException e) {
            throw new RuntimeException(op.getName() + ": error in argument list");
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
