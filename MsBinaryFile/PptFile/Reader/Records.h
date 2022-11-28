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

#include "PPTFileDefines.h"
#include "../Enums/RecordType.h"
#include "../Reader/ReadStructures.h"
#include "../../Common/Vml/PPTShape/Enums.h"
#include "../../XlsFile/Format/Binary/CFStream.h"
#include "../../../Common/3dParty/pole/pole.h"
#include "../../../OfficeCryptReader/source/CryptTransform.h"
#include "../../../OOXML/Base/Nullable.h"


#include <boost/smart_ptr/shared_array.hpp>

//#include <execinfo.h>

//using namespace PPT_FORMAT;
using namespace XLS;

using NSCommon::nullable;

std::string GetRecordName(PPT_FORMAT::RecordType dwType);

class SRecordHeader 
{ 
public:
    unsigned char           RecVersion;
    unsigned short          RecInstance;
    PPT_FORMAT::RecordType	RecType;
    _UINT32                 RecLen;

    bool bBadHeader;

    void Clear();
    SRecordHeader();
    bool ReadFromStream(const CFStreamPtr &pStream);

    bool ReadFromStream(POLE::Stream * pStream);

    bool IsContainer();

    SRecordHeader& operator =(const SRecordHeader& oSrc);
};

class IRecord
{
public:
    SRecordHeader m_oHeader;

    virtual ~IRecord(){}
    virtual void ReadFromStream(SRecordHeader & oHeader, const CFStreamPtr &pStream) = 0;
    virtual void ReadFromStream(SRecordHeader & oHeader, POLE::Stream* pStream) = 0;
};

class CUnknownRecord : public IRecord
{
    // этот класс - просто для того, чтобы нигде не проверять,
    // реализована ли у нас такая запись

public:
    CUnknownRecord();

    ~CUnknownRecord();
    virtual void ReadFromStream(SRecordHeader & oHeader, const CFStreamPtr &pStream);
    virtual void ReadFromStream(SRecordHeader & oHeader, POLE::Stream* pStream);

    std::wstring ReadStringW(const CFStreamPtr &pStream, int size);
    std::string	 ReadStringA(const CFStreamPtr &pStream, int size);
};

IRecord* CreateByType(SRecordHeader oHeader);

class CRecordsContainer : public CUnknownRecord
{
public:
    std::vector<IRecord*> m_arRecords;

    CRecordsContainer();

    virtual ~CRecordsContainer();

    void Clear();

    virtual void ReadFromStream(SRecordHeader & oHeader, const CFStreamPtr &pStream);
    virtual void ReadFromStream(SRecordHeader & oHeader, POLE::Stream* pStream);

    template <typename T>
    void GetRecordsByType(std::vector<T>* pArray, bool bIsChild, bool bOnlyFirst = false) const
    {
        if (NULL == pArray)
            return;

        // возвращаем указатели, их не удалять наверху!!!!
        for (size_t nIndex = 0; nIndex < m_arRecords.size(); ++nIndex)
        {
            T pRec = dynamic_cast<T>(m_arRecords[nIndex]);
            if (NULL != pRec)
            {
                pArray->push_back(pRec);
                if (bOnlyFirst)
                {
                    return;
                }
            }
            else if ((bIsChild) && (m_arRecords[nIndex]->m_oHeader.IsContainer()))
            {
                CRecordsContainer* pContainer = dynamic_cast<CRecordsContainer*>(m_arRecords[nIndex]);
                if (NULL != pContainer)
                {
                    pContainer->GetRecordsByType(pArray, bIsChild, bOnlyFirst);
                }
            }
        }
    }
};

/**************************************************************
    контейнеры... (по идее они не нужны, потом можно убрать)
**************************************************************/

/*
class CRecordDocument					: public CRecordsContainer {};
class CRecordExAviMovie					: public CRecordsContainer {};
class CRecordExCDAudio					: public CRecordsContainer {};
class CRecordExControl					: public CRecordsContainer {};
class CRecordExEmbed					: public CRecordsContainer {};
class CRecordExHyperlink				: public CRecordsContainer {};
class CRecordExHyperlink9				: public CRecordsContainer {};
class CRecordExLink						: public CRecordsContainer {};
class CRecordExMCIMovie					: public CRecordsContainer {};
class CRecordExMIDIAudio				: public CRecordsContainer {};
class CRecordExObjList					: public CRecordsContainer {};
class CRecordExOleObjStg				: public CRecordsContainer {};
class CRecordExQuickTimeMovie			: public CRecordsContainer {};
class CRecordExVideo					: public CRecordsContainer {};
class CRecordExWAVAudioEmbedded			: public CRecordsContainer {};
class CRecordExWAVAudioLink				: public CRecordsContainer {};
class CRecordFontCollection10			: public CRecordsContainer {};
class CRecordHandout					: public CRecordsContainer {};
class CRecordHeadersFooters				: public CRecordsContainer {};
class CRecordHTMLPublishInfo			: public CRecordsContainer {};
class CRecordInteractiveInfo			: public CRecordsContainer {};
class CRecordList						: public CRecordsContainer {};
class CRecordMainMaster					: public CRecordsContainer {};
class CRecordMsofbtClientData			: public CRecordsContainer {};
class CRecordNamedShow					: public CRecordsContainer {};
class CRecordNamedShows					: public CRecordsContainer {};
class CRecordNotes						: public CRecordsContainer {};
class CRecordNotesTextViewInfo			: public CRecordsContainer {};
class CRecordOutlineTextProps9			: public CRecordsContainer {};
class CRecordOutlineTextProps10			: public CRecordsContainer {};
class CRecordOutlineTextProps11			: public CRecordsContainer {};
class CRecordOutlineViewInfo			: public CRecordsContainer {};
class CRecordParaBuild					: public CRecordsContainer {};
class CRecordProgBinaryTag				: public CRecordsContainer {};
class CRecordProgStringTag				: public CRecordsContainer {};
class CRecordProgTags					: public CRecordsContainer {};
class CRecordSlide						: public CRecordsContainer {};
class CRecordSlideListTable10			: public CRecordsContainer {};
class CRecordSlideListWithText			: public CRecordsContainer {};
class CRecordSlideSyncInfo12			: public CRecordsContainer {};
class CRecordSlideViewInfo				: public CRecordsContainer {};
class CRecordSorterViewInfo				: public CRecordsContainer {};
class CRecordSound						: public CRecordsContainer {};
class CRecordSoundCollection			: public CRecordsContainer {};
class CRecordSrKinsoku					: public CRecordsContainer {};
class CRecordSummary					: public CRecordsContainer {};
class CRecordVBAInfo					: public CRecordsContainer {};
*/

/********************************************************************************
        создаем по типу (остальные будем добавлять по мере необходимости)
********************************************************************************/
