#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "Server_Application.h"
//#include "http-header.h"
//#include "mpeg-header.h"
#include <ns3/random-variable-stream.h>
#include <ns3/tcp-socket.h>
#include <ns3/double.h>

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("DashServer");
NS_OBJECT_ENSURE_REGISTERED (DashServer);

TypeId DashServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DashServer")
          .SetParent<Application> ()
          .AddConstructor<DashServer> ()
          .AddAttribute ("Local", "The Address on which to Bind the rx socket.", AddressValue (),
                         MakeAddressAccessor (&DashServer::m_local), MakeAddressChecker ())
          .AddAttribute ("Protocol", "The type id of the protocol to use for the rx socket.",
                         TypeIdValue (TcpSocketFactory::GetTypeId ()),
                         MakeTypeIdAccessor (&DashServer::m_tid), MakeTypeIdChecker ())
          .AddTraceSource ("Rx", "A packet has been received",
                           MakeTraceSourceAccessor (&DashServer::m_rxTrace),
                           "ns3::Packet::TracedCallback");
  return tid;
}

DashServer::DashServer () : m_socket (0)
{
	NS_LOG_FUNCTION (this);
}

DashServer::~DashServer ()
{
  NS_LOG_FUNCTION (this);
}

Ptr<Socket> DashServer::GetListeningSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

std::list<Ptr<Socket>> DashServer::GetAcceptedSockets (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socketList;
}

void DashServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socketList.clear ();

  // chain up
  Application::DoDispose ();
}

// Application Methods
void DashServer::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
  if (!m_socket)
  {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      m_socket->Bind (m_local);
      m_socket->Listen ();
  }

  m_socket->SetRecvCallback (MakeCallback (&DashServer::HandleRead, this));

  m_socket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
                               MakeCallback (&DashServer::HandleAccept, this));
  m_socket->SetCloseCallbacks (MakeCallback (&DashServer::HandlePeerClose, this),
                               MakeCallback (&DashServer::HandlePeerError, this));
}

void DashServer::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  while (!m_socketList.empty ()) //these are accepted sockets, close them
    {
      Ptr<Socket> acceptedSocket = m_socketList.front ();
      m_socketList.pop_front ();
      acceptedSocket->Close ();
    }
  if (m_socket)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
    }
}

void DashServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  int transmission_size = 660;
  //int transmission_size =4;
  int gate =7;
  while ((packet = socket->RecvFrom (from)))
  {
  	if (packet->GetSize () <= 1)
  	{ 
  		//EOF
  		break;
  	}
  	if (InetSocketAddress::IsMatchingType (from))
  	{
  		m_totalRx[socket] += packet->GetSize ();
  		
  		NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "s packet sink received "
                                  << packet->GetSize () << " bytes from "
                                  << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port "
                                  << InetSocketAddress::ConvertFrom (from).GetPort ()
                                  << " total Rx " << m_totalRx[socket] << " bytes");
        
        if(m_totalRx[socket] / transmission_size >=1)
        { 	
        	filters(gate, transmission_size);
        	m_totalRx[socket] = m_totalRx[socket] - transmission_size;
        }
        
    }
    else if (Inet6SocketAddress::IsMatchingType (from))
    {
    	m_totalRx[socket] += packet->GetSize ();
        NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "s packet sink received "
                                  << packet->GetSize () << " bytes from "
                                  << Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port "
                                  << Inet6SocketAddress::ConvertFrom (from).GetPort ()
                                  << " total Rx " << m_totalRx[socket] << " bytes");
    }
  	
    //m_totalRx += packet->GetSize ();

    //HTTPHeader header;
    //packet->RemoveHeader (header);

    //SendSegment (header.GetVideoId (), header.GetResolution (), header.GetSegmentId (), socket);

      m_rxTrace (packet, from);
  }
    
}

void DashServer::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void DashServer::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void DashServer::HandleAccept (Ptr<Socket> s, const Address &from)
{
  NS_LOG_FUNCTION (this << s << from);
  m_totalRx[s]=0;
  s->SetRecvCallback (MakeCallback (&DashServer::HandleRead, this));
  //s->SetSendCallback (MakeCallback (&DashServer::DataSend, this));
  m_socketList.push_back (s);
  NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "accept connection from " << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port "
                                  << InetSocketAddress::ConvertFrom (from).GetPort ());
}

void DashServer::filters(int gate, int transmission_size)
{
	struct timeval t2_start, t2_end;
	gettimeofday(&t2_start,NULL);
	switch(gate){
		case 0:
			TSMobject.firstfilter();
			TSMobject.secondfilter();
			TSMobject.thirdfilter();
			TSMobject.fourthfilter();
			TSMobject.fifthfilter();
			TSMobject.sixthfilter();
			TSMobject.seventhfilter();
			break;
		case 1:
			TSMobject.secondfilter();
			TSMobject.thirdfilter();
			TSMobject.fourthfilter();
			TSMobject.fifthfilter();
			TSMobject.sixthfilter();
			TSMobject.seventhfilter();
			break;
		case 2:
			TSMobject.thirdfilter();
			TSMobject.fourthfilter();
			TSMobject.fifthfilter();
			TSMobject.sixthfilter();
			TSMobject.seventhfilter();
			break;
		case 3:
			TSMobject.fourthfilter();
			TSMobject.fifthfilter();
			TSMobject.sixthfilter();
			TSMobject.seventhfilter();
			break;
		case 4:
			TSMobject.fifthfilter();
			TSMobject.sixthfilter();
			TSMobject.seventhfilter();
			break;
		case 5:
			TSMobject.sixthfilter();
			TSMobject.seventhfilter();
			break;
		case 6:
			TSMobject.seventhfilter();
		case 7:
			break;
	}	
	
	gettimeofday(&t2_end,NULL);
	double t2 = 1000000*(t2_end.tv_sec-t2_start.tv_sec)+t2_end.tv_usec-t2_start.tv_usec; //us
    printf("gate: %d, one image Server used time:%f us\n", gate, t2);
	
}
/*
void DashServer::DataSend (Ptr<Socket> socket, uint32_t)
{
  NS_LOG_FUNCTION (this);

  while (!m_queues[socket].empty ())
    {
      uint32_t max_tx_size = socket->GetTxAvailable ();

      if (max_tx_size <= 0)
        {
          NS_LOG_INFO ("Socket Send buffer is full");
          return;
        }

      Ptr<Packet> frame = m_queues[socket].front ().Copy ();
      m_queues[socket].pop_front ();

      uint32_t init_size = frame->GetSize ();

      if (max_tx_size < init_size)
        {
          NS_LOG_INFO ("Insufficient space in send buffer, fragmenting");
          Ptr<Packet> frag0 = frame->CreateFragment (0, max_tx_size);
          Ptr<Packet> frag1 = frame->CreateFragment (max_tx_size, init_size - max_tx_size);

          m_queues[socket].push_front (*frag1);
          frame = frag0;
        }

      uint32_t bytes;
      if ((bytes = socket->Send (frame)) < frame->GetSize ())
        {
          NS_FATAL_ERROR ("Couldn't send packet, though space should be available");
          exit (1);
        }
      else
        {
          NS_LOG_INFO ("Just sent " << frame->GetSerializedSize () << " " << frame->GetSize ());
          //socket->Send(Create<Packet>(0));
        }
    }
}
*/
} // Namespace ns3