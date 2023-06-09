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

// 78
namespace NSCustomShapesConvert
{
class CRightArrowCalloutType : public CCustomShape
{
public:
	CRightArrowCalloutType()

    {
        m_bConcentricFill = false;
        m_eJoin = ODRAW::lineJoinMiter;
        m_strPath = _T("m,l,21600@0,21600@0@5@2@5@2@4,21600,10800@2@1@2@3@0@3@0,x"); 

        AddGuide(_T("val #0"));     
        AddGuide(_T("val #1"));     
        AddGuide(_T("val #2"));     
        AddGuide(_T("val #3"));     
        AddGuide(_T("sum 21600 0 #1"));     
        AddGuide(_T("sum 21600 0 #3"));     
        AddGuide(_T("prod #0 1 2"));

		m_arAdjustments.push_back(14400);
		m_arAdjustments.push_back(5400);
		m_arAdjustments.push_back(18000);
		m_arAdjustments.push_back(8100);

        LoadConnectorsList(_T("@6,0;0,10800;@6,21600;21600,10800"));
        
		m_arConnectorAngles.push_back(270);
		m_arConnectorAngles.push_back(180);
		m_arConnectorAngles.push_back(90);
		m_arConnectorAngles.push_back(0);

        LoadTextRect(_T("0,0,@0,21600"));

        CHandle_ oHandle1;
        oHandle1.position = _T("#0,topLeft");
        oHandle1.xrange = _T("0,@2");
        m_arHandles.push_back(oHandle1);

        CHandle_ oHandle2;
        oHandle2.position = _T("bottomRight,#1");
        oHandle2.yrange = _T("0,@3");
        m_arHandles.push_back(oHandle2);

        CHandle_ oHandle3;
        oHandle3.position = _T("#2,#3");
        oHandle3.xrange = _T("@0,21600");
        oHandle3.yrange = _T("@1,10800");
        m_arHandles.push_back(oHandle3);
    }
};
}


