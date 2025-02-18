var DEFAULT_HOSTNAME = 'localhost';
var DEFAULT_PORT = 23765;
var MIN_RECONNECT_DELAY = 100;
var MAX_RECONNECT_DELAY = 10000;
var DEFAULT_LOGLEVEL = anLogLevelWarning;
var DEFAULT_POPUPSUPPORT = false;

var TYPE_CHROME = 'Chrome';

var sServerHostname = DEFAULT_HOSTNAME;
var nServerPort = DEFAULT_PORT;
var nReconnectDelay = MIN_RECONNECT_DELAY;
var nMinReconnectDelay = MIN_RECONNECT_DELAY;
var nMaxReconnectDelay = MAX_RECONNECT_DELAY;
var webSocket = null;
var srpcProtocol = null;

var aWindows = null;
var aTabs = null;

var anLogLevel = DEFAULT_LOGLEVEL;
var bTabCallbacksStarted = false;
var bWasConnected = false;
var bPopupSupport = DEFAULT_POPUPSUPPORT;

// --------------------------
// Window

function Window(windowId)
{
  this.id = windowId;
  this.selectedTabId = null;
}

function AddWindow(windowId)
{
  anLogTrace('AddWindow' + windowId);
  aWindows[windowId] = new Window(windowId);
}

function DeleteWindow(windowId)
{
  if (aWindows[windowId] != null) {
    anLogTrace('DeleteWindow' + windowId);
    delete aWindows[windowId];
  }
}

function GetWindow(windowId) { return aWindows[windowId]; }
function HasWindow(windowId) { return aWindows[windowId] != null; }

function SetWindowSelectedTab(windowId, tabId)
{
  var myWindow = GetWindow(windowId);
  if (myWindow != null) {
    var currentTabId = myWindow.selectedTabId;
    if (currentTabId != null) {
      OnHideTab(currentTabId);
    }
    OnShowTab(tabId);
    myWindow.selectedTabId = tabId;
  }
}

// --------------------------
// Tab

function Tab(tabId, windowId)
{
  this.id = tabId;
  this.windowId = windowId;
  this.sUrl = '';
  this.sContext = '';
  this.bVisible = false;
  this.bIsOpen = false;
}

function AddTab(tabId, windowId)
{
  anLogTrace('AddTab ' + tabId);
  aTabs[tabId] = new Tab(tabId, windowId);
}

function DeleteTab(tabId)
{
  OnCloseTab(tabId);
  if (aTabs[tabId] != null) {
    anLogTrace('DeleteTab ' + tabId);
    delete aTabs[tabId];
  }
}

function GetTab(tabId) { return aTabs[tabId]; }
function HasTab(tabId) { return aTabs[tabId] != null; }

// --------------------------
// hContext

function RequestTabContext(tabId)
{
  var fCallback = function (response)
  {
    var myTab = GetTab(tabId);
    if (myTab != null) {
      if (response.getInt('Status') == 1) {
        var hContext = response.getString('hResult');
        if (hContext != '') {
          anLogVerbose('TabContext tab=' + tabId + ' ctxt=' + hContext);
          myTab.sContext = hContext;

          if (IsValidUrl(myTab.sUrl)) {
            NavigateContext(myTab.sContext, myTab.sUrl);
            myTab.bIsOpen = true;
            IdentifyTab(tabId);
          }

        }
      }
    }
  }

  var myTab = GetTab(tabId);
  if (myTab != null) {
    if (srpcProtocol != null) {
      srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'System_GetHandle'), fCallback);
    } else {
      anLogWarning('RequestTabContext ignored(not-connected)');
    }
  }
}

function IsValidUrl(sUrl)
{
  if (sUrl.indexOf('http://') == 0 || sUrl.indexOf('https://') == 0) {
    return true;
  }
  return false;
}

