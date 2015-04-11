/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;

/**
 * Elements that are instances of a NED type: submodule and connection.
 *
 * @author rhornig, andras
 */
public interface ISubmoduleOrConnection extends IHasName, IHasResolver, IHasDisplayString, IHasParameters {
    /**
     * Returns the compound module that contains this submodule or connection
     */
    public CompoundModuleElementEx getCompoundModule();

    /**
     * Returns the display string for this submodule or connection, assuming
     * that the actual type is the module or channel type passed in the <code>actualType</code>
     * parameter. This is useful when the submodule/channel is defined with the "like"
     * keyword, and the actual submodule type is known (and not only the <code>likeType</code>).
     * The actual type usually comes from an ini file or some other source outside the INedElement
     * tree.
     *
     * Note that it cannot be assumed that for a submodule the actualType is a module type
     * (and not a channel) and vica versa, because the NED source or inifile values may be
     * bogus.
     */
    public DisplayString getDisplayString(INedTypeElement actualType);

    /**
     * Returns the type of the submodule or connection, as it occurs in the NED source.
     * Not used for parametric submodules or connections (returns null or the empty string.)
     */
    public String getType();

    /**
     * Sets the type of the submodule or connection, as it occurs in the NED source.
     */
    public void setType(String type);

    /**
     * For a parametric submodule or connection, it returns the interface name as it occurs
     * after the "like" keyword in the NED source; otherwise it returns null or the empty string.
     */
    public String getLikeType();

    /**
     * Sets the interface name for a parametric submodule or connection.
     */
    public void setLikeType(String type);

    /**
     * For a parametric submodule or connection, it returns the type name expression as it occurs
     * between the angle brackets in the NED source; otherwise it returns null or the empty string.
     */
    public String getLikeExpr();

    /**
     * Sets the type name expression for for a parametric submodule or connection.
     */
    public void setLikeExpr(String type);

    /**
     * Returns true if the parametric type name expression (the "like-expr") is just a default value,
     * that is, it occurs in the NED text as <code>&lt;default(expression)&gt;</code>.
     */
    public boolean getIsDefault();

    /**
     * Sets whether the parametric type name expression (the "like-expr") is to be surrounded with
     * <code>default(...)</code> in the NED source.
     */
    public void setIsDefault(boolean isDefault);

    /**
     * Returns the type, or the likeType if type was not specified. For connections with
     * implicit type (i.e. where getType()==""), it returns one of "ned.IdealChannel",
     * "ned.DelayChannel" or "ned.DatarateChannel", based on the parameter assignments
     * in the connection's "parameters" section.
     */
    public String getEffectiveType();

    /**
     * Returns the typeinfo for the effective type.
     *
     * Returns null if the effective type is not filled in, or is not a valid NED type,
     * or not a type that's accepted at the given place (e.g. a channel for submodule type).
     */
    public INedTypeInfo getNedTypeInfo();

    /**
     * Returns the model element for the effective type. Equivalent to
     * getNedTypeInfo().getNedElement(), but handles nulls.
     *
     * Returns null if the effective type is not filled in, or is not a valid NED type,
     * or not a type that's accepted at the given place (e.g. a channel for submodule type).
     */
    public INedTypeElement getEffectiveTypeRef();

}
