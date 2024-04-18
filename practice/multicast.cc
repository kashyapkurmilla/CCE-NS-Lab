#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main (int argc, char *argv[])
{
    CommandLine cmd (__FILE__);
    cmd.Parse (argc, argv);

    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer c;
    c.Create (8);

    NodeContainer c0 = NodeContainer(c.Get(0),c.Get(1),c.Get(2));
    NodeContainer c1 = NodeContainer(c.Get(2),c.Get(3),c.Get(4));
    NodeContainer c2 = NodeContainer(c.Get(5),c.Get(6),c.Get(7));
    NodeContainer p0 = NodeContainer(c.Get(4),c.Get(5));

    CsmaHelper csma1,csma2,csma3;
    csma1.SetChannelAttribute("DataRate", DataRateValue(DataRate (200000)));
    csma1.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (12)));

    csma2.SetChannelAttribute("DataRate", DataRateValue(DataRate (200000)));
    csma2.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));

    csma3.SetChannelAttribute("DataRate", DataRateValue(DataRate (200000)));
    csma3.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (8)));

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("6Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer nd0 = csma1.Install (c0);  // First LAN
    NetDeviceContainer nd1 = csma2.Install (c1);  // Second LAN
    NetDeviceContainer nd2 = csma3.Install (c2);  // Third LAN
    NetDeviceContainer nd3 = p2p.Install (p0);    // Point-to-Point

    InternetStackHelper internet;
    internet.Install (c); 

    Ipv4AddressHelper ipv4Addr;
    ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
    ipv4Addr.Assign (nd0);
    ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
    ipv4Addr.Assign (nd1);
    ipv4Addr.SetBase ("10.1.3.0", "255.255.255.0");
    ipv4Addr.Assign (nd2);
    ipv4Addr.SetBase ("10.1.4.0", "255.255.255.0");
    ipv4Addr.Assign (nd3);

    Ipv4Address multicastSource ("10.1.1.1");
    Ipv4Address multicastGroup ("225.1.2.4");

    Ipv4StaticRoutingHelper multicast;

    Ptr<Node> multicastRouter = c.Get (2);  // The node in question
    Ptr<NetDevice> inputIf = nd0.Get (2);    // The input NetDevice
    NetDeviceContainer outputDevices;        // A container of output NetDevices
    outputDevices.Add (nd1.Get (0));         // (we only need one NetDevice here)

    Ptr<Node> multicastRouter1 = c.Get (4); // The node in question
    Ptr<NetDevice> inputIf1 = nd1.Get (2);   // The input NetDevice
    NetDeviceContainer outputDevices1;       // A container of output NetDevices
    outputDevices1.Add (nd3.Get (0));        // (we only need one NetDevice here)

    Ptr<Node> multicastRouter2 = c.Get (5); // The node in question
    Ptr<NetDevice> inputIf2 = nd3.Get (1);   // The input NetDevice
    NetDeviceContainer outputDevices2;       // A container of output NetDevices
    outputDevices2.Add (nd2.Get (0));        // (we only need one NetDevice here)

    multicast.AddMulticastRoute (multicastRouter, multicastSource, multicastGroup, inputIf, outputDevices);
    multicast.AddMulticastRoute (multicastRouter1, multicastSource, multicastGroup, inputIf1, outputDevices1);
    multicast.AddMulticastRoute (multicastRouter2, multicastSource, multicastGroup, inputIf2, outputDevices2);

    Ptr<Node> sender = c.Get (1);
    Ptr<NetDevice> senderIf = nd0.Get (1);
    multicast.SetDefaultMulticastRoute (sender, senderIf);

    uint16_t multicastPort = 9;   // Discard port (RFC 863)

    OnOffHelper onoff ("ns3::UdpSocketFactory", Address (InetSocketAddress (multicastGroup, multicastPort)));
    onoff.SetConstantRate (DataRate ("255b/s"));
    onoff.SetAttribute ("PacketSize", UintegerValue (128));

    ApplicationContainer srcC = onoff.Install (c0.Get (1));
    srcC.Start (Seconds (1.));
    srcC.Stop (Seconds (10.));

    PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

    ApplicationContainer sinkC = sink.Install (c2.Get (0)); // Node n5
    sinkC.Start (Seconds (1.0));
    sinkC.Stop (Seconds (10.0));

    AnimationInterface anim ("multicast.xml");
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
