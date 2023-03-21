package drl_adhoc_clustering;
import inet.mobility.single.*;

simple Txc0
{
    gates:
        input in;
        output out;
}


module Lo extends Txc0
{
    submodules:
        mobility: RandomWaypointMobility {
            @display("p=100,50");
			speed = 1000mps;
			waitTime = 1s;
            
            constraintAreaMinX = 0m;
            constraintAreaMinY = 500m;
            constraintAreaMinZ = 0m;
            constraintAreaMaxX = 2500m;
            constraintAreaMaxY = 1000m;
            constraintAreaMaxZ = 1000m; 
        }
}

module Hi extends Txc0
{
    submodules:
        mobility: LinearMobility {
            @display("p=100,50");
            speed = 1000mps;
            
            constraintAreaMinX = 0m;
            constraintAreaMinY = 0m;
            constraintAreaMinZ = 0m;
            constraintAreaMaxX = 2500m;
            constraintAreaMaxY = 500m;
            constraintAreaMaxZ = 1000m; 
        }
}

//
// Two instances (tic and toc) of Txc1 connected both ways.
// Tic and toc will pass messages to one another.
//
network MobileClustering
{
    @display("bgb=2500,1000");
    submodules:
        hi: Hi;
        lo: Lo;
    connections:
        hi.out --> {  delay = 100ms; } --> lo.in;
        hi.in <-- {  delay = 100ms; } <-- lo.out;
}
