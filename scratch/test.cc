/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"
#include <string>
#include <fstream>


using namespace ns3;

Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
 uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

// static void
// GoodputSampling (std::string fileName, ApplicationContainer app, Ptr<OutputStreamWrapper> stream, float period)
// {
//   Simulator::Schedule (Seconds (period), &GoodputSampling, fileName, app, stream, period);
//   double goodput;
//   uint64_t totalPackets = DynamicCast<PacketSink> (app.Get (0))->GetTotalRx ();
//   goodput = totalPackets * 8 / (Simulator::Now ().GetSeconds () * 1024); // Kbit/s
//   *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << goodput << std::endl;
// }

static void
GoodputSampling ()
{
  Simulator::Schedule (Seconds (1), &GoodputSampling);
  double goodput;
  // uint64_t totalPackets = DynamicCast<PacketSink> (app.Get (0))->GetTotalRx ();
  uint64_t totalPackets = sink->GetTotalRx ();
  goodput = totalPackets * 8 / (Simulator::Now ().GetSeconds () * 1024); // Kbit/s
  //*stream->GetStream () << Simulator::Now ().GetSeconds () << " " << goodput << std::endl;
  std::cout << Simulator::Now ().GetSeconds () << "\t" << goodput << "" << std::endl;

}

void
 CalculateThroughput ()
 {
   Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
   double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
   //std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
   std::cout << now.GetSeconds () << "\t" << cur << "" << std::endl;
   lastTotalRx = sink->GetTotalRx ();
   Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
 }

int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("Scratch Simulator");

  uint32_t maxBytes = 0;
  CommandLine cmd;
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes1;
  nodes1.Create (2);

  NodeContainer nodes2;
  nodes2.Add(nodes1.Get(1));
  nodes2.Create(1);

  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));

  PointToPointHelper pointToPoint2;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("5ms"));

  NetDeviceContainer devices1, devices2;
  devices1 = pointToPoint1.Install (nodes1);
  devices2 = pointToPoint2.Install (nodes2);

  InternetStackHelper stack;
  stack.Install (nodes1);
  stack.Install (nodes2.Get(1));

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interface1;
  interface1 = address.Assign (devices1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interface2;
  interface2 = address.Assign (devices2);

  //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


   uint16_t port = 9; 

   BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (interface1.GetAddress (1), port));
   source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
   ApplicationContainer sourceApps = source.Install (nodes1.Get (0));
   sourceApps.Start (Seconds (1));
   sourceApps.Stop (Seconds (9));


  /*OnOffHelper onOffHelper ("ns3::UdpSocketFactory", Address ());
  onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));
  onOffHelper.SetAttribute ("Remote", interface2.GetAddress(0));

  ApplicationContainer spokeApps = onOffHelper.Install(nodes2.Get(1));
  spokeApps.Start (Seconds (1.0));
  spokeApps.Stop (Seconds (10.0));*/

  PacketSinkHelper packetSinkHelper2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer sinkApps2 = packetSinkHelper2.Install (nodes1.Get (1)); 
  sinkApps2.Start (Seconds (0));
  sinkApps2.Stop (Seconds (10));
  sink = StaticCast<PacketSink> (sinkApps2.Get (0));

  Simulator::Schedule (Seconds (1.1), &CalculateThroughput);
  Simulator::Schedule (Seconds (1.1), &GoodputSampling);


  NS_LOG_INFO ("Run Simulation.");

  Simulator::Stop (Seconds (10 + 1));
  Simulator::Run ();
  //double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * 10));

  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps2.Get (0));

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  //std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;

  return 0;

}