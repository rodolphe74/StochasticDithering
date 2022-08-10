#ifndef PDFUTILS_HPP
#define PDFUTILS_HPP

#include <cstdint>
#include <hpdf.h>
#include <iostream>
#include <string>
#include <map>
#include "structs.hpp"
#include "voronoiUtils.hpp"


inline void haru_error_handler(HPDF_STATUS   error_no,
	HPDF_STATUS   detail_no,
	void *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT) error_no,
		(HPDF_UINT) detail_no);
}

class PDFUtils {
public:
	// Constructors
	PDFUtils(int32_t w, int32_t h) : width(w), height(h)
	{
		// pdf init
		pdf = HPDF_New(haru_error_handler, NULL);
		font = HPDF_GetFont(pdf, "Helvetica", NULL);
		page = HPDF_AddPage(pdf);
		HPDF_Page_SetLineWidth(page, (HPDF_REAL) 0.1);
		HPDF_Page_SetWidth(page, (HPDF_REAL) w);
		HPDF_Page_SetHeight(page, (HPDF_REAL) h);

		HPDF_Page_GSave(page);
		gstate = HPDF_CreateExtGState(pdf);
		HPDF_ExtGState_SetBlendMode(gstate, HPDF_BM_MULTIPLY);
		HPDF_Page_SetExtGState(page, gstate);

	};

	void drawTriangle(st::Pixel a, st::Pixel b, st::Pixel c, st::Colorf color);
	void drawTriangle(st::Pixel a, st::Pixel b, st::Pixel c, st::Colorf color, const Rectangle &r);
	void drawPolygon(std::vector<st::Pixel> pixels, st::Colorf color);
	static void ditherOverPdf(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist);
	static void ditherOverPdfRGB(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist);

	void saveIt(std::string filename) {
		HPDF_SaveToFile(pdf, filename.c_str());
	}

	// Destructor
	~PDFUtils()
	{
		//HPDF_Page_GRestore(page);
		HPDF_Free(pdf);
	}

	// Operators
	PDFUtils &operator=(const PDFUtils &assign);

private:
	int32_t width;
	int32_t height;
	HPDF_Doc pdf;
	HPDF_Page page;
	HPDF_Font font;	
	HPDF_ExtGState gstate;
};

#endif