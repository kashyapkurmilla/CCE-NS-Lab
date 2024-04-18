#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("OnOffHelperExample");

int main(int argc, char *argv[]) {
  // Enable logging
  LogComponentEnable("OnOffHelperExample", LOG_LEVEL_INFO);

  // Create nodes
  NodeContainer nodes;
  nodes.Create(4);

  // Create a point-to-point link
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devicesAB = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
  NetDeviceContainer devicesBC = pointToPoint.Install(nodes.Get(1), nodes.Get(2));

  // Install internet stack
  InternetStackHelper internet;
  internet.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesAB = address.Assign(devicesAB);
  Ipv4InterfaceContainer interfacesBC = address.Assign(devicesBC);

  // Set up TCP receiver on node 2
  uint16_t tcpPort = 9;
  Address tcpLocalAddress(InetSocketAddress(interfacesBC.GetAddress(0), tcpPort));
  PacketSinkHelper tcpSinkHelper("ns3::TcpSocketFactory", tcpLocalAddress);
  ApplicationContainer tcpSinkApp = tcpSinkHelper.Install(nodes.Get(2));
  tcpSinkApp.Start(Seconds(0.0));
  tcpSinkApp.Stop(Seconds(10.0));

  // Set up UDP receiver on node 2
  uint16_t udpPort = 5000;
  Address udpLocalAddress(InetSocketAddress(interfacesBC.GetAddress(0), udpPort));
  PacketSinkHelper udpSinkHelper("ns3::UdpSocketFactory", udpLocalAddress);
  ApplicationContainer udpSinkApp = udpSinkHelper.Install(nodes.Get(2));
  udpSinkApp.Start(Seconds(0.0));
  udpSinkApp.Stop(Seconds(10.0));

  // Set up TCP bulk sender on node 0
  uint32_t maxBytes = 1000000;
  AddressValue remoteAddress(InetSocketAddress(interfacesBC.GetAddress(0), tcpPort));
  OnOffHelper tcpClientHelper("ns3::TcpSocketFactory", remoteAddress.Get());
  tcpClientHelper.SetAttribute("MaxBytes", UintegerValue(maxBytes));
  ApplicationContainer tcpClientApp = tcpClientHelper.Install(nodes.Get(0));
  tcpClientApp.Start(Seconds(1.0));
  tcpClientApp.Stop(Seconds(9.0));

  // Set up UDP bulk sender on node 1
  AddressValue udpRemoteAddress(InetSocketAddress(interfacesBC.GetAddress(0), udpPort));
  OnOffHelper udpClientHelper("ns3::UdpSocketFactory", udpRemoteAddress.Get());
  udpClientHelper.SetAttribute("MaxBytes", UintegerValue(maxBytes));
  ApplicationContainer udpClientApp = udpClientHelper.Install(nodes.Get(1));
  udpClientApp.Start(Seconds(2.0));
  udpClientApp.Stop(Seconds(8.0));

   AnimationInterface anim("onoffhelp.xml");
  // Run simulation
  Simulator::Stop(Seconds(10.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
