//topology practice

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(11);


    PointToPointHelper pointToPoint[8];
    for (int i = 0; i < 8; i++)
    {
        pointToPoint[i].SetDeviceAttribute("DataRate", StringValue("5Mbps"));
        pointToPoint[i].SetChannelAttribute("Delay", StringValue("2ms"));
    }
    
    int coords[8][2]{
        {0,2},{3,2},{1,2},{5,6},{6,8},{6,7},{9,7},{9,8}
    };

    NetDeviceContainer p2pdevices[8],csmadevices1 ,csmadevices2;
    for (int i = 0; i < 8; i++)
    {
        p2pDevices[i] = pointToPoint[i].Install(nodes.Get(coords[i][0]), nodes.Get(coords[i][1]));
    }

    csmaHelper csma[2];
     csma[0].SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma[0].SetChannelAttribute("Delay", StringValue("10ms"));
    csma[1].SetChannelAttribute("DataRate", StringValue("70Mbps"));
    csma[1].SetChannelAttribute("Delay", StringValue("13ms"));

    
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}