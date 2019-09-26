#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MixedSendExample");

int main(int argc, char *argv[]){

	bool tracing = false;
	uint32_t TxBytes = 0;
	uint32_t nCsma = 2;

	CommandLine cmd;
	cmd.AddValue("tracing","Enable Ascii Tracing",tracing);
	cmd.AddValue("TxBytes","Number of Bytes to be transferred",TxBytes);
	cmd.AddValue("nCsma","Number of csma nodes in the LAN",nCsma);
	cmd.Parse(argc,argv);

	NS_LOG_UNCOND("Creating Nodes");
	NodeContainer nodes;
	nodes.Create(nCsma);

	NS_LOG_UNCOND("Establishing Channel Attributes");
	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate",StringValue("10Mbps"));
	csma.SetChannelAttribute("Delay",StringValue("1ms"));
	NetDeviceContainer csmaDevices;
	csmaDevices = csma.Install(nodes);

	NS_LOG_UNCOND("Installing Internet Stack");
	InternetStackHelper stack;
	stack.Install(nodes);

	NS_LOG_UNCOND("Assigning IP Addresses");
	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0","255.255.255.0");  
	Ipv4InterfaceContainer csmaInterfaces;
	csmaInterfaces = address.Assign(csmaDevices);
	uint16_t portNo = 9;

	NS_LOG_UNCOND("Creating BulkSend Application");
	BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces.GetAddress(1),portNo));
	source.SetAttribute("MaxBytes",UintegerValue(TxBytes));
	ApplicationContainer sourceApps = source.Install(nodes.Get(nCsma-2));
	sourceApps.Start (Seconds(0.0));
	sourceApps.Stop (Seconds(10.0));

	NS_LOG_UNCOND("Creating PacketSink Application");
	PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),portNo));
	ApplicationContainer sinkApps = sink.Install(nodes.Get(nCsma-1));
	sinkApps.Start (Seconds(0.0));
	sinkApps.Stop (Seconds(10.0));

	//To populate routing tables for the csma channel
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	if(tracing){
		NS_LOG_UNCOND("Creating Ascii traces");
		AsciiTraceHelper ascii;
		csma.EnableAsciiAll(ascii.CreateFileStream ("csma-bulk-send.tr"));
		// csma.EnablePcap ("csma", csmaDevices.Get (clientNode), true);
	}

	NS_LOG_UNCOND("Running Simulation");
	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_UNCOND("Done");

	Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  	std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
}