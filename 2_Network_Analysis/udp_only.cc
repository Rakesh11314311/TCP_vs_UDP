/* Code created for Lab 3.3 of CS313 - Aug-Dec 2014
Copywrite: Sreelakshmi Manjunath, IIT Mandi
Last edit: Sept 26th 2024.
 */

//
// Two clients, one TCP & one UDP. One server for each client. Two routers that connect the client-server pairs.
//
// TCP congestion window tracing "tcp-udp-sharing-cwnd.tr"
// - pcap traces also generated in the following files
//   "tcp-udp-sharing-$n-$i.pcap" where n and i represent node and interface numbers respectively
//  Usage (e.g.): ./ns3 run tcp-large-transfer

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpUdpSharing");

// TCP client parameters
// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 240000000;
static uint32_t currentTxBytes = 0;
// Perform series of 1040 byte writes (this is a multiple of 26 since we want to detect data splicing in the output stream)
static const uint32_t writeSize = 1040;
uint8_t data[writeSize];
// initialising data stream for cwnd data
Ptr<OutputStreamWrapper> cwnd_stream;

// These are for starting the writing process, and handling the sending 
// socket's notification upcalls (events).  These two together more or less

void StartFlow (Ptr<Socket>, Ipv4Address, uint16_t);
void WriteUntilBufferFull (Ptr<Socket>, uint32_t);
static std::ofstream cwndFile("cwnd_log.txt");
// function for tracing TCP congestion window
static void 
CwndTracer (uint32_t oldval, uint32_t newval)
{
    cwndFile << Simulator::Now().GetSeconds() << " " << newval << std::endl;
  *cwnd_stream ->GetStream () << Simulator::Now ().GetSeconds () << "\t" << newval/1040 << std::endl;
        //NS_LOG_INFO ("Moving cwnd from " << oldval << " to " << newval);
}

int main (int argc, char *argv[])
{

  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  // initialize the tx buffer.
  for(uint32_t i = 0; i < writeSize; ++i)
    {
      char m = toascii (97 + i % 26);
      data[i] = m;
    }

// Create client nodes in a container named Sources

NodeContainer Sources;
Sources.Create (2); //0 for tcp, 1 for udp

// Create router nodes in a container named r0r1

NodeContainer r0r1;
r0r1.Create (2);

// Create server nodes in a container named Sinks

NodeContainer Sinks;
Sinks.Create (2);

// Put TCP client and left router in a container named Source1r0

NodeContainer Source1r0;
Source1r0.Add (Sources.Get (0));
Source1r0.Add (r0r1.Get (0));

// Put UDP client and left router in a container named Source2r0

NodeContainer Source2r0;
Source2r0.Add (Sources.Get (1));
Source2r0.Add (r0r1.Get (0));

// Put TCP server and right router in a container named r1Sink1

NodeContainer r1Sink1;
r1Sink1.Add (r0r1.Get (1));
r1Sink1.Add (Sinks.Get (0));
// Put UDP server and right router in a container named r1Sink2

NodeContainer r1Sink2;
r1Sink2.Add (r0r1.Get (1));
r1Sink2.Add (Sinks.Get (1));
// Creating all the required p2p channels

PointToPointHelper p2p2mbps;
p2p2mbps.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (2000000)));
p2p2mbps.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

PointToPointHelper p2p3mbps;
p2p3mbps.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (3000000)));
p2p3mbps.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));


  // And then install devices and channels connecting the nodes are required for the given topology. One device container for each p2p link
  
