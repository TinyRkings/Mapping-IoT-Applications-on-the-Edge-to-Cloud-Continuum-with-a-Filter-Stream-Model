#include "Client_helper.h"
#include "ns3/inet-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/string.h"
#include "ns3/names.h"

namespace ns3 {

DashClientHelper::DashClientHelper (std::string tcpProtocol, Address address)
{
  m_factory.SetTypeId ("ns3::DashClient");
  m_factory.Set ("Protocol", StringValue (tcpProtocol));
  m_factory.Set ("Remote", AddressValue (address));
}

DashClientHelper::DashClientHelper (std::string tcpProtocol, Address address, std::string algorithm)
{
  m_factory.SetTypeId (algorithm);
  m_factory.Set ("Protocol", StringValue (tcpProtocol));
  m_factory.Set ("Remote", AddressValue (address));
}
void DashClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer DashClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer DashClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer DashClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application> DashClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3