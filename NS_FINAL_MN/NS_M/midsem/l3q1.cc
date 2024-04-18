#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/netanim-module.h" //i 
#include<fstream>               //i  [ file stream]
// Default Network Topology
//
//       10.1.1.0       10.1.3.0
// n0-------------n1-------------- n2   n3   n4   n5
//                  point-to-point  |    |    |    |
//                                  ================
//                                    LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int main (int argc, char *argv[])
{
   // std::string animeFile="l2q2.xml";
  uint32_t nCsma = 4;

      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer p2pNodes;
  p2pNodes.Create (3);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (2)); //c 
  csmaNodes.Create (nCsma);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices1,p2pDevices2;
  p2pDevices1 = pointToPoint.Install (p2pNodes.Get(0),p2pNodes.Get(1));
  p2pDevices2 = pointToPoint.Install (p2pNodes.Get(1),p2pNodes.Get(2));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (p2pNodes.Get (1)); //i
  stack.Install (csmaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer p2pInterfaces1,p2pInterfaces2;
  p2pInterfaces1 = address.Assign (p2pDevices1);
  p2pInterfaces2 = address.Assign (p2pDevices2);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (20.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (20.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  AsciiTraceHelper ascii;                                           //i
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("l3q1.tr"));   //i
  csma.EnableAsciiAll(ascii.CreateFileStream("l3q1.tr"));           //i
  
  AnimationInterface anim("l3q1.xml");
  Simulator::Run ();
   Simulator::Destroy ();
  return 0;
}
  
/*

At time +2s client sent 1024 bytes to 10.1.2.5 port 9
At time +2.01049s server received 1024 bytes from 10.1.1.1 port 49153
At time +2.01049s server sent 1024 bytes to 10.1.1.1 port 49153

At time +3s client sent 1024 bytes to 10.1.2.5 port 9
At time +3.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +3.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +4s client sent 1024 bytes to 10.1.2.5 port 9
At time +4.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +4.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +5s client sent 1024 bytes to 10.1.2.5 port 9
At time +5.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +5.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +6s client sent 1024 bytes to 10.1.2.5 port 9
At time +6.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +6.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +7s client sent 1024 bytes to 10.1.2.5 port 9
At time +7.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +7.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +8s client sent 1024 bytes to 10.1.2.5 port 9
At time +8.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +8.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +9s client sent 1024 bytes to 10.1.2.5 port 9
At time +9.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +9.00746s server sent 1024 bytes to 10.1.1.1 port 49153

At time +10s client sent 1024 bytes to 10.1.2.5 port 9
At time +10.0075s server received 1024 bytes from 10.1.1.1 port 49153
At time +10.0075s server sent 1024 bytes to 10.1.1.1 port 49153

At time +11s client sent 1024 bytes to 10.1.2.5 port 9
At time +11.0075s server received 1024 bytes from 10.1.1.1 port 49153
At time +11.0075s server sent 1024 bytes to 10.1.1.1 port 49153

*/
 
