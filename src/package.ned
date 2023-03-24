package drl_adhoc_clustering;
import inet.mobility.single.*;


module MobileNode 
{
    parameters:
        @class(BaseNode);
        @display("i=abstract/penguin_vs");
        bool initiate = default(false);
        double timeout @unit(ms) = default(10000ms);
	gates:
        input in @loose; 
        output out @loose;
	submodules:
        mobility: RandomWaypointMobility;
}

network MobileClustering
{
    parameters:
        @display("bgb=800,560;bgi=background/terrain");
        int numHosts = default(10);
        node[0].initiate = true;
    types:
        channel C extends ned.DatarateChannel {
        	@display("ls=#4169E1");
        	delay = truncnormal(100ms, 20ms);
        }
    
    submodules:
        node[numHosts]: MobileNode;
    connections:
        for i=0..sizeof(node)-2
        {
            node[i].out --> C --> node[i+1].in;
        }
        node[sizeof(node)-1].out --> C --> node[0].in;
}
