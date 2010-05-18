// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "MsgBrowserInfo.h"
#include "Local.h"
#include "ApLog.h"
#include "Context.h"

void Context::create() // throws ApException
{
  Msg_VpView_ContextCreated msg;
  msg.hContext = apHandle();
  msg.Send();
}

void Context::destroy() // throws ApException
{
  {
    Msg_BrowserInfo_EndTrackCoordinates msg;
    msg.hContext = apHandle();
    if (!msg.Request()) {
      throw ApException("Msg_BrowserInfo_BeginTrackCoordinates failed: %s", StringType(msg.sComment));
    }
  }

  {
    Msg_Vp_CloseContext msg;
    msg.hContext = apHandle();
    if (!msg.Request()) {
      throw ApException("Msg_Vp_CloseContext failed: %s", StringType(msg.sComment));
    }
  }

  {
    Msg_VpView_ContextDestroyed msg;
    msg.hContext = apHandle();
    msg.Send();
  }
}

//----------------------------------------------------------

void Context::navigate(const String& sUrl) // throws ApException
{
  Msg_Vp_NavigateContext msg;
  msg.hContext = apHandle();
  msg.sUrl = sUrl;
  if (!msg.Request()) {
    throw ApException("Msg_Vp_NavigateContext failed: %s", StringType(msg.sComment));
  }
}

void Context::nativeWindow(const String& sType, Apollo::KeyValueList& kvSignature) // throws ApException
{
  Msg_BrowserInfo_BeginTrackCoordinates msg;
  msg.hContext = apHandle();
  msg.sType = sType;
  msg.kvSignature = kvSignature;
  if (!msg.Request()) {
    throw ApException("Msg_BrowserInfo_BeginTrackCoordinates failed: %s", StringType(msg.sComment));
  }
}

void Context::show() // throws ApException
{
  Msg_VpView_ContextVisibility msg;
  msg.hContext = apHandle();
  msg.bVisible = 1;
  msg.Send();
}

void Context::hide() // throws ApException
{
  Msg_VpView_ContextVisibility msg;
  msg.hContext = apHandle();
  msg.bVisible = 0;
  msg.Send();
}

void Context::position(int nX, int nY) // throws ApException
{
  Msg_VpView_ContextPosition msg;
  msg.hContext = apHandle();
  msg.nX = nX;
  msg.nY = nY;
  msg.Send();
}

void Context::size(int nWidth, int nHeight) // throws ApException
{
  Msg_VpView_ContextSize msg;
  msg.hContext = apHandle();
  msg.nWidth = nWidth;
  msg.nHeight = nHeight;
  msg.Send();
}
