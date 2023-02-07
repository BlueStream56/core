#ifndef CSVGFILE_H
#define CSVGFILE_H

#include "../../../graphics/IRenderer.h"
#include "../../../graphics/config.h"
#include "../../../graphics/pro/Fonts.h"

#include "CSvgParser.h"
#include "SvgObjects/CStyle.h"

class GRAPHICS_DECL CSvgFile
{
	public:
		CSvgFile();
		~CSvgFile();

		bool ReadFromBuffer(BYTE* pBuffer, unsigned int unSize);
		bool OpenFromFile(const std::wstring& wsFile);
		bool Load(const std::wstring& wsContent);

		void Close();

		bool GetBounds(double& dX, double& dY, double& dWidth, double& dHeight) const;
		const SVG::CSvgCalculator* GetSvgCalculator() const;

		void SetFontManager(NSFonts::IFontManager* pFontManager);

		void AddStyles(const std::wstring& wsStyles);

		bool Draw(IRenderer* pRenderer, double dX, double dY, double dWidth, double dHeight);
	private:
		void Init();

		SVG::CSvgParser    *m_pParser;
		SVG::CContainer    *m_pContainer;
		SVG::CSvgCalculator m_oSvgCalculator;
};

#endif // CSVGFILE_H