function IdentifyTab(tabId)
{
  anLogTrace('IdentifyTab' + tabId);

  var myTab = GetTab(tabId);
  if (myTab != null) {
    chrome.windows.get(myTab.windowId, function (win)
    {
      if (srpcProtocol != null) {

        var msg = new SrpcMessage();
        msg.setString('Method', 'Navigation_ContextNativeWindow');
        msg.setString('hContext', myTab.sContext);

        var selectedTab = null;
        var aTabs = win.tabs;
        if (aTabs != null) {
          for (var i = 0; i < aTabs.length; ++i) {
            var tab = aTabs[i];
            if (tab.selected) {
              selectedTab = tab;
            }
          }
        }
        var sTitle = '';
        if (selectedTab != null) {
          sTitle = selectedTab.title;
        }

        var kvSignature = new SrpcMessage();
        kvSignature.setString('sType', TYPE_CHROME);
        kvSignature.setString('sVersion', navigator.userAgent);
        if (sTitle != '') {
          kvSignature.setString('sTitle', sTitle);
        }
        kvSignature.setString('nLeft', win.left);
        kvSignature.setString('nTop', win.top);
        kvSignature.setString('nWidth', win.width);
        kvSignature.setString('nHeight', win.height);
        msg.setString('kvSignature', kvSignature.toString());

        srpcProtocol.sendRequest(msg);

        if (myTab.bVisible) {
          ShowContext(myTab.sContext);
        } else {
          HideContext(myTab.sContext);
        }

      } else {
        anLogWarning('IdentifyTab ignored: not-connected ' + tabId);
      }
    });
  }
}

// --------------------------
// Core tab events

function OnOpenTab(tabId)
{
  anLogVerbose('OnOpenTab ' + tabId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    //OpenContext(myTab.sContext);
  }
}

function OnShowTab(tabId)
{
  anLogVerbose('OnShowTab ' + tabId);
  var bVisible = true;
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var bOldVisible = myTab.bVisible;
    if (bOldVisible != bVisible) {
      myTab.bVisible = bVisible;
      if (myTab.sContext != '') {
        ShowContext(myTab.sContext);
      }
    }
  }
}

function OnHideTab(tabId)
{
  anLogVerbose('OnHideTab ' + tabId);
  var bVisible = false;
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var bOldVisible = myTab.bVisible;
    if (bOldVisible != bVisible) {
      myTab.bVisible = bVisible;
      if (myTab.sContext != '') {
        HideContext(myTab.sContext);
      }
    }
  }
}

function OnNavigateTab(tabId, sUrl)
{
  anLogVerbose('OnNavigateTab ' + tabId + ' ' + sUrl);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var sOldUrl = myTab.sUrl;
    if (sOldUrl != sUrl) {
      myTab.sUrl = sUrl;
      if (myTab.sContext != '') {

        if (IsValidUrl(sUrl)) {
          NavigateContext(myTab.sContext, sUrl);
          if (!myTab.bIsOpen) {
            myTab.bIsOpen = true;
            IdentifyTab(tabId);
          }
        } else {
          if (myTab.bIsOpen) {
            myTab.bIsOpen = false;
            CloseContext(myTab.sContext);
          }
        }

      }
    }
  }
}

function OnReparentTab(tabId, newWindowId)
{
  anLogVerbose('OnReparentTab tab=' + tabId + ' to win=' + newWindowId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    if (myTab.windowId != newWindowId) {
      myTab.windowId = newWindowId;
      IdentifyTab(tabId);
    }
  }
}

function OnCloseTab(tabId)
{
  anLogVerbose('OnCloseTab ' + tabId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    if (myTab.sContext != '') {
      if (myTab.bIsOpen) {
        myTab.bIsOpen = false;
        CloseContext(myTab.sContext);
      }
    }
  }
}

// --------------------------
// Context

function OpenContext(sContext)
{
  anLogTrace('OpenContext ' + sContext);
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextOpen').setString('hContext', sContext));
  } else {
    anLogWarning('OpenContext ignored(not-connected) ' + sContext);
  }
}

function ShowContext(sContext)
{
  anLogTrace('ShowContext ' + sContext);
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextShow').setString('hContext', sContext));
  } else {
    anLogWarning('ShowContext ignored(not-connected) ' + sContext);
  }
}

function HideContext(sContext)
{
  anLogTrace('HideContext ' + sContext);
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextHide').setString('hContext', sContext));
  } else {
    anLogWarning('HideContext ignored(not-connected) ' + sContext);
  }
}

function NavigateContext(sContext, sUrl)
{
  anLogTrace('NavigateContext ' + sContext + ' ' + sUrl)
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextNavigate').setString('hContext', sContext).setString('sUrl', sUrl));
  } else {
    anLogWarning('NavigateContext ignored(not-connected) ' + sContext);
  }
}

function CloseContext(sContext)
{
  anLogTrace('CloseContext ' + sContext)
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextClose').setString('hContext', sContext));
  } else {
    anLogWarning('CloseContext ignored(not-connected) ' + sContext);
  }
}

