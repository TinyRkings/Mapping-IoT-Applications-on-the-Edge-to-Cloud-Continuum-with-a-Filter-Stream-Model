#ifndef DASH_CLIENT_H
#define DASH_CLIENT_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
//#include "mpeg-player.h"
#include "ns3/traced-callback.h"
//#include "http-parser.h"

#include "TSM.h"
#include <sys/time.h>

namespace ns3 {

class DashClient : public Application
{
  //friend class MpegPlayer;
  //friend class HttpParser;

public:
  static TypeId GetTypeId (void);

  DashClient ();

  virtual ~DashClient ();

  /**
     * \return pointer to associated socket
     */
  Ptr<Socket> GetSocket (void) const;

  /**
     * \brief Prints some statistics.
     */
  void GetStats ();

  

  void CheckBuffer ();

protected:
  virtual void DoDispose (void);

  

private:
  /**
     * \brief Called the next MPEG segment should be requested from the server.
     *
     * \param The bitrate of the next segment.
     */
  void RequestSegment ();

  /**
     * \brief Called by the HttpParser when it has received a complete HTTP
     * message containing an MPEG frame.
     *
     * \param the message that was received
     */
  //bool MessageReceived (Packet message);

  // inherited from Application base class.
  virtual void StartApplication (void); // Called at time specified by Start
  virtual void StopApplication (void); // Called at time specified by Stop
  void ConnectionSucceeded (Ptr<Socket> socket); // Called when the connections has succeeded
  void ConnectionFailed (Ptr<Socket> socket); // Called when the connection has failed.
  void DataSend (Ptr<Socket>, uint32_t); // Called when the data has been transmitted
  void HandleRead (Ptr<Socket>); // Called when we receive data from the server
  //virtual void CalcNextSegment (uint32_t currRate, uint32_t &nextRate, Time &delay);
  //void LogBufferLevel (Time t);
  void inline SetWindow (Time time)
  {
    m_window = time;
  }
  
  TSM TSMobject; 
  //HttpParser m_parser; // An HttpParser object for parsing the incoming stream into http messages
  Ptr<Socket> m_socket; // Associated socket
  Address m_peer; // Peer address
  bool m_connected; // True if connected
  uint32_t m_totBytes; // Total bytes received.

  TypeId m_tid;
  TracedCallback<Ptr<const Packet>> m_txTrace;
  uint32_t m_videoId; // The Id of the video that is requested
  Time m_started; // Time of application starting
  Time m_sumDt; // Used for calculating the average buffering time
  Time m_lastDt; // The previous buffering time (used for calculating the differential
  static int m_countObjs; // Number of DashClient instances (for generating unique id)
  int m_id;
  Time m_requestTime; // Time of sending the last request
  uint32_t m_segment_bytes; // Bytes of the current segment that have been received so far
  uint32_t m_bitRate; // The bitrate of the current segment.
  Time m_window;
  Time m_segmentFetchTime;
  bool m_RequestPending = false; // So that we don't request the same segment repeatedly
};

} // namespace ns3

#endif /* DASH_CLIENT_H */