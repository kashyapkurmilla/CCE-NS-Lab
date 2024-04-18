#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("lab1q1");

int main(int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(5); // Create 5 nodes

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("3p"));

    // Link n0 to n1, n2, n4
    NetDeviceContainer devN0N1 = p2p.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer devN0N2 = p2p.Install(nodes.Get(0), nodes.Get(2));
    NetDeviceContainer devN0N4 = p2p.Install(nodes.Get(0), nodes.Get(4));

    // Link n1 to n0, n4, n3
    NetDeviceContainer devN1N0 = p2p.Install(nodes.Get(1), nodes.Get(0));
    NetDeviceContainer devN1N3 = p2p.Install(nodes.Get(1), nodes.Get(3));
    NetDeviceContainer devN1N4 = p2p.Install(nodes.Get(1), nodes.Get(4));

    // Link n2 to n1, n4, n3
    NetDeviceContainer devN2N1 = p2p.Install(nodes.Get(2), nodes.Get(1));
    NetDeviceContainer devN2N3 = p2p.Install(nodes.Get(2), nodes.Get(3));
    NetDeviceContainer devN2N4 = p2p.Install(nodes.Get(2), nodes.Get(4));

    // Link n3 to n1, n2, n4
    NetDeviceContainer devN3N1 = p2p.Install(nodes.Get(3), nodes.Get(1));
    NetDeviceContainer devN3N2 = p2p.Install(nodes.Get(3), nodes.Get(2));
    NetDeviceContainer devN3N4 = p2p.Install(nodes.Get(3), nodes.Get(4));

    // Link n4 to n0, n1, n2, n3
    NetDeviceContainer devN4N0 = p2p.Install(nodes.Get(4), nodes.Get(0));
    NetDeviceContainer devN4N1 = p2p.Install(nodes.Get(4), nodes.Get(1));
    NetDeviceContainer devN4N2 = p2p.Install(nodes.Get(4), nodes.Get(2));
    NetDeviceContainer devN4N3 = p2p.Install(nodes.Get(4), nodes.Get(3));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    address.Assign(devN0N1);
    address.Assign(devN0N2);
    address.Assign(devN0N4);
    address.Assign(devN1N0);
    address.Assign(devN1N3);
    address.Assign(devN1N4);
    address.Assign(devN2N1);
    address.Assign(devN2N3);
    address.Assign(devN2N4);
    address.Assign(devN3N1);
    address.Assign(devN3N2);
    address.Assign(devN3N4);
    address.Assign(devN4N0);
    address.Assign(devN4N1);
    address.Assign(devN4N2);
    address.Assign(devN4N3);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(3)); // Install server on node 3 (n3)
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Get the IPv4 address of node 1 (n1)
    Ipv4InterfaceAddress addr = nodes.Get(1)->GetObject<Ipv4>()->GetAddress(1, 0);

    UdpEchoClientHelper echoClient(addr.GetLocal(), 9); // Using node 1's (n1) IP address
    echoClient.SetAttribute("MaxPackets", UintegerValue(3));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(3)); // Install client on node 3 (n3)
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    AnimationInterface anim("mad_exec.xml");

    // Set node positions in the animation
    anim.SetConstantPosition(nodes.Get(0), 0, 0);
    anim.SetConstantPosition(nodes.Get(1), 10, 0);
    anim.SetConstantPosition(nodes.Get(2), 0, 10);
    anim.SetConstantPosition(nodes.Get(3), 10, 10);
    anim.SetConstantPosition(nodes.Get(4), 5, 5);
    
  
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}

