#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Create nodes
  NodeContainer nodes;
  nodes.Create(13); // Adding 13 nodes

  // Set up node positions
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add(Vector(30, 20, 0.0)); // n0
  positionAlloc->Add(Vector(50, 20, 0.0)); // n1
  positionAlloc->Add(Vector(30, 120, 0.0)); // n2
  positionAlloc->Add(Vector(50, 80, 0.0)); // n3
  positionAlloc->Add(Vector(70, 70, 0.0)); // n4
  positionAlloc->Add(Vector(110, 20, 0.0)); // n5
  positionAlloc->Add(Vector(110, 70, 0.0)); // n6
  positionAlloc->Add(Vector(110, 120, 0.0)); // n7
  positionAlloc->Add(Vector(160, 70, 0.0)); // n8
  positionAlloc->Add(Vector(190, 80, 0.0)); // n9
  positionAlloc->Add(Vector(200, 140, 0.0)); // n10
  positionAlloc->Add(Vector(190, 25, 0.0)); // n11
  positionAlloc->Add(Vector(210, 25, 0.0)); // n12

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(nodes);

  // Configure point-to-point links
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("5ms"));
  NetDeviceContainer n2n3 = p2p.Install(nodes.Get(2), nodes.Get(3));

  p2p.SetDeviceAttribute("DataRate", StringValue("6Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer n9n10 = p2p.Install(nodes.Get(9), nodes.Get(10));
  
  // Configure LAN for nodes 0, 1, and 2
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("200Mbps"));
  csma.SetChannelAttribute("Delay", StringValue("8ms"));
  NetDeviceContainer n0n1n2 = csma.Install(NodeContainer(nodes.Get(0), nodes.Get(1), nodes.Get(2)));

  // Configure LAN for nodes 10, 11, and 12
  NetDeviceContainer n10n11n12 = csma.Install(NodeContainer(nodes.Get(10), nodes.Get(11), nodes.Get(12)));

  // Install internet stack
  InternetStackHelper stack;
  stack.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(n2n3);
  p2pInterfaces = address.Assign(n9n10);
  p2pInterfaces = address.Assign(n0n1n2);
  p2pInterfaces = address.Assign(n10n11n12);

  // Populate routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // TCP application: Node 12 communicates with Node 0
  uint16_t tcpPort = 9;
  Address sinkAddress(InetSocketAddress(nodes.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), tcpPort));
  PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), tcpPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install(nodes.Get(0)); // Node 0 as the receiver
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(20.0));

  OnOffHelper tcpOnOff("ns3::TcpSocketFactory", sinkAddress);
  tcpOnOff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  tcpOnOff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  tcpOnOff.SetAttribute("DataRate", DataRateValue(DataRate("448kb/s")));
  tcpOnOff.SetAttribute("PacketSize", UintegerValue(1024));

  ApplicationContainer tcpApps = tcpOnOff.Install(nodes.Get(12)); // Node 12
  tcpApps.Start(Seconds(3.0));
  tcpApps.Stop(Seconds(11.0));

  // Enable tracing
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll(ascii.CreateFileStream("nnvrf.tr"));
  AnimationInterface anim("nnvrf.xml");

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
