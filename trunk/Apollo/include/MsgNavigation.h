// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgNavigation_h_INCLUDED)
#define MsgNavigation_h_INCLUDED

#include "ApMessage.h"

// navigation module -> navigator via TCP connection
class Msg_Navigation_Send: public ApRequestMessage
{
public:
  Msg_Navigation_Send() : ApRequestMessage("Navigation_Send") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// navigator via TCP connection -> navigation module
class Msg_Navigation_Receive: public ApRequestMessage
{
public:
  Msg_Navigation_Receive() : ApRequestMessage("Navigation_Receive") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// navigation module -> navigation module
class Msg_Navigation_Connected: public ApNotificationMessage
{
public:
  Msg_Navigation_Connected() : ApNotificationMessage("Navigation_Connected") {}
  ApIN ApHandle hConnection;
};

// navigation module -> navigation module
class Msg_Navigation_Disconnected: public ApNotificationMessage
{
public:
  Msg_Navigation_Disconnected() : ApNotificationMessage("Navigation_Disconnected") {}
  ApIN ApHandle hConnection;
};

// -------------------------------

// SRPC via TCP navigator protocol

#define Navigation_SrpcMethod_GetHandle "System.GetHandle"
// out: h

#define Navigation_SrpcMethod_Connect "Connection.Connect"

#define Navigation_SrpcMethod_Navigate "Context.Navigate"
// in: hContext
// in: sUrl;

#define Navigation_SrpcMethod_Attach "Context.Attach"
// in: hContext

#define Navigation_SrpcMethod_Open "Context.Open"
// in: hContext

#define Navigation_SrpcMethod_Close "Context.Close"
// in: hContext

#define Navigation_SrpcMethod_NativeWindow "Context.NativeWindow"
// in: hContext
// in: sType
  #define Navigation_SrpcMethod_NativeWindow_Type_Firefox "Firefox"
  #define Navigation_SrpcMethod_NativeWindow_Type_InternetExplorer "InternetExplorer"
// in: kvSignature
  #define Navigation_SrpcMethod_NativeWindow_Signature_Title "sTitle"
  #define Navigation_SrpcMethod_NativeWindow_Signature_Left "nLeft"
  #define Navigation_SrpcMethod_NativeWindow_Signature_Top "nTop"
  #define Navigation_SrpcMethod_NativeWindow_Signature_Width "nWidth"
  #define Navigation_SrpcMethod_NativeWindow_Signature_Height "nHeight"

#define Navigation_SrpcMethod_Show "Context.Show"
// in: hContext

#define Navigation_SrpcMethod_Hide "Context.Hide"
// in: hContext

#define Navigation_SrpcMethod_Position "Context.Position"
// in: hContext
// in: nLeft
// in: nBottom

#define Navigation_SrpcMethod_Size "Context.Size"
// in: hContext
// in: nWidth
// in: nHeight

#endif // !defined(MsgNavigation_h_INCLUDED)
