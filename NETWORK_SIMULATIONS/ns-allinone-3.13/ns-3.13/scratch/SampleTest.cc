#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

int
main (int argc, char *argv[])
{
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	
	NodeContainer nodes;
	nodes.Create(2);
	
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	p2p.SetChannelAttribute("Delay", StringValue("10ms"));

	NetDeviceContainer devices;
	devices = p2p.Install(nodes);

	InternetStackHelper network1;
	network1.Install(nodes);

	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");

	Ipv4InterfaceContainer interfaces = address.Assign (devices);
	
	UdpEchoServerHelper echoServer(9);
	UdpEchoServerHelper echoServer1(10);
	ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
	ApplicationContainer serverApps1 = echoServer1.Install(nodes.Get(1));
	serverApps.Start(Seconds(1.0));
	serverApps1.Start(Seconds(2.0));
	serverApps.Stop(Seconds(10.0));
	serverApps.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient (interfaces.GetAddress(1), 9);
	UdpEchoClientHelper echoClient1 (interfaces.GetAddress(1), 10);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
	
	echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
	echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

	ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
	ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(0));
	clientApps.Start (Seconds (2.0));
	clientApps1.Start(Seconds(3.0));
	clientApps1.Stop (Seconds (10.0));
	clientApps1.Stop(Seconds(10.0));
	
	FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor;
	monitor = flowmon.InstallAll();
	monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));	

	Simulator::Stop (Seconds(15.0));


	Simulator::Run ();
	monitor->SerializeToXmlFile("results.xml", true, true);
	Simulator::Destroy ();
	return 0;


}
