package drl_adhoc_clustering;
import inet.mobility.single.*;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.contract.INetworkNode;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.node.inet.AdhocHost;

module Node extends AdhocHost
{
    parameters:
        @class(BaseNode);
        @display("i=abstract/penguin_vs");
}

network MobileClustering
{
    parameters:
        @display("bgb=800,560;bgi=background/terrain");
        @statistic[receptionPower](host="receptionMinSignalPower(radioMedium.signalArrivalStarted)"; record=last);
        int numHosts = default(10);
    submodules:
        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))> like IIntegratedVisualizer if typename != "" {
            @display("p=30,-50");
        
        }
       	configurator: Ipv4NetworkConfigurator {
       		@display("p=130,-50");
       	}
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=230,-50");
        }
        host[numHosts]: Node;
}
