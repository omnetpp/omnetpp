package org.omnetpp.ned.model;

/**
 * Constants for INEDElement and subclasses
 */
public interface NEDElementConstants
{
    public static final int  NED_GATETYPE_NONE = 100;
    public static final int  NED_GATETYPE_INPUT = 101;
    public static final int  NED_GATETYPE_OUTPUT = 102;
    public static final int  NED_GATETYPE_INOUT = 103;

    public static final int  NED_ARROWDIR_R2L = 201;
    public static final int  NED_ARROWDIR_L2R = 202;
    public static final int  NED_ARROWDIR_BIDIR = 203;

    public static final int  NED_PARTYPE_NONE = 300;
    public static final int  NED_PARTYPE_DOUBLE = 301;
    public static final int  NED_PARTYPE_INT = 302;
    public static final int  NED_PARTYPE_STRING = 303;
    public static final int  NED_PARTYPE_BOOL = 304;
    public static final int  NED_PARTYPE_XML = 305;

    public static final int  NED_CONST_DOUBLE = 401;
    public static final int  NED_CONST_INT = 402;
    public static final int  NED_CONST_STRING = 403;
    public static final int  NED_CONST_BOOL = 404;
    public static final int  NED_CONST_UNIT = 405;
    public static final int  NED_CONST_SPEC = 406;

    public static final int  NED_SUBGATE_NONE = 500;
    public static final int  NED_SUBGATE_I = 501;
    public static final int  NED_SUBGATE_O = 502;

    public static final String gatetype_vals[] = {"", "input", "output", "inout"};
    public static final int gatetype_nums[] = {NED_GATETYPE_NONE, NED_GATETYPE_INPUT, NED_GATETYPE_OUTPUT, NED_GATETYPE_INOUT};
    public static final int gatetype_n = 4;

    public static final String arrowdir_vals[] = {"l2r", "r2l", "bidir"};
    public static final int arrowdir_nums[] = {NED_ARROWDIR_L2R, NED_ARROWDIR_R2L, NED_ARROWDIR_BIDIR};
    public static final int arrowdir_n = 3;

    public static final String partype_vals[] = {"", "double", "int", "string", "bool", "xml"};
    public static final int partype_nums[] = {NED_PARTYPE_NONE, NED_PARTYPE_DOUBLE, NED_PARTYPE_INT, NED_PARTYPE_STRING, NED_PARTYPE_BOOL, NED_PARTYPE_XML};
    public static final int partype_n = 6;

    public static final String littype_vals[] = {"double", "int", "string", "bool", "unit", "spec"};
    public static final int littype_nums[] = {NED_CONST_DOUBLE, NED_CONST_INT, NED_CONST_STRING, NED_CONST_BOOL, NED_CONST_UNIT, NED_CONST_SPEC};
    public static final int littype_n = 6;

    public static final String subgate_vals[] = {"", "i", "o"};
    public static final int subgate_nums[] = {NED_SUBGATE_NONE, NED_SUBGATE_I, NED_SUBGATE_O};
    public static final int subgate_n = 3;
};

