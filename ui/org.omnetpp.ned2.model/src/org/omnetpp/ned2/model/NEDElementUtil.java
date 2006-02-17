package org.omnetpp.ned2.model;

/**
 * Constants for NEDElement and subclasses
 */
public interface NEDElementUtil
{
    public static final int NED_GATEDIR_INPUT = 101;
    public static final int NED_GATEDIR_OUTPUT = 102;

    public static final int NED_ARROWDIR_LEFT = 201;
    public static final int NED_ARROWDIR_RIGHT = 202;

    public static final int NED_CONST_BOOL = 301;
    public static final int NED_CONST_INT = 302;
    public static final int NED_CONST_REAL = 303;
    public static final int NED_CONST_STRING = 304;
    public static final int NED_CONST_TIME = 305;

    public static final String io_vals[] = {"input", "output"};
    public static final int io_nums[] = {NED_GATEDIR_INPUT, NED_GATEDIR_OUTPUT};
    public static final int io_n = 2;

    public static final String lr_vals[] = {"left", "right"};
    public static final int lr_nums[] = {NED_ARROWDIR_LEFT, NED_ARROWDIR_RIGHT};
    public static final int lr_n = 2;

    public static final String type_vals[] = {"bool", "int", "real", "string", "time"};
    public static final int type_nums[] = {NED_CONST_BOOL, NED_CONST_INT, NED_CONST_REAL, NED_CONST_STRING, NED_CONST_TIME};
    public static final int type_n = 5;
};

