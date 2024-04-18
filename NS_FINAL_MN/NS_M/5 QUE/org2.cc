#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/csma-helper.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("FirstScriptExample");
int main(int argc, char *argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    NodeContainer nodes;
    nodes.Create(11);
    PointToPointHelper pointToPoint[8];
    pointToPoint[0].SetDeviceAttribute("DataRate", StringValue("6Mbps"));
    pointToPoint[0].SetChannelAttribute("Delay", StringValue("10ms"));
    for (int i = 1; i < 8; i++)
    {
        pointToPoint[i].SetDeviceAttribute("DataRate", StringValue("3Mbps"));
        pointToPoint[i].SetChannelAttribute("Delay", StringValue("5ms"));
        pointToPoint[i].SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("20p"));
    }
    int coords[8][2]{
        {0, 2},
        {3, 2},
        {1, 2},
        {5, 6},
        {6, 8},
        {6, 7},
        {9, 8},
        {9, 7},
    };
    NetDeviceContainer p2pDevices[8], csmaDevice1, csmaDevice2;
    p2pDevices[2] = pointToPoint[0].Install(nodes.Get(1), nodes.Get(2));
    // p2pDevices[7] = pointToPoint[7].Install(nodes.Get(9), nodes.Get(7));
    for (int i = 0; i < 7; i++)
    {
        if (i == 2)
            continue;
        p2pDevices[i] = pointToPoint[i].Install(nodes.Get(coords[i][0]), nodes.Get(coords[i][1]));
    }
    CsmaHelper csma[2];
    csma[0].SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma[0].SetChannelAttribute("Delay", StringValue("10ms"));
    csma[1].SetChannelAttribute("DataRate", StringValue("70Mbps"));
    csma[1].SetChannelAttribute("Delay", StringValue("13ms"));
    NodeContainer csmaNodes1, csmaNodes2;
    csmaNodes1.Add(nodes.Get(2));
    csmaNodes1.Add(nodes.Get(4));
    csmaNodes1.Add(nodes.Get(5));
    // for (int i = 7; i <= 9; i++)
    csmaNodes2.Add(nodes.Get(9));
    csmaNodes2.Add(nodes.Get(10));
    // csmaNodes2.Add(nodes.Get(i));
    // NetDeviceContainer device02,device32
    csmaDevice1 = csma[0].Install(csmaNodes1);
    csmaDevice2 = csma[1].Install(csmaNodes2);
    // device02.Add(csma.Install(nodes.Get(1), nodes.Get(2)));
    InternetStackHelper stack;
    stack.Install(nodes);
    Ipv4AddressHelper address1, address2;
    // address1.SetBase("10.1.1.0", "255.255.255.0");
    // Ipv4InterfaceContainer interface1 = address1.Assign(device01);
    // address1.SetBase("10.1.2.0", "255.255.255.0");
    // Ipv4InterfaceContainer interface2 = address1.Assign(device02);
    // address1.SetBase("10.1.3.0", "255.255.255.0");
    // Ipv4InterfaceContainer interface3 = address1.Assign(device46);
    // address1.SetBase("10.1.4.0", "255.255.255.0");
    // Ipv4InterfaceContainer interface4 = address1.Assign(devices65);
    // address1.SetBase("10.1.5.0", "255.255.255.0");
    // Ipv4InterfaceContainer interface5 = address1.Assign(devices57);
    Ipv4InterfaceContainer interface[10];
    for (int i = 0; i < 8; i++)
    {
        std::ostringstream subnet;
        subnet << "10.1." << i << ".0";
        address1.SetBase(subnet.str().c_str(), "255.255.255.0");
        interface[i] = address1.Assign(p2pDevices[i]);
    }
    address1.SetBase("10.1.8.0", "255.255.255.0");
    interface[8] = address1.Assign(csmaDevice1);
    address1.SetBase("10.1.9.0", "255.255.255.0");
    interface[9] = address1.Assign(csmaDevice2);
    uint16_t port = 9;
    Ipv4Address multicastSource("10.1.5.2");
    Ipv4Address multicastGroup("225.1.2.4");
    Ipv4StaticRoutingHelper multicast;
    // 1) Configure a (static) multicast route on node n2 (multicastRouter)
    Ptr<Node> multicastRouter = nodes.Get(5);    // The node in question
    Ptr<NetDevice> inputIf = csmaDevice1.Get(2); // The input NetDevice
    NetDeviceContainer outputDevices;            // A container of output NetDevices
    outputDevices.Add(p2pDevices[5].Get(0));
    // outputDevices.Add(p2pDevices[0].Get(1)); // (we only need one NetDevice here)
    // outputDevices.Add(p2pDevices[1].Get(0)); // (we only need one NetDevice here)
    // outputDevices.Add(csmaDevice1.Get(1));   // (we only need one NetDevice here)
    // outputDevices.Add(csmaDevice1.Get(2));   // (we only need one NetDevice here)
    // (we only need one NetDevice here)
    // multicast.AddMulticastRoute(multicastRouter, multicastSource,
    //                             multicastGroup, inputIf, outputDevices);
    multicastRouter = nodes.Get(2);    // The node in question
    inputIf = p2pDevices[0].Get(1);    // The input NetDevice
    NetDeviceContainer outputDevices1; // A container of output NetDevices
    outputDevices1.Add(csmaDevice1.Get(1));
    // outputDevices.Add(p2pDevices[0].Get(1)); // (we only need one NetDevice here)
    // outputDevices.Add(p2pDevices[1].Get(0)); // (we only need one NetDevice here)
    // outputDevices.Add(csmaDevice1.Get(1));   // (we only need one NetDevice here)
    // outputDevices.Add(csmaDevice1.Get(2));   // (we only need one NetDevice here)
    // (we only need one NetDevice here)
    // multicast.AddMulticastRoute(multicastRouter, multicastSource,
    //                             multicastGroup, inputIf, outputDevices1);
    uint16_t multicastPort = 90; // Discard port (RFC 863)
    // Configure a multicast packet generator that generates a packet
    // every few seconds
    OnOffHelper onoffm("ns3::UdpSocketFactory",
                       Address(InetSocketAddress(multicastGroup, multicastPort)));
    onoffm.SetConstantRate(DataRate("255b/s"));
    onoffm.SetAttribute("PacketSize", UintegerValue(128));
    ApplicationContainer srcC = onoffm.Install(nodes.Get(7));
    //
    // Tell the application when to start and stop.
    //
    srcC.Start(Seconds(1.));
    srcC.Stop(Seconds(10.));
    // Create an optional packet sink to receive these packets
    PacketSinkHelper sinkm("ns3::UdpSocketFactory",
                           InetSocketAddress(Ipv4Address::GetAny(), multicastPort));
    ApplicationContainer sinkC[6];
    int mnodes[]{0, 2, 3, 4, 5, 6};
    for (int i = 0; i < 6; i++)
        sinkC[i] = sinkm.Install(nodes.Get(mnodes[i])); // Node n4
    // sink.Install(node.Get())
    // Start the sink
    for (int i = 0; i < 6; i++)
    {
        sinkC[i].Start(Seconds(1.0));
        sinkC[i].Stop(Seconds(10.0));
    }
    // Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("eval.tr");
    // pointToPoint1.EnableAsciiAll(stream);
    // csma.EnableAsciiAll(stream);
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(interface[1].GetAddress(0), port)));
    onoff.SetConstantRate(DataRate("500kb/s"));
    ApplicationContainer app = onoff.Install(nodes.Get(10));
    // Start the application
    app.Start(Seconds(1.09));
    app.Stop(Seconds(12.0));
    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    app = sink.Install(nodes.Get(3));
    // app.Add(sink.Install(c1.Get(1)));
    // app.Add(sink.Install(c1.Get(2)));
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));
    OnOffHelper onoff1("ns3::TcpSocketFactory",
                       Address(InetSocketAddress(interface[2].GetAddress(0), port)));
    onoff1.SetConstantRate(DataRate("500kb/s"));
    ApplicationContainer app1 = onoff1.Install(nodes.Get(6));
    // Start the application
    app1.Start(Seconds(8.0));
    app1.Stop(Seconds(13.0));
    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink1("ns3::TcpSocketFactory",
                           Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    app = sink1.Install(nodes.Get(1));
    // app.Add(sink.Install(c1.Get(1)));
    // app.Add(sink.Install(c1.Get(2)));
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Ptr<ConstantPositionMobilityModel> s[11];
    for (int i = 0; i < 11; i++)
        s[i] = nodes.Get(i)->GetObject<ConstantPositionMobilityModel>();
    Vector ncoords[11] = {
        Vector(30, 60, 0),
        Vector(0, 30, 0),
        Vector(30, 30, 0),
        Vector(0, 60, 0),
        Vector(60, 30, 0),
        Vector(90, 30, 0),
        Vector(120, 30, 0),
        Vector(150, 0, 0),
        Vector(150, 60, 0),
        Vector(210, 30, 0),
        Vector(240, 30, 0),
    };
    for (int i = 0; i <= 10; i++)
        s[i]->SetPosition(ncoords[i]);
    AnimationInterface anim("midterm.XML");
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("midterm.tr");
    for (int i = 0; i < 8; i++)
    {
        pointToPoint[i].EnableAsciiAll(stream);
    }
    csma[0].EnableAsciiAll(stream);
    csma[1].EnableAsciiAll(stream);
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}