#include "ProxyCore.h"
#include <chrono>
#include <cstring>

void ProxyCore::start() 
{ 
    tcpRunning = tcp.start(this); 
}

void ProxyCore::stop() 
{ 
    tcp.stop(); 
}

void ProxyCore::addClient(short c)
{
    std::unique_lock<std::mutex> l(mtx);
    auto it = clients.find(c);
    if (it == clients.end())
    {
        clients.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(c),
            std::forward_as_tuple()
        );
    }
}

short ProxyCore::sendConnect(
    short nDeviceID, 
    char* fpchProtocol, 
    long lTxBufferSize, 
    long lRcvBufferSize, 
    short nIsAppPacketizingIncomingMsgs)
{
    short tmpClientID = --nextId;
    addClient(tmpClientID);
    tcp.sendConnect(tmpClientID, nDeviceID, fpchProtocol, lTxBufferSize, lRcvBufferSize, nIsAppPacketizingIncomingMsgs);

    return tmpClientID;
}

short ProxyCore::clientConnect(
    short nDeviceID, 
    char* fpchProtocol, 
    long lTxBufferSize, 
    long lRcvBufferSize, 
    short nIsAppPacketizingIncomingMsgs)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    short tmpClientID = sendConnect(nDeviceID, fpchProtocol, lTxBufferSize, lRcvBufferSize, nIsAppPacketizingIncomingMsgs);
    
    MsgResult result = getMessageData(tmpClientID, MsgType::ClientConnect);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size(); 
    if (len != sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    std::lock_guard<std::mutex> l(mtx);
    clients.erase(tmpClientID);

    short clientId;
    memcpy(&clientId, msgData.data.data(), len);
    return clientId;
}

short ProxyCore::clientDisconnect(short nClientID)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    tcp.sendDisconnect(nClientID);

    MsgResult result = getMessageData(nClientID, MsgType::ClientDisconnect);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len != sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    std::lock_guard<std::mutex> l(mtx); 
    clients.erase(nClientID);

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), len);

    return resultCode;
}

short ProxyCore::sendMessage(
    short nClientID, 
    char* fpchClientMessage, 
    short nMessageSize, 
    short nNotifyStatusOnTx, 
    short nBlockOnSend)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(nClientID);
    tcp.sendData(nClientID, fpchClientMessage, nMessageSize, nNotifyStatusOnTx, nBlockOnSend);

    MsgResult result = getMessageData(nClientID, MsgType::SendMsg);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len != sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), len);

    return resultCode;
}

short ProxyCore::readMessage(
    short nClientID, 
    char* fpchAPIMessage, 
    short nBufferSize, 
    short nBlockOnRead)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(nClientID);
    tcp.readData(nClientID, nBufferSize, nBlockOnRead);

    MsgResult result = getMessageData(nClientID, MsgType::ReadMsg);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len < sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), sizeof(short));
    if (resultCode != (short)ErrorCode::NO_ERRORS) return resultCode;

    int dataLen = min(nBufferSize, (short)msgData.data.size() - sizeof(short));
    memcpy(fpchAPIMessage, msgData.data.data() + sizeof(short), dataLen);
    return dataLen;
}

short ProxyCore::sendCommand(
    short nCommandNumber, 
    short nClientID, 
    char* fpchClientCommand, 
    short nMessageSize)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(nClientID);
    tcp.sendCommand(nClientID, nCommandNumber, fpchClientCommand, nMessageSize);

    MsgResult result = getMessageData(nClientID, MsgType::SendCommand);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len != sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), sizeof(short));

    return resultCode;
}

