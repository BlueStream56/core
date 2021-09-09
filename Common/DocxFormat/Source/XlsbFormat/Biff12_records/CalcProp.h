﻿/*
 * (c) Copyright Ascensio System SIA 2010-2021
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-12 Ernesta Birznieka-Upisha
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#pragma once

#include <Logic/Biff_records/BiffRecord.h>
#include <Logic/Biff_structures/Xnum.h>
#include "../../XlsxFormat/WritingElement.h"
#include "../XlsbElementsType.h"
using namespace XLS;

namespace XLSB
{
    // Logical representation of BrtCalcProp record in BIFF12
    class CalcProp: public BiffRecord
    {
            BIFF_RECORD_DEFINE_TYPE_INFO(CalcProp)
            BASE_OBJECT_DEFINE_CLASS_NAME(CalcProp)
        public:
            CalcProp();
            virtual ~CalcProp();

            BaseObjectPtr clone();

            void readFields(CFRecord& record);

            enum calcMode {
                MANUAL       = 0x00000000,
                AUTOMATIC    = 0x00000001,
                AUTONOTABLE  = 0x00000002
            };

            _UINT32        recalcID;
            _UINT32        fAutoRecalc; //calcMode
            _UINT32        cCalcCount;
            Xnum           xnumDelta;
            _INT32         cUserThreadCount;
            bool           fFullCalcOnLoad;
            bool           fRefA1;
            bool           fIter;
            bool           fFullPrec;
            bool           fSomeUncalced;
            bool           fSaveRecalc;
            bool           fMTREnabled;
            bool           fUserSetThreadCount;
            bool           fNoDeps;


    };

} // namespace XLSB

