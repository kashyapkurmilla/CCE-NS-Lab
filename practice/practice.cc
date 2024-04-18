#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"
#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-standards.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/energy-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-radio-energy-model-helper.h"
#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"

//
//		
//		=================	|------	n8------|	
//		|	|	|	|		|
//	n1------n2	n4	n5-----	n7		n10----	n11
//	|	|			|		|
//	|	|			|------n9-------|
//	n3	n6
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("practice");

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
 nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
 nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}

void SetUpLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
    nodeA->GetObject<Ipv4> ()->SetUp (interfaceA);
    nodeB->GetObject<Ipv4> ()->SetUp (interfaceB);
}

int main (int argc, char *argv[])
{

 bool verbose = false;
 bool printRoutingTables = false;
 bool showPings = false;
 std::string SplitHorizon ("PoisonReverse");
 
 CommandLine cmd;
 cmd.AddValue ("verbose", "turn on log components", verbose);
 cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds",printRoutingTables);
 cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
 cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon,SplitHorizon, PoisonReverse)", SplitHorizon);
 cmd.Parse (argc, argv);
 
 if (verbose)
 {
	 LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE));
	 LogComponentEnable ("RipSimpleRouting", LOG_LEVEL_INFO);
	 LogComponentEnable ("Rip", LOG_LEVEL_ALL);
	 LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
	 LogComponentEnable ("Icmpv4L4Protocol", LOG_LEVEL_ALL);
	 LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
	 LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
	 LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);
 }
 
 if (SplitHorizon == "NoSplitHorizon")
 {
 	Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));
 }
 
 else if (SplitHorizon == "SplitHorizon")
 {
 	Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));
 }
 
 else
 {
 	Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));
 }

 Time::SetResolution (Time::NS);

 LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
 LogComponentEnable("practice",LOG_LEVEL_INFO);
 
 NS_LOG_INFO ("Create Applications.");

 NodeContainer c;
 c.Create (11);
 
 MobilityHelper mobility;
 mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
 mobility.Install (c);

 NodeContainer c0 = NodeContainer(c.Get(1),c.Get(3),c.Get(4));
 
 CsmaHelper csma1;
 csma1.SetChannelAttribute("DataRate", DataRateValue(DataRate (200000)));
 csma1.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (12)));

 PointToPointHelper p2p;
 p2p.SetDeviceAttribute ("DataRate", StringValue ("6Mbps"));
 p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

 NetDeviceContainer nd0 = p2p.Install (c.Get(0),c.Get(2));  
 NetDeviceContainer nd1 = p2p.Install (c.Get(0),c.Get(1));  
 NetDeviceContainer nd2 = p2p.Install (c.Get(1),c.Get(5));
 NetDeviceContainer nd3 = csma1.Install (c0);  
 NetDeviceContainer nd4 = p2p.Install(c.Get(4),c.Get(6));
 NetDeviceContainer nd5 = p2p.Install(c.Get(6),c.Get(8));
 NetDeviceContainer nd6 = p2p.Install(c.Get(6),c.Get(7));
 NetDeviceContainer nd7 = p2p.Install(c.Get(7),c.Get(9));
 NetDeviceContainer nd8 = p2p.Install(c.Get(8),c.Get(9));
 NetDeviceContainer nd9 = p2p.Install(c.Get(9),c.Get(10));
 
 NS_LOG_INFO ("rip define.");
 
 RipHelper ripRouting;

 Ipv4ListRoutingHelper listRH;
 listRH.Add (ripRouting, 0);

 InternetStackHelper internet;
 internet.SetRoutingHelper (listRH);
 internet.Install (c); 

 Ipv4InterfaceContainer if0,if1,if2,if3,if4,if5,if6,if7,if8,if9;
 
 Ipv4AddressHelper ipv4Addr;
 ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
 if0=ipv4Addr.Assign (nd0);
 ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
 if1=ipv4Addr.Assign (nd1);
 ipv4Addr.SetBase ("10.1.3.0", "255.255.255.0");
 if2=ipv4Addr.Assign (nd2);
 ipv4Addr.SetBase ("10.1.4.0", "255.255.255.0");
 if3=ipv4Addr.Assign (nd3);
 ipv4Addr.SetBase ("10.1.5.0", "255.255.255.0");
 if4=ipv4Addr.Assign (nd4);
 ipv4Addr.SetBase ("10.1.6.0", "255.255.255.0");
 if5=ipv4Addr.Assign (nd5);
 ipv4Addr.SetBase ("10.1.7.0", "255.255.255.0");
 if6=ipv4Addr.Assign (nd6);
 ipv4Addr.SetBase ("10.1.8.0", "255.255.255.0");
 if7=ipv4Addr.Assign (nd7);
 ipv4Addr.SetBase ("10.1.9.0", "255.255.255.0");
 if8=ipv4Addr.Assign (nd8);
 ipv4Addr.SetBase ("10.1.10.0", "255.255.255.0");
 if9=ipv4Addr.Assign (nd9);
 
 if (printRoutingTables)
 {
	 RipHelper routingHelper;
	 Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);
	 routingHelper.PrintRoutingTableAt (Seconds (5.0), c.Get(1), routingStream);
 }
 
 
 NS_LOG_INFO ("send packets");
 
  OnOffHelper onoff_udp ("ns3::UdpSocketFactory", Address (InetSocketAddress (if0.GetAddress(1), 9)));
  onoff_udp.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff_udp.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff_udp.SetAttribute ("DataRate",StringValue ("255kb/s"));
  onoff_udp.SetAttribute ("PacketSize",UintegerValue(128));

  ApplicationContainer srcC = onoff_udp.Install (c.Get (9));
  srcC.Start (Seconds (2.0));
  srcC.Stop (Seconds (7.0));

  PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer sinkC = sink.Install (c.Get (2));
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (20.0));
  
  OnOffHelper onoff_tcp ("ns3::TcpSocketFactory", Address (InetSocketAddress (if0.GetAddress(1), 9)));
  onoff_tcp.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff_tcp.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff_tcp.SetAttribute ("DataRate",StringValue ("255kb/s"));
  onoff_tcp.SetAttribute ("PacketSize",UintegerValue(128));

  ApplicationContainer srcC1 = onoff_tcp.Install (c.Get (5));
  srcC1.Start (Seconds (5.0));
  srcC1.Stop (Seconds (10.0));

  PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer sinkC1 = sink2.Install (c.Get (2));
  sinkC1.Start (Seconds (1.0));
  sinkC1.Stop (Seconds (20.0));
  
  NS_LOG_INFO ("ascii running");

  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("practice.tr");
  csma1.EnableAsciiAll (stream);
  p2p.EnableAsciiAll (stream);
  
  AnimationInterface anim ("practice.xml");
  Ptr<ConstantPositionMobilityModel> s1 = c.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s2 = c.Get (1)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s3 = c.Get (2)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s4 = c.Get (3)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s5 = c.Get (4)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s6 = c.Get (5)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s7 = c.Get (6)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s8 = c.Get (7)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s9 = c.Get (8)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s10 = c.Get (9)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s11 = c.Get (10)->GetObject<ConstantPositionMobilityModel> ();

  s1->SetPosition (Vector ( 10, 50, 0 ));
  s2->SetPosition (Vector ( 40, 50, 0 ));
  s3->SetPosition (Vector ( 10, 90, 0 ));
  s4->SetPosition (Vector ( 70, 50, 0 ));
  s5->SetPosition (Vector ( 100, 50, 0 ));
  s6->SetPosition (Vector ( 40, 90, 0 ));
  s7->SetPosition (Vector ( 130, 50, 0 ));
  s8->SetPosition (Vector ( 160, 20, 0 ));
  s9->SetPosition (Vector ( 160, 80, 0 ));
  s10->SetPosition (Vector ( 190, 50, 0 ));
  s11->SetPosition (Vector ( 220, 50, 0 ));
  
  Simulator::Schedule (Seconds (4), &TearDownLink, c.Get(4),c.Get(6), 2, 1);
  Simulator::Schedule (Seconds (8), &SetUpLink, c.Get(4), c.Get(6), 2, 1);
  
  Simulator::Stop (Seconds (30.0));
  Simulator::Run ();
  
  Simulator::Destroy ();
  Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(sinkC.Get(0));
  std::cout << "Total UDP Bytes Received at node 3: " << sink1->GetTotalRx() << "\n"<<std::endl;
  Ptr<PacketSink> sink3 = DynamicCast<PacketSink>(sinkC1.Get(0));
  std::cout << "Total TCP Bytes Received at node 3: " << sink3->GetTotalRx() << "\n"<<std::endl;
  NS_LOG_INFO ("Done.");

  return 0;

}