//
// Copyright (C) 2006 Rudolf Hornig
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


//
// Sends the messages to different outputs depending on messages type or priority.
// If no output found for a message, it is sent to the "rest" gate.
// Messages with type = i will be sent to out[i] (if dispatchField = type).
// Messages with priority = i will be sent to out[i] (if dispatchField = priority).
//
// @author rhornig
//
simple Classifier
{
    parameters:
        @group(Queueing);
        @display("i=block/classifier");
        string dispatchField = default("type") @enum("type","priority");
    gates:
        input in[];
        output out[];
        output rest;     // messages that were not classified
}