// --------------------------
// chrome.tab and chrome.window hooks

function OnWindowGetSelectedTab(tab)
{
  anLogTrace('OnWindowGetSelectedTab ' + tab.id);
  SetWindowSelectedTab(tab.windowId, tab.id);
}

function OnWindowGetAllResult(windows)
{
  anLogTrace('OnWindowGetAllResult');
  var nWindows = windows.length;
  for (var i = 0; i < nWindows; i++) {
    var win = windows[i];
    OnWindowCreated(win);
    var nTabs = win.tabs.length;
    for (var j = 0; j < nTabs; j++) {
      var tab = win.tabs[j];
      OnTabCreated(tab);
      OnNavigateTab(tab.id, tab.url);
      if (tab.selected) {
        OnShowTab(tab.id);
      } else {
        OnHideTab(tab.id);
      }
    }
    chrome.tabs.getSelected(win.id, OnWindowGetSelectedTab);
  }
}

function OnWindowCreated(win)
{
  anLogTrace('OnWindowCreated ' + win.id + ' left=' + win.left + ' top=' + win.top + ' width=' + win.width + ' height=' + win.height + ' ');
  if (win.type == 'normal' || (win.type == 'popup' && bPopupSupport)) {
    AddWindow(win.id, win);
  }
}

function OnWindowRemoved(windowId)
{
  anLogTrace('OnWindowRemoved ' + windowId);
  DeleteWindow(windowId);
}

function OnTabCreated(tab)
{
  anLogTrace('OnTabCreated');
  var tabId = tab.id;
  if (!HasTab(tabId)) {
    AddTab(tabId, tab.windowId, tab);
    RequestTabContext(tabId);
  }
}

function OnTabRemoved(tabId, removeInfo)
{
  anLogTrace('OnTabRemoved ' + tabId);
  DeleteTab(tabId);
}

function OnTabUpdated(tabId, changeInfo, tab)
{
  anLogTrace('OnTabUpdated');
  if (HasTab(tabId) && tab.url != null) {
    OnNavigateTab(tabId, tab.url);
  }
}

function OnTabSelectionChanged(tabId, selectInfo)
{
  anLogTrace('OnTabSelectionChanged');
  SetWindowSelectedTab(selectInfo.windowId, tabId);
}

function OnTabDetached(tabId, detachInfo)
{
  anLogTrace('OnTabDetached');
  var myTab = GetTab(tabId);
  if (myTab != null) {
//     if (myTab.bVisible) {
//       myTab.bAttachVisible = true;
//       OnHideTab(tabId);
//     }
  }
}

function OnTabAttached(tabId, attachInfo)
{
  anLogTrace('OnTabAttached');
  var myTab = GetTab(tabId);
  if (myTab != null) {
    OnReparentTab(tabId, attachInfo.newWindowId);
//     if (myTab.bAttachVisible) {
//       myTab.bAttachVisible = false;
//       OnShowTab(tabId);
//     }
  }
}

// --------------------------
// Start

function StartTabs()
{
  anLogTrace('StartTabs');
  aWindows = new Object();
  aTabs = new Object();

  chrome.windows.getAll({ 'populate': true }, OnWindowGetAllResult);

  chrome.windows.onCreated.addListener(OnWindowCreated);
  chrome.windows.onRemoved.addListener(OnWindowRemoved);
  chrome.tabs.onCreated.addListener(OnTabCreated);
  chrome.tabs.onRemoved.addListener(OnTabRemoved);
  chrome.tabs.onUpdated.addListener(OnTabUpdated);
  chrome.tabs.onSelectionChanged.addListener(OnTabSelectionChanged);
  chrome.tabs.onAttached.addListener(OnTabAttached);
  chrome.tabs.onDetached.addListener(OnTabDetached);

  bTabCallbacksStarted = true;
}

function StopTabs()
{
  anLogTrace('StopTabs');
  if (bTabCallbacksStarted) {
    chrome.windows.onCreated.removeListener(OnWindowCreated);
    chrome.windows.onRemoved.removeListener(OnWindowRemoved);
    chrome.tabs.onCreated.removeListener(OnTabCreated);
    chrome.tabs.onRemoved.removeListener(OnTabRemoved);
    chrome.tabs.onUpdated.removeListener(OnTabUpdated);
    chrome.tabs.onSelectionChanged.removeListener(OnTabSelectionChanged);
    chrome.tabs.onAttached.removeListener(OnTabAttached);
    chrome.tabs.onDetached.removeListener(OnTabDetached);

    bTabCallbacksStarted = false;
  }
}

