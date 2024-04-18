//
// n0-------n2-------n1
//          |
//          |
///
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h" //i
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lab1q1");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (4);

  //0 and 2(link1)
  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));
  pointToPoint1.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("3p")); //i

  //1 and 2 (link2)
  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("3ms"));
  pointToPoint2.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("3p"));
  //2 and 3 (link2)
  PointToPointHelper pointToPoint3;
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("4ms"));
  pointToPoint3.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("3p"));  

  /*  NetDeviceContainer devices = pointToPoint.Install (nodes); */
  NetDeviceContainer device1 = pointToPoint1.Install (nodes.Get(0), nodes.Get(2));  //i
  NetDeviceContainer device2 = pointToPoint2.Install (nodes.Get(1), nodes.Get(2));  //i
  NetDeviceContainer device3 = pointToPoint3.Install (nodes.Get(3), nodes.Get(2));  //i

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

//Ipv4InterfaceContainer interfaces = address.Assign (devices);
  Ipv4InterfaceContainer if1 = address.Assign (device1);
  Ipv4InterfaceContainer if2 = address.Assign (device2);
  Ipv4InterfaceContainer if3 = address.Assign (device3);
  
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get(2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  

  UdpEchoClientHelper echoClient (if3.GetAddress (1), 9); //c  if3[last interface name]
  echoClient.SetAttribute ("MaxPackets", UintegerValue (16));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (5.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (1));  //c
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps2 = echoClient.Install (nodes.Get (0)); //c
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps3 = echoClient.Install (nodes.Get (3));  //c
  clientApps3.Start (Seconds (2.0));
  clientApps3.Stop (Seconds (10.0));
 
 AnimationInterface anim("l1q2.xml");   //i

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
/*
student@MIT-ICT-LAB9-19:/opt/ns-allinone-3.35/ns-3.35$ ./waf
student@MIT-ICT-LAB9-19:/opt/ns-allinone-3.35/ns-3.35$ ./waf --run scratch/l1q2

At time +2s client sent 1024 bytes to 10.1.1.6 port 9
At time +2s client sent 1024 bytes to 10.1.1.6 port 9
At time +2s client sent 1024 bytes to 10.1.1.6 port 9
At time +2.00284s server received 1024 bytes from 10.1.1.1 port 49153
At time +2.00284s server sent 1024 bytes to 10.1.1.1 port 49153
At time +2.00484s server received 1024 bytes from 10.1.1.5 port 49153
At time +2.00484s server sent 1024 bytes to 10.1.1.5 port 49153
At time +2.00505s server received 1024 bytes from 10.1.1.3 port 49153
At time +2.00505s server sent 1024 bytes to 10.1.1.3 port 49153
At time +2.00969s client received 1024 bytes from 10.1.1.6 port 9
At time +3s client sent 1024 bytes to 10.1.1.6 port 9
At time +3s client sent 1024 bytes to 10.1.1.6 port 9
At time +3s client sent 1024 bytes to 10.1.1.6 port 9
At time +3.00284s server received 1024 bytes from 10.1.1.1 port 49153
At time +3.00284s server sent 1024 bytes to 10.1.1.1 port 49153
At time +3.00484s server received 1024 bytes from 10.1.1.5 port 49153
At time +3.00484s server sent 1024 bytes to 10.1.1.5 port 49153
At time +3.00505s server received 1024 bytes from 10.1.1.3 port 49153
At time +3.00505s server sent 1024 bytes to 10.1.1.3 port 49153
At time +3.00969s client received 1024 bytes from 10.1.1.6 port 9
At time +4s client sent 1024 bytes to 10.1.1.6 port 9
At time +4s client sent 1024 bytes to 10.1.1.6 port 9
At time +4s client sent 1024 bytes to 10.1.1.6 port 9
At time +4.00284s server received 1024 bytes from 10.1.1.1 port 49153
At time +4.00284s server sent 1024 bytes to 10.1.1.1 port 49153
At time +4.00484s server received 1024 bytes from 10.1.1.5 port 49153
At time +4.00484s server sent 1024 bytes to 10.1.1.5 port 49153
At time +4.00505s server received 1024 bytes from 10.1.1.3 port 49153
At time +4.00505s server sent 1024 bytes to 10.1.1.3 port 49153
At time +4.00969s client received 1024 bytes from 10.1.1.6 port 9

*/
