#include "./../include/OOXMLSignerBuffer.h"
#include "./../src/XmlTransform.h"
#include "./../../../../OfficeUtils/js/wasm/src/base.h"
#include "./../../../../OfficeUtils/js/wasm/src/engine.h"
#include <cstdio>
#include <ctime>
#include <time.h>
#include <vector>

class CFile
{
public:
    std::wstring m_sPath;
    BYTE* m_pData;
    DWORD m_nLength;
public:
    CFile(const std::wstring& sPath, BYTE* pData, DWORD nLength) : m_sPath(sPath), m_pData(pData), m_nLength(nLength) {}

    std::wstring GetFileExtention()
    {
        std::wstring::size_type nPos = m_sPath.rfind('.');
        if (nPos != std::wstring::npos)
            return m_sPath.substr(nPos + 1);
        return m_sPath;
    }
};

class CFileManagerBuffer
{
    vector<CFile> arrFiles;

public:
    bool addFile(const std::wstring& sPath, BYTE* pData, DWORD nLength)
    {
        if (!ExistFile(sPath))
        {
            arrFiles.push_back(CFile(sPath, pData, nLength));
            return true;
        }
        return false;
    }

    bool removeFile(const std::wstring& sPath)
    {
        for (std::vector<CFile>::iterator i = arrFiles.begin(); i != arrFiles.end(); i++)
        {
            if (i->m_sPath == sPath)
            {
                arrFiles.erase(i);
                return true;
            }
        }
        return false;
    }

    bool ExistFile(const std::wstring& sPath)
    {
        for (CFile& oFile : arrFiles)
            if (oFile.m_sPath == sPath)
                return true;
        return false;
    }

    bool ExistDirectory(const std::wstring& sDirectory)
    {
        for (CFile& oFile : arrFiles)
            if (oFile.m_sPath.find(sDirectory) == 0)
                return true;
        return false;
    }

    std::vector<CFile> GetFiles(const std::wstring& sDirectory, bool bIsRecursion)
    {
        std::vector<CFile> oRes;

        for (CFile& oFile : arrFiles)
        {
            if (bIsRecursion)
            {
                if (oFile.m_sPath.find(sDirectory) == 0)
                    oRes.push_back(oFile);
            }
            else
            {
                size_t nFindDirectory = oFile.m_sPath.find(sDirectory);
                if (nFindDirectory == 0 && oFile.m_sPath.find_first_of(L"\\/", nFindDirectory) == std::wstring::npos)
                    oRes.push_back(oFile);
            }
        }

        return oRes;
    }

    CFile* GetFile(const std::wstring& sPath)
    {
        for (CFile& oFile : arrFiles)
            if (oFile.m_sPath == sPath)
                return &oFile;
        return NULL;
    }
};

class COOXMLSignerBuffer_private
{
public:
    ICertificate*                           m_certificate;
    CData*                                  m_file;
    CFileManagerBuffer                      m_file_manager;

    std::wstring                            m_date;

    std::map<std::wstring, std::wstring>    m_content_types;
    std::vector<std::wstring>               m_rels;
    std::vector<std::wstring>               m_files;

    NSStringUtils::CStringBuilderA          m_signed_info;

    std::wstring                            m_image_valid;
    std::wstring                            m_image_invalid;

