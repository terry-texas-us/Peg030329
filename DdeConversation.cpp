#include "stdafx.h"

#include "Dde.h"

using namespace dde;

/// @brief Add a conversation to our list.
bool dde::ConversationAdd(HCONV hConv, HSZ hszTopic) {
  auto pCI = (PCONVERSATIONINFO) new char[sizeof(CONVERSATIONINFO)];
  if (!pCI) return false;

  ::ZeroMemory(pCI, sizeof(CONVERSATIONINFO));
  pCI->hConv = hConv;
  pCI->hszTopicName = hszTopic;
  pCI->pNext = ServerInfo.pConvList;
  ServerInfo.pConvList = pCI;
  return true;
}
/// @brief Find the first occurence of a topic conversation in our list
PCONVERSATIONINFO dde::ConversationFind(HSZ hszTopic) {
  PCONVERSATIONINFO pCI = ServerInfo.pConvList;

  while (pCI)  // Try to find the info in the list
  {
    if (DdeCmpStringHandles(pCI->hszTopicName, hszTopic) == 0) return pCI;

    pCI = pCI->pNext;
  }
  return nullptr;
}
/// @brief Remove a conversation from our list.
bool dde::ConversationRemove(HCONV hConv, HSZ hszTopic) {
  PCONVERSATIONINFO pCI = ServerInfo.pConvList;
  PCONVERSATIONINFO pPrevCI = nullptr;

  // Try to find the info in the list
  while (pCI) {
    if ((pCI->hConv == hConv) &&
        (DdeCmpStringHandles(pCI->hszTopicName, hszTopic) == 0)) {  // Found it. Unlink it from the list
      if (pPrevCI)
        pPrevCI->pNext = pCI->pNext;
      else
        ServerInfo.pConvList = pCI->pNext;
      delete[] pCI;
      return true;
    }
    pPrevCI = pCI;
    pCI = pCI->pNext;
  }
  return false;  // Not in the list
}
