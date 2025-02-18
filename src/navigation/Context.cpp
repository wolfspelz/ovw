// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "MsgBrowserInfo.h"
#include "Context.h"

void Context::create() // throws ApException
{
  {
    Msg_Vp_OpenContext msg;
    msg.hContext = apHandle();
    if (!msg.Request()) {
      throw ApException(LOG_CONTEXT, "Msg_Vp_OpenContext failed: %s", _sz(msg.sComment));
    }
  }

  {
    Msg_VpView_ContextCreated msg;
    msg.hContext = apHandle();
    msg.Send();
  }
}

void Context::destroy() // throws ApException
{
  if (bTrackingCoordinates_) {
    Msg_BrowserInfo_EndTrackCoordinates msg;
    msg.hContext = apHandle();
    if (!msg.Request()) {
      throw ApException(LOG_CONTEXT, "Msg_BrowserInfo_BeginTrackCoordinates failed: %s", _sz(msg.sComment));
    }
    bTrackingCoordinates_ = 0;
  }

  {
    Msg_Vp_CloseContext msg;
    msg.hContext = apHandle();
    if (!msg.Request()) {
      throw ApException(LOG_CONTEXT, "Msg_Vp_CloseContext failed: %s", _sz(msg.sComment));
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
    throw ApException(LOG_CONTEXT, "Msg_Vp_NavigateContext failed: %s", _sz(msg.sComment));
  }
}

void Context::nativeWindow(Apollo::KeyValueList& kvSignature) // throws ApException
{
  if (bTrackingCoordinates_) {
    Msg_BrowserInfo_EndTrackCoordinates msg;
    msg.hContext = apHandle();
    if (!msg.Request()) {
      throw ApException(LOG_CONTEXT, "Msg_BrowserInfo_BeginTrackCoordinates failed: %s", _sz(msg.sComment));
    }
    bTrackingCoordinates_ = 0;
  }

  {
    Msg_BrowserInfo_BeginTrackCoordinates msg;
    msg.hContext = apHandle();
    msg.kvSignature = kvSignature;
    if (!msg.Request()) {
      throw ApException(LOG_CONTEXT, "Msg_BrowserInfo_BeginTrackCoordinates failed: %s", _sz(msg.sComment));
    } else {
      bTrackingCoordinates_ = 1;
    }
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

void Context::position(int nLeft, int nBottom) // throws ApException
{
  Msg_VpView_ContextPosition msg;
  msg.hContext = apHandle();
  msg.nLeft = nLeft;
  msg.nBottom = nBottom;
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
