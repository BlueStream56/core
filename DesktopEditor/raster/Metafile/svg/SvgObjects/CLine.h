#ifndef CLINE_H
#define CLINE_H

#include "CObjectBase.h"

namespace SVG
{
	class CLine : public CObjectBase
	{
	public:
		CLine(CObjectBase* pParent = NULL);

		void SetData(const std::map<std::wstring, std::wstring>& mAttributes, unsigned short ushLevel, bool bHardMode = false) override;

		bool ReadFromXmlNode(XmlUtils::CXmlNode& oNode) override;
		bool Draw(IRenderer* pRenderer) const override;
	private:
		void ApplyStyle(IRenderer* pRenderer, int& nTypePath) const override;

		SvgDigit m_oX1;
		SvgDigit m_oY1;
		SvgDigit m_oX2;
		SvgDigit m_oY2;
	};
}

#endif // CLINE_H
