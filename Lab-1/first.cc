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
#include "ns3/netanim-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");
int
main (int argc, char *argv[])
{
 CommandLine cmd (__FILE__);
 cmd.Parse (argc, argv);
 
 Time::SetResolution (Time::NS);
 LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
 LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
 NodeContainer nodes;
 nodes.Create (3);
 

 PointToPointHelper pointToPoint1, pointToPoint2;
  NetDeviceContainer devices1 , devices2;
 
 pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
 pointToPoint1.SetChannelAttribute ("Delay", StringValue ("10ms"));
 devices1.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(1)));
 
 pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("100kbps"));
 pointToPoint2.SetChannelAttribute ("Delay", StringValue ("5ms"));
 devices2.Add(pointToPoint2.Install(nodes.Get(1), nodes.Get(2)));

 InternetStackHelper stack;
 stack.Install (nodes);
 
 Ipv4AddressHelper address1, address2;
 address1.SetBase ("10.1.1.0", "255.255.255.0");
 address2.SetBase ("10.1.2.0", "255.255.255.0");
 
 Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
 Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);
 
 UdpEchoServerHelper echoServer (9);
 
 ApplicationContainer serverApps = echoServer.Install (nodes.Get (2));
 serverApps.Start (Seconds (1.0));
 serverApps.Stop (Seconds (20.0));
 UdpEchoClientHelper echoClient (interfaces2.GetAddress (1), 9);
 
 echoClient.SetAttribute ("MaxPackets", UintegerValue (6));
 echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
 echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
 
 ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
 clientApps.Start (Seconds (2.0));
 clientApps.Stop (Seconds (10.0));
 Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
 AnimationInterface anim ("animFile.xml" );
 Simulator::Run ();
 Simulator::Destroy ();
 return 0;
}
