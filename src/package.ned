package drl_adhoc_clustering;
import inet.mobility.single.*;

simple Node
{
    @class(BaseNode);
    gates:
        input in;
        output out;
}

module MobileNode extends Node
{
    parameters:
	    double minX @unit(m) = default(0m);
	    double minY @unit(m) = default(0m);
	    double minZ @unit(m) = default(0m);
	    double maxX @unit(m) = default(2500m);
	    double maxY @unit(m) = default(1000m);
	    double maxZ @unit(m) = default(0m);
	    double waitTime @unit(s) = default(0s);
	    double speed @unit(mps) = default(500mps);
	submodules:
        mobility: RandomWaypointMobility {
            @display("p=100,50");
			speed = parent.speed;
			waitTime = parent.waitTime;
            constraintAreaMinX = parent.minX;
            constraintAreaMinY = parent.minY;
            constraintAreaMinZ = parent.minZ;
            constraintAreaMaxX = parent.maxX;
            constraintAreaMaxY = parent.maxY;
            constraintAreaMaxZ = parent.maxZ;
        }
}

network MobileClustering
{
    @display("bgb=2500,1000");
    types:
        channel C extends ned.DatarateChannel {
        	// how can I know a good mean, std?
        	delay = normal(100ms, 20ms);
        }
    submodules:
        ding: MobileNode {
            maxY = 500m;
            waitTime = 1s;
        }
        dong: MobileNode {
        	minY = 500m;
        	waitTime = 2s;
        	speed = 2000mps;
        }
    connections:
        ding.out --> C --> dong.in;
        dong.out --> C --> ding.in;
}
