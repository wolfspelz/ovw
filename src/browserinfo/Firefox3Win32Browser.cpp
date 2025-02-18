// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "apLog.h"
#include "Local.h"
#include "MsgBrowserInfo.h"
#include "Win32Window.h"
#include "Firefox3Win32Browser.h"

void Firefox3Win32Browser::OnTimer()
{
  HWND hWnd = win_;

  if (hWnd != NULL) {
    HWND hContent = NULL;

    // Firefox before 3.6.8?
    //ChildClassPathFinder ccpf(hWnd, "MozillaWindowClass/MozillaWindowClass/MozillaWindowClass/MozillaContentWindowClass");
    //ccpf.Run();
    //// Assuming tabs are stacked and at the same position
    //// Find grandparent and check valid size (invisible tabs have size 0x0)
    //// Then take the grandparent as content rect
    //for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
    //  RECT rContent;
    //  HWND hParent = ::GetParent(pNode->Value());
    //  HWND hGrandParent = ::GetParent(hParent);
    //  ::GetWindowRect(hGrandParent, &rContent);
    //  if (rContent.right - rContent.left != 0 && rContent.bottom - rContent.top) {
    //    hContent = hGrandParent;
    //    break;
    //  }
    //}

    // Firefox 3.6.8
    ChildClassPathFinder ccpf(hWnd, "MozillaWindowClass/MozillaContentWindowClass/MozillaWindowClass");
    ccpf.Run();
    // Assuming tabs are stacked and at the same position
    // Find parent and check valid size (invisible tabs have size 0x0)
    // Then take the parent as content rect
    for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
      RECT rContent;
      HWND hParent = ::GetParent(pNode->Key());
      ::GetWindowRect(hParent, &rContent);
      if (rContent.right - rContent.left != 0 && rContent.bottom - rContent.top != 0) {
        hContent = hParent;
        break;
      }
    }

    RECT rect;
    ::GetWindowRect(hContent, &rect);

    int bVisible = 1;
    int nLeft = rect.left;
    int nBottom = rect.bottom - 1;
    int nWidth = rect.right - rect.left;
    int nHeight = rect.bottom - rect.top;

    if (rect.top < -30000) {
      bVisible = 0;
    }

    AdjustPosition(bVisible, nLeft, nBottom, nWidth, nHeight);
    AdjustStackingOrder();

  } // hWnd != NULL
}
