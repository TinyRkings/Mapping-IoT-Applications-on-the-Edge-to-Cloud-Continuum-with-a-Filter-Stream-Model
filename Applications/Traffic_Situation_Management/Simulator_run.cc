#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

#include "Client_helper.h"
#include "Server_helper.h"


using namespace std;
//using namespace cv;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SimulatorCNN");


int main (int argc, char *argv[])
{
  uint32_t users = 3;
  uint32_t nCsma = 1;
  uint32_t nWifi = 3;
  bool tracing = false;
  double stopTime = 100.0;
  std::string p2plinkRate = "5Mbps";
  std::string p2pdelay = "1ms";
  
  std::string csmalinkRate = "100Mbps";

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("stopTime", "The time when the clients will stop requesting segments", stopTime);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
  {
  	std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
  	return 1;
  }
  
  
  NS_LOG_INFO ("Create nodes.");
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NS_LOG_INFO ("Create channels.");
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate",StringValue(p2plinkRate));
  pointToPoint.SetChannelAttribute ("Delay",StringValue(p2pdelay));
  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate",StringValue(csmalinkRate));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

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
  mobility.Install (wifiStaNodes);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);

  
  // Install the internet stack on the nodes
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices);
  address.Assign (apDevices);


  NS_LOG_INFO ("Create Applications.");
  
  uint16_t port = 8080; // port number
  std::vector<DashClientHelper> clients;
  std::vector<ApplicationContainer> clientApps;
  for (uint32_t user = 0; user < users; user++)
  {
  	DashClientHelper client ("ns3::TcpSocketFactory", InetSocketAddress (p2pInterfaces.GetAddress (1), port));
  	//client.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    //client.SetAttribute ("VideoId", UintegerValue (user + 1)); // VideoId should be positive
    //client.SetAttribute ("TargetDt", TimeValue (Seconds (target_dt)));
    //client.SetAttribute ("window", TimeValue (Time (window)));
    //client.SetAttribute ("bufferSpace", UintegerValue (bufferSpace));

    ApplicationContainer clientApp = client.Install (wifiStaNodes.Get (user));
    clientApp.Start (MilliSeconds(150));
    clientApp.Stop (Seconds (stopTime));

    clients.push_back (client);
    clientApps.push_back (clientApp);
  }
  DashServerHelper server ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer serverApps = server.Install (p2pNodes.Get (1));
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (stopTime + 5.0));

  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  NS_LOG_INFO ("Run Simulation.");

  Simulator::Stop (Seconds (stopTime));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcap ("third", apDevices.Get (0));
      csma.EnablePcap ("third", csmaDevices.Get (0), true);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

