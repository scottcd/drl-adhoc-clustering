package drl_adhoc_clustering;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.unitdisk.UnitDiskRadioMedium;import inet.visualizer.contract.IIntegratedVisualizer;
import inet.node.inet.AdhocHost;
import inet.applications.contract.IApp;



module Node extends AdhocHost
{
    parameters:
        @class(BaseNode);
        @display("i=abstract/penguin_vs");
}

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
        int numHosts = default(10);
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
        host[numHosts]: Node;
}

