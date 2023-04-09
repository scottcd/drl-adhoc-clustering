//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package drl_adhoc_clustering;


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.unitdisk.UnitDiskRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.node.inet.AdhocHost;
import inet.applications.contract.IApp;


simple DRLApp like IApp {
    gates:
        input socketIn @labels(TcpControlInfo/up);
        output socketOut @labels(TcpControlInfo/down);
        output out @loose;
}

simple TCPAdhocServerApp like IApp {
	parameters:
	    string localAddress = default(""); // local address; may be left empty ("")
        int localPort = default(5000);     // localPort number to listen on
        double replyDelay @unit(s) = default(0s);
        @display("i=block/app");
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);    // extra time after lifecycle stop operation finished
        double stopOperationTimeout @unit(s) = default(2s);    // timeout value for lifecycle stop operation
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
    gates:
        input socketIn @labels(TcpControlInfo/up);
        output socketOut @labels(TcpControlInfo/down);
        input drlIn @loose;
}

simple TCPAdhocClientApp like IApp {
	parameters:
		int numNodes = default(10);
	    @lifecycleSupport;
	    string localAddress = default(""); // may be left empty ("")
        int localPort = default(-1); // port number to listen on
        string connectAddress = default("");  // server address (may be symbolic)
        int connectPort = default(5000); // port number to connect to
        double startTime @unit(s) = default(1s); // time first session begins
        double stopTime @unit(s) = default(-1s);  // time of finishing sending, negative values mean forever
        volatile int numRequestsPerSession = default(1);  // number of requests sent per session
        volatile int requestLength @unit(B) = default(200B); // length of a request
        volatile int replyLength @unit(B) = default(1MiB); // length of a reply
        volatile double thinkTime @unit(s); // time gap between requests
        volatile double idleInterval @unit(s); // time gap between sessions
        volatile double reconnectInterval @unit(s) = default(30s);  // if connection breaks, waits this much before trying to reconnect
        int timeToLive = default(-1); // if not -1, set the TTL (IPv4) or Hop Limit (IPv6) field of sent packets to this value
        int dscp = default(-1); // if not -1, set the DSCP (IPv4/IPv6) field of sent packets to this value
        int tos = default(-1); // if not -1, set the Type Of Service (IPv4) / Traffic Class (IPv6) field of sent packets to this value
        @display("i=block/app");
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);    // extra time after lifecycle stop operation finished
        double stopOperationTimeout @unit(s) = default(2s);    // timeout value for lifecycle stop operation
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[connect](type=long);  // 1 for open, -1 for close
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        @statistic[numActiveSessions](title="number of active sessions"; source=warmup(sum(connect)); record=max,timeavg,vector; interpolationmode=sample-hold; autoWarmupFilter=false);
        @statistic[numSessions](title="total number of sessions"; source="sum(connect+1)/2"; record=last);
    gates:
        input socketIn @labels(TcpControlInfo/up);
        output socketOut @labels(TcpControlInfo/down);
        input drlIn @loose;
}

//simple MyApp extends TCPAppBase {
//}

network MobileClustering
{
    parameters:
        @display("bgb=800,560;bgi=background/terrain");
        @statistic[receptionPower](host="receptionMinSignalPower(radioMedium.signalArrivalStarted)"; record=last);
        int numNodes = default(10);
    submodules:
        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))> like IIntegratedVisualizer if typename != "" {
            @display("p=30,-50");
        }
       	configurator: Ipv4NetworkConfigurator {
       		@display("p=130,-50");
       	}
        radioMedium: UnitDiskRadioMedium {
            @display("p=230,-50");
        }
        node[numNodes]: AdhocHost {
	        @display("i=abstract/penguin_vs");
        }
}


