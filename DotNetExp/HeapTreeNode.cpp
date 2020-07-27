#include "pch.h"
#include "HeapTreeNode.h"
#include "SortHelper.h"

static const struct {
	PCWSTR header;
	int width;
	int format = LVCFMT_LEFT;
} columns[] = {
	{ L"Type Name", 420 },
	{ L"Method Table", 120, LVCFMT_RIGHT },
	{ L"Object Count", 100, LVCFMT_RIGHT },
	{ L"Total Size", 80, LVCFMT_RIGHT },
};

HeapTreeNode::HeapTreeNode(CTreeItem item, DataTarget* dt, int heap) : TreeNodeBase(item), _dt(dt), _heap(heap) {
}

int HeapTreeNode::GetColumnCount() const {
	return _countof(columns);
}

CString HeapTreeNode::GetColumnInfo(int column, int& width, int& format) const {
	auto& info = columns[column];
	width = info.width;
	format = info.format;
	return info.header;
}

int HeapTreeNode::GetRowCount() {
	return (int)_items.size();
}

CString HeapTreeNode::GetColumnText(int row, int col) const {
	auto& item = _items[row];
	CString text;

	switch (col) {
		case 0: return item.TypeName;
		case 1: text.Format(L"0x%llX", item.MethodTable); break;
		case 2: text.Format(L"%u", item.ObjectCount); break;
		case 3: text.Format(L"%lld", item.TotalSize); break;
	}
	return text;
}

bool HeapTreeNode::InitList() {
	_items = _dt->GetHeapStats(_heap);
	return true;
}

void HeapTreeNode::TermList() {
	_items.clear();
}

void HeapTreeNode::SortList(int col, bool asc) {
	std::sort(_items.begin(), _items.end(), [&](const auto& t1, const auto& t2) {
		switch (col) {
			case 0: return SortHelper::SortStrings(t1.TypeName, t2.TypeName, asc);
			case 1: return SortHelper::SortNumbers(t1.MethodTable, t2.MethodTable, asc);
			case 2: return SortHelper::SortNumbers(t1.ObjectCount, t2.ObjectCount, asc);
			case 3: return SortHelper::SortNumbers(t1.TotalSize, t2.TotalSize, asc);
		}
		return false;
		});
}

int HeapTreeNode::GetRowIcon(int row) const {
	auto& item = _items[row];
	switch (item.Type) {
		case OBJ_STRING: return 14;
		case OBJ_ARRAY: return 19;
		case OBJ_FREE: return 20;
	}
	if (_dt->GetMethodTableInfo(item.MethodTable).BaseName == L"System.ValueType")
		return 13;

	return 12;
}
