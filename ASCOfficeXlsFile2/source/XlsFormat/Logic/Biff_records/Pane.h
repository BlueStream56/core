#pragma once

#include "BiffRecord.h"
#include <Logic/Biff_structures/CellRef.h>
#include <Logic/Biff_structures/PaneType.h>

namespace XLS
{


// Logical representation of Pane record in BIFF8
class Pane: public BiffRecord
{
	BIFF_RECORD_DEFINE_TYPE_INFO(Pane)
	BASE_OBJECT_DEFINE_CLASS_NAME(Pane)
public:
	Pane();
	~Pane();

	BaseObjectPtr clone();

	void writeFields(CFRecord& record);
	void readFields(CFRecord& record);

	static const ElementType	type = typePane;

//-----------------------------
	BIFF_WORD	x;
	BIFF_WORD	y;
	R_RwU		rwTop;
	ColU		colLeft;
	PaneType	pnnAcct;
	
	BIFF_BSTR	topLeftCell;
};

} // namespace XLS