    std::wstring                            m_guid;

private:
    unsigned int GetLength(BYTE* x)
    {
        return x[0] | x[1] << 8 | x[2] << 16 | x[3] << 24;
    }

public:
    COOXMLSignerBuffer_private(BYTE* file, ICertificate* pContext)
    {
        m_file = new CData(file, GetLength(file));

        Zlib* zlib = Zlib_Load(file + 4, GetLength(file));
        BYTE* sPaths = Zlib_GetPathsInArchive(zlib);
        unsigned int nLength = GetLength(sPaths);
        unsigned int i = 4;
        nLength -= 4;

        while (i < nLength)
        {
            unsigned int nPathLength = GetLength(sPaths + i);
            i += 4;

            BYTE* sFile = Zlib_GetFileFromArchive(zlib, std::string((char*)(sPaths + i), nPathLength).c_str());
            m_file_manager.addFile(NSFile::CUtf8Converter::GetUnicodeStringFromUTF8(sPaths + i, nPathLength), sFile + 4, GetLength(sFile));
            i += nPathLength;
        }
        RELEASEARRAYOBJECTS(sPaths);

        m_certificate = pContext;

        m_date = L"2017-04-21T08:30:21Z";

        std::time_t rawtime;
        std::tm* timeinfo;
        char buffer1[100];
        char buffer2[100];

        std::time(&rawtime);
        timeinfo = std::gmtime(&rawtime);

        std::strftime(buffer1, 100, "%Y-%m-%d", timeinfo);
        std::strftime(buffer2, 100, "%H:%M:%S", timeinfo);

        std::string date = (std::string(buffer1) + "T" + std::string(buffer2) + "Z");
        m_date = NSFile::CUtf8Converter::GetUnicodeFromCharPtr(date);

        m_signed_info.WriteString("<CanonicalizationMethod Algorithm=\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\"/>");
        m_signed_info.WriteString("<SignatureMethod Algorithm=\"");
        m_signed_info.WriteString(ICertificate::GetSignatureMethodA(m_certificate->GetHashAlg()));
        m_signed_info.WriteString("\"/>");
    }
    ~COOXMLSignerBuffer_private() {}

    std::wstring GetReference(const std::wstring& file, const std::wstring& content_type)
    {
        std::wstring sXml = L"<Reference URI=\"" + file + L"?ContentType=" + content_type + L"\">";
        sXml += (L"<DigestMethod Algorithm=\"" + ICertificate::GetDigestMethod(m_certificate->GetHashAlg()) + L"\"/>");
        sXml += L"<DigestValue>";
        CFile* oFile = m_file_manager.GetFile(file);
        std::string sTmp = oFile ? m_certificate->GetHash(oFile->m_pData, oFile->m_nLength, m_certificate->GetHashAlg()) : "";
        sXml += UTF8_TO_U(sTmp);
        sXml += L"</DigestValue>";
        sXml += L"</Reference>";
        return sXml;
    }

    std::string GetHashXml(const std::wstring& xml)
    {
        std::string sXmlSigned = U_TO_UTF8(xml);
        sXmlSigned = XmlUtils::NSXmlCanonicalizator::Execute(sXmlSigned, XmlUtils::XML_C14N_1_0);
        return m_certificate->GetHash(sXmlSigned, m_certificate->GetHashAlg());
    }

    std::string GetReferenceMain(const std::wstring& xml, const std::wstring& id, const bool& isCannon = true)
    {
        std::wstring sXml1 = L"<Object xmlns=\"http://www.w3.org/2000/09/xmldsig#\"";
        if (id.empty())
            sXml1 += L">";
        else
            sXml1 += (L" Id=\"" + id + L"\">");
        sXml1 += xml;
        sXml1 += L"</Object>";

        std::string sHash = GetHashXml(sXml1);

        std::string sRet;
        if (isCannon)
            sRet = "<Transforms><Transform Algorithm=\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\"/></Transforms>";

        sRet += ("<DigestMethod Algorithm=\"" + ICertificate::GetDigestMethodA(m_certificate->GetHashAlg()) + "\"/><DigestValue>" + sHash + "</DigestValue>");

        return sRet;
    }

    std::wstring GetImageBase64(BYTE* file)
    {
        /*
        if (0 == file.find(L"data:image/"))
        {
            return file.substr(file.find(L",") + 1);
        }
        */

        DWORD dwLen = GetLength(file);
        BYTE* pData = file + 4;

        char* pDataC = NULL;
        int nLen = 0;
        NSFile::CBase64Converter::Encode(pData, (int)dwLen, pDataC, nLen, NSBase64::B64_BASE64_FLAG_NOCRLF);

        std::wstring sReturn = NSFile::CUtf8Converter::GetUnicodeFromCharPtr(pDataC, (LONG)nLen, FALSE);

        RELEASEARRAYOBJECTS(pData);
        RELEASEARRAYOBJECTS(pDataC);

        return sReturn;
    }