NetDeviceContainer dev0 = p2p2mbps.Install (Source1r0);
NetDeviceContainer dev1 = p2p2mbps.Install (Source2r0);
NetDeviceContainer dev2 = p2p2mbps.Install (r1Sink1);
NetDeviceContainer dev3 = p2p2mbps.Install (r1Sink2);
NetDeviceContainer dev4 = p2p3mbps.Install (r0r1);

  // Now add ip/tcp stack to all nodes.
  InternetStackHelper internet;
  internet.InstallAll ();

  // Adding IP addresses to all the devices created above
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs0 = ipv4.Assign (dev0);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs1 = ipv4.Assign (dev1);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs2 = ipv4.Assign (dev2);
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs3 = ipv4.Assign (dev3);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs4 = ipv4.Assign (dev4);


  // and setup ip routing tables to get total ip-level connectivity.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

 // Define port number for TCP server
  uint16_t servPort = 50000;

  // Create a TCP packet sink at the server
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), servPort));

  ApplicationContainer apps_TCP = sink.Install (r1Sink1.Get (1));
  // assign start and stop times for the TCP app
  apps_TCP.Start (Seconds (0.0));
  apps_TCP.Stop (Seconds (100.0));

 // Create and bind the TCP socket...
  Ptr<Socket> localSocket =
    Socket::CreateSocket (Source1r0.Get (0), TcpSocketFactory::GetTypeId ());
  localSocket->Bind ();
 
  // Trace changes to the congestion window
 AsciiTraceHelper ascii_cwnd;
  cwnd_stream = ascii_cwnd.CreateFileStream("tcp-sharing-cwnd.tr");      
 // Make a call back to the congestion window tracing function at the TCP client socket
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));

  // Start the flow at the TCP socket
  Simulator::ScheduleNow (&StartFlow, localSocket,
                          ipInterfs2.GetAddress (1), servPort);

  // Define a port number for UDP server
  uint16_t port = 9;  

  // Create a UDP server (not echo server)
  UdpServerHelper server (port);
  ApplicationContainer apps_UDP = server.Install (r1Sink2.Get (1));
  // assign start and stop times for the UDP server app
  apps_UDP.Start (Seconds (1.0));
  apps_UDP.Stop (Seconds (100.0));

// Create a UdpClient application to send UDP datagrams from UDP client to UDP server
//
  // initialise packet size, packet count and sending interval
  uint32_t packetSize = 1040;
  uint32_t maxPacketCount = 10000000;
  Time interPacketInterval = Seconds (0.001);
  // Set up UDP client
  Address serverAddress = Address(ipInterfs3.GetAddress(1));
  
  UdpClientHelper client (serverAddress, port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps_UDP = client.Install (Source2r0.Get (0));
  // assign start and stop times for the UDP client app
  apps_UDP.Start (Seconds (1.0));
  apps_UDP.Stop (Seconds (100.0));

  // Enable pcap tracing

  p2p3mbps.EnablePcapAll ("tcp-udp-sharing");

  // Set up the simulator to run.  
  Simulator::Stop (Seconds (150));
  Simulator::Run ();
   cwndFile.close();
  Simulator::Destroy ();
}


//begin implementation of sending "Application"

void StartFlow (Ptr<Socket> localSocket,
                Ipv4Address servAddress,
                uint16_t servPort)
{
  NS_LOG_LOGIC ("Starting flow at time " <<  Simulator::Now ().GetSeconds ());
  localSocket->Connect (InetSocketAddress (servAddress, servPort)); //connect

  // tell the tcp implementation to call WriteUntilBufferFull again
  // if we blocked and new tx buffer space becomes available
  localSocket->SetSendCallback (MakeCallback (&WriteUntilBufferFull));
  WriteUntilBufferFull (localSocket, localSocket->GetTxAvailable ());
}


// function for writing data into TCP socket buffer 

void WriteUntilBufferFull (Ptr<Socket> localSocket, uint32_t txSpace)
{
  while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable () > 0) 
    {
      uint32_t left = totalTxBytes - currentTxBytes;
      uint32_t dataOffset = currentTxBytes % writeSize;
      uint32_t toWrite = writeSize - dataOffset;
      toWrite = std::min (toWrite, left);
      toWrite = std::min (toWrite, localSocket->GetTxAvailable ());
      int amountSent = localSocket->Send (&data[dataOffset], toWrite, 0);
      if(amountSent < 0)
        {
          // we will be called again when new tx space becomes available.
          return;
        }
      currentTxBytes += amountSent;
    }
  if (currentTxBytes >= totalTxBytes)
    {
      localSocket->Close ();
    }
}



