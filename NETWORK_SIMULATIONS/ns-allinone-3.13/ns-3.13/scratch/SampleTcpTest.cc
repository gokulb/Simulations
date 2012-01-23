#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int
main (int argc, char *argv[])
{

	NodeContainer nodes;
	nodes.Create(6);

	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	p2p.SetChannelAttribute("Delay", StringValue("10ms"));

	NetDeviceContainer devices;
	devices = p2p.Install(nodes);

	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

//Dumbbell topology
	positionAlloc->Add(Vector (100,100,0.0)); //Node1
	positionAlloc->Add(Vector (100,500,0.0));	//Node2
	positionAlloc->Add(Vector (700,100,0.0));//Node5
	positionAlloc->Add(Vector (700,500,0.0));//Node6
	positionAlloc->Add(Vector (300,300,0.0)); //Node3	
	positionAlloc->Add(Vector (500,300,0.0)); //Node4

 



}