    std::wstring GetRelsReference(const std::wstring& file)
    {
        CFile* oFile = m_file_manager.GetFile(file);
        if (!oFile)
            return L"";
        COOXMLRelationships oRels(NSFile::CUtf8Converter::GetUnicodeStringFromUTF8(oFile->m_pData, oFile->m_nLength), false);
        if (oRels.rels.size() == 0)
            return L"";

        if (L"/_rels/.rels" == file)
        {
            long nLength;
            oRels.CheckOriginSigs(oFile->m_pData, nLength);
            oFile->m_nLength = nLength;

            // удалим все лишнее
            std::vector<COOXMLRelationship>::iterator i = oRels.rels.begin();
            while (i != oRels.rels.end())
            {
                if (0 == i->target.find(L"docProps/"))
                    i = oRels.rels.erase(i);
                else if (0 == i->target.find(L"_xmlsignatures/"))
                    i = oRels.rels.erase(i);
                else
                    i++;
            }
        }

        NSStringUtils::CStringBuilder builder;
        builder.WriteString(L"<Reference URI=\"");
        builder.WriteString(file);
        builder.WriteString(L"?ContentType=application/vnd.openxmlformats-package.relationships+xml\">");
        builder.WriteString(oRels.GetTransforms());
        builder.WriteString(L"<DigestMethod Algorithm=\"" + ICertificate::GetDigestMethod(m_certificate->GetHashAlg()) + L"\"/><DigestValue>");

        std::wstring sXml = oRels.GetXml();
        std::string sHash = GetHashXml(sXml);

        std::wstring sHashW = UTF8_TO_U(sHash);
        builder.WriteString(sHashW);

        builder.WriteString(L"</DigestValue></Reference>");

        return builder.GetData();
    }

    int GetCountSigns(const std::wstring& folder)
    {
        std::wstring sRelsFolder = folder + L"/_rels";

        std::vector<CFile> arFiles = m_file_manager.GetFiles(sRelsFolder, false);
        std::map<std::wstring, bool> arSigFiles;

        for (std::vector<CFile>::iterator iter = arFiles.begin(); iter != arFiles.end(); iter++)
        {
            XmlUtils::CXmlNode oNodeRels;
            if (oNodeRels.FromXmlString(NSFile::CUtf8Converter::GetUnicodeStringFromUTF8(iter->m_pData, iter->m_nLength)))
                continue;
            XmlUtils::CXmlNodes oNodesRels = oNodeRels.GetNodes(L"Relationship");
            int nCount = oNodesRels.GetCount();
            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
                XmlUtils::CXmlNode oNodeRel;
                oNodesRels.GetAt(nIndex, oNodeRel);

                std::wstring sTarget = oNodeRel.GetAttribute(L"Target");
                if (!sTarget.empty() && arSigFiles.find(sTarget) == arSigFiles.end() && NSFile::CFileBinary::Exists(folder + L"/" + sTarget))
                    arSigFiles.insert(std::pair<std::wstring, bool>(sTarget, true));
            }
            m_file_manager.removeFile(iter->m_sPath);
        }

        int nCountSigs = (int)arSigFiles.size();
        std::wstring sFile = sRelsFolder + L"/origin.sigs.rels";

