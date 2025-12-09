#include "stdafx.h"

#include "dde.h"

using namespace dde;

// Add a new item.
PITEMINFO dde::ItemAdd(const TCHAR* lpszTopic, const TCHAR* lpszItem, LPWORD lpFormatList, PREQUESTFN lpReqFn,
                       PPOKEFN lpPokeFn) {
  PITEMINFO pItem = nullptr;
  PTOPICINFO pTopic = TopicFind(lpszTopic);

  if (!pTopic)  // We need to add this as a new topic
    pTopic = TopicAdd(lpszTopic, nullptr, nullptr, nullptr);

  if (!pTopic) return nullptr;  // failed

  pItem = ItemFind(pTopic, const_cast<LPSTR>(lpszItem));

  if (pItem) {  // Already have this item.  Just update the info in it.
    pItem->pfnRequest = lpReqFn;
    pItem->pfnPoke = lpPokeFn;
    pItem->pFormatList = lpFormatList;
  } else {  // Create a new item
    pItem = (PITEMINFO) new TCHAR[sizeof(ITEMINFO)];
    if (!pItem) return nullptr;

    ::ZeroMemory(pItem, sizeof(ITEMINFO));
    pItem->pszItemName = const_cast<LPSTR>(lpszItem);
    pItem->hszItemName = DdeCreateStringHandle(ServerInfo.dwInstance, lpszItem, CP_WINANSI);
    pItem->pTopic = pTopic;
    pItem->pfnRequest = lpReqFn;
    pItem->pfnPoke = lpPokeFn;
    pItem->pFormatList = lpFormatList;

    // Add it to the existing item list for this topic
    pItem->pNext = pTopic->pItemList;
    pTopic->pItemList = pItem;
  }
  return pItem;
}
///<summary>Find an item by its name in a topic </summary>
PITEMINFO dde::ItemFind(PTOPICINFO pTopic, const TCHAR* lpszItem) {
  PITEMINFO pItem = pTopic->pItemList;
  while (pItem) {
    if (lstrcmpi(pItem->pszItemName, lpszItem) == 0) break;

    pItem = pItem->pNext;
  }
  return pItem;
}
///<summary>Find an item by its HSZ in a topic </summary>
PITEMINFO dde::ItemFind(PTOPICINFO pTopic, HSZ hszItem) {
  PITEMINFO pItem = pTopic->pItemList;
  while (pItem) {
    if (DdeCmpStringHandles(pItem->hszItemName, hszItem) == 0) break;

    pItem = pItem->pNext;
  }
  return pItem;
}
// Remove an item from a topic.
bool dde::ItemRemove(LPSTR lpszTopic, LPSTR lpszItem) {
  PTOPICINFO pTopic = TopicFind(lpszTopic);
  PITEMINFO pItem, pPrevItem;

  // See if we have this topic
  if (!pTopic) return false;

  // Walk the topic item list looking for this item.

  pPrevItem = nullptr;
  pItem = pTopic->pItemList;
  while (pItem) {
    if (lstrcmpi(pItem->pszItemName, lpszItem) == 0)  // Found it.  Unlink it from the list.
    {
      if (pPrevItem)
        pPrevItem->pNext = pItem->pNext;
      else
        pTopic->pItemList = pItem->pNext;

      // Release its string handle
      DdeFreeStringHandle(ServerInfo.dwInstance, pItem->hszItemName);

      // Free the memory associated with it
      delete[] pItem;
      return true;
    }
    pPrevItem = pItem;
    pItem = pItem->pNext;
  }
  // We don't have that one
  return false;
}
