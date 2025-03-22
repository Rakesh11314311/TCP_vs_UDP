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

NS_LOG_COMPONENT_DEFINE("TcpUdpSharing");

// TCP client parameters
// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 2000000;
static uint32_t currentTxBytes = 0;
// Perform series of 1040 byte writes (this is a multiple of 26 since we want to detect data splicing in the output stream)
static const uint32_t writeSize = 1040;
uint8_t data[writeSize];
// initialising data stream for cwnd data
Ptr<OutputStreamWrapper> cwnd_stream;

// These are for starting the writing process, and handling the sending
// socket's notification upcalls (events).  These two together more or less

void StartFlow(Ptr<Socket>, Ipv4Address, uint16_t);
void WriteUntilBufferFull(Ptr<Socket>, uint32_t);

// function for tracing TCP congestion window
static void 
CwndTracer (????????????)
{
  *cwnd_stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << newval / 1040 << std::endl;
  // NS_LOG_INFO ("Moving cwnd from " << oldval << " to " << newval);
}

int main(int argc, char *argv[])
{

  CommandLine cmd(__FILE__);
  cmd.Parse(argc, argv);

  // initialize the tx buffer.
  for (uint32_t i = 0; i < writeSize; ++i)
  {
    char m = toascii(97 + i % 26);
    data[i] = m;
  }

  // Create client nodes in a container named Sources
  NodeContainer Sources;
  Sources.Create(2);

  // Create router nodes in a container named r0r1
  NodeContainer r0r1;
  r0r1.Create(2);

  // Create server nodes in a container named Sinks
  NodeContainer Sinks;
  Sinks.Create(2);

  // Put TCP client and left router in a container named Source1r0

  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                            // Put UDP client and left router in a container named Source2r0

      ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                // Put TCP server and right router in a container named r1Sink1

      ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                // Put UDP server and right router in a container named r1Sink2

      ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                              // Creating all the required p2p channels

      ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                              // And then install devices and channels connecting the nodes are required for the given topology. One device container for each p2p link

      ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                      // Now add ip/tcp stack to all nodes.
      InternetStackHelper internet;
  internet.InstallAll();

  // Adding IP addresses to all the devices created above
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs0 = ipv4.Assign(dev0);
  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                                              // and setup ip routing tables to get total ip-level connectivity.
      Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Define port number for TCP server
  uint16_t servPort = 50000;

  // Create a TCP packet sink at the server
  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                          ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?
                                                                                                                                                    // assign start and stop times for the TCP app
      apps_TCP.Start(Seconds(0.0));
  apps_TCP.Stop(Seconds(100.0));

  // Create and bind the TCP socket...
  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                        // Trace changes to the congestion window
      AsciiTraceHelper ascii_cwnd;
  cwnd_stream = ascii_cwnd.CreateFileStream("tcp-sharing-cwnd.tr");
  // Make a call back to the congestion window tracing function at the TCP client socket
  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                        // Start the flow at the TCP socket
      ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                          // Define a port number for UDP server
      uint16_t port = 9;

  // Create a UDP server (not echo server)
  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?
                                                                                                                                  // assign start and stop times for the UDP server app
      apps_UDP.Start(Seconds(1.0));
  apps_UDP.Stop(Seconds(100.0));

  // Create a UdpClient application to send UDP datagrams from UDP client to UDP server
  //
  // initialise packet size, packet count and sending interval
  uint32_t packetSize = 1040;
  uint32_t maxPacketCount = ? ? ? ? ? ? ? ;
  Time interPacketInterval = Seconds (???????);
  // Set up UDP client
  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                          ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                          ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                                                                                  ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?
                                                                                                                                                                                                                                                                                          // assign start and stop times for the UDP client app
      apps_UDP.Start(Seconds(1.0));
  apps_UDP.Stop(Seconds(100.0));

  // Enable pcap tracing

  p2p_r0r1.EnablePcapAll("tcp-udp-sharing");

  // Set up the simulator to run.
  Simulator::Stop(Seconds(150));
  Simulator::Run();
  Simulator::Destroy();
}

// begin implementation of sending "Application"

? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                        ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                                                                        ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                                                                                                                                                ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                                                                                                                                                                                                                        ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                                                                                                                                                                                                                                                                                                ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?   ?
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                // function for writing data into TCP socket buffer

    void
    WriteUntilBufferFull(Ptr<Socket> localSocket, uint32_t txSpace)
{
  while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable() > 0)
  {
    uint32_t left = totalTxBytes - currentTxBytes;
    uint32_t dataOffset = currentTxBytes % writeSize;
    uint32_t toWrite = writeSize - dataOffset;
    toWrite = std::min(toWrite, left);
    toWrite = std::min(toWrite, localSocket->GetTxAvailable());
    int amountSent = localSocket->Send(&data[dataOffset], toWrite, 0);
    if (amountSent < 0)
    {
      // we will be called again when new tx space becomes available.
      return;
    }
    currentTxBytes += amountSent;
  }
  if (currentTxBytes >= totalTxBytes)
  {
    localSocket->Close();
  }
}
