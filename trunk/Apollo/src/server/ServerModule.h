// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ServerModule_H_INCLUDED)
#define ServerModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"

class HttpConnection;
class HttpServer;
class TcpConnection;
class TcpServer;

typedef ApHandlePointerTree<HttpConnection*> HttpConnectionList;
typedef ApHandlePointerTreeNode<HttpConnection*> HttpConnectionNode;
typedef ApHandlePointerTreeIterator<HttpConnection*> HttpConnectionIterator;

typedef ApHandlePointerTree<TcpConnection*> TcpConnectionList;
typedef ApHandlePointerTreeNode<TcpConnection*> TcpConnectionNode;
typedef ApHandlePointerTreeIterator<TcpConnection*> TcpConnectionIterator;

class ServerModule
{
public:
  ServerModule()
    :pHttpServer_(0)
    ,pTcpServer_(0)
  {}

  int init();
  void exit();

  int addHttpConnection(const ApHandle& hConnection, HttpConnection* pConnection);
  int removeHttpConnection(const ApHandle& hConnection);
  HttpConnection* findHttpConnection(const ApHandle& hConnection);
  int addTcpConnection(const ApHandle& hConnection, TcpConnection* pConnection);
  int removeTcpConnection(const ApHandle& hConnection);
  TcpConnection* findTcpConnection(const ApHandle& hConnection);

  void On_Server_StartHTTP(Msg_Server_StartHTTP* pMsg);
  void On_Server_StopHTTP(Msg_Server_StopHTTP* pMsg);
  void On_HttpServer_Request(Msg_HttpServer_Request* pMsg);
  void On_HttpServer_SendResponse(Msg_HttpServer_SendResponse* pMsg);

  void On_Server_StartTCP(Msg_Server_StartTCP* pMsg);
  void On_Server_StopTCP(Msg_Server_StopTCP* pMsg);
  void On_TcpServer_Connected(Msg_TcpServer_Connected* pMsg);
  void On_TcpServer_SrpcRequest(Msg_TcpServer_SrpcRequest* pMsg);
  void On_TcpServer_SendSrpc(Msg_TcpServer_SendSrpc* pMsg);
  void On_TcpServer_Disconnected(Msg_TcpServer_Disconnected* pMsg);

  void On_System_RunLevel(Msg_System_RunLevel* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  HttpServer* pHttpServer_;
  HttpConnectionList httpConnections_;
  String sHttpAddress_;
  int nHttpPort_;

  TcpServer* pTcpServer_;
  TcpConnectionList tcpConnections_;
  String sTcpAddress_;
  int nTcpPort_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ServerModule> ServerModuleInstance;

#endif // ServerModule_H_INCLUDED
