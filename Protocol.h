#pragma once
#include <cstdint>

#define GLOBAL_CLIENTID 0

enum class MsgType : uint8_t 
{ 
	ClientConnect = 1,
	ClientDisconnect,
	SendMsg,
	ReadMsg,
	SendCommand,
	ReadVersion,
	GetErrorMsg,
	GetHardwareStatus,
	GetLastErrorMsg,
	ReadDetailedVersion,
	Ioctl
};

enum class ErrorCode : short
{
	NO_ERRORS  									= 0,    // No Errors. Success.  
	ERR_DLL_NOT_INITIALIZED  					= 128,  // API DLL was not initialized.  
	ERR_INVALID_CLIENT_ID  						= 129,  // Client Identifier was invalid.  
	ERR_CLIENT_ALREADY_CONNECTED  				= 130,  // A client is already connected to the specified device.  
	ERR_CLIENT_AREA_FULL  						= 131,  // The maximum number of connections has been reached.  
	ERR_FREE_MEMORY  							= 132,  // An error occurred while processing memory deallocation.  
	ERR_NOT_ENOUGH_MEMORY  						= 133,  // API DLL could not allocate enough memory to create the client.  
	ERR_INVALID_DEVICE  						= 134,  // The specified device ID is invalid or can not be used.  
	ERR_DEVICE_IN_USE  							= 135,  // The specified device is already in use. (No-Multiclients).  
	ERR_INVALID_PROTOCOL  						= 136,  // The specified protocol is invalid or unsupported.  
	ERR_TX_QUEUE_FULL  							= 137,  // API DLL’s transmit message queue is full.  
	ERR_TX_QUEUE_CORRUPT  						= 138,  // API DLL’s receive message queue is corrupt.  
	ERR_RX_QUEUE_FULL  							= 139,  // API DLL’s receive message queue is full.  
	ERR_RX_QUEUE_CORRUPT  						= 140,  // API DLL’s receive message queue is corrupt.  
	ERR_MESSAGE_TOO_LONG  						= 141,  // The message to be received is too long for the user’s buffer.  
	ERR_HARDWARE_NOT_RESPONDING  				= 142,  // The device hardware interface is not responding.  
	ERR_COMMAND_NOT_SUPPORTED  					= 143,  // The command number is not supported by the API DLL.  
	ERR_INVALID_COMMAND  						= 144,  // The command number or its parameters are wrongly specified.  
	ERR_TXMESSAGE_STATUS  						= 145,  // Error TX Message Status.  
	ERR_ADDRESS_CLAIM_FAILED  					= 146,  // The API was not able to claim the requested address.  
	ERR_CANNOT_SET_PRIORITY  					= 147,  // Error Can not Set Priority.  
	ERR_CLIENT_DISCONNECTED  					= 148,  // RP1210_ClientDisconnect was called while blocking.  
	ERR_CONNECT_NOT_ALLOWED  					= 149,  // Only one connection is allowed in the requested Mode.  
	ERR_CHANGE_MODE_FAILED  					= 150,  // Error Change mode Failed.  
	ERR_BUS_OFF  								= 151,  // A CAN packet was not transmitt due to a BUS_OFF condition.  
	ERR_COULD_NOT_TX_ADDRESS_CLAIMED  			= 152,  // The API was not able to request an address.  
	ERR_ADDRESS_LOST  							= 153,  // API was forced to concede the address to another node on the J1939 network.  
	ERR_CODE_NOT_FOUND  						= 154,  // The error code provided does not have an associated textual representation.  
	ERR_BLOCK_NOT_ALLOWED  						= 155,  // This was a legacy WindowsTM 3.1x return code.  
	ERR_MULTIPLE_CLIENTS_CONNECTED  			= 156,  // The API was not able to reset the device. Multiple clients connected.  
	ERR_ADDRESS_NEVER_CLAIMED  					= 157,  // J1939 client never issued a protect address.  
	ERR_WINDOW_HANDLE_REQUIRED  				= 158,  // This was a legacy WindowsTM 3.1x return code.  
	ERR_MESSAGE_NOT_SENT  						= 159,  // Returned if blocking is used and the message was not sent.  
	ERR_MAX_NOTIFY_EXCEEDED  					= 160,  // Error Max Notify Exceeded.  
	ERR_MAX_FILTERS_EXCEEDED  					= 161,  // Error Max Filters Exeeded.  
	ERR_HARDWARE_STATUS_CHANGE  				= 162,  // Error Hardware Status Changed.  
	ERR_INVALID_PARAMETER  						= 193,  // The given parameter(s) is(are) invalid or can not be used.  
	ERR_INVALID_LICENCE  						= 194,  // The Licence could not be found, it has expired or is invalid.  
	ERR_INI_FILE_NOT_IN_WIN_DIR  				= 202,  // The file "RP1210.ini" or "PEAKRP32.ini" was not found.  
	ERR_INI_SECTION_NOT_FOUND  					= 204,  // The referenced section was not found within the ini file.  
	ERR_INI_KEY_NOT_FOUND  						= 205,  // The referenced key was not found within the ini file.  
	ERR_INVALID_KEY_STRING  					= 206,  // The given key is invalid.  
	ERR_DEVICE_NOT_SUPPORTED  					= 207,  // The referenced device is not supported.  
	ERR_INVALID_PORT_PARAM  					= 208,  // The given port number is invalid.  
	ERR_COMMAND_TIMED_OUT  						= 213,  // A time out occurred requesting the command.  
	ERR_OS_NOT_SUPPORTED  						= 220,  // The current operating system is not supported.  
	ERR_COMMAND_QUEUE_IS_FULL  					= 222,  // The command queue is full. No further co  
	ERR_CANNOT_SET_CAN_BAUDRATE  				= 224,  // The baud rate cannot be changed / Changing the baud rate is not supported.  
	ERR_CANNOT_CLAIM_BROADCAST_ADDRESS  		= 225,  // The address for broadcast messaging cannot be claimed.  
	ERR_OUT_OF_ADDRESS_RESOURCES  				= 226,  // There is not address available / all addresses are in use.  
	ERR_ADDRESS_RELEASE_FAILED  				= 227,  // Error while releasing a claimed address.  
	ERR_COMM_DEVICE_IN_USE  					= 230,  // The referenced communication port is in use.  
	ERR_DATA_LINK_CONFLICT  					= 441,  // There is a conflict within the data link.  
	ERR_ADAPTER_NOT_RESPONDING  				= 453,  // The adapter is not responding.  
	ERR_CAN_BAUD_SET_NONSTANDARD  				= 454,  // The baud rate for the CAN protocol is set to a value different than 250 kBit/s.  
	ERR_MULTIPLE_CONNECTIONS_NOT_ALLOWED_NOW  	= 455,  // The current connected client doesn't allow further clients to use the device at the same time.  
	ERR_J1708_BAUD_SET_NONSTANDARD  			= 456,  // The baud rate for the J1708 protocol is set to a value that is not the standard one.  
	ERR_J1939_BAUD_SET_NONSTANDARD  			= 457,  // The baud rate for the J1939 protocol is set to a value different than 250 kBit/s.  
	ERR_ISO15765_BAUD_SET_NONSTANDARD  			= 458,  // The baud rate for the ISO15765 protocol is set to a value that is not the standard one.  
	ERR_INVALID_IOCTL_ID  						= 600,  // The ID used for the IOCTL call is invalid.  
	ERR_NULL_PARAMETER  						= 601,  // The given parameter(s) cannot be NULL.  
	ERR_HARDWARE_NOT_SUPPORTED  				= 602,  // The referenced hardware is not supported.  
	ERR_INVALID_RESPONSE						= 700,  // Invalid Response Received
};

#pragma pack(push,1)
struct MsgHeader 
{ 
	MsgType type; 
	uint16_t clientId; 
	uint16_t length; 
};
#pragma pack(pop)