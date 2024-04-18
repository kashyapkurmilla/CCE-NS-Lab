#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/wifi-standards.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/bulk-send-application.h"
#include "ns3/bulk-send-helper.h"
#include "ns3/rectangle.h"


using namespace ns3;

int main (int argc, char *argv[])
{
	double datarates[] = {100000,3000, 200, 100};
  	uint32_t bandwidths[] = {20, 40, 80, 160};//in ms
for(int i=0;i<4;i++)
{
	uint32_t datarate=datarates[i];
	double bandwidth=bandwidths[i];
    // Enable logging
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create nodes
    NodeContainer nodes;
    nodes.Create(10);

    // Install mobility if needed
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0),
                                   "MinY", DoubleValue (0.0),
                                   "DeltaX", DoubleValue (5.0),
                                   "DeltaY", DoubleValue (10.0),
                                   "GridWidth", UintegerValue (3),
                                   "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.Install (nodes);

    // Install wireless devices and channels
    WifiHelper wifi;
    wifi.SetStandard (WIFI_STANDARD_80211ac);
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                  "DataMode", StringValue ("HtMcs7"),
                                  "ControlMode", StringValue ("HtMcs0"));
    
    YansWifiChannelHelper channel;
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
    YansWifiPhyHelper phy;
    phy.SetChannel (channel.Create ());
    phy.Set("ChannelWidth", UintegerValue(bandwidth)); // Set channel width to 20 MHz
    //phy.Set("ShortGuardEnabled", BooleanValue(true));

    // Install wireless devices
    WifiMacHelper mac;
    mac.SetType ("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

    // Install Internet stack
    InternetStackHelper stack;
    stack.Install (nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase ("10.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    // Install UDP echo server on node 0
    UdpEchoServerHelper echoServer (9);
    ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
    serverApps.Start (Seconds (2.0));
    serverApps.Stop (Seconds (5.0));
    
    uint32_t packetSize=1024;
    double interval = static_cast<double>(packetSize * 8) / static_cast<double>(datarate);
    
    UdpEchoClientHelper echoClient (interfaces.GetAddress (0), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (interval)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));
    
    ApplicationContainer clientApps = echoClient.Install (nodes.Get (1));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (4.0));
    
    // Install TCP bulk send application on node 2
	uint16_t port = 9;
	Ipv4Address sinkAddress = interfaces.GetAddress(1); // Get the IP address of the second interface (node 3)
	BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port)); // Set destination address to node 3
	source.SetAttribute ("MaxBytes", UintegerValue (1000000));
	
	ApplicationContainer sourceApps = source.Install (nodes.Get (0));
	sourceApps.Start (Seconds (1.0));
	sourceApps.Stop (Seconds (10.0));

	// Install TCP sink application on node 3
	PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port)); // Listen on any address
	ApplicationContainer sinkApps = sink.Install (nodes.Get (1));
	sinkApps.Start (Seconds (2.0));
	sinkApps.Stop (Seconds (4.1));

    char abc[20];
    sprintf(abc, "Lab7_1trace%d.tr", i);
    AsciiTraceHelper ascii;
    phy.EnableAsciiAll (ascii.CreateFileStream (abc));

    // Run simulation
    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();
    Simulator::Destroy ();
    
    Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(sinkApps.Get(0));
    std::cout << "Bandwidth: " << bandwidth << " Mbps, Data Rate: " << datarate << " ms, Total Bytes Received: " << sink1->GetTotalRx() << "\n"<<std::endl;
    }
    return 0;
}

