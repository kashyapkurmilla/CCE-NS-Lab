

// Network topology
//
//        10.1.1.0        
//    n0----------n1   n2   n3   n4
//                |    |     |    |
//              ======================
//               Lan  10.2.1.0
//
// - Multicast source is at node n0;
// - Multicast forwarded by node n1 onto LAN;
// - Nodes n0, n1, n2, n3, and n4 receive the multicast frame.
// - Node n4 listens for the data 

#include <iostream>     //i
#include <fstream>     //i

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"       //i
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("l4q1m");

int 
main (int argc, char *argv[])
{
  //
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  //
  LogComponentEnable ("l4q1m", LOG_LEVEL_INFO);                       //i
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);            //i   
  

  //
  // Set up default values for the simulation.
  //
  // Select DIX/Ethernet II-style encapsulation (no LLC/Snap header)
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix"));    //i

  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  //CommandLine cmd (__FILE__);
  //cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (5);
  // We will later want two subcontainers of these nodes, for the two LANs
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (2));//point to point                 //i
  NodeContainer c1 = NodeContainer (c.Get (1),c.Get (2), c.Get (3), c.Get (4));            //i

  NS_LOG_INFO ("Build Topology.");
  PointToPointHelper pp;
  pp.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pp.SetChannelAttribute ("Delay", StringValue ("2ms"));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  // We will use these NetDevice containers later, for IP addressing
  NetDeviceContainer nd0 = pp.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN

  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c);            //c

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4Addr.Assign (nd0);                             //i  
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");   //i
  ipv4Addr.Assign (nd1);

  NS_LOG_INFO ("Configure multicasting.");
  //
  // Now we can configure multicasting.  As described above, the multicast 
  // source is at node zero, which we assigned the IP address of 10.1.1.1 
  // earlier.  We need to define a multicast group to send packets to.  This
  // can be any multicast address from 224.0.0.0 through 239.255.255.255
  // (avoiding the reserved routing protocol addresses).
  //

  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  // Now, we will set up multicast routing.  We need to do three things:
  // 1) Configure a (static) multicast route on node n1
  // 2) Set up a default multicast route on the sender n0 
  // 3) Have node n4 join the multicast group
  // We have a helper that can help us with static multicast
  Ipv4StaticRoutingHelper multicast;

  // 1) Configure a (static) multicast route on node n1 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (2);  // The node in question               //?
  Ptr<NetDevice> inputIf = nd0.Get (1);  // The input NetDevice
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd1.Get (1));  // (we only need one NetDevice here)

  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                               multicastGroup, inputIf, outputDevices);

  // 2) Set up a default multicast route on the sender n0 
  Ptr<Node> sender = c.Get (0);
  Ptr<NetDevice> senderIf = nd0.Get (0);
  multicast.SetDefaultMulticastRoute (sender, senderIf);

  //
  // Create an OnOff application to send UDP datagrams from node zero to the
  // multicast group (node four will be listening).
  //
  NS_LOG_INFO ("Create Applications.");

  uint16_t multicastPort = 9;   // Discard port (RFC 863)

  // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (multicastGroup, multicastPort)));
  onoff.SetConstantRate (DataRate ("255b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));

  ApplicationContainer srcC = onoff.Install (c0.Get (0));

  //
  // Tell the application when to start and stop.
  //
  srcC.Start (Seconds (1.));
  srcC.Stop (Seconds (10.));

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

  ApplicationContainer sinkC = sink.Install (c1.Get (3)); // Node n4                //c
  // Start the sink
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (10.0));

  NS_LOG_INFO ("Configure Tracing.");
  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Ascii trace output will be sent to the file "csma-multicast.tr"
  // //
  // AsciiTraceHelper ascii;
  // csma.EnableAsciiAll (ascii.CreateFileStream ("csma-multicast.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     csma-multicast-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  // csma.EnablePcapAll ("csma-multicast", false);

  //
  // Now, do the actual simulation.
  //
    AsciiTraceHelper ascii;                                             //i
  csma.EnableAsciiAll (ascii.CreateFileStream ("l4q1m.tr"));            //i
  NS_LOG_INFO ("Run Simulation.");
  AnimationInterface anim ("l4q1m.xml");                                //i
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

/*
Create nodes.
Build Topology.
Add IP Stack.
Assign IP Addresses.
Configure multicasting.
Create Applications.
Configure Tracing.
Run Simulation.
Done
*/