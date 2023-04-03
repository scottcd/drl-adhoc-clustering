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
import inet.physicallayer.wireless.unitdisk.UnitDiskRadioMedium;import inet.visualizer.contract.IIntegratedVisualizer;
import inet.node.inet.AdhocHost;
import inet.applications.contract.IApp;


simple MyApp like IApp {
    gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
}

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