void ProxyCore::readVersion(
    char* fpchDLLMajorVersion,
    char* fpchDLLMinorVersion,
    char* fpchAPIMajorVersion,
    char* fpchAPIMinorVersion)
{
    if (!tcpRunning) return;

    addClient(GLOBAL_CLIENTID);
    tcp.readVersion();

    MsgResult result = getMessageData(GLOBAL_CLIENTID, MsgType::ReadVersion);
    if (result.code < 0) return;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len <= sizeof(uint8_t) * 4) return;

    short dllMjLen = 0, dllMnLen = 0, apiMjLen = 0, apiMnLen = 0;
    memcpy(&dllMjLen, msgData.data.data(), sizeof(uint8_t));
    memcpy(&dllMnLen, msgData.data.data() + sizeof(uint8_t), sizeof(uint8_t));
    memcpy(&apiMjLen, msgData.data.data() + sizeof(uint8_t) * 2, sizeof(uint8_t));
    memcpy(&apiMnLen, msgData.data.data() + sizeof(uint8_t) * 3, sizeof(uint8_t));

    short totalLen = sizeof(uint8_t) * 4 +
        dllMjLen +
        dllMnLen +
        apiMjLen +
        apiMnLen;
    if (totalLen != len) return;

    memcpy(fpchDLLMajorVersion, msgData.data.data() + sizeof(uint8_t) * 4, dllMjLen);
    memcpy(fpchDLLMinorVersion, msgData.data.data() + sizeof(uint8_t) * 4 + dllMjLen, dllMnLen);
    memcpy(fpchAPIMajorVersion, msgData.data.data() + sizeof(uint8_t) * 4 + dllMjLen + dllMnLen, apiMjLen);
    memcpy(fpchAPIMinorVersion, msgData.data.data() + sizeof(uint8_t) * 4 + dllMjLen + dllMnLen + apiMjLen, apiMnLen);
}

short ProxyCore::getErrorMsg(
    short errorCode,
    char* fpchDescription)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(GLOBAL_CLIENTID);
    tcp.getErrorMsg(errorCode);

    MsgResult result = getMessageData(GLOBAL_CLIENTID, MsgType::GetErrorMsg);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len <= sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), sizeof(short));
    if (resultCode != (short)ErrorCode::NO_ERRORS) return resultCode;

    int dataLen = (short)msgData.data.size() - sizeof(short);
    memcpy(fpchDescription, msgData.data.data() + sizeof(short), dataLen);

    return resultCode;
}

short ProxyCore::getHardwareStatus(
    short nClientID,
    char* fpchClientInfo,
    short nInfoSize,
    short nBlockOnRequest)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(nClientID);
    tcp.getHardwareStatus(nClientID, nInfoSize, nBlockOnRequest);

    MsgResult result = getMessageData(nClientID, MsgType::GetHardwareStatus);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len <= sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), sizeof(short));
    if (resultCode != (short)ErrorCode::NO_ERRORS) return resultCode;

    int dataLen = (short)msgData.data.size() - sizeof(short);
    memcpy(fpchClientInfo, msgData.data.data() + sizeof(short), dataLen);

    return resultCode;
}

short ProxyCore::getLastErrorMsg(
    short nClientID,
    short errorCode,
    int* subErrorCode,
    char* fpchClientInfo)
{
    if (!tcpRunning)
    {
        std::strcpy(fpchClientInfo, lastError.c_str());
        return (short)ErrorCode::NO_ERRORS;
    }

    addClient(nClientID);
    tcp.getLastErrorMsg(nClientID, errorCode);

    MsgResult result = getMessageData(nClientID, MsgType::GetLastErrorMsg);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len <= sizeof(short) + sizeof(int)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), sizeof(short));
    if (resultCode != (short)ErrorCode::NO_ERRORS) return resultCode;

    memcpy(subErrorCode, msgData.data.data() + sizeof(short), sizeof(int));
    int dataLen = (short)msgData.data.size() - sizeof(short) - sizeof(int);
    memcpy(fpchClientInfo, msgData.data.data() + sizeof(short) + sizeof(int), dataLen);

    return resultCode;
}

