#include <ns3/log.h>
#include <ns3/uinteger.h>
#include <ns3/tcp-socket-factory.h>
#include <ns3/simulator.h>
#include <ns3/inet-socket-address.h>
#include <ns3/inet6-socket-address.h>
//#include "http-header.h"
#include "Client_Application.h"
#include "TSM.h"

NS_LOG_COMPONENT_DEFINE ("DashClient");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DashClient);

int DashClient::m_countObjs = 0;

TypeId DashClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DashClient")
          .SetParent<Application> ()
          .AddConstructor<DashClient> ()
          .AddAttribute ("VideoId", "The Id of the video that is played.", UintegerValue (0),
                         MakeUintegerAccessor (&DashClient::m_videoId),
                         MakeUintegerChecker<uint32_t> (1))
          .AddAttribute ("Remote", "The address of the destination", AddressValue (),
                         MakeAddressAccessor (&DashClient::m_peer), MakeAddressChecker ())
          .AddAttribute ("Protocol", "The type of TCP protocol to use.",
                         TypeIdValue (TcpSocketFactory::GetTypeId ()),
                         MakeTypeIdAccessor (&DashClient::m_tid), MakeTypeIdChecker ());
  return tid;
}

DashClient::DashClient ()
    : m_socket (0),
      m_connected (false),
      m_totBytes (0),
      m_started (Seconds (0)),
      m_sumDt (Seconds (0)),
      m_lastDt (Seconds (-1)),
      m_id (m_countObjs++),
      m_requestTime ("0s"),
      m_segment_bytes (0),
      m_bitRate (45000),
      m_window (Seconds (10))
{
  NS_LOG_FUNCTION (this);
  //m_parser.SetApp (this); // So the parser knows where to send the received messages
}

DashClient::~DashClient ()
{
  NS_LOG_FUNCTION (this);
}

Ptr<Socket> DashClient::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

void
DashClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void DashClient::StartApplication (void) // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  // Create the socket if not already

  NS_LOG_INFO ("trying to create connection");
  if (!m_socket)
    {
      NS_LOG_INFO ("m_socket is null");

      m_started = Simulator::Now ();

      m_socket = Socket::CreateSocket (GetNode (), m_tid);

      // Fatal error if socket type is not NS3_SOCK_STREAM or NS3_SOCK_SEQPACKET
      if (m_socket->GetSocketType () != Socket::NS3_SOCK_STREAM &&
          m_socket->GetSocketType () != Socket::NS3_SOCK_SEQPACKET)
        {
          NS_FATAL_ERROR ("Using HTTP with an incompatible socket type. "
                          "HTTP requires SOCK_STREAM or SOCK_SEQPACKET. "
                          "In other words, use TCP instead of UDP.");
        }

      if (Inet6SocketAddress::IsMatchingType (m_peer))
        {
          m_socket->Bind6 ();
        }
      else if (InetSocketAddress::IsMatchingType (m_peer))
        {
          m_socket->Bind ();
        }

      m_socket->Connect (m_peer);
      m_socket->SetRecvCallback (MakeCallback (&DashClient::HandleRead, this));
      m_socket->SetConnectCallback (MakeCallback (&DashClient::ConnectionSucceeded, this),
                                    MakeCallback (&DashClient::ConnectionFailed, this));
      m_socket->SetSendCallback (MakeCallback (&DashClient::DataSend, this));
      NS_LOG_INFO ("Connected callbacks");
    }
  NS_LOG_INFO ("Just started connection");
}

void DashClient::StopApplication (void) // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
      m_connected = false;
    }
  else
    {
      NS_LOG_WARN ("DashClient found null socket to close in StopApplication");
    }
}

// Private helpers

void DashClient::RequestSegment ()
{
  NS_LOG_FUNCTION (this);

  if (m_connected == false)
    {
      return;
    }
  //int int_size = sizeof(int);  
  //char sendline[int_size];
  int transmission_size= 660;
  //int transmission_size= 4;
  char constantss[transmission_size];
  char sendline[transmission_size];
  char* moving = sendline;
  int m_packetsSent =0;
  int res = 0;
  struct timeval t2_start, t2_end;
  gettimeofday(&t2_start,NULL);
  while(++m_packetsSent < 2)
  {
  	    	
	TSMobject.firstfilter();
	
	TSMobject.secondfilter();
	
	TSMobject.thirdfilter();
	
	TSMobject.fourthfilter();
	
	TSMobject.fifthfilter();
	
	//border
	TSMobject.sixthfilter();
	
	TSMobject.seventhfilter();
	
	memcpy(moving, &constantss, transmission_size);//convert struct into char array
	Ptr<Packet> packet = Create<Packet> (transmission_size); 
  	//NS_LOG_UNCOND("availavle"<<m_socket->GetTxAvailable());
  	if (((unsigned) (res = m_socket->Send (packet))) != packet->GetSize ())
    {
      NS_FATAL_ERROR ("Oh oh. Couldn't send packet! res=" << res << " size=" << packet->GetSize ());
    }
	//gettimeofday(&t11_end,NULL);
  }
	gettimeofday(&t2_end,NULL);
	
    //NS_LOG_UNCOND("Packet_size: "<< packet->GetSize ());
    double t2 = 1000000*(t2_end.tv_sec-t2_start.tv_sec)+t2_end.tv_usec-t2_start.tv_usec; //us
    printf("one image t2 Client used time:%f us\n", t2);
    //NS_LOG_UNCOND (" At " << Simulator::Now ().GetMilliSeconds ());
}

void DashClient::CheckBuffer ()
{
  NS_LOG_FUNCTION (this);
  //m_parser.ReadSocket(m_socket);
}

void
DashClient::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  //m_parser.ReadSocket (socket);
}

void
DashClient::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () <<" DashClient " << m_id <<" Connection succeeded");
  m_connected = true;
  //if(m_id!= 0)
  Simulator::Schedule(Seconds(3.0), &DashClient::RequestSegment,this);
  //RequestSegment ();
}

void
DashClient::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_LOGIC ("DashClient " << m_id << ", Connection Failed, retrying...");
  m_socket = 0;
  m_connected = false;
  StartApplication ();
}

void DashClient::DataSend (Ptr<Socket>, uint32_t)
{
  NS_LOG_FUNCTION (this);

  if (m_connected)
    { // Only send new data if the connection has completed

      NS_LOG_INFO ("Something was sent");
    }
  else
    {
      NS_LOG_INFO ("NOT CONNECTED!!!!");
    }
}

} // Namespace ns3