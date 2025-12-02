#pragma once

#include <Windows.h>

#include <afx.h>

class CPrim;

/**
 * @brief Fills the extra list control with data from the given primitive.
 *
 * Parses the formatted extra string from the primitive and populates the list view with property-value pairs.
 *
 * @param hDlg Handle to the dialog box.
 * @param pPrim Pointer to the CPrim object.
 */
void DlgProcEditTrap_CommandsQueryFillExtraList(HWND hDlg, CPrim* pPrim);

/**
 * @brief Dialog procedure for the Edit Trap Commands Query dialog.
 *
 * Handles initialization, notifications, and commands for the dialog, including setting up tree view and list controls,
 * responding to tree selection changes, and filling lists with primitive data.
 *
 * @param hDlg Handle to the dialog box.
 * @param nMsg Message identifier.
 * @param wParam Additional message information.
 * @param lParam Additional message information.
 * @return BOOL TRUE if the message was processed, FALSE otherwise.
 */
BOOL CALLBACK DlgProcEditTrap_CommandsQuery(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam);

/**
 * @brief Fills the geometry list control with data from the given primitive.
 *
 * Parses the formatted geometry string from the primitive and populates the list view with property and axis values.
 *
 * @param hDlg Handle to the dialog box.
 * @param pPrim Pointer to the CPrim object.
 */
void DlgProcEditTrap_CommandsQueryFillGeometryList(HWND hDlg, CPrim* pPrim);

/**
 * @brief Adds an item to a tree view control.
 *
 * This function inserts a new item into the specified tree view with the given text and associated object.
 *
 * @param hTree Handle to the tree view control.
 * @param hParent Handle to the parent item. Use TVI_ROOT for root level.
 * @param pszText Text to display for the item.
 * @param pOb Pointer to the associated CObject.
 * @return HTREEITEM Handle to the newly inserted item.
 */
HTREEITEM tvAddItem(HWND hTree, HTREEITEM hParent, LPCTSTR pszText, CObject* pOb);
