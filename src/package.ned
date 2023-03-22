package drl_adhoc_clustering;
import inet.*.*.RandomWaypointMobility;


module MobileNode 
{
    parameters:
        @class(BaseNode);
	    double minX @unit(m) = default(0m);
	    double minY @unit(m) = default(0m);
	    double minZ @unit(m) = default(0m);
	    double maxX @unit(m) = default(2500m);
	    double maxY @unit(m) = default(1000m);
	    double maxZ @unit(m) = default(0m);
	    double waitTime @unit(s) = default(0s);
	    double speed @unit(mps) = default(500mps);
	gates:
        input in;
        output out;
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
        	@display("ls=#4169E1");
        	// how can I know a good mean, std?
        	delay = normal(100ms, 20ms);
        }
    submodules:
        ding: MobileNode {
            @display("i=abstract/penguin");
            maxY = 500m;
            waitTime = .5s;
            speed = normal(2000mps, 200mps);
        }
        dong: MobileNode {
        	@display("i=abstract/penguin");
        	minY = 500m;
        	waitTime = 1s;
        	speed = normal(2000mps, 200mps);
        }
    connections:
        ding.out --> C --> dong.in;
        dong.out --> C --> ding.in;
}
