#include "pch.h"
#include "TypesTreeNode.h"
#include "SortHelper.h"
#include "FormatHelper.h"

static const struct {
	PCWSTR header;
	int width;
	int format = LVCFMT_LEFT;
} columns[] = {
	{ L"Name", 300 },
	{ L"Module", 120, LVCFMT_RIGHT },
	{ L"Token", 80, LVCFMT_RIGHT },
	{ L"Size", 60, LVCFMT_RIGHT },
	{ L"Base Type", 250 },
	{ L"Interfaces", 70, LVCFMT_RIGHT },
	{ L"Methods", 60, LVCFMT_RIGHT },
	{ L"Virtuals", 60, LVCFMT_RIGHT },
	{ L"VTable Slots", 60, LVCFMT_RIGHT },
	{ L"Static Fields", 70, LVCFMT_RIGHT },
	{ L"Instance Fields", 70, LVCFMT_RIGHT },
	{ L"Thread Local Fields", 70, LVCFMT_RIGHT },
	{ L"Details", 200 },
};

TypesTreeNode::TypesTreeNode(CTreeItem item, DataTarget* dt, CLRDATA_ADDRESS module) : TreeNodeBase(item), _dt(dt), _module(module) {
}

int TypesTreeNode::GetColumnCount() const {
	return _countof(columns);
}

CString TypesTreeNode::GetColumnInfo(int column, int& width, int& format) const {
	auto& info = columns[column];
	width = info.width;
	format = info.format;
	return info.header;
}

int TypesTreeNode::GetRowCount() {
	return (int)_items.size();
}

CString TypesTreeNode::GetColumnText(int row, int col) const {
	auto& item = _items[row];
	CString text;

	switch (col) {
		case 0: return item.Name;
		case 1: text.Format(L"0x%llX", item.Module); break;
		case 2: text.Format(L"0x%X", item.cl); break;
		case 3: text.Format(L"%u\n", item.BaseSize); break;
		case 4: return item.BaseName;
		case 5: text.Format(L"%d\n", (int)item.wNumInterfaces); break;
		case 6: text.Format(L"%d\n", (int)item.wNumMethods); break;
		case 7: text.Format(L"%d\n", (int)item.wNumVirtuals); break;
		case 8: text.Format(L"%d\n", (int)item.wNumVtableSlots); break;
		case 9: text.Format(L"%d\n", (int)item.FieldData.wNumStaticFields); break;
		case 10: text.Format(L"%d\n", (int)item.FieldData.wNumInstanceFields); break;
		case 11: text.Format(L"%d\n", (int)item.FieldData.wNumThreadStaticFields); break;
		case 12: return FormatHelper::TypeAttributesToString(item.dwAttrClass);
	}
	return text;
}

bool TypesTreeNode::InitList() {
	CWaitCursor wait;
	_items = _module == 0 ? _dt->EnumMethodTables() : _dt->EnumMethodTables(_module);
	return true;
}

void TypesTreeNode::TermList() {
	_items.clear();
	_items.shrink_to_fit();
}

void TypesTreeNode::SortList(int col, bool asc) {
	std::sort(_items.begin(), _items.end(), [&](const auto& t1, const auto& t2) {
		switch (col) {
			case 0: return SortHelper::SortStrings(t1.Name, t2.Name, asc);
			case 1: return SortHelper::SortNumbers(t1.Module, t2.Module, asc);
			case 2: return SortHelper::SortNumbers(t1.cl, t2.cl, asc);
			case 3: return SortHelper::SortNumbers(t1.BaseSize, t2.BaseSize, asc);
			case 4: return SortHelper::SortStrings(t1.BaseName, t2.BaseName, asc);
			case 5: return SortHelper::SortNumbers(t1.wNumInterfaces, t2.wNumInterfaces, asc);
			case 6: return SortHelper::SortNumbers(t1.wNumMethods, t2.wNumMethods, asc);
			case 7: return SortHelper::SortNumbers(t1.wNumVirtuals, t2.wNumVirtuals, asc);
			case 8: return SortHelper::SortNumbers(t1.wNumVtableSlots, t2.wNumVtableSlots, asc);
			case 9: return SortHelper::SortNumbers(t1.FieldData.wNumStaticFields, t2.FieldData.wNumStaticFields, asc);
			case 10: return SortHelper::SortNumbers(t1.FieldData.wNumInstanceFields, t2.FieldData.wNumInstanceFields, asc);
			case 11: return SortHelper::SortNumbers(t1.FieldData.wNumThreadStaticFields, t2.FieldData.wNumThreadStaticFields, asc);
			case 12: return SortHelper::SortNumbers(t1.dwAttrClass, t2.dwAttrClass, asc);
		}
		return false;
		});
}

int TypesTreeNode::GetRowIcon(int row) const {
	auto& item = _items[row];
	if (item.bIsFree)
		return 13;
	if (item.dwAttrClass & tdInterface)
		return 18;
	if (item.Kind == ManagedTypeKind::Delegate)
		return 17;
	if (item.Kind == ManagedTypeKind::Struct)
		return 13;
	if (item.dwAttrClass & tdAbstract)
		return 15;

	return 12;
}
