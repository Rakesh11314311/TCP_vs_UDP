/* Code created for Lab 3.3 of CS313 - Aug-Dec 2014
Copywrite: Sreelakshmi Manjunath, IIT Mandi
Last edit: Sept 26th 2024.
 */

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
static const uint32_t totalTxBytes = 2000000;
static uint32_t currentTxBytes = 0;
static const uint32_t writeSize = 1040;
uint8_t data[writeSize];
Ptr<OutputStreamWrapper> cwnd_stream;

void StartFlow(Ptr<Socket>, Ipv4Address, uint16_t);
void WriteUntilBufferFull(Ptr<Socket>, uint32_t);

// Function for tracing TCP congestion window
static void
CwndTracer(uint32_t oldval, uint32_t newval)
{
    *cwnd_stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << newval / 1040 << std::endl;
}

int main(int argc, char *argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    for (uint32_t i = 0; i < writeSize; ++i)
    {
        char m = toascii(97 + i % 26);
        data[i] = m;
    }

    // Create client nodes in a container named Sources
    NodeContainer Sources;
    Sources.Create(2); // CT (TCP client) and CU (UDP client)

    // Create router nodes in a container named r0r1
    NodeContainer r0r1;
    r0r1.Create(2); // R1 and R2

    // Create server nodes in a container named Sinks
    NodeContainer Sinks;
    Sinks.Create(2); // ST (TCP server) and SU (UDP server)

    // Define connections between routers and clients/servers
    NodeContainer Source1r0 = NodeContainer(Sources.Get(0), r0r1.Get(0)); // TCP Client to R1
    NodeContainer Source2r0 = NodeContainer(Sources.Get(1), r0r1.Get(0)); // UDP Client to R1
    NodeContainer r1Sink1 = NodeContainer(r0r1.Get(1), Sinks.Get(0));     // R2 to TCP Server
    NodeContainer r1Sink2 = NodeContainer(r0r1.Get(1), Sinks.Get(1));     // R2 to UDP Server

    // Creating all the required p2p channels
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("3Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // Install devices on the created links
    NetDeviceContainer dev0 = p2p.Install(Source1r0);
    NetDeviceContainer dev1 = p2p.Install(Source2r0);
    NetDeviceContainer dev2 = p2p.Install(r1Sink1);
    NetDeviceContainer dev3 = p2p.Install(r1Sink2);
    NetDeviceContainer dev_r0r1 = p2p.Install(r0r1);

    // Add IP/TCP stack to all nodes.
    InternetStackHelper internet;
    internet.InstallAll();

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ipInterfs0 = ipv4.Assign(dev0);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer ipInterfs1 = ipv4.Assign(dev1);
    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ipInterfs2 = ipv4.Assign(dev2);
    ipv4.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer ipInterfs3 = ipv4.Assign(dev3);
    ipv4.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer ipInterfs_r0r1 = ipv4.Assign(dev_r0r1);

    // Set up IP routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // TCP server
    uint16_t servPort = 50000;
    Address tcpServerAddress(InetSocketAddress(ipInterfs2.GetAddress(1), servPort));
    PacketSinkHelper tcpSinkHelper("ns3::TcpSocketFactory", tcpServerAddress);
    ApplicationContainer apps_TCP = tcpSinkHelper.Install(Sinks.Get(0));
    apps_TCP.Start(Seconds(0.0));
    apps_TCP.Stop(Seconds(100.0));

    // TCP client setup
    Ptr<Socket> tcpSocket = Socket::CreateSocket(Sources.Get(0), TcpSocketFactory::GetTypeId());
    tcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndTracer));

    Simulator::ScheduleNow(&StartFlow, tcpSocket, ipInterfs2.GetAddress(1), servPort);

    // UDP server
    uint16_t port = 9;
    UdpServerHelper udpServer(port);
    ApplicationContainer apps_UDP = udpServer.Install(Sinks.Get(1));
    apps_UDP.Start(Seconds(1.0));
    apps_UDP.Stop(Seconds(100.0));

    // UDP client setup
    UdpClientHelper udpClient(ipInterfs3.GetAddress(1), port);
    udpClient.SetAttribute("MaxPackets", UintegerValue(320));
    udpClient.SetAttribute("Interval", TimeValue(Seconds(0.05)));
    udpClient.SetAttribute("PacketSize", UintegerValue(1040));
    ApplicationContainer clientApps = udpClient.Install(Sources.Get(1));
    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(100.0));

    // Enable pcap tracing
    p2p.EnablePcapAll("tcp-udp-sharing");

    // Run the simulation
    Simulator::Stop(Seconds(150));
    Simulator::Run();
    Simulator::Destroy();
}

void StartFlow(Ptr<Socket> localSocket, Ipv4Address servAddress, uint16_t servPort)
{
    localSocket->Connect(InetSocketAddress(servAddress, servPort));
    localSocket->SetSendCallback(MakeCallback(&WriteUntilBufferFull));
    WriteUntilBufferFull(localSocket, localSocket->GetTxAvailable());
}

void WriteUntilBufferFull(Ptr<Socket> localSocket, uint32_t txSpace)
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
            return;
        }
        currentTxBytes += amountSent;
    }
    if (currentTxBytes >= totalTxBytes)
    {
        localSocket->Close();
    }
}
