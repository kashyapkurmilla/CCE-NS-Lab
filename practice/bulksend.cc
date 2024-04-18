#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("BulkSendExample");

int main(int argc, char *argv[]) {
  // Enable logging
  LogComponentEnable("BulkSendExample", LOG_LEVEL_INFO);

  // Create nodes
  NodeContainer nodes;
  nodes.Create(2);

  // Create a point-to-point link
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install(nodes);

  // Install internet stack
  InternetStackHelper internet;
  internet.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  // Set up TCP receiver on node 1
  uint16_t tcpPort = 9;
  Address tcpLocalAddress(InetSocketAddress(interfaces.GetAddress(1), tcpPort));
  PacketSinkHelper tcpSinkHelper("ns3::TcpSocketFactory", tcpLocalAddress);
  ApplicationContainer tcpSinkApp = tcpSinkHelper.Install(nodes.Get(1));
  tcpSinkApp.Start(Seconds(0.0));
  tcpSinkApp.Stop(Seconds(10.0));

  // Set up TCP bulk sender on node 0
  uint32_t maxBytes = 1000000;
  BulkSendHelper tcpSender("ns3::TcpSocketFactory", InetSocketAddress(interfaces.GetAddress(1), tcpPort));
  tcpSender.SetAttribute("MaxBytes", UintegerValue(maxBytes));
  ApplicationContainer tcpApp = tcpSender.Install(nodes.Get(0));
  tcpApp.Start(Seconds(1.0));
  tcpApp.Stop(Seconds(9.0));

  AnimationInterface anim("bulksend.xml");

  // Run simulation
  Simulator::Stop(Seconds(10.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
