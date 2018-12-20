// ImageFadeIn.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ImageFadeIn.h"
#include <omp.h>

inline void fadeIn(CImage* prev, CImage* next, BYTE alpha, CImage& res) throw (const char*) {
    UINT16 nPrevWidth = prev->GetWidth(), nPrevHeight = prev->GetHeight();
    UINT16 nNextWidth = next->GetWidth(), nNextHeight = next->GetHeight();
    UINT nBytesPerPixel;
    if (nNextWidth != nPrevWidth || nNextHeight != nPrevHeight) {
        throw "Sizes of Source Images Unmatched";
    }
    if ((nBytesPerPixel = prev->GetBPP() / 8) != next->GetBPP() / 8) {
        throw "Bits-per-pixel not Equal";
    }

    UINT16 nWidth = nPrevWidth, nHeight = nPrevHeight;

    UINT nPitch = res.GetPitch();
    res.Destroy();
    res.Create(nWidth, nHeight, nBytesPerPixel * 8);
    LPBYTE p = (LPBYTE)res.GetBits();

#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < (int)nHeight; i++) {
            BYTE* pPixelLinePrev = (BYTE*)prev->GetPixelAddress(0, i);
            BYTE* pPixelLineNext = (BYTE*)next->GetPixelAddress(0, i);
            BYTE* pPixelLineNew = (BYTE*)res.GetPixelAddress(0, i);

            for (UINT j = 0; j < nWidth; j++) {

                BYTE A_bit[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
                BYTE B_bit[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

                for (UINT k = 0; k < nBytesPerPixel; k++) {
                    *(A_bit + 2 * k) = pPixelLinePrev[j * nBytesPerPixel + k];

                    *(B_bit + 2 * k) = pPixelLineNext[j * nBytesPerPixel + k];
                }

                __m128i xmm0, xmm1, xmm2, xmm3;

                xmm3 = _mm_setzero_si128();
                xmm0 = _mm_loadu_si128((__m128i*)(A_bit));
                xmm1 = _mm_loadu_si128((__m128i*)(B_bit));

                xmm1 = _mm_sub_epi16(xmm1, xmm0);//minus
                xmm2 = _mm_set_epi16(alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha);//set fade
                xmm1 = _mm_mullo_epi16(xmm1, xmm2);//multi
                xmm1 = _mm_srai_epi16(xmm1, 7);//move right 8 bit
                xmm1 = _mm_add_epi16(xmm1, xmm0);//add
                xmm1 = _mm_packus_epi16(xmm1, xmm3);//compress

                pPixelLineNew[j * nBytesPerPixel] = _mm_extract_epi8(xmm1, 0);
                pPixelLineNew[j * nBytesPerPixel + 1] = _mm_extract_epi8(xmm1, 1);
                pPixelLineNew[j * nBytesPerPixel + 2] = _mm_extract_epi8(xmm1, 2);
                if (nBytesPerPixel >= 4) {
                    pPixelLineNew[j * nBytesPerPixel + 3] = _mm_extract_epi8(xmm1, 3);
                }
            }
        }
    }
}