        NSStringUtils::CStringBuilder oBuilder;
        oBuilder.WriteString(L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");

        for (int nIndex = 0; nIndex <= nCountSigs/*old + one new*/; nIndex++)
        {
            oBuilder.WriteString(L"<Relationship Id=\"rId");
            oBuilder.WriteString(std::to_wstring(nIndex + 1));
            oBuilder.WriteString(L"\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/digital-signature/signature\" Target=\"sig");
            oBuilder.WriteString(std::to_wstring(nIndex + 1));
            oBuilder.WriteString(L".xml\"/>");
        }

        oBuilder.WriteString(L"</Relationships>");

        NSFile::CFileBinary::Remove(sFile);
        NSFile::CFileBinary oFile;
        oFile.CreateFileW(sFile);
        oFile.WriteStringUTF8(oBuilder.GetData());
        oFile.CloseFile();

        // теперь перебьем все имена файлов

        std::vector<std::wstring> arSigs;

        std::vector<std::wstring> arFilesXml = NSDirectory::GetFiles(folder, false);
        for (std::vector<std::wstring>::iterator iter = arFilesXml.begin(); iter != arFilesXml.end(); iter++)
        {
            std::wstring sXmlFileName = NSFile::GetFileName(*iter);
            if (NSFile::GetFileExtention(sXmlFileName) != L"xml")
                continue;

            std::map<std::wstring, bool>::iterator find = arSigFiles.find(sXmlFileName);
            if (find == arSigFiles.end())
            {
                // ненужная xml
                NSFile::CFileBinary::Remove(*iter);
                continue;
            }

            arSigs.push_back(sXmlFileName);
        }

        std::sort(arSigs.begin(), arSigs.end());
        for (std::vector<std::wstring>::iterator iter = arSigs.begin(); iter != arSigs.end(); iter++)
        {
            NSFile::CFileBinary::Move(folder + L"/" + *iter, folder + L"/onlyoffice_" + *iter);
        }
        int nSigNumber = 1;
        for (std::vector<std::wstring>::iterator iter = arSigs.begin(); iter != arSigs.end(); iter++)
        {
            NSFile::CFileBinary::Move(folder + L"/onlyoffice_" + *iter, folder + L"/sig" + std::to_wstring(nSigNumber++) + L".xml");
        }

        return (int)arSigs.size();
    }

    void ParseContentTypes()
    {
        CFile* file = m_file_manager.GetFile(L"[Content_Types].xml");
        XmlUtils::CXmlNode oNode;
        oNode.FromXmlString(NSFile::CUtf8Converter::GetUnicodeStringFromUTF8(file->m_pData, file->m_nLength));

        XmlUtils::CXmlNodes nodesDefaults;
        oNode.GetNodes(L"Default", nodesDefaults);

        XmlUtils::CXmlNodes nodesOverrides;
        oNode.GetNodes(L"Override", nodesOverrides);

        int nCount = nodesDefaults.GetCount();
        for (int i = 0; i < nCount; ++i)
        {
            XmlUtils::CXmlNode node;
            nodesDefaults.GetAt(i, node);

            m_content_types.insert(std::pair<std::wstring, std::wstring>(node.GetAttribute("Extension"), node.GetAttribute("ContentType")));
        }

        nCount = nodesOverrides.GetCount();
        for (int i = 0; i < nCount; ++i)
        {
            XmlUtils::CXmlNode node;
            nodesOverrides.GetAt(i, node);

            m_content_types.insert(std::pair<std::wstring, std::wstring>(node.GetAttribute("PartName"), node.GetAttribute("ContentType")));
        }
    }

    void Parse()
    {
        // 1) Parse Content_Types.xml
        ParseContentTypes();

        // 2) Parse files in directory
        std::vector<CFile> files = m_file_manager.GetFiles(L"", true);

        // 3) Check each file
        for (std::vector<CFile>::iterator i = files.begin(); i != files.end(); i++)
        {
            std::wstring sCheckFile = i->m_sPath;

            // check needed file
            if (0 == sCheckFile.find(L"_xmlsignatures") ||
                0 == sCheckFile.find(L"docProps") ||
                0 == sCheckFile.find(L"[Content_Types].xml") ||
                0 == sCheckFile.find(L"[trash]"))
                continue;

            // check rels and add to needed array
            std::wstring::size_type posExt = sCheckFile.rfind(L".");
            if (std::wstring::npos == posExt)
                continue;

            std::wstring sExt = sCheckFile.substr(posExt + 1);
            if (sExt == L"rels")
                m_rels.push_back(sCheckFile);
            else
                m_files.push_back(sCheckFile);
        }

        std::sort(m_rels.begin(), m_rels.end());
        std::sort(m_files.begin(), m_files.end());
    }

    void WriteRelsReferences(NSStringUtils::CStringBuilder& builder)
    {
        for (std::vector<std::wstring>::iterator i = m_rels.begin(); i != m_rels.end(); i++)
        {
            builder.WriteString(GetRelsReference(*i));
        }
    }

