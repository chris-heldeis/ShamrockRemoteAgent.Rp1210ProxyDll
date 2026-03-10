#include "Rp1210.h"
#include "ProxyCore.h"

static ProxyCore g;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD r, LPVOID)
{ 
	if (r == DLL_PROCESS_ATTACH)
	{
		g.hModule = hModule;
		g.start();
	}
	if (r==DLL_PROCESS_DETACH) 
		g.stop(); 
	return TRUE; 
}

extern "C" {

	short __stdcall RP1210_ClientConnect(
		HWND hWnd, 
		short nDeviceID, 
		char* fpchProtocol, 
		long lTxBufferSize, 
		long lRcvBufferSize, 
		short nIsAppPacketizingIncomingMsgs)
	{ 
		return g.clientConnect(nDeviceID, fpchProtocol, lTxBufferSize, lRcvBufferSize, nIsAppPacketizingIncomingMsgs);
	}

	short __stdcall RP1210_ClientDisconnect(short nClientID)
	{ 
		return g.clientDisconnect(nClientID);
	}

	short __stdcall RP1210_SendMessage(
		short nClientID, 
		char* fpchClientMessage, 
		short nMessageSize, short nNotifyStatusOnTx, short nBlockOnSend)
	{ 
		return g.sendMessage(nClientID, fpchClientMessage, nMessageSize, nNotifyStatusOnTx, nBlockOnSend);
	}

	short __stdcall RP1210_ReadMessage(
		short nClientID, 
		char* fpchAPIMessage, 
		short nBufferSize, 
		short nBlockOnRead)
	{ 
		return g.readMessage(nClientID, fpchAPIMessage, nBufferSize, nBlockOnRead); 
	}

	short __stdcall RP1210_SendCommand(
		short nCommandNumber,
		short nClientID,
		char* fpchClientCommand,
		short nMessageSize)
	{
		return g.sendCommand(nCommandNumber, nClientID, fpchClientCommand, nMessageSize);
	}

	void __stdcall RP1210_ReadVersion(
		char* fpchDLLMajorVersion,
		char* fpchDLLMinorVersion,
		char* fpchAPIMajorVersion,
		char* fpchAPIMinorVersion)
	{ 
		g.readVersion(fpchDLLMajorVersion, fpchDLLMinorVersion, fpchAPIMajorVersion, fpchAPIMinorVersion);
	}

	short __stdcall RP1210_GetErrorMsg(
		short errorCode,
		char* fpchDescription)
	{ 
		return g.getErrorMsg(errorCode, fpchDescription);
	}

	short __stdcall RP1210_GetHardwareStatus(
		short nClientID,
		char* fpchClientInfo,
		short nInfoSize,
		short nBlockOnRequest)
	{
		return g.getHardwareStatus(nClientID, fpchClientInfo, nInfoSize, nBlockOnRequest);
	}

	short __stdcall RP1210_GetLastErrorMsg(
		short errorCode,
		int* subErrorCode,
		char* fpchDescription,
		short nClientID)
	{
		return g.getLastErrorMsg(nClientID, errorCode, subErrorCode, fpchDescription);
	}

	short __stdcall RP1210_ReadDetailedVersion(
		short nClientID,
		char* fpchAPIVersionInfo,
		char* fpchDLLVersionInfo,
		char* fpchFWVersionInfo)
	{
		return g.readDetailedVersion(nClientID, fpchAPIVersionInfo, fpchDLLVersionInfo, fpchFWVersionInfo);
	}

	short __stdcall RP1210_Ioctl(
		short nClientID,
		long nIoctlID,
		void* pInput,
		void* pOutput)
	{
		return g.ioctl(nClientID, nIoctlID, pInput, pOutput);
	}
}
