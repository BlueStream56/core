﻿/*
 * (c) Copyright Ascensio System SIA 2010-2019
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
#include "../CustomShape.h"

// 7
namespace NSCustomShapesConvert
{
class CParallelogramType : public CCustomShape
{
public:
	CParallelogramType()

    {
        m_bConcentricFill = true;
        m_eJoin = ODRAW::lineJoinMiter;

        m_strPath = _T("m@0,l,21600@1,21600,21600,xe");

        AddGuide(_T("val #0"));
        AddGuide(_T("sum width 0 #0"));
        AddGuide(_T("prod #0 1 2"));
        AddGuide(_T("sum width 0 @2"));
        AddGuide(_T("mid #0 width"));
        AddGuide(_T("mid @1 0"));
        AddGuide(_T("prod height width #0"));
        AddGuide(_T("prod @6 1 2"));
        AddGuide(_T("sum height 0 @7"));
        AddGuide(_T("prod width 1 2"));
        AddGuide(_T("sum #0 0 @9"));
        AddGuide(_T("if @10 @8 0"));
        AddGuide(_T("if @10 @7 height"));

		m_arAdjustments.push_back(5400);
        
        LoadConnectorsList(_T("@4,0;10800,@11;@3,10800;@5,21600;10800,@12;@2,10800"));
        LoadTextRect(_T("1800,1800,19800,19800;8100,8100,13500,13500;10800,10800,10800,10800"));

        CHandle_ oHandle1;
        oHandle1.position = _T("#0,topLeft");
        oHandle1.xrange = _T("0,21600");
        m_arHandles.push_back(oHandle1);
	}
};
}