    void WriteFilesReferences(NSStringUtils::CStringBuilder& builder)
    {
        for (std::vector<std::wstring>::iterator i = m_files.begin(); i != m_files.end(); i++)
        {
            std::wstring sFile = *i;
            std::wstring sContentType = L"application/xml";

            std::map<std::wstring, std::wstring>::iterator _find = m_content_types.find(sFile);
            if (_find != m_content_types.end())
            {
                sContentType = _find->second;
            }
            else
            {
                std::wstring::size_type posExt = sFile.rfind(L".");
                if (std::wstring::npos != posExt)
                {
                    std::wstring sExt = sFile.substr(posExt + 1);

                    _find = m_content_types.find(sExt);
                    if (_find != m_content_types.end())
                        sContentType = _find->second;
                }
            }

            builder.WriteString(GetReference(sFile, sContentType));
        }
    }

    void WriteManifest(NSStringUtils::CStringBuilder& builder)
    {
        builder.WriteString(L"<Manifest>");
        WriteRelsReferences(builder);
        WriteFilesReferences(builder);
        builder.WriteString(L"</Manifest>");
    }

    void CorrectContentTypes(int nCountSignatures)
    {
        CFile* file = m_file_manager.GetFile(L"[Content_Types].xml");
        XmlUtils::CXmlNode oNode;
        oNode.FromXmlString(NSFile::CUtf8Converter::GetUnicodeStringFromUTF8(file->m_pData, file->m_nLength));

        NSStringUtils::CStringBuilder oBuilder;
        oBuilder.WriteString(L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");

        int nAttributesCount = oNode.GetAttributesCount();
        oBuilder.WriteNodeBegin(oNode.GetName(), nAttributesCount != 0);

        std::vector<std::wstring> attrNames;
        std::vector<std::wstring> attrValues;
        oNode.GetAllAttributes(attrNames, attrValues);

        for (int nAttrIndex = 0; nAttrIndex < nAttributesCount; ++nAttrIndex)
        {
            oBuilder.WriteAttributeEncodeXml(attrNames[nAttrIndex], attrValues[nAttrIndex]);
        }

        oBuilder.WriteNodeEnd(oNode.GetName(), true, false);

        XmlUtils::CXmlNodes oNodes = oNode.GetNodes(L"*");

        int nCountNodes = oNodes.GetCount();
        for (int nIndex = 0; nIndex < nCountNodes; ++nIndex)
        {
            XmlUtils::CXmlNode oCurrentRecord;
            oNodes.GetAt(nIndex, oCurrentRecord);

            if (L"Default" == oCurrentRecord.GetName() && oCurrentRecord.GetAttributeA("Extension") == "sigs")
                continue;
            if (L"Override" == oCurrentRecord.GetName() && oCurrentRecord.GetAttributeA("ContentType") == "application/vnd.openxmlformats-package.digital-signature-xmlsignature+xml")
                continue;

            oBuilder.WriteString(oCurrentRecord.GetXml());
        }

        if (0 != nCountSignatures)
            oBuilder.WriteString(L"<Default Extension=\"sigs\" ContentType=\"application/vnd.openxmlformats-package.digital-signature-origin\"/>");

        for (int nIndex = 0; nIndex < nCountSignatures; ++nIndex)
        {
            oBuilder.WriteString(L"<Override PartName=\"/_xmlsignatures/sig");
            oBuilder.WriteString(std::to_wstring(nIndex + 1));
            oBuilder.WriteString(L".xml\" ContentType=\"application/vnd.openxmlformats-package.digital-signature-xmlsignature+xml\"/>");
        }

        oBuilder.WriteNodeEnd(oNode.GetName());

        m_file_manager.removeFile(L"[Content_Types].xml");
        BYTE* pData; LONG nLength;
        NSFile::CUtf8Converter::GetUtf8StringFromUnicode(oBuilder.GetData().c_str(), oBuilder.GetSize(), pData, nLength);
        m_file_manager.addFile(L"[Content_Types].xml", pData, nLength);
    }

    void SetGuid(const std::wstring& guid)
    {
        m_guid = guid;
    }
    void SetImageValid(BYTE* file)
    {
        m_image_valid = GetImageBase64(file);
    }
    void SetImageInvalid(BYTE* file)
    {
        m_image_invalid = GetImageBase64(file);
    }

    std::wstring GeneratePackageObject()
    {
        NSStringUtils::CStringBuilder builder;
        WriteManifest(builder);

        builder.WriteString(L"<SignatureProperties><SignatureProperty Id=\"idSignatureTime\" Target=\"#idPackageSignature\">");
        builder.WriteString(L"<mdssi:SignatureTime xmlns:mdssi=\"http://schemas.openxmlformats.org/package/2006/digital-signature\">");
        builder.WriteString(L"<mdssi:Format>YYYY-MM-DDThh:mm:ssTZD</mdssi:Format>");
        builder.WriteString(L"<mdssi:Value>");
        builder.WriteString(m_date);
        builder.WriteString(L"</mdssi:Value>");
        builder.WriteString(L"</mdssi:SignatureTime></SignatureProperty></SignatureProperties>");

        std::wstring sXml = builder.GetData();

        m_signed_info.WriteString("<Reference Type=\"http://www.w3.org/2000/09/xmldsig#Object\" URI=\"#idPackageObject\">");
        m_signed_info.WriteString(GetReferenceMain(sXml, L"idPackageObject", false));
        m_signed_info.WriteString("</Reference>");

        return (L"<Object Id=\"idPackageObject\">" + sXml + L"</Object>");
    }
    std::wstring GenerateOfficeObject()
    {
        NSStringUtils::CStringBuilder builder;

        builder.WriteString(L"<SignatureProperties><SignatureProperty Id=\"idOfficeV1Details\" Target=\"#idPackageSignature\">");
        builder.WriteString(L"<SignatureInfoV1 xmlns=\"http://schemas.microsoft.com/office/2006/digsig\">");
        builder.WriteString(L"<SetupID>");

        std::wstring sGUID = m_guid;
        if (0 == sGUID.find(L"/_xmlsignatures"))
            sGUID = L"";

        builder.WriteString(sGUID);
        builder.WriteString(L"</SetupID>");
        builder.WriteString(L"<SignatureText></SignatureText>");
        builder.WriteString(L"<SignatureImage>");
        builder.WriteString(m_image_valid);
        builder.WriteString(L"</SignatureImage>");

        if (!m_certificate->IsGOST())
        {
            builder.WriteString(L"<SignatureComments/>\
<WindowsVersion>10.0</WindowsVersion>\
<OfficeVersion>16.0</OfficeVersion>\
<ApplicationVersion>16.0</ApplicationVersion>\
<Monitors>2</Monitors>\
<HorizontalResolution>1680</HorizontalResolution>\
<VerticalResolution>1050</VerticalResolution>\
<ColorDepth>32</ColorDepth>\
<SignatureProviderId>{00000000-0000-0000-0000-000000000000}</SignatureProviderId>\
<SignatureProviderUrl/>\
<SignatureProviderDetails>9</SignatureProviderDetails>\
<SignatureType>2</SignatureType>\
</SignatureInfoV1>\
</SignatureProperty>\
</SignatureProperties>");
        }
        else
        {
            builder.WriteString(L"<SignatureComments/>\
<WindowsVersion>10.0</WindowsVersion>\
<OfficeVersion>16.0</OfficeVersion>\
<ApplicationVersion>16.0</ApplicationVersion>\
<Monitors>2</Monitors>\
<HorizontalResolution>1680</HorizontalResolution>\
<VerticalResolution>1050</VerticalResolution>\
<ColorDepth>32</ColorDepth>\
<SignatureProviderId>{F5AC7D23-DA04-45F5-ABCB-38CE7A982553}</SignatureProviderId>\
<SignatureProviderUrl>http://www.cryptopro.ru/products/office/signature</SignatureProviderUrl>\
<SignatureProviderDetails>8</SignatureProviderDetails>\
<SignatureType>1</SignatureType>\
</SignatureInfoV1>\
</SignatureProperty>\
</SignatureProperties>");
        }

        m_signed_info.WriteString("<Reference Type=\"http://www.w3.org/2000/09/xmldsig#Object\" URI=\"#idOfficeObject\">");
        m_signed_info.WriteString(GetReferenceMain(builder.GetData(), L"idOfficeObject", false));
        m_signed_info.WriteString("</Reference>");

        return (L"<Object Id=\"idOfficeObject\">" + builder.GetData() + L"</Object>");
    }

    std::wstring GenerateImageObject()
    {
        if (m_image_valid.empty())
            return L"";

        m_signed_info.WriteString("<Reference Type=\"http://www.w3.org/2000/09/xmldsig#Object\" URI=\"#idValidSigLnImg\">");
        m_signed_info.WriteString(GetReferenceMain(m_image_valid, L"idValidSigLnImg", false));
        m_signed_info.WriteString("</Reference>");

        m_signed_info.WriteString("<Reference Type=\"http://www.w3.org/2000/09/xmldsig#Object\" URI=\"#idInvalidSigLnImg\">");
        m_signed_info.WriteString(GetReferenceMain(m_image_invalid, L"idInvalidSigLnImg", false));
        m_signed_info.WriteString("</Reference>");

        return (L"<Object Id=\"idValidSigLnImg\">" + m_image_valid + L"</Object><Object Id=\"idInvalidSigLnImg\">" + m_image_invalid + L"</Object>");
    }

    std::wstring GenerateSignPropertiesObject()
    {
        std::wstring sName = m_certificate->GetIssuerName();

        std::string sKeyA = m_certificate->GetNumber();
        std::wstring sKey = UTF8_TO_U(sKeyA);

        std::string sCertHA = m_certificate->GetCertificateHash();
        std::wstring sCertHW = UTF8_TO_U(sCertHA);

        std::wstring sXml = (L"<xd:SignedSignatureProperties>\
<xd:SigningTime>" + m_date + L"</xd:SigningTime>\
<xd:SigningCertificate>\
<xd:Cert>\
<xd:CertDigest>\
<DigestMethod Algorithm=\"" + ICertificate::GetDigestMethod(m_certificate->GetHashAlg()) + L"\"/>\
<DigestValue>" + sCertHW + L"</DigestValue>\
</xd:CertDigest>\
<xd:IssuerSerial>\
<X509IssuerName>CN=" + sName + L"</X509IssuerName>\
<X509SerialNumber>" + sKey + L"</X509SerialNumber>\
</xd:IssuerSerial>\
</xd:Cert>\
</xd:SigningCertificate>\
<xd:SignaturePolicyIdentifier>\
<xd:SignaturePolicyImplied/>\
</xd:SignaturePolicyIdentifier>\
</xd:SignedSignatureProperties>");

        std::wstring sSignedXml = L"<xd:SignedProperties xmlns=\"http://www.w3.org/2000/09/xmldsig#\" xmlns:xd=\"http://uri.etsi.org/01903/v1.3.2#\" Id=\"idSignedProperties\">";
        sSignedXml += sXml;
        sSignedXml += L"</xd:SignedProperties>";

        std::string sXmlTmp = XmlUtils::NSXmlCanonicalizator::Execute(U_TO_UTF8(sSignedXml), XmlUtils::XML_C14N_1_0);

        m_signed_info.WriteString("<Reference Type=\"http://uri.etsi.org/01903#SignedProperties\" URI=\"#idSignedProperties\">");
        m_signed_info.WriteString("<Transforms><Transform Algorithm=\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\"/></Transforms>");
        m_signed_info.WriteString("<DigestMethod Algorithm=\"" + ICertificate::GetDigestMethodA(m_certificate->GetHashAlg()) + "\"/><DigestValue>");
        m_signed_info.WriteString(m_certificate->GetHash(sXmlTmp, m_certificate->GetHashAlg()));
        m_signed_info.WriteString("</DigestValue></Reference>");

        return (L"<Object><xd:QualifyingProperties xmlns:xd=\"http://uri.etsi.org/01903/v1.3.2#\" Target=\"#idPackageSignature\">\
<xd:SignedProperties Id=\"idSignedProperties\">" + sXml + L"</xd:SignedProperties></xd:QualifyingProperties></Object>");
    }

    int AddSignatureReference()
    {
        std::wstring sDirectory = L"_xmlsignatures";

        // remove old .sig file
        std::vector<CFile> arFiles = m_file_manager.GetFiles(sDirectory, false);
        for (std::vector<CFile>::iterator i = arFiles.begin(); i != arFiles.end(); i++)
            if (i->GetFileExtention() == L"sigs")
                arFiles.erase(i);

        std::wstring sOriginName = L"origin.sigs";
        if (!m_file_manager.ExistFile(sDirectory + L"/" + sOriginName))
            m_file_manager.addFile(sDirectory + L"/" + sOriginName, NULL, 0);

        int nSignNum = GetCountSigns(sDirectory);

        CorrectContentTypes(nSignNum + 1);
        return nSignNum;
    }

    int Sign()
    {
        Parse();

        std::string sSignedData;

        NSStringUtils::CStringBuilder builderMain;

        builderMain.WriteString(GeneratePackageObject());
        builderMain.WriteString(GenerateOfficeObject());
        builderMain.WriteString(GenerateSignPropertiesObject());
        builderMain.WriteString(GenerateImageObject());

        std::string sSignedInfoData = m_signed_info.GetData();
        std::string sSignedXml = "<SignedInfo xmlns=\"http://www.w3.org/2000/09/xmldsig#\">" + sSignedInfoData + "</SignedInfo>";
        sSignedXml = XmlUtils::NSXmlCanonicalizator::Execute(sSignedXml, XmlUtils::XML_C14N_1_0);
        sSignedXml = m_certificate->Sign(sSignedXml);

        NSStringUtils::CStringBuilder builderResult;
        builderResult.WriteString(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?><Signature xmlns=\"http://www.w3.org/2000/09/xmldsig#\" Id=\"idPackageSignature\"><SignedInfo>");
        builderResult.WriteString(UTF8_TO_U(sSignedInfoData));
        builderResult.WriteString(L"</SignedInfo>");
        builderResult.WriteString(L"<SignatureValue>");
        builderResult.WriteString(UTF8_TO_U(sSignedXml));
        builderResult.WriteString(L"</SignatureValue>");
        builderResult.WriteString(L"<KeyInfo><X509Data><X509Certificate>");
        builderResult.WriteString(UTF8_TO_U(m_certificate->GetCertificateBase64()));
        builderResult.WriteString(L"</X509Certificate></X509Data></KeyInfo>");

        builderResult.Write(builderMain);
        builderResult.WriteString(L"</Signature>");

        int nSignNum = AddSignatureReference();

        BYTE* pData; LONG nLength = 0;
        NSFile::CUtf8Converter::GetUtf8StringFromUnicode(builderResult.GetData().c_str(), builderResult.GetSize(), pData, nLength);
        m_file_manager.addFile(L"_xmlsignatures/sig" + std::to_wstring(nSignNum + 1) + L".xml", pData, nLength);

        return (sSignedXml.empty()) ? 1 : 0;
    }
};

COOXMLSignerBuffer::COOXMLSignerBuffer(unsigned char* arrFileTree, ICertificate* pContext)
{
    m_internal = new COOXMLSignerBuffer_private(arrFileTree, pContext);
}

COOXMLSignerBuffer::~COOXMLSignerBuffer()
{
    RELEASEOBJECT(m_internal);
}

void COOXMLSignerBuffer::SetGuid(const std::wstring& guid)
{
    m_internal->SetGuid(guid);
}

void COOXMLSignerBuffer::SetImageValid(unsigned char* file)
{
    m_internal->SetImageValid(file);
}

void COOXMLSignerBuffer::SetImageInvalid(unsigned char* file)
{
    m_internal->SetImageInvalid(file);
}

int COOXMLSignerBuffer::Sign()
{
    return m_internal->Sign();
}
