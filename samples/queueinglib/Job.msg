//#
//# Copyright (C) 2006 Rudolf Hornig
//#
//# This program is free software; you can redistribute it and/or
//# modify it under the terms of the GNU General Public License
//# as published by the Free Software Foundation; either version 2
//# of the License, or (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


namespace queueing;

//
// Job message definition for queueing framework
//
message Job
{
    simtime_t totalQueueingTime; // total time spent standing in queues
    simtime_t totalServiceTime;  // total time spent in servers
    simtime_t totalDelayTime;    // total time spent in delay modules
    int queueCount;              // the number of queue modules visited by the job
    int delayCount;              // the number of delay modules visiteb by the job
    int generation;              // how many times the original ancestor was copied
}