#ifndef CLIENT_HELPER_H
#define CLIENT_HELPER_H

#include <stdint.h>
#include <string>
#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"

namespace ns3 {

class DashClientHelper
{
public:
  /**
   * Create an DashClientHelper to make it easier to work with DashClients
   *
   * \param protocol the name of the protocol to use to send traffic
   *        by the applications. This string identifies the socket
   *        factory type used to create sockets for the applications.
   *        A typical value would be ns3::UdpSocketFactory.
   * \param address the address of the remote node to send traffic
   *        to.
   */
  DashClientHelper (std::string tcpProtocol, Address address);

  /**
    * Create an DashClientHelper to make it easier to work with DashClients
    *
    * \param protocol the name of the protocol to use to send traffic
    *        by the applications. This string identifies the socket
    *        factory type used to create sockets for the applications.
    *        A typical value would be ns3::UdpSocketFactory.
    * \param address the address of the remote node to send traffic
    *        to.
    * \param algorithm The algorithm that will be implemented by the Dash
    *        Client to determine the resolution and the delay for the request
    *        for the next segment.
    */
  DashClientHelper (std::string tcpProtocol, Address address, std::string algorithm);

  /**
   * Helper function used to set the underlying application attributes, 
   * _not_ the socket attributes.
   *
   * \param name the name of the application attribute to set
   * \param value the value of the application attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Install an ns3::DashClient on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which an DashClient
   * will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (NodeContainer c) const;

  /**
   * Install an ns3::DashClient on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param node The node on which an DashClient will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install an ns3::DashClient on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param nodeName The node on which an DashClient will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (std::string nodeName) const;

private:
  /**
   * \internal
   * Install an ns3::DashClient on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param node The node on which an DashClient will be installed.
   * \returns Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;
  std::string m_protocol;
  Address m_remote;
  ObjectFactory m_factory;
};

} // namespace ns3

#endif /* ON_OFF_HELPER_H */