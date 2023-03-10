package drl_adhoc_clustering;
import inet.mobility.single.*;

simple Txc0
{
    gates:
        input in;
        output out;
}


module Txxx extends Txc0
{
    submodules:
        mobility: LinearMobility {
            @display("p=100,50");
            speed = 10mps;
        }
}
//
// Two instances (tic and toc) of Txc1 connected both ways.
// Tic and toc will pass messages to one another.
//
network Tic
{
    @display("bgb=186,183");
    submodules:
        tic: Txxx;
        toc: Txxx {
            @display("p=141,78");
        }
    connections:
        tic.out --> {  delay = 300ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}