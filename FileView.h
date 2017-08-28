
#pragma once

#include "ViewTree.h"

struct TreeItem;

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// Construction
public:
	CFileView();

	void AdjustLayout();

// Attributes
protected:
    ULONG m_Notify;
    LPITEMIDLIST m_pRootPidl;
    CComPtr<IMalloc> m_Malloc;
	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;

protected:
	void FillFileView();
    void InsertChildren(HTREEITEM hNode, TreeItem* ti);
    void InsertChildren(CComPtr<IShellFolder>& Folder, HTREEITEM hParent);
    HTREEITEM FindItem(HTREEITEM hParentItem, PCITEMID_CHILD pidls);
    HTREEITEM FindItem(PCIDLIST_RELATIVE pidls, BOOL bExpandChildren);
    HTREEITEM FindParentItem(PCIDLIST_RELATIVE pidls);
    HTREEITEM InsertChild(HTREEITEM hParent, CComPtr<IShellFolder>& folder, PITEMID_CHILD ItemId);
    HTREEITEM FindSortedPos(HTREEITEM hParent, const TreeItem* tir);

    void OnDeleteItem(PCIDLIST_RELATIVE pidls);
    void OnRenameItem(PCIDLIST_RELATIVE pidls, PCIDLIST_RELATIVE new_pidls);
    void OnAddItem(PCIDLIST_RELATIVE pidls);
    void OnUpdateItem(PCIDLIST_RELATIVE pidls);

// Implementation
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
    afx_msg void OnSync();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
    afx_msg void OnEditRename();
    afx_msg void OnEditView();
    afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnItemExpanding(NMHDR* pHdr, LRESULT* pResult);
    afx_msg void OnDeleteItem(NMHDR* pHdr, LRESULT* pResult);
    afx_msg void OnBeginLabelEdit(NMHDR* pHdr, LRESULT* pResult);
    afx_msg void OnEndLabelEdit(NMHDR* pHdr, LRESULT* pResult);
    afx_msg void OnDblClick(NMHDR* pHdr, LRESULT* pResult);
    afx_msg LRESULT OnShellChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

