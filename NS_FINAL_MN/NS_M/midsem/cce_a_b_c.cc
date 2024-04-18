#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"

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
  nodes.Create(11); // Adding 11 nodes

  // Set up node positions
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add(Vector(40, 50, 0.0)); // n0
  positionAlloc->Add(Vector(20, 20, 0.0)); // n1
  positionAlloc->Add(Vector(40, 20, 0.0)); // n2
  positionAlloc->Add(Vector(20, 50, 0.0)); // n3
  positionAlloc->Add(Vector(60, 20, 0.0)); // n4
  positionAlloc->Add(Vector(80, 20, 0.0)); // n5
  positionAlloc->Add(Vector(100, 20, 0.0)); // n6
  positionAlloc->Add(Vector(110, 0, 0.0)); // n7
  positionAlloc->Add(Vector(110, 40, 0.0)); // n8
  positionAlloc->Add(Vector(120, 20, 0.0)); // n9
  positionAlloc->Add(Vector(130, 20, 0.0)); // n10

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(nodes);

  // Configure point-to-point links
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("6Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer n1n2 = p2p.Install(nodes.Get(1), nodes.Get(2));

  p2p.SetDeviceAttribute("DataRate", StringValue("3Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("5ms"));
  NetDeviceContainer n3n2 = p2p.Install(nodes.Get(3), nodes.Get(2));

  p2p.SetDeviceAttribute("DataRate", StringValue("3Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("5ms"));
  NetDeviceContainer n0n2 = p2p.Install(nodes.Get(0), nodes.Get(2));

  // Configure LAN for nodes 2, 4, and 5
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
  csma.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer n2n4n5 = csma.Install(NodeContainer(nodes.Get(2), nodes.Get(4), nodes.Get(5)));

  // Configure drop-tail queues for point-to-point links
  p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("20p"));
  NetDeviceContainer n6n7 = p2p.Install(nodes.Get(5), nodes.Get(6));
  NetDeviceContainer n6n8 = p2p.Install(nodes.Get(5), nodes.Get(7));
  NetDeviceContainer n7n9 = p2p.Install(nodes.Get(6), nodes.Get(8));
  NetDeviceContainer n8n9 = p2p.Install(nodes.Get(7), nodes.Get(8));

  // Configure LAN for nodes 9 and 10
  NetDeviceContainer n9n10 = csma.Install(NodeContainer(nodes.Get(9), nodes.Get(10)));

  // Install internet stack
  InternetStackHelper stack;
  stack.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(n1n2);

  address.SetBase("10.1.2.0", "255.255.255.0");
  p2pInterfaces = address.Assign(n3n2);

  address.SetBase("10.1.3.0", "255.255.255.0");
  p2pInterfaces = address.Assign(n0n2);

  address.SetBase("10.1.4.0", "255.255.255.0");
  p2pInterfaces = address.Assign(n2n4n5);

  address.SetBase("10.1.5.0", "255.255.255.0");
  NetDeviceContainer csmaDevices;
  csmaDevices.Add(n9n10.Get(0));
  csmaDevices.Add(n9n10.Get(1));
  Ipv4InterfaceContainer csmaInterfaces = address.Assign(csmaDevices);

  // Populate routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // UDP application: Node 10 communicates with Node 3
  uint16_t udpPort = 9; // Can be any port number
  UdpEchoClientHelper udpClient (nodes.Get(3)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(), udpPort);
  udpClient.SetAttribute ("MaxPackets", UintegerValue (1));
  udpClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); // Time between packets
  udpClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer udpApps = udpClient.Install (nodes.Get (9)); // Node 10
  udpApps.Start (Seconds (1.09));
  udpApps.Stop (Seconds (12.0));

  // TCP application: Node 6 communicates with Node 1
  uint16_t tcpPort = 9; // Use the same
  Address sinkAddress (InetSocketAddress (nodes.Get(1)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(), tcpPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), tcpPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1)); // Node 1 as the receiver
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (20.0));

  OnOffHelper tcpOnOff ("ns3::TcpSocketFactory", sinkAddress);
  tcpOnOff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.05]")); // 50 milliseconds
  tcpOnOff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.05]")); // 50 milliseconds
  tcpOnOff.SetAttribute ("DataRate", DataRateValue (DataRate ("448kb/s")));
  tcpOnOff.SetAttribute ("PacketSize", UintegerValue (1500)); // Packet size set to 1500 bytes

  // Calculate the maximum bytes to be sent
  uint32_t maxBytes = 25 * 1500;
  tcpOnOff.SetAttribute ("MaxBytes", UintegerValue (maxBytes));

  ApplicationContainer tcpApps = tcpOnOff.Install (nodes.Get (10)); // Node 10
  tcpApps.Start (Seconds (1.0)); // Start sending packets
  tcpApps.Stop (Seconds (12.0)); // Stop sending packets

  // Multicasting scenario
  uint16_t port = 9; // UDP port

  // Define multicast group address
  Ipv4Address multicastGroup("239.255.1.1");

  // Create UDP multicast sender on node N7
  UdpEchoServerHelper udpMulticastServer(multicastGroup, port);
  ApplicationContainer multicastSender = udpMulticastServer.Install(nodes.Get(7)); // N7
  multicastSender.Start(Seconds(4.0)); // Start multicast transmission at 4th second
  multicastSender.Stop(Seconds(6.0));  // Stop multicast transmission at 6th second

  // Create UDP multicast receivers on nodes N0, N2, N3, N4, N5, and N10
  UdpEchoClientHelper udpMulticastClient(multicastGroup, port);
  udpMulticastClient.SetAttribute("MaxPackets", UintegerValue(1));
  udpMulticastClient.SetAttribute("Interval", TimeValue(Seconds(1.0))); // Time between packets
  udpMulticastClient.SetAttribute("PacketSize", UintegerValue(1024));

  ApplicationContainer multicastReceivers;
  multicastReceivers.Add(udpMulticastClient.Install(nodes.Get(0))); // N0
  multicastReceivers.Add(udpMulticastClient.Install(nodes.Get(2))); // N2
  multicastReceivers.Add(udpMulticastClient.Install(nodes.Get(3))); // N3
  multicastReceivers.Add(udpMulticastClient.Install(nodes.Get(4))); // N4
  multicastReceivers.Add(udpMulticastClient.Install(nodes.Get(5))); // N5
  multicastReceivers.Add(udpMulticastClient.Install(nodes.Get(6))); // N6
  multicastReceivers.Start(Seconds(3.0)); // Start receiving multicast packets

  AsciiTraceHelper ascii;                                   
  csma.EnableAsciiAll (ascii.CreateFileStream ("ccea.tr"));  

  AnimationInterface anim ("ccea.xml");   

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
