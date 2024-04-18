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

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/netanim-module.h"
// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ThirdScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nWifi = 3;
    bool tracing = false;

    CommandLine cmd(_FILE_);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.
    if (nWifi > 18)
    {
        std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box"
                  << std::endl;
        return 1;
    }

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    NodeContainer csmaNodes1;
    csmaNodes1.Create(3);
    
    NodeContainer p2pNodes1;
    p2pNodes1.Add(csmaNodes1.Get(2));
    p2pNodes1.Create(1);

    NodeContainer wifiStaNodes;
    wifiStaNodes.Add(p2pNodes1.Get(1));
    wifiStaNodes.Create(6);
    
    NodeContainer p2pNodes2;
    p2pNodes2.Add(wifiStaNodes.Get(6));
    p2pNodes2.Create(1);
    
    NodeContainer wifiApNode1 = p2pNodes1.Get(1);
    NodeContainer wifiApNode2 = p2pNodes2.Get(0);
    
    NodeContainer csmaNodes2;
    csmaNodes2.Add(p2pNodes2.Get(1));
    csmaNodes2.Create(2);
    
    NetDeviceContainer p2pDevices1,p2pDevices2;
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));
    p2pDevices1 = pointToPoint.Install(p2pNodes1);
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("6Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));
    p2pDevices1 = pointToPoint.Install(p2pNodes1);
    p2pDevices2 = pointToPoint.Install(p2pNodes2);


NetDeviceContainer csmaDevices1,csmaDevices2;
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("200Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(12)));
csmaDevices1 = csma.Install(csmaNodes1);
csma.SetChannelAttribute("DataRate", StringValue("200Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(8)));
    
    //csmaDevices1 = csma.Install(csmaNodes1);
    csmaDevices2 = csma.Install(csmaNodes2);

    
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");

    WifiHelper wifi;

    NetDeviceContainer staDevices;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    NetDeviceContainer apDevices1,apDevices2;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevices1 = wifi.Install(phy, mac, wifiApNode1);
    apDevices2 = wifi.Install(phy, mac, wifiApNode2);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));
                                  
                                  
    mobility.Install(wifiStaNodes);
    mobility.Install(wifiApNode1);
    mobility.Install(wifiApNode2);
    InternetStackHelper stack;
    stack.Install(csmaNodes1);
    stack.Install(csmaNodes2);
    stack.Install(wifiApNode1);
    stack.Install(wifiApNode2);
    stack.Install(wifiStaNodes);
    stack.Install(p2pNodes1);
    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces1;
    csmaInterfaces1 = address.Assign(csmaDevices1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces1;
    p2pInterfaces1 = address.Assign(p2pDevices1);

    address.SetBase("10.1.3.0", "255.255.255.0");
    address.Assign(staDevices);
    address.Assign(apDevices1);
    address.Assign(apDevices2);
    
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces2;
    p2pInterfaces2 = address.Assign(p2pDevices2);
    
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces2;
    csmaInterfaces2 = address.Assign(csmaDevices2);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(csmaNodes1.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(30.0));

    UdpEchoClientHelper echoClient(csmaInterfaces1.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(csmaNodes2.Get(2));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(30.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    

    if (tracing)
    {
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        pointToPoint.EnablePcapAll("third");
        //phy.EnablePcap("third", apDevices1.Get(0));
        //csma.EnablePcap("third", csmaDevices1.Get(0), true);
    }
        AnimationInterface anim("nsendsem3.xml");
        //anim.SetMaxSeconds(30.0);
    anim.SetConstantPosition(wifiStaNodes.Get(0),70.0,70.0);
    anim.SetConstantPosition(wifiStaNodes.Get(1),110.0,20.0);
    anim.SetConstantPosition(wifiStaNodes.Get(2),110.0,70.0);
    anim.SetConstantPosition(wifiStaNodes.Get(3),110.0,120.0);
    anim.SetConstantPosition(wifiStaNodes.Get(4),160.0,70.0);
    anim.SetConstantPosition(p2pNodes1.Get(1),50.0,80.0);
    anim.SetConstantPosition(p2pNodes2.Get(0),190.0,80.0);

    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}