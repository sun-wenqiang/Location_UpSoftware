#include "nodecontroller.h"


bool NodeController::sendMessage(uint8_t cmd_id, const std::vector<uint8_t>& cmd_data)
{
    if (!m_client)
    {
        qWarning()<< "Tcp client is not exist";
        return false;
    }
    if (m_client->sendCommand(cmd_id, cmd_data))
    {
        setStatus(ClientState::waitingResponse);
        return true;
    }
    else
    {
        return false;
    }
}


void NodeController::setStatus(ClientState new_state)
{
    m_client->setStatus(new_state);
}


ClientState NodeController::getStatus()
{
    return m_client->getStatus();
}


bool NodeController::connectToServer()
{
    if (m_client->connectToServer())
    {
        setStatus(ClientState::Connected);
        return true;
    }
    else
    {
        setStatus(ClientState::Error);
        return false;
    }
}


bool NodeController::disconnectFromServer()
{
    if (m_client->disconnectFromServer())
    {
        setStatus(ClientState::Idle);
        return true;
    }
    else
    {
        setStatus(ClientState::Error);
        return false;
    }
}
