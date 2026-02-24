#include "TcpClient.h"
#include "ProxyCore.h"
#include <vector>

void TcpClient::start(ProxyCore* c) 
{
    core = c; 
    runFlag = true;
    WSADATA w; 
    WSAStartup(MAKEWORD(2, 2), &w);
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr{}; 
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(s, (sockaddr*)&addr, sizeof(addr));
    th = std::thread(&TcpClient::run, this);
}

void TcpClient::stop() 
{
    runFlag = false; 
    if (th.joinable()) th.join();
    if (s != INVALID_SOCKET) closesocket(s); 
    WSACleanup();
}

bool TcpClient::sendPacket(const MsgHeader& h, const uint8_t* p) 
{
    int total=sizeof(h) + h.length; 
    std::vector<uint8_t> buf(total);
    memcpy(buf.data(), &h, sizeof(h));
    if (h.length) memcpy(buf.data() + sizeof(h), p, h.length);
    return send(s, (const char*)buf.data(), total, 0) == total;
}

void TcpClient::sendConnect(
    short clientId, 
    short nDeviceID, 
    char* fpchProtocol, 
    long lTxBufferSize, 
    long lRcvBufferSize, 
    short nIsAppPacketizingIncomingMsgs) 
{
    uint16_t length = sizeof(short) +
        std::strlen(fpchProtocol) +
        sizeof(long) +
        sizeof(long) +
        sizeof(short);
    MsgHeader h{ MsgType::ClientConnect, (uint16_t)clientId, length};

    uint8_t* pData = new uint8_t[length];
    uint8_t* cursor = pData;
    memcpy((void*)cursor, &nDeviceID, sizeof(short));
    cursor += sizeof(short);
    memcpy((void*)cursor, fpchProtocol, std::strlen(fpchProtocol));
    cursor += std::strlen(fpchProtocol);
    memcpy((void*)cursor, &lTxBufferSize, sizeof(long));
    cursor += sizeof(long);
    memcpy((void*)cursor, &lRcvBufferSize, sizeof(long));
    cursor += sizeof(long);
    memcpy((void*)cursor, &nIsAppPacketizingIncomingMsgs, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::sendDisconnect(short nClientID) 
{
    MsgHeader h{ MsgType::ClientDisconnect,(uint16_t)nClientID, 0};
    sendPacket(h,nullptr);
}

void TcpClient::sendData(
    short nClientID, 
    char* fpchClientMessage, 
    short nMessageSize, 
    short nNotifyStatusOnTx, 
    short nBlockOnSend) 
{
    uint16_t length = nMessageSize +
        sizeof(short) +
        sizeof(short) +
        sizeof(short);
    MsgHeader h{MsgType::SendMsg, (uint16_t)nClientID, length};

    uint8_t* pData = new uint8_t[length];
    uint8_t* cursor = pData;
    memcpy((void*)cursor, fpchClientMessage, nMessageSize);
    cursor += nMessageSize;
    memcpy((void*)cursor, &nMessageSize, sizeof(short));
    cursor += sizeof(short);
    memcpy((void*)cursor, &nNotifyStatusOnTx, sizeof(short));
    cursor += sizeof(short);
    memcpy((void*)cursor, &nBlockOnSend, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::readData(
    short nClientID,
    short nBufferSize,
    short nBlockOnRead)
{
    uint16_t length = sizeof(short) +
        sizeof(short);
    MsgHeader h{ MsgType::ReadMsg, (uint16_t)nClientID, length };

    uint8_t* pData = new uint8_t[length];
    uint8_t* cursor = pData;
    memcpy((void*)cursor, &nBufferSize, sizeof(short));
    cursor += sizeof(short);
    memcpy((void*)cursor, &nBlockOnRead, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::sendCommand(
    short nClientID,
    short nCommandNumber,
    char* fpchClientCommand,
    short nMessageSize)
{
    uint16_t length = sizeof(short) +
        nMessageSize +
        sizeof(short);
    MsgHeader h{ MsgType::SendCommand, (uint16_t)nClientID, length };

    uint8_t* pData = new uint8_t[length];
    uint8_t* cursor = pData;
    memcpy((void*)cursor, &nCommandNumber, sizeof(short));
    cursor += sizeof(short);
    memcpy((void*)cursor, fpchClientCommand, nMessageSize);
    cursor += nMessageSize;
    memcpy((void*)cursor, &nMessageSize, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::readVersion()
{
    MsgHeader h{ MsgType::ReadVersion, 0, 0 };
    sendPacket(h, nullptr);
}

void TcpClient::getErrorMsg(short errorCode)
{
    MsgHeader h{ MsgType::GetErrorMsg, 0, 2 };

    uint8_t* pData = new uint8_t[2];
    memcpy((void*)pData, &errorCode, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::getHardwareStatus(
    short nClientID,
    short nInfoSize,
    short nBlockOnRequest)
{
    uint16_t length = sizeof(short) * 2;
    MsgHeader h{ MsgType::GetHardwareStatus, (uint16_t)nClientID, length };

    uint8_t* pData = new uint8_t[length];
    uint8_t* cursor = pData;
    memcpy((void*)cursor, &nInfoSize, sizeof(short));
    cursor += sizeof(short);
    memcpy((void*)cursor, &nBlockOnRequest, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::getLastErrorMsg(
    short nClientID,
    short errorCode)
{
    uint16_t length = sizeof(short);
    MsgHeader h{ MsgType::GetLastErrorMsg, (uint16_t)nClientID, length };

    uint8_t* pData = new uint8_t[length];
    memcpy((void*)pData, &errorCode, sizeof(short));
    sendPacket(h, pData);
}

void TcpClient::readDetailedVersion(short nClientID)
{
    MsgHeader h{ MsgType::ReadDetailedVersion, (uint16_t)nClientID, 0 };
    sendPacket(h, nullptr);
}

void TcpClient::ioctl(
    short nClientID,
    long nIoctlID,
    void* pInput)
{
    uint16_t length = sizeof(long);
    MsgHeader h{ MsgType::Ioctl, (uint16_t)nClientID, length };

    uint8_t* pData = new uint8_t[length];
    memcpy((void*)pData, &nIoctlID, sizeof(long));
    sendPacket(h, pData);
}

void TcpClient::run() 
{
    while (runFlag) 
    {
        MsgHeader h{}; 
        int r = recv(s, (char*)&h, sizeof(h), MSG_WAITALL);
        if (r <= 0) break;
        std::vector<uint8_t> payload(h.length);
        if (h.length) recv(s, (char*)payload.data(), h.length, MSG_WAITALL);
        core->injectBuffer(h.clientId, h.type, std::move(payload));
    }
}