function SetDefaultOption(sName, sDefaultValue)
{
  var sValue = localStorage[sName];
  if (sValue == null || sValue == '') {
    localStorage[sName] = sDefaultValue;
  }
}

function SetDefaultOptions()
{
  anLogTrace('SetDefaultOptions');
  SetDefaultOption('ServerHostname', DEFAULT_HOSTNAME);
  SetDefaultOption('ServerPort', DEFAULT_PORT);
  SetDefaultOption('MinReconnectDelay', MIN_RECONNECT_DELAY);
  SetDefaultOption('MaxReconnectDelay', MAX_RECONNECT_DELAY);
  SetDefaultOption('LogLevel', anLogLevelWarning);
  SetDefaultOption('PopupSupport', DEFAULT_POPUPSUPPORT);
}

function GetOption(sName)
{
  return localStorage[sName];
}

function GetOptions()
{
  anLogTrace('GetOptions');
  sServerHostname = GetOption('ServerHostname');
  nServerPort = GetOption('ServerPort');
  nMinReconnectDelay = GetOption('MinReconnectDelay');
  nMaxReconnectDelay = GetOption('MaxReconnectDelay');
  anLogLevel = GetOption('LogLevel');
  var sPopupSupport = GetOption('PopupSupport');
  if (sPopupSupport == 'true') { bPopupSupport = true; }
  if (sPopupSupport == 'false') { bPopupSupport = false; }
}

function Start()
{
  anLogTrace('Start');
  SetDefaultOptions();
  GetOptions();
  StartReconnectTimer();
}

function StartReconnectTimer()
{
  nReconnectDelay *= 2;
  if (nReconnectDelay > nMaxReconnectDelay) {
    nReconnectDelay = nMaxReconnectDelay;
  }
  anLogVerbose('StartReconnectTimer: scheduling connect in  ' + nReconnectDelay + ' msec');
  window.setTimeout('Reconnect();', nReconnectDelay);
}

// --------------------------
// Connection

function OnReceiveMessage(msg)
{
  var sMethod = msg.getString('Method');
  anLogVerbose('OnReceiveMessage Method=' + sMethod);
}

function OnSendData(sData)
{
  anLogVerbose('OUT: ' + sData);
  webSocket.send(sData);
}

function OnHelloResponse(msg)
{
  anLogTrace('OnHelloResponse');
  StartTabs();
}

function OnWebSocketOpened(evt)
{
  anLogInfo('Connected');
  nReconnectDelay = nMinReconnectDelay;
  bWasConnected = true;
  
  srpcProtocol = new SrpcProtocol();
  srpcProtocol.onReceiveMessage = OnReceiveMessage;
  srpcProtocol.onSendData = OnSendData;

  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_NavigatorHello'), OnHelloResponse);
  }
}

function OnWebSocketMesssge(evt)
{
  var sData  = evt.data;
  anLogVerbose('IN: ' + sData);
  srpcProtocol.handleMessage(sData);
}

function OnWebSocketClosed(evt)
{
  if (bWasConnected) {
    anLogInfo('Connection closed');
  } else {
    anLogTrace('OnWebSocketClosed');
  }

  bWasConnected = false;
  webSocket = null;
  srpcProtocol = null;

  StopTabs();

  StartReconnectTimer();
}

function Reconnect()
{
  var sUrl = 'ws://' + sServerHostname + ':' + nServerPort;
  anLogInfo('Connecting to ' + sUrl);
  webSocket = new WebSocket(sUrl);
  webSocket.onopen = OnWebSocketOpened;
  webSocket.onmessage = OnWebSocketMesssge;
  webSocket.onclose = OnWebSocketClosed;
}

// --------------------------

function ShowChatWindow()
{
  //alert('ShowChatWindow');
//   if (srpcProtocol != null) {
//     var msg = new SrpcMessage();
//     msg.setString('ApType', 'Navigator_CallDisplay');
//     msg.setString('Method', 'ShowChat');
//     msg.setString('hContext', hContext);
//     msg.setInt('bShow', 1);
//     srpcProtocol.sendRequest(msg);
//   }
}

function ReconnectNavigation()
{
  //alert('ReconnectNavigation');
  //webSocket.close();
}

Start();