short ProxyCore::readDetailedVersion(
    short nClientID,
    char* fpchAPIVersionInfo,
    char* fpchDLLVersionInfo,
    char* fpchFWVersionInfo)
{
    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(nClientID);
    tcp.readDetailedVersion(nClientID);

    MsgResult result = getMessageData(nClientID, MsgType::ReadDetailedVersion);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len <= sizeof(short) + sizeof(uint8_t) * 3) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), sizeof(short));
    if (resultCode != (short)ErrorCode::NO_ERRORS) return resultCode;

    short apiVerLen = 0, dllVerLen = 0, hwVerLen = 0;
    memcpy(&apiVerLen, msgData.data.data() + sizeof(short), sizeof(uint8_t));
    memcpy(&dllVerLen, msgData.data.data() + sizeof(short) + sizeof(uint8_t), sizeof(uint8_t));
    memcpy(&hwVerLen, msgData.data.data() + sizeof(short) + sizeof(uint8_t) * 2, sizeof(uint8_t));

    short totalLen = sizeof(short) +
        sizeof(uint8_t) * 3 +
        apiVerLen +
        dllVerLen +
        hwVerLen;
    if (totalLen != len) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short startPos = sizeof(short) + sizeof(uint8_t) * 3;
    memcpy(fpchAPIVersionInfo, msgData.data.data() + startPos, apiVerLen);
    memcpy(fpchDLLVersionInfo, msgData.data.data() + startPos + apiVerLen, dllVerLen);
    memcpy(fpchFWVersionInfo, msgData.data.data() + startPos + apiVerLen + dllVerLen, hwVerLen);

    return resultCode;
}

short ProxyCore::ioctl(
    short nClientID,
    long nIoctlID,
    void* pInput,
    void* pOutput)
{
    return (short)ErrorCode::ERR_INVALID_COMMAND;

    if (!tcpRunning) return (short)ErrorCode::ERR_DLL_NOT_INITIALIZED;

    addClient(nClientID);
    tcp.ioctl(nClientID, nIoctlID, pInput);

    MsgResult result = getMessageData(nClientID, MsgType::Ioctl);
    if (result.code < 0) return result.code;

    MsgData msgData = result.data;
    int len = (short)msgData.data.size();
    if (len < sizeof(short)) return (short)ErrorCode::ERR_INVALID_RESPONSE;

    short resultCode;
    memcpy(&resultCode, msgData.data.data(), len);
    if (resultCode != (short)ErrorCode::NO_ERRORS) return resultCode;

    int dataLen = (short)msgData.data.size() - sizeof(short);
    if (dataLen > 0)
        memcpy(pOutput, msgData.data.data() + sizeof(short), dataLen);

    return resultCode;
}

ProxyCore::MsgResult ProxyCore::getMessageData(short clientID, MsgType type)
{
    std::unique_lock<std::mutex> l(mtx);
    ProxyCore::MsgResult result = { (short)ErrorCode::NO_ERRORS, MsgData() };
    auto it = clients.find(clientID);
    if (it == clients.end())
    {
        result.code = (short)ErrorCode::ERR_INVALID_CLIENT_ID;
        return result;
    }
    if (!it->second.cv.wait_for(l, std::chrono::seconds(300), [&] {
        if (it->second.msgBuf.empty()) return false;
        auto msg = std::find_if(it->second.msgBuf.begin(), it->second.msgBuf.end(), [&](const MsgData& d) {
            return d.type == type;
        });
        return msg != it->second.msgBuf.end();
    }))
    {
        result.code = (short)ErrorCode::ERR_COMMAND_TIMED_OUT;
        return result;
    };

    auto msg = std::find_if(it->second.msgBuf.begin(), it->second.msgBuf.end(), [&](const MsgData& d) {
        return d.type == type;
    });

    result.data = *msg;
    it->second.msgBuf.erase(msg);
    return result;
}

void ProxyCore::injectBuffer(short c, MsgType type, std::vector<uint8_t> d)
{
    std::lock_guard<std::mutex> l(mtx); 
    MsgData msg = { type, d };
    clients[c].msgBuf.emplace_back(msg);
    clients[c].cv.notify_one();
}
