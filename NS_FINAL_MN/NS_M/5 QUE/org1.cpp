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
void PacketReceived(Ptr<const Packet> packet, const Address& from) {
    NS_LOG_INFO("Packet Received at Sink: " << packet->GetSize() << " bytes");
}
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
    csmaNodes2.Add(nodes.Get(9));
 
    csmaNodes2.Add(nodes.Get(10));
    csmaDevice1 = csma[0].Install(csmaNodes1);
 
    csmaDevice2 = csma[1].Install(csmaNodes2);
    InternetStackHelper stack;
 
    stack.Install(nodes);
    Ipv4AddressHelper address1, address2;
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
    Ptr<Node> multicastRouter = nodes.Get(5);
 
    Ptr<NetDevice> inputIf = csmaDevice1.Get(2);
 
    NetDeviceContainer outputDevices;
 
    outputDevices.Add(p2pDevices[5].Get(0));
    uint16_t multicastPort = 90;
 
    OnOffHelper onoffm("ns3::UdpSocketFactory",
 
                       Address(InetSocketAddress(multicastGroup, multicastPort)));
 
    onoffm.SetConstantRate(DataRate("255b/s"));
 
    onoffm.SetAttribute("PacketSize", UintegerValue(128));
 
    ApplicationContainer srcC = onoffm.Install(nodes.Get(7));
 
    srcC.Start(Seconds(1.));
 
    srcC.Stop(Seconds(10.));
    PacketSinkHelper sinkm("ns3::UdpSocketFactory",
 
                           InetSocketAddress(Ipv4Address::GetAny(), multicastPort));
 
    ApplicationContainer sinkC[6];
 
    int mnodes[]{0, 2, 3, 4, 5, 6};
 
        // After installing sinks, retrieve them and print the number of received packets
for (int i = 0; i < 6; i++) {
sinkC[i] = sinkm.Install(nodes.Get(mnodes[i]));
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkC[i].Get(0));
    if (sink) {
        sink->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceived));
    }
}

    for (int i = 0; i < 6; i++)
 
    {
 
        sinkC[i].Start(Seconds(1.0));
 
        sinkC[i].Stop(Seconds(10.0));
 
    }
    OnOffHelper onoff("ns3::UdpSocketFactory",
 
                      Address(InetSocketAddress(interface[1].GetAddress(0), port)));
 
    onoff.SetConstantRate(DataRate("500kb/s"));
 
    ApplicationContainer app = onoff.Install(nodes.Get(10));
 
    app.Start(Seconds(1.09));
 
    app.Stop(Seconds(12.0));
    PacketSinkHelper sink("ns3::UdpSocketFactory",
 
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
 
    app = sink.Install(nodes.Get(3));
 
    app.Start(Seconds(1.0));
 
    app.Stop(Seconds(10.0));
    OnOffHelper onoff1("ns3::TcpSocketFactory",
 
                       Address(InetSocketAddress(interface[2].GetAddress(0), port)));
 
    onoff1.SetConstantRate(DataRate("500kb/s"));
 
    ApplicationContainer app1 = onoff1.Install(nodes.Get(6));
 
    app1.Start(Seconds(8.0));
 
    app1.Stop(Seconds(13.0));
    PacketSinkHelper sink1("ns3::TcpSocketFactory",
 
                           Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
 
    app = sink1.Install(nodes.Get(1));
 
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
    AnimationInterface anim("midterm111.XML");
 
    AsciiTraceHelper ascii;
 
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("midterm.tr");
 
    for (int i = 0; i < 8; i++)
 
    {
 
        pointToPoint[i].EnableAsciiAll(stream);
 
    }
 
    csma[0].EnableAsciiAll(stream);
 
    csma[1].EnableAsciiAll(stream);
    // Generate 25 packets per 50 milliseconds at node N10 and set the MTU to 1500 bytes at MAC level for TCP
 
    Ptr<OnOffApplication> onoffApp = onoff.Install(nodes.Get(10)).Get(0)->GetObject<OnOffApplication>();
 
    onoffApp->SetAttribute("PacketSize", UintegerValue(150));  // Set packet size to 1500 bytes
 
    onoffApp->SetAttribute("DataRate", DataRateValue(DataRate("1Mb/s")));  // Set data rate to 5 Mb/s
 
    onoffApp->SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.05]")); // Set on time to 50 milliseconds
 
    onoffApp->SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]")); // Set off time to 0 milliseconds
    Simulator::Run();
 
    Simulator::Destroy();
    return 0;
 
}