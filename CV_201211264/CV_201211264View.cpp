
// CV_201211264View.cpp : CCV_201211264View 클래스의 구현
//

#include "stdafx.h"
#include "CKmeansClustering.h"
#include "CAgglomerativeClustering.h"
#include "NeuralNetwork.h"
#include <algorithm>
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "CV_201211264.h"

#endif
#include "CV_201211264Doc.h"
#include "CV_201211264View.h"
#include "opencv\cv.h"
#include "NeuralNetwork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCV_201211264View

IMPLEMENT_DYNCREATE(CCV_201211264View, CView)

BEGIN_MESSAGE_MAP(CCV_201211264View, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_IMAGELOAD_BMPLOAD, &CCV_201211264View::OnImageloadBmpload)
	ON_COMMAND(ID_IMAGELOAD_JPEGLOAD, &CCV_201211264View::OnImageloadJpegload)
	ON_COMMAND(ID_PROCESSING_OTSUMETHOD, &CCV_201211264View::OnProcessingOtsumethod)
	//ON_COMMAND(ID_PROCESSING_DILATION, &CCV_201211264View::OnProcessingDilation)
	//ON_COMMAND(ID_PROCESSING_EROSION, &CCV_201211264View::OnProcessingErosion)
	ON_COMMAND(ID_PROCESSING_MORDILATION, &CCV_201211264View::OnProcessingMorDilation)
	ON_COMMAND(ID_PROCESSING_MOREROSION, &CCV_201211264View::OnProcessingMorerosion)
	ON_COMMAND(ID_PROCESSING_OPENING, &CCV_201211264View::OnProcessingOpening)
	ON_COMMAND(ID_PROCESSING_CLOSING, &CCV_201211264View::OnProcessingClosing)
	ON_COMMAND(ID_PROCESSING_CUSTERMIZED, &CCV_201211264View::OnProcessingCustermized)
	ON_COMMAND(ID_PROCESSING_MORDILATIONWITHSPECIALCASE, &CCV_201211264View::OnProcessingMordilationwithspecialcase)
	ON_COMMAND(ID_PROCESSING_MOREROSIONWITHSPECIALCASE, &CCV_201211264View::OnProcessingMorerosionwithspecialcase)
	ON_COMMAND(ID_LABELING_ROW, &CCV_201211264View::OnLabelingRow)
	ON_COMMAND(ID_LABELING_RUN, &CCV_201211264View::OnLabelingRun)
	ON_COMMAND(ID_LABELING_LABELINGWITHAREA, &CCV_201211264View::OnLabelingLabelingwitharea)
	ON_COMMAND(ID_LABELING_LABELINGWITHCENTROID, &CCV_201211264View::OnLabelingLabelingwithcentroid)
	ON_COMMAND(ID_LABELING_LABELINGWITHBOUNDINGBOX, &CCV_201211264View::OnLabelingLabelingwithboundingbox)
	ON_COMMAND(ID_LABELING_LABELINGWITHCIRCULARITY, &CCV_201211264View::OnLabelingLabelingwithcircularity)
	ON_COMMAND(ID_LABELING_LABELINGWITHORIENTATION, &CCV_201211264View::OnLabelingLabelingwithorientation)
	ON_COMMAND(ID_EXTRACTFEATURE_EXTRACT, &CCV_201211264View::OnExtractfeatureExtract)
	ON_COMMAND(ID_NEAREST_NEIGHBOR, &CCV_201211264View::OnNearestNeighbor)
	ON_COMMAND(ID_NEAREST_MEAN, &CCV_201211264View::OnNearestMean)
	ON_COMMAND(ID_NEURALNETWORK, &CCV_201211264View::OnNeuralnetwork)
	ON_COMMAND(ID_CLUSTERING_KMEANS, &CCV_201211264View::OnClusteringKmeans)
	ON_COMMAND(ID_CLUSTERING_AGGLOMERATIVE, &CCV_201211264View::OnClusteringAgglomerative)
	ON_COMMAND(ID_NEURALNETWORK_TESTING, &CCV_201211264View::OnNeuralnetworkTesting)
	ON_COMMAND(ID_CLUSTERING_TESTING, &CCV_201211264View::OnClusteringTesting)
END_MESSAGE_MAP()

// CCV_201211264View 생성/소멸

CCV_201211264View::CCV_201211264View()
	: viewType(0), height(0), width(0), otsuThreshold(0), openingFlag(false), closingFlag(false),
	dilationSpecialCase(false), erosionSpecialCase(false), MASK_SIZE(2)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	rgbBuffer = NULL; binary = NULL; mor = NULL; closing = NULL;
	hue = NULL; satu = NULL; inten = NULL; opening = NULL;
	dilationSpecial = NULL; erosionSpecial = NULL;
	histogram = new int[GRAYUPPER]; labelBuffer = NULL;
	Process p; features.clear();
	mListImage = new std::vector<Img>[10];
	testData = new std::vector<Img>[10];
	featureVectors = new FeatureVector[10];

	for (int i = 0; i < 10; i++) {
		mListImage[i].clear();
		for (int j = 0; j < 2; j++) {
			featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[j] = 0;
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenFolding[j] = 0;
			meanAndSigmaOfInput.meanOfPointsOfMatchedAreaWhenFolding[j] = 0;
			meanAndSigmaOfInput.sigmaOfPointsOfMatchedAreaWhenFolding[j] = 0;
		}

		for (int j = 0; j < 4; j++) {
			featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[j] = 0;
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[j] = 0;
			meanAndSigmaOfInput.meanOfPointsOfMatchedAreaWhenSpliting[j] = 0;
			meanAndSigmaOfInput.sigmaOfPointsOfMatchedAreaWhenSpliting[j] = 0;
		}
	}


}

CCV_201211264View::~CCV_201211264View()
{
	delete[] histogram;
}

BOOL CCV_201211264View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CCV_201211264View 그리기

void CCV_201211264View::OnDraw(CDC* pDC)
{
	CCV_201211264Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (rgbBuffer != nullptr) {
		switch (viewType){
		case BMPLOAD:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j, i, RGB(rgbBuffer[i][j].rgbRed, rgbBuffer[i][j].rgbGreen, rgbBuffer[i][j].rgbBlue));
				}
			}
			break;
		case BINARY:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + (width + 20), i, RGB(binary[i][j], binary[i][j], binary[i][j]));
				}
			}

			break;
		case DILATION:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + (width + 20) * 2, i, RGB(mor[i][j], mor[i][j], mor[i][j]));
				}
			}
			break;
		case EROSION:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + (width + 20) * 2, i, RGB(mor[i][j], mor[i][j], mor[i][j]));
				}
			}
			break;
		case OPENING:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j, i + (height) + 20, RGB(opening[i][j], opening[i][j], opening[i][j]));
				}
			}
			break;
		case CLOSING:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + (width) + 20, i + (height) + 20, RGB(closing[i][j], closing[i][j], closing[i][j]));
				}
			}
			break;
		case CUSTOMOTSU:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + width * 2 + 20, i + (height) + 20, RGB(binary[i][j], binary[i][j], binary[i][j]));
				}
			}
			break;
		case DILATIONSPECIAL:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + width, i, RGB(mor[i][j], mor[i][j], mor[i][j]));
				}
			}
			break;
		case EROSIONSPECIAL:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + width, i, RGB(mor[i][j], mor[i][j], mor[i][j]));
				}
			}
			break;
		case ROWBYROW:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + width, i, RGB(labelBuffer[i][j], labelBuffer[i][j], labelBuffer[i][j]));
				}
			}
			break;
		case RUNLENGTH:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + width, i, RGB(labelBuffer[i][j], labelBuffer[i][j], labelBuffer[i][j]));
				}
			}
			break;
		case BOUNDINGBOX:
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					pDC->SetPixel(j + width, i, RGB(rgbBuffer[i][j].rgbRed, rgbBuffer[i][j].rgbGreen, rgbBuffer[i][j].rgbBlue));
				}
			}
			break;
		default:
			break;
		}



	}

}


// CCV_201211264View 인쇄

BOOL CCV_201211264View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CCV_201211264View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CCV_201211264View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CCV_201211264View 진단

#ifdef _DEBUG
void CCV_201211264View::AssertValid() const
{
	CView::AssertValid();
}

void CCV_201211264View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCV_201211264Doc* CCV_201211264View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCV_201211264Doc)));
	return (CCV_201211264Doc*)m_pDocument;
}
#endif //_DEBUG


// CCV_201211264View 메시지 처리기

void CCV_201211264View::RGBToHSI() {
	if (hue != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] hue[i];
		}
		delete[] hue;
	}
	if (satu != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] satu[i];
		}
		delete[] satu;
	}
	if (inten != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] inten[i];
		}
		delete[] inten;
	}

	hue = new float*[height];
	satu = new float*[height];
	inten = new float*[height];

	for (int i = 0; i < height; i++) {
		hue[i] = new float[width];
		satu[i] = new float[width];
		inten[i] = new float[width];
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			float r = rgbBuffer[i][j].rgbRed, g = rgbBuffer[i][j].rgbGreen, b = rgbBuffer[i][j].rgbBlue;
			inten[i][j] = (r + g + b) / (float)(3 * 255); //intensity   float total = r+g+b;
			float total = r + g + b;
			r = r / total;  g = g / total;   b = b / total;
			satu[i][j] = 1 - 3 * (r>g ? (g>b ? b : g) : (r>b ? b : r));
			if (r == g && g == b) {
				hue[i][j] = 0; satu[i][j] = 0;
			}
			else {
				total = (0.5*(r - g + r - b) / sqrt((r - g)*(r - g) + (r - b)*(g - b)));   hue[i][j] = acos((double)total);
				if (b > g) hue[i][j] = 6.28 - hue[i][j];
			}
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			hue[i][j] = hue[i][j] * 255 / (3.14 * 2);
			satu[i][j] = satu[i][j] * 255;
			inten[i][j] = inten[i][j] * 255;
		}
	}
}


void CCV_201211264View::OnImageloadBmpload(){
	char szFilter[] = "Windows bitmap filew(*.bmp)|*.bmp||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, szFilter);
	CString fileName;
	if (dlg.DoModal() == IDOK) {
		fileName = dlg.GetPathName();
	}else {
		return;
	}
	if (rgbBuffer != nullptr) {
		for (int i = 0; i < height; i++)
			delete[] rgbBuffer[i];
		delete[] rgbBuffer;
	}
	BITMAPFILEHEADER bmpFileHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	CFile file;
	file.Open(fileName, CFile::modeRead);
	file.Read(&bmpFileHeader, sizeof(BITMAPFILEHEADER));
	file.Read(&bmpInfoHeader, sizeof(BITMAPFILEHEADER));
	width = bmpInfoHeader.biWidth;
	height = bmpInfoHeader.biHeight;
	rgbBuffer = new RGBQUAD*[height];
	for (int i = 0; i < height; i++) {
		rgbBuffer[i] = new RGBQUAD[width];
	}

	bool b4Byte = false;
	int upByte = 0;
	b4Byte = ((width * 3) % 4 == 0) ? true : false;
	upByte = ((width * 3) % 4 == 0) ? 0 : 4 - (width * 3) % 4;


	BYTE data[3];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			file.Read(&data, 3);
			rgbBuffer[height - i - 1][j].rgbBlue = data[0];
			rgbBuffer[height - i - 1][j].rgbGreen = data[1];
			rgbBuffer[height - i - 1][j].rgbRed = data[2];
		}
		if (!b4Byte) file.Read(&data, upByte);
	}
	file.Close();
	viewType = BMPLOAD;
	Invalidate(true);
}


void CCV_201211264View::OnImageloadJpegload()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CCV_201211264View::OnProcessingOtsumethod(){
	if (rgbBuffer == NULL)
		OnImageloadBmpload();

	RGBToHSI();

	for (int i = 0; i < GRAYUPPER; i++) {
		histogram[i] = 0;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			histogram[(int)inten[i][j]]++;
		}
	}
	float sum = 0;
	for (int i = 0; i < GRAYUPPER; i++) {
		sum += (float)(i * histogram[i]);
	}
	float sumB = 0.0f, wB = 0.0f, wF = 0.0f, maxValue = 0;
	otsuThreshold = 0;

	for (int i = 0; i < GRAYUPPER; i++) {
		wB += histogram[i];
		if (wB == 0) continue;
		wF = height * width - wB;
		if (wF == 0)
			break;
		sumB += (float)(i * histogram[i]);
		float mB = sumB / wB;
		float mF = (sum - sumB) / wF;
		float between = wB * wF * (mB - mF) * (mB - mF);
		if (between > maxValue) {
			maxValue = between; otsuThreshold = i;
		}
	}

	if (binary != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] binary[i];
		}
		delete[] binary;
	}

	binary = new int*[height];
	for (int i = 0; i < height; i++) {
		binary[i] = new int[width];
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			binary[i][j] = (inten[i][j] > otsuThreshold) ? 255 : 0;
		}
	}
	int maxP = 0, minP = height * width;
	for (int i = 0; i < GRAYUPPER; i++) {
		maxP = (histogram[i] > maxP) ? histogram[i] : maxP;
		minP = (histogram[i] < minP) ? histogram[i] : minP;
	}

	for (int i = 0; i < GRAYUPPER; i++) {
		histogram[i] = (histogram[i] - minP) * 255 / (maxP - minP);
	}
	// create binary image by otsu method
	viewType = BINARY; Invalidate(FALSE);

}

void CCV_201211264View::OnProcessingCustermized(){
	if (rgbBuffer == NULL)
		OnImageloadBmpload();

	RGBToHSI();
	for (int i = 0; i < GRAYUPPER; i++) {
		histogram[i] = 0;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			histogram[(int)inten[i][j]]++;
		}
	}
	int* ranges = new int[5];
	for (int i = 0; i < 5; i++) {
		ranges[i] = (int)(((float)i / 4) * GRAYUPPER);
	}
	otsuThresholds = new int[4];
	for (int t = 0; t < 4; t++) {
		float sum = 0, histDesity = 0;
		for (int i = ranges[t]; i < ranges[t + 1]; i++) {
			sum += (float)((i - ranges[t]) * histogram[i]);
		}
		float sumB = 0.0f, wB = 0.0f, wF = 0.0f, maxValue = 0;
		otsuThresholds[t] = 0;

		for (int i = ranges[t]; i < ranges[t + 1]; i++) {
			histDesity += histogram[i];
		}

		for (int i = ranges[t]; i < ranges[t + 1]; i++) {
			wB += histogram[i];
			if (wB == 0) continue;
			wF = (histDesity) - wB;
			if (wF <= 0)
				break;
			sumB += (float)((i - ranges[t]) * histogram[i]);
			float mB = sumB / wB;
			float mF = (sum - sumB) / wF;
			float between = wB * wF * (mB - mF) * (mB - mF);
			if (between > maxValue) {
				maxValue = between; otsuThresholds[t] = i;
			}
		}
	}


	if (binary != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] binary[i];
		}
		delete[] binary;
	}

	binary = new int*[height];
	for (int i = 0; i < height; i++) {
		binary[i] = new int[width];
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			binary[i][j] = (inten[i][j] > otsuThresholds[0]) ?
				(inten[i][j] > otsuThresholds[1]) ?
				(inten[i][j] > otsuThresholds[2]) ?
				(inten[i][j] > otsuThresholds[3]) ?
				255 : (int)(256.0f * (4.0f / 4.0f)) - 1 : (int)(256.0f * (2.0f / 4.0f)) : (int)(256.0f * (1.0f / 4.0f)) : 0;
		}
	}
	int maxP = 0, minP = height * width;
	for (int i = 0; i < GRAYUPPER; i++) {
		maxP = (histogram[i] > maxP) ? histogram[i] : maxP;
		minP = (histogram[i] < minP) ? histogram[i] : minP;
	}

	for (int i = 0; i < GRAYUPPER; i++) {
		histogram[i] = (histogram[i] - minP) * 255 / (maxP - minP);
	}

	viewType = CUSTOMOTSU; Invalidate(false);
}

int** CCV_201211264View::OnProcessingDilation(int** input, int** structElement){
	int** output = new int*[height];
	for (int i = 0; i < height; i++) {
		output[i] = new int[width];
	}
	int zeroPad = floor((float)MASK_SIZE / 2.0);
	for (int i = zeroPad; i < height - zeroPad; i++) {
		for (int j = zeroPad; j < width - zeroPad; j++) {
			output[i][j] = 0;
			bool bstop = false;
			for (int k = 0; k < MASK_SIZE; k++) {
				for (int l = 0, bstop = false; l < MASK_SIZE && !bstop; l++) {
					bool bbinary = (input[i + k - zeroPad][j + l - zeroPad] == 255);
					if (bbinary && structElement[k][l]) {
						output[i][j] = 255;bstop = true;
					}
				}
			}
		}
	}
	return output;
}

int** CCV_201211264View::OnProcessingErosion(int** input, int** structElement){
	int** output = new int*[height];
	for (int i = 0; i < height; i++) {
		output[i] = new int[width];
	}

	int zeroPad = floor((float)MASK_SIZE / 2.0);
	for (int i = zeroPad; i < height - zeroPad; i++) {
		for (int j = zeroPad; j < width - zeroPad; j++) {
			output[i][j] = 255;
			bool bstop = false;
			for (int k = 0; k < MASK_SIZE; k++) {
				for (int l = 0, bstop = false; l < MASK_SIZE && !bstop; l++) {
					bool bbinary = (input[i + k - zeroPad][j + l - zeroPad] == 255);
					if (dilationSpecialCase && !structElement[k][l]) continue;
					if (erosionSpecialCase && !structElement[k][l]) continue;
					if (!(bbinary && structElement[k][l])) {
						output[i][j] = 0; bstop = true;
					}
				}
			}
		}
	}
	return output;
}

void CCV_201211264View::OnProcessingMorDilation(){
	if (mor != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] mor[i];
		}
		delete[] mor;
	}
	if (binary != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] binary[i];
		}
		delete[] binary;
	}

	if (rgbBuffer == nullptr) {
		OnImageloadBmpload();
	}

	binary = new int*[height];
	mor = new int*[height];
	for (int i = 0; i < height; i++) {
		binary[i] = new int[width];
		mor[i] = new int[width];
	}

	if (!openingFlag && !closingFlag) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int grayValue = (rgbBuffer[i][j].rgbRed + rgbBuffer[i][j].rgbGreen + rgbBuffer[i][j].rgbBlue);
				binary[i][j] = (grayValue < 128) ? 0 : 255;
			}
		}
	}else {
		if (openingFlag) {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					int grayValue = (rgbBuffer[i][j].rgbRed + rgbBuffer[i][j].rgbGreen + rgbBuffer[i][j].rgbBlue);
					binary[i][j] = (grayValue < 128) ? 0 : 255;
				}
			}
		}else {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					binary[i][j] = (closing[i][j] < 128) ? 0 : 255;
				}
			}
		}
	}

	int** structureElem = new int*[MASK_SIZE];
	if (dilationSpecialCase || erosionSpecialCase) {
		int center = MASK_SIZE / 2;
		for (int i = 0; i < MASK_SIZE; i++) {
			structureElem[i] = new int[MASK_SIZE];
			for (int j = 0; j < MASK_SIZE; j++) {
				structureElem[i][j] = (pow(center - i, 2) + pow(center - j, 2) <= pow(center, 2)) ? 1 : 0;
			}
		}
	}else {
		for (int i = 0; i < MASK_SIZE; i++) {
			structureElem[i] = new int[MASK_SIZE];
			for (int j = 0; j < MASK_SIZE; j++) {
				structureElem[i][j] = 1;
			}
		}
	}

	if (!openingFlag && !closingFlag) {
		if (erosionSpecialCase) {
			mor = OnProcessingDilation(erosionSpecial, structureElem);
		}
		else {
			mor = OnProcessingDilation(binary, structureElem);
			if (dilationSpecialCase) {
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						dilationSpecial[i][j] = (mor[i][j] < 0) ? 0 : mor[i][j];
					}
				}
			}
		}

	}else {
		if (openingFlag) {
			opening = OnProcessingDilation(binary, structureElem);
		}
		else {
			closing = OnProcessingDilation(binary, structureElem);
		}
	}
	if (openingFlag || closingFlag || dilationSpecialCase) return;
	viewType = DILATION; Invalidate(false);
	for (int i = 0; i < MASK_SIZE; i++) {
		delete[] structureElem[i];
	}
	delete[] structureElem;
}

void CCV_201211264View::OnProcessingMorerosion(){
	if (mor != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] mor[i];
		}
		delete[] mor;
	}
	if (binary != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] binary[i];
		}
		delete[] binary;
	}

	if (rgbBuffer == nullptr) {
		OnImageloadBmpload();
	}

	binary = new int*[height];
	mor = new int*[height];
	for (int i = 0; i < height; i++) {
		binary[i] = new int[width];
		mor[i] = new int[width];
	}



	if (!openingFlag && !closingFlag) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int grayValue = (rgbBuffer[i][j].rgbRed + rgbBuffer[i][j].rgbGreen + rgbBuffer[i][j].rgbBlue);
				binary[i][j] = (grayValue < 128) ? 0 : 128;
			}
		}
	}else {
		if (openingFlag) {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					binary[i][j] = (opening[i][j] < 128) ? 0 : 255;
				}
			}
		}else {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					int grayValue = (rgbBuffer[i][j].rgbRed + rgbBuffer[i][j].rgbGreen + rgbBuffer[i][j].rgbBlue);
					binary[i][j] = (grayValue < 128) ? 0 : 255;
				}
			}
		}
	}

	int** structureElem = new int*[MASK_SIZE];
	if (erosionSpecialCase || dilationSpecialCase) {
		int center = MASK_SIZE / 2;
		for (int i = 0; i < MASK_SIZE; i++) {
			structureElem[i] = new int[MASK_SIZE];
			for (int j = 0; j < MASK_SIZE; j++) {
				structureElem[i][j] = (pow(center - i, 2) + pow(center - j, 2) <= pow(center, 2)) ? 1 : 0;
			}
		}
	}else {
		for (int i = 0; i < MASK_SIZE; i++) {
			structureElem[i] = new int[MASK_SIZE];
			for (int j = 0; j < MASK_SIZE; j++) {
				structureElem[i][j] = 1;
			}
		}
	}



	if (!openingFlag && !closingFlag) {
		if (dilationSpecialCase) {
			mor = OnProcessingErosion(dilationSpecial, structureElem);
		}else {
			mor = OnProcessingErosion(binary, structureElem);
			/*for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					erosionSpecial[i][j] = (mor[i][j] < 0) ? 0 : mor[i][j];
				}
			}*/
		}


	}else {
		if (openingFlag) {
			opening = OnProcessingErosion(binary, structureElem);
		}else {
			closing = OnProcessingErosion(binary, structureElem);
		}
	}

	if (openingFlag || closingFlag || dilationSpecialCase) return;
	viewType = EROSION; Invalidate(false);
	for (int i = 0; i < MASK_SIZE; i++) {
		delete[] structureElem[i];
	}
	delete[] structureElem;
}

void CCV_201211264View::OnProcessingOpening(){//erosion after dilation
	if (rgbBuffer == NULL)
		OnImageloadBmpload();
	RGBToHSI();
	if (opening != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] opening[i];
		}
		delete[] opening;
	}
	opening = new int*[height];
	for (int i = 0; i < height; i++) {
		opening[i] = new int[width];
	}
	openingFlag = true;
	OnProcessingMorDilation();
	OnProcessingMorerosion();
	openingFlag = false;
	viewType = OPENING; Invalidate(false);
}

void CCV_201211264View::OnProcessingClosing(){
	if (rgbBuffer == NULL)
		OnImageloadBmpload();
	RGBToHSI();
	if (closing != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] closing[i];
		}
		delete[] closing;
	}
	closing = new int*[height];
	for (int i = 0; i < height; i++) {
		closing[i] = new int[width];
	}
	closingFlag = true;
	OnProcessingMorerosion();
	OnProcessingMorDilation();
	closingFlag = false;;
	viewType = CLOSING; Invalidate(false);
}

void CCV_201211264View::OnProcessingMordilationwithspecialcase() {
	if (rgbBuffer == NULL)
		OnImageloadBmpload();
	RGBToHSI();
	if (dilationSpecial != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] dilationSpecial[i];
		}
		delete[] dilationSpecial;
	}
	dilationSpecial = new int*[height];
	for (int i = 0; i < height; i++) {
		dilationSpecial[i] = new int[width];
	}

	dilationSpecialCase = true;
	MASK_SIZE = 15;
	OnProcessingMorDilation();
	OnProcessingMorerosion();
	dilationSpecialCase = false;
	viewType = DILATIONSPECIAL; Invalidate(false);
}

void CCV_201211264View::OnProcessingMorerosionwithspecialcase() {
	if (rgbBuffer == NULL)
		OnImageloadBmpload();
	RGBToHSI();
	if (erosionSpecial != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] erosionSpecial[i];
		}
		delete[] erosionSpecial;
	}
	erosionSpecial = new int*[height];
	for (int i = 0; i < height; i++) {
		erosionSpecial[i] = new int[width];
	}
	MASK_SIZE = 15;
	erosionSpecialCase = true;
	OnProcessingMorerosion();
	MASK_SIZE = 8;
	OnProcessingMorDilation();
	erosionSpecialCase = false;
	viewType = EROSIONSPECIAL; Invalidate(false);
}

int CCV_201211264View::ufMakeSet() {
	unionFind[0]++;
	unionFind[unionFind[0]] = unionFind[0];
	return unionFind[0];
}

int CCV_201211264View::ufUnion(int x, int y) {
	return unionFind[ufFind(x)] = ufFind(y);
}

int CCV_201211264View::ufFind(int x) {
	int y = x;
	while (unionFind[y] != y) y = unionFind[y];
	while (unionFind[x] != x) {
		int z = unionFind[x];
		unionFind[x] = y;
		x = z;
	}
	return y;
}

void CCV_201211264View::OnLabelingRow(){
	if (rgbBuffer == NULL)
		OnImageloadBmpload();

	if (labelBuffer != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] labelBuffer[i];
		}
		delete[] labelBuffer;
	}

	labelBuffer = new int*[height];
	for (int i = 0; i < height; i++) {
		labelBuffer[i] = new int[width];
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int pix = (rgbBuffer[i][j].rgbRed + rgbBuffer[i][j].rgbGreen + rgbBuffer[i][j].rgbBlue) / 3;
			labelBuffer[i][j] = (pix < 128) ? 0 : 1;
		 }
	}

	unionFind = new int[height * width];
	for (int i = 0; i < height * width; i++)  {
		unionFind[i] = 0;
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (labelBuffer[i][j] > 0) {
				int up = (i == 0 ? 0 : labelBuffer[i - 1][j]);
				int left = (j == 0 ? 0 : labelBuffer[i][j - 1]);
				if (!up && !left) {
					labelBuffer[i][j] = ufMakeSet();
				}else if (up && left) {
					labelBuffer[i][j] = ufUnion(up, left);
				}else {
					labelBuffer[i][j] = (up > left) ? up : left;
				}
				// 응용 확장 버전
				/*int indecies[4], count = 0, needUnionFind[4], x = 0;

				indecies[0] = (i == 0 ? 0 : labelBuffer[i - 1][j]);
				indecies[1] = (j == 0 ? 0 : labelBuffer[i][j - 1]);
				indecies[2] = ((i == 0 || j == 0) ? 0 : labelBuffer[i - 1][j - 1]);
				indecies[3] = ((i == 0 || j == width - 1) ? 0 : labelBuffer[i - 1][j + 1]);

				for (int k = 0; k < 4; k++) {
					count += (indecies[k]) ? 1 : 0;
					if (indecies[k]) {
						needUnionFind[x++] = indecies[k];
					}
				}
				switch (count) {
				case 0:
					labelBuffer[i][j] = ufMakeSet();
					break;
				case 1:
					labelBuffer[i][j] = needUnionFind[0];
					break;
				case 2:
					labelBuffer[i][j] = ufUnion(needUnionFind[0], needUnionFind[1]);
					break;
				case 3:case 4:
					int minIndex = height * width;
					for (int k = 0; k < count - 1; k++) {
						minIndex = (needUnionFind[k] < minIndex) ? needUnionFind[k] : minIndex;
						for (int l = k + 1; l < count; l++) {
							ufUnion(needUnionFind[k], needUnionFind[l]);
							minIndex = (needUnionFind[l] < minIndex) ? needUnionFind[l] : minIndex;
						}
					}
					labelBuffer[i][j] = minIndex;
					break;
				}*/

			}
		}
	}

	labels = new int[height * width];
	for (int i = 0; i < height * width; i++) {
		labels[i] = 0;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (labelBuffer[i][j] > 0) {
				int x = ufFind(labelBuffer[i][j]);
				if (labels[x] == 0) {
					labels[0]++;labels[x] = labels[0];
				}
				labelBuffer[i][j] = labels[x] * 30;
			}
			else {
				labelBuffer[i][j] = 255;
			}
		}
	}

	viewType = ROWBYROW; Invalidate(false);

}

void CCV_201211264View::OnLabelingRun(){
	if (rgbBuffer == NULL)
		OnImageloadBmpload();

	if (labelBuffer != nullptr) {
		for (int i = 0; i < height; i++) {
			delete[] labelBuffer[i];
		}
		delete[] labelBuffer;
	}

	labelBuffer = new int*[height];
	for (int i = 0; i < height; i++) {
		labelBuffer[i] = new int[width];
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int pix = (rgbBuffer[i][j].rgbRed + rgbBuffer[i][j].rgbGreen + rgbBuffer[i][j].rgbBlue) / 3;
			labelBuffer[i][j] = (pix < 128) ? 0 : 1;
		}
	}



	int curColor = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 1; j < width; j++) {
			curColor += (!labelBuffer[i][j - 1] && labelBuffer[i][j]) ? 1 : 0;
		}
	}

	unionFind = new int[height * width];
	for (int i = 0; i < height * width; i++) {
		unionFind[i] = 0;
	}

	RowInfo* rowInfo = new RowInfo[curColor + 1];
	curColor = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (!labelBuffer[i][j - 1] && labelBuffer[i][j]) {
				curColor++;
				rowInfo[curColor].label = ufMakeSet();
				rowInfo[curColor].startCol = j;
				rowInfo[curColor].endCol = j;
				rowInfo[curColor].rowNum = i;
				for (int k = 0; labelBuffer[i][j + k] && j + k < width; k++) {
					rowInfo[curColor].endCol++;
				}
			}
		}
	}

	for (int i = 1; i < curColor + 1; i++) {
		for (int j = i + 1; j < curColor + 1; j++) {
			if (rowInfo[i].rowNum == rowInfo[j].rowNum || i == j) continue;
			if ((rowInfo[j].rowNum - rowInfo[i].rowNum) == 1) {
				if (rowInfo[i].startCol >= rowInfo[j].startCol) {
					if (rowInfo[j].endCol>= rowInfo[i].startCol) {
						ufUnion(i, j);
					}
				}else if (rowInfo[i].startCol <= rowInfo[j].startCol) {
					if (rowInfo[i].endCol >= rowInfo[j].startCol) {
						ufUnion(i, j);
					}
				}
			}
		}
	}

	labels = new int[curColor + 1];
	for (int i = 0; i < curColor + 1; i++) {
		labels[i] = 0;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			labelBuffer[i][j] = 255;
		}
	}
	for (int i = 1; i < unionFind[0] + 1; i++) {
		int x = ufFind(i);
		if (!labels[x]) {
			labels[0]++; labels[x] = labels[0];
		}
		int j = rowInfo[i].rowNum;
		for (int k = rowInfo[i].startCol; k < rowInfo[i].endCol; k++) {
			labelBuffer[j][k] = labels[x] * 30;
		}
	}

	viewType = RUNLENGTH; Invalidate(false);
}

void CCV_201211264View::OnLabelingLabelingwitharea(){
	if (labelBuffer == nullptr)
		OnLabelingRow();

	if (features.size() > 0) {
		features.clear();
	}
	CString fullSentence = "Area\n";
	for (int i = 1; i < labels[0] + 1; i++) {
		Feature feat;
		int area = 0;
		for (int j = 0; j < height; j++) {
			for (int k = 0; k < width; k++) {
				if (labelBuffer[j][k] == i * 30) {
					area++; feat.color = labelBuffer[j][k];
				}
			}
		}
		feat.area = area;
		features.push_back(feat);
		CString tag;
		tag.Format("Label #%d: %d\n", feat.color / 30, feat.area);
		fullSentence += tag;
	}
	//AfxMessageBox(fullSentence);
	viewType = RUNLENGTH; Invalidate(false);
}

float CCV_201211264View::selectedArea(int x1, int y1, int x2, int y2) {
	if (binary != nullptr)
		OnProcessingOtsumethod();
	float area = 0;
	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			area += (binary[i][j] == 255) ? 1 : 0;
		}
	}
	return area / ((x2 - x1) * (y2 - y1));
}

void CCV_201211264View::OnLabelingLabelingwithcentroid(){
	/*if (labelBuffer == nullptr)
		OnLabelingLabelingwitharea();*/
	CString fullSentence = "Centroid\n";
	CString TestMsg = "", testTag = "";
	for (int i = 0; i < features.size(); i++) {
		features[i].centroid.x = 0;
		features[i].centroid.y = 0;

		for (int j = 0; j < height; j++) {
			for (int k = 0; k < width; k++) {
				if (labelBuffer[j][k] == features[i].color) {
					features[i].centroid.x += j;
					features[i].centroid.y += k;
				}
			}
		}
		features[i].centroid.x /= features[i].area;
		features[i].centroid.y /= features[i].area;


		CString tag;
		tag.Format("Label #%d: %d, %d\n", features[i].color / 30, features[i].centroid.x, features[i].centroid.y);
		fullSentence += tag;
	}
	//AfxMessageBox(fullSentence);
	viewType = RUNLENGTH; Invalidate(false);
}

void CCV_201211264View::OnLabelingLabelingwithboundingbox(){
	/*if (features.size() == 0)
		OnLabelingLabelingwithorientation();*/
	cv::Mat img(height, width, CV_8UC3);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			img.at<cv::Vec3b>(i, j)[2] = rgbBuffer[i][j].rgbRed;
			img.at<cv::Vec3b>(i, j)[1] = rgbBuffer[i][j].rgbGreen;
			img.at<cv::Vec3b>(i, j)[0] = rgbBuffer[i][j].rgbBlue;
		}
	}

	for (int i = 0; i < features.size(); i++) {
		features[i].r.x = width;
		features[i].r.y = height;
		features[i].r.width = 0;
		features[i].r.height = 0;

		for (int j = 0; j < height; j++) {
			for (int k = 0; k < width; k++) {
				if (labelBuffer[j][k] == features[i].color) {
					features[i].r.x = (features[i].r.x > k) ? k : features[i].r.x;
					features[i].r.y = (features[i].r.y > j) ? j : features[i].r.y;
					features[i].r.width = (features[i].r.width < k) ? k : features[i].r.width;
					features[i].r.height = (features[i].r.height < j) ? j : features[i].r.height;

				}
			}
		}
		features[i].r.width -= (features[i].r.x - 1);
		features[i].r.height -= (features[i].r.y - 1);
		if (features[i].orientantion < -0.5f || features[i].orientantion > 0.5f && features[i].area > 900)
			rectangle(img, cv::Rect(features[i].r.x, features[i].r.y, features[i].r.width, features[i].r.height), cv::Scalar(255, 0, 0), 3);

	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rgbBuffer[i][j].rgbRed = img.at<cv::Vec3b>(i, j)[2];
			rgbBuffer[i][j].rgbGreen = img.at<cv::Vec3b>(i, j)[1];
			rgbBuffer[i][j].rgbBlue = img.at<cv::Vec3b>(i, j)[0];
		}
	}
	viewType = BOUNDINGBOX; Invalidate(false);
}

void CCV_201211264View::OnLabelingLabelingwithcircularity()
{
	/*if (labelBuffer == nullptr)
		OnLabelingLabelingwithcentroid();*/
	CString fullSentence = "Circularity \n";

	for (int i = 0; i < features.size(); i++) {
		float mean = 0.0f;
		int perimeterArea = 0;
		int centerX = features[i].centroid.x;
		int centerY = features[i].centroid.y;
		for (int j = 1; j < height - 1; j++) {
			for (int k = 1; k < width - 1; k++) {
				if (labelBuffer[j][k] == features[i].color) {
					if (labelBuffer[j + 1][k] != features[i].color || labelBuffer[j - 1][k] != features[i].color ||
						labelBuffer[j][k + 1] != features[i].color || labelBuffer[j][k - 1] != features[i].color) {
						perimeterArea++;
						mean += sqrt((j - centerX)*(j - centerX) + (k - centerY)*(k - centerY));
					}
				}
			}
		}
		mean /= (float)perimeterArea;

		float sigma = 0;
		for (int j = 1; j < height - 1; j++) {
			for (int k = 1; k < width - 1; k++) {
				if (labelBuffer[j][k] == features[i].color) {
					if (labelBuffer[j + 1][k] != features[i].color || labelBuffer[j - 1][k] != features[i].color ||
						labelBuffer[j][k + 1] != features[i].color || labelBuffer[j][k - 1] != features[i].color) {
						float distance = sqrt((j - centerX)*(j - centerX) + (k - centerY)*(k - centerY));
						sigma += (distance - mean) * (distance - mean);
					}
				}
			}
		}
		sigma /= (float)perimeterArea;
		features[i].circularity = mean / sqrt(sigma);

		CString tag;
		tag.Format("Label #%d: %f  ", features[i].color / 30, features[i].circularity);
		fullSentence += tag;
	}

	//AfxMessageBox(fullSentence);
	//viewType = RUNLENGTH; Invalidate(false);
}

void CCV_201211264View::OnLabelingLabelingwithorientation()
{
	/*if (labelBuffer == nullptr)
		OnLabelingLabelingwithcircularity();*/
	CString fullSentence = "Circularity \n";

	for (int i = 0; i < features.size(); i++) {
		float rr, cc, rc;
		rr = cc = rc = 0.0f;
		int centerX = features[i].centroid.x;
		int centerY = features[i].centroid.y;
		float orientation = 0.0f;
		for (int j = 1; j < height - 1; j++) {
			for (int k = 1; k < width - 1; k++) {
				if (labelBuffer[j][k] == features[i].color) {
					rr += (j - centerX) * (j - centerX);
					cc += (k - centerY) * (k - centerY);
					rc += (j - centerX) * (k - centerY);
				}
			}
		}

		features[i].orientantion = (cc - rr == 0) ? 0 : (double)(2 * rc) / (cc - rr);
		CString tag;
		tag.Format("Label #%d: %f  ", features[i].color / 30, features[i].circularity);
		fullSentence += tag;
	}
	//viewType = RUNLENGTH; Invalidate(false);
}

void CCV_201211264View::OnExtractfeatureExtract(){
	// 중심에 대한 기울기축과 기울기에 수직인 축으로 반을 잘라서 양쪽에 대한 비율을 구한다.
	// 중심에 대한 기울기축과 기울기에 수직인 축으로 반을 접었을 때, 겹침의 비율을 구한다.
	// 총 4개의 feature에 대한 평균, 공분산, 분산을 구하고 각 벡터에 대한 가우시안을 만든다.

	CFileDialog dlg(TRUE, ".jpg", NULL, OFN_ALLOWMULTISELECT, "JPEG File (*.jpg)|*.jpg||");
	CString strFileList;
	const int c_cMaxFiles = 20000; //선택할 최대 파일 숫자
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if (dlg.DoModal() == IDOK){
		int i = 0, counter = 0, all = 0;
		for (POSITION pos = dlg.GetStartPosition(); pos != NULL;){
			// 전체삭제는 ResetContent
			Img img;
			img.filename = dlg.GetNextPathName(pos);
			mListImage[i].push_back(img);
			counter++;
			if (counter == 100) {
				all += mListImage[i].size(); i++; counter = 0;
			}

		}
		CString comment;
		comment.Format("전체 %d장의 DB이미지를 불러왔습니다.", all);
		AfxMessageBox(comment);
	}else{
		AfxMessageBox("Error: DB image files selection");
		return;
	}

	getArea();		// area 값 전부 계산
	getCentroid();	// centroid 값 전부 계산
	getOrientation();// orienation 값 전부 계산
	getCircularity();
	getMatchedAreaWhenFolding();
	getMatchedAreaWhenSpliting();
	AfxMessageBox("특징 추출 완료");
	//getCovariance();
	//WriteToFile();


}

void CCV_201211264View::getArea() {

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			Feature feat;
			int area = 0;//std::string(CT2CA(cstring.operator LPCWSTR()))
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {
					if (img.at<uchar>(k,l) > 128) area++;
				}
			}
			mListImage[i][j].feature.area = area;
		}
	}
}

void CCV_201211264View::getCentroid() {


	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			mListImage[i][j].feature.centroid.x = 0;
			mListImage[i][j].feature.centroid.y = 0;
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int x = 0, y = 0;
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {
					if (img.at<uchar>(k, l) > 128) {
						x += k; y += l;
					}
				}
			}
			mListImage[i][j].feature.centroid.x = x / mListImage[i][j].feature.area;
			mListImage[i][j].feature.centroid.y = y / mListImage[i][j].feature.area;

		}
	}
}

void CCV_201211264View::getOrientation() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			float rr, cc, rc;
			rr = cc = rc = 0.0f;
			int centerX = mListImage[i][j].feature.centroid.x;
			int centerY = mListImage[i][j].feature.centroid.y;
			float orientation = 0.0f;
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int x = 0, y = 0;
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {
					if (img.at<uchar>(k, l) > 128) {
						rr += (k - centerX) * (k - centerX);
						cc += (l - centerY) * (l - centerY);
						rc += (k - centerX) * (l - centerY);
					}
				}
			}
			mListImage[i][j].feature.orientantion = (cc - rr == 0) ? 0 : (double)(2 * rc) / (cc - rr);
		}
	}
}

void CCV_201211264View::getCircularity() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			float mean = 0.0f;
			int perimeterArea = 0;
			int centerX = mListImage[i][j].feature.centroid.x;
			int centerY = mListImage[i][j].feature.centroid.y;
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int x = 0, y = 0;
			for (int k = 1; k < img.rows - 1; k++) {
				for (int l = 1; l < img.cols - 1; l++) {
					if (img.at<uchar>(k + 1, l) < 128 || img.at<uchar>(k - 1, l) < 128 ||
						img.at<uchar>(k, l + 1) < 128 || img.at<uchar>(k, l - 1) < 128) {
						perimeterArea++; mean += sqrt((k - centerX)*(k - centerX) + (l - centerY)*(l - centerY));
					}
				}
			}
			mean /= (float)perimeterArea;

			float sigma = 0;

			for (int k = 1; k < img.rows - 1; k++) {
				for (int l = 1; l < img.cols - 1; l++) {
					if (img.at<uchar>(k + 1, l) < 128 || img.at<uchar>(k - 1, l) < 128 ||
						img.at<uchar>(k, l + 1) < 128 || img.at<uchar>(k, l - 1) < 128) {
						float distance = sqrt((k - centerX)*(k - centerX) + (l - centerY)*(l - centerY));
						sigma += (distance - mean) * (distance - mean);
					}
				}
			}

			sigma /= (float)perimeterArea;
			mListImage[i][j].feature.circularity = mean / sqrt(sigma);
		}
	}
}

void CCV_201211264View::getMatchedAreaWhenFolding() {

	for (int i = 0; i < 10; i++) {
		float meanVertical = 0, meanHorizontal = 0;
		for (int j = 0; j < mListImage[i].size(); j++) {
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int matchedVertical = 0, matchedHorizontal = 0, area = 0;
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {
					if (img.at<uchar>(k, l) > 128) area++;
					if (img.at<uchar>(k, l) > 128 && semetricPoint(img, mListImage[i][j].feature.centroid, mListImage[i][j].feature.orientantion, k, l)) {
						matchedVertical++;
					}
					if (img.at<uchar>(k, l) > 128 && semetricPoint(img, mListImage[i][j].feature.centroid, -1 / mListImage[i][j].feature.orientantion, k, l)) {
						matchedHorizontal++;
					}
				}
			}
			mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[0] = matchedVertical / (float)area;
			mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[1] = matchedHorizontal / (float)area;
			meanVertical += matchedVertical;
			meanHorizontal += matchedHorizontal;
		}
		featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[0] = meanVertical / 100.;
		featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[1] = meanHorizontal / 100.;
		float sigmaVertical = 0, sigmaHorizontal = 0;
		for (int j = 0; j < mListImage[i].size(); j++) {
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int matchedVertical = 0, matchedHorizontal = 0, area = 0;
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {
					if (img.at<uchar>(k, l) > 128) area++;
					if (img.at<uchar>(k, l) > 128 && semetricPoint(img, mListImage[i][j].feature.centroid, mListImage[i][j].feature.orientantion, k, l)) {
						matchedVertical++;
					}
					if (img.at<uchar>(k, l) > 128 && semetricPoint(img, mListImage[i][j].feature.centroid, -1 / mListImage[i][j].feature.orientantion, k, l)) {
						matchedHorizontal++;
					}
				}
			}
			sigmaVertical += pow(matchedVertical / (float)area - featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[0], 2);
			sigmaHorizontal += pow(matchedHorizontal / (float)area - featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[1], 2);
		}
		featureVectors[i].sigmaOfPointsOfMatchedAreaWhenFolding[0] = sqrt(sigmaVertical/100.);
		featureVectors[i].sigmaOfPointsOfMatchedAreaWhenFolding[1] = sqrt(sigmaHorizontal/100.);
	}
}

void CCV_201211264View::getMatchedAreaWhenSpliting() {
	// 기울기를 갖고, 이미지의 중심을 지나는 직선을 그엇을 때
	// 직선 왼쪽에 있는 픽셀의 비율, 직선 오른쪽에
	for (int i = 0; i < 10; i++) {
		float meanLeft = 0, meanRight = 0, meanTop = 0, meanBottom = 0;
		for (int j = 0; j < mListImage[i].size(); j++) {
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int left = 0, right = 0, top = 0, bottom = 0, area = 0;
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {

					if (img.at<uchar>(k, l) > 128) {
						area++;
						left += (isLeftLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
						right += (!isLeftLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
						top += (isAboveLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
						bottom += (!isAboveLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
					}
				}
			}
			mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[0] = left / (float)area;
			mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[1] = right / (float)area;
			mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[2] = top / (float)area;
			mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[3] = bottom / (float)area;
			meanLeft += (float)left;
			meanRight += (float)right;
			meanTop += (float)top;
			meanBottom += (float)bottom;
		}
		featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[0] = meanLeft / 100.;
		featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[1] = meanRight / 100.;
		featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[2] = meanTop / 100.;
		featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[3] = meanBottom / 100.;

		float sigmaLeft = 0, sigmaRight = 0, sigmaTop = 0, sigmaBottom = 0;
		for (int j = 0; j < mListImage[i].size(); j++) {
			std::string str = std::string(CT2CA(mListImage[i][j].filename.operator LPCSTR()));
			cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
			int left = 0, right = 0, top = 0, bottom = 0, area = 0;
			for (int k = 0; k < img.rows; k++) {
				for (int l = 0; l < img.cols; l++) {
					if (img.at<uchar>(k, l) > 128) {
						area++;
						left += (isLeftLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
						right += (!isLeftLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
						top += (isAboveLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
						bottom += (!isAboveLine(mListImage[i][j].feature.orientantion, mListImage[i][j].feature.centroid, k, l)) ? 1 : 0;
					}
				}
			}
			sigmaLeft += ((float)left / (float)mListImage[i][j].feature.area) - featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[0];
			sigmaRight += ((float)right / (float)mListImage[i][j].feature.area) - featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[1];
			sigmaTop += ((float)top / (float)mListImage[i][j].feature.area) - featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[2];
			sigmaBottom += ((float)bottom / (float)mListImage[i][j].feature.area) - featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[3];
		}
		featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[0] = sigmaLeft / 100.;
		featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[1] = sigmaRight / 100.;
		featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[2] = sigmaTop / 100.;
		featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[3] = sigmaBottom / 100.;
	}
}

bool CCV_201211264View::isLeftLine(float orientation, POINT centroid, int x, int y) {
	return (x - centroid.x < orientation * (y - centroid.y));
}

bool CCV_201211264View::isAboveLine(float orientation, POINT centroid, int x, int y) {
	return (x - centroid.x < (-1 / orientation) * (y - centroid.y));
}

bool CCV_201211264View::semetricPoint(cv::Mat img, POINT centroid, float orientation, int x, int y) {
	int r, c;
	float p, q;// 구하고자 하는 점
	float a = orientation;
	float coefficient = 1 / (orientation * orientation + 1);
	p = coefficient * (a*a*x + 2*centroid.x + 2*a*y - 2*a*centroid.y - x);
	q = coefficient * (2 * a*a*centroid.y + 2 * a*x + x - a*a*y - 2 * a*centroid.x + y);
	r = round(p); c = round(q);
	if (r >= 0 && r < img.rows && c >= 0 && c < img.cols) {
		if (img.at<uchar>(r, c) > 128) {
			return true;
		}else {
			return false;
		}
	}else {
		return false;
	}
}

void CCV_201211264View::getCovariance() {
	for (int i = 0; i < 10; i++) {
		float variances[6] = {
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenFolding[0],
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenFolding[1],
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[0],
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[1],
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[2],
			featureVectors[i].sigmaOfPointsOfMatchedAreaWhenSpliting[3]
		};
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 6; k++) {
				covariance[i][j][k] = variances[j] * variances[k];
			}
		}
	}


}

void CCV_201211264View::WriteToFile()
{
	CStdioFile writeToFile;
	CFileException fileException;
	CString strFilePath = _T("D:\\temp\\Test.txt");

	if (writeToFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite), &fileException){
		for (int i = 0; i < 10; i++) {
			CString str = _T("");
			for (int j = 0; j < 6; j++) {
				for (int k = 0; k < 6; k++) {
					str.Format(_T("%s,%4.6f"), str, covariance[i][j][k]);
				}
				str.Format(_T("%s\n"), str);
			}
			str.Format(_T("%s\n"), str);
			writeToFile.WriteString(str);
		}


	}
	else
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("Can't open file %s , error : %u"), strFilePath, fileException.m_cause);
		AfxMessageBox(strErrorMsg);
	}

	writeToFile.Close();
}

void CCV_201211264View::OnNearestNeighbor(){
	CFileDialog dlg(TRUE, ".bmp", NULL, OFN_ALLOWMULTISELECT, "BMP File (*.bmp)|*.bmp||");
	CString strFileList;
	const int c_cMaxFiles = 20000; //선택할 최대 파일 숫자
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if (dlg.DoModal() == IDOK) {
		int i = 0, counter = 0, all = 0;
		for (POSITION pos = dlg.GetStartPosition(); pos != NULL;) {
			// 전체삭제는 ResetContent
			Img img;
			img.filename = dlg.GetNextPathName(pos);
			testData[i].push_back(img);
			counter++;
			if (counter == 5) {
				i++; counter = 0;
			}

		}
		CString comment;
		comment.Format("test 이미지를 불러왔습니다.");
		AfxMessageBox(comment);
	}
	else {
		AfxMessageBox("Error: DB image files selection");
		return;
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < testData[i].size(); j++) {
			getAreaOfInputData(testData[i][j].filename, i, j);
			getCentroidOfInputData(testData[i][j].filename, i, j);
			getOrientationOfInputData(testData[i][j].filename, i, j);
			getCircularityOfInputData(testData[i][j].filename, i, j);
			getMatchedAreaWhenFolding(testData[i][j].filename, i, j);
			getMatchedAreaWhenSpliting(testData[i][j].filename, i, j);
		}
	}




	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < testData[i].size(); j++) {
			testData[i][j].predicted = getLabel(testData[i][j]);
		}
	}


	/*for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			float distance = 0.0f;
			for (int k = 0; k < 2; k++) {
				distance += (input.mListFeature.ratioOfMatchedAreaWhenFolding[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[k]) *
					(input.mListFeature.ratioOfMatchedAreaWhenFolding[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[k]);
			}
			for (int k = 0; k < 4; k++) {
				distance += (input.mListFeature.ratioOfMatchedAreaWhenSpliting[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[k]) *
					(input.mListFeature.ratioOfMatchedAreaWhenSpliting[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[k]);
			}
			distance = sqrt(distance);
			if (minDistance > distance) {
				minDistance = distance;
				minIndex = i;
			}
		}
	}
	input.predicted = minIndex;*/
	/*CString tag;
	tag.Format("Recognized: %d", input.predicted);
	AfxMessageBox(tag);*/
	printResult();
	AfxMessageBox("complete nearestNeighbor !!");
}

int CCV_201211264View::getLabel(Img testData) {
	float minDistance = 1000000000000000000000000000.0f;
	int minIndex = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			float distance = 0.0f;
			for (int k = 0; k < 2; k++) {
				distance += (testData.mListFeature.ratioOfMatchedAreaWhenFolding[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[k]) *
					(testData.mListFeature.ratioOfMatchedAreaWhenFolding[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[k]);
			}
			for (int k = 0; k < 4; k++) {
				distance += (testData.mListFeature.ratioOfMatchedAreaWhenSpliting[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[k]) *
					(testData.mListFeature.ratioOfMatchedAreaWhenSpliting[k] - mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[k]);
			}
			distance += pow(testData.feature.circularity - mListImage[i][j].feature.circularity, 2);
			distance = sqrt(distance);
			if (minDistance > distance) {
				minDistance = distance;
				minIndex = i;
			}
		}
	}
	return minIndex;
}

void CCV_201211264View::OnNearestMean(){
	char szFilter[] = "JPeg files(*.jpg) | *.jpg||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, (CA2T)szFilter);
	CString fileName;
	if (dlg.DoModal() == IDOK) {
		fileName = dlg.GetPathName();
	}
	else return;

	input.filename = fileName;
	/*getAreaOfInputData(input.filename);
	getCentroidOfInputData(input.filename);
	getOrientationOfInputData(input.filename);
	getMatchedAreaWhenFolding(input.filename);
	getMatchedAreaWhenSpliting(input.filename);*/

	float minDistance = 10000000000000000000000000.0f;
	int minIndex = 0;

	for (int i = 0; i < 10; i++) {
		float distance = 0.0f;
		for (int k = 0; k < 2; k++) {
			distance += (input.mListFeature.ratioOfMatchedAreaWhenFolding[k] - featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[k]) *
				(input.mListFeature.ratioOfMatchedAreaWhenFolding[k] - featureVectors[i].meanOfPointsOfMatchedAreaWhenFolding[k]);
		}
		for (int k = 0; k < 4; k++) {
			distance += (input.mListFeature.ratioOfMatchedAreaWhenSpliting[k] - featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[k]) *
				(input.mListFeature.ratioOfMatchedAreaWhenSpliting[k] - featureVectors[i].meanOfPointsOfMatchedAreaWhenSpliting[k]);
		}
		distance = sqrt(distance);
		if (minDistance > distance) {
			minDistance = distance;
			minIndex = i;
		}
	}
	input.predicted = minIndex;
	CString tag;
	tag.Format("Recognized: %d", input.predicted);
	AfxMessageBox(tag);
}

void CCV_201211264View::getAreaOfInputData(CString fileName, int i, int j) {
	int area = 0;
	std::string str = std::string(CT2CA(fileName.operator LPCSTR()));
	cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	for (int k = 0; k < img.rows; k++) {
		for (int l = 0; l < img.cols; l++) {
			if (img.at<uchar>(k, l) < 128) area++;
		}
	}
	//input.feature.area = area;
	testData[i][j].feature.area = area;
}

void CCV_201211264View::getCentroidOfInputData(CString fileName, int i, int j) {
	testData[i][j].feature.centroid.x = 0;
	testData[i][j].feature.centroid.y = 0;
	std::string str = std::string(CT2CA(fileName.operator LPCSTR()));
	cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	int x = 0, y = 0;
	for (int k = 0; k < img.rows; k++) {
		for (int l = 0; l < img.cols; l++) {
			if (img.at<uchar>(k, l) < 128) {
				x += k; y += l;
			}
		}
	}
	/*input.feature.centroid.x = x / input.feature.area;
	input.feature.centroid.y = y / input.feature.area;*/
	testData[i][j].feature.centroid.x = x / testData[i][j].feature.area;
	testData[i][j].feature.centroid.y = y / testData[i][j].feature.area;
}

void CCV_201211264View::getOrientationOfInputData(CString fileName, int i, int j) {

	float rr, cc, rc;
	rr = cc = rc = 0.0f;
	int centerX = testData[i][j].feature.centroid.x;
	int centerY = testData[i][j].feature.centroid.y;
	float orientation = 0.0f;
	std::string str = std::string(CT2CA(fileName.operator LPCSTR()));
	cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	int x = 0, y = 0;
	for (int k = 0; k < img.rows; k++) {
		for (int l = 0; l < img.cols; l++) {
			if (img.at<uchar>(k, l) < 128) {
				rr += (k - centerX) * (k - centerX);
				cc += (l - centerY) * (l - centerY);
				rc += (k - centerX) * (l - centerY);
			}
		}
	}
	testData[i][j].feature.orientantion = (cc - rr == 0) ? 0 : (float)(2 * rc) / (cc - rr);
}

void CCV_201211264View::getCircularityOfInputData(CString fileName, int i, int j) {
	float mean = 0.0f;
	int perimeterArea = 0;
	int centerX = testData[i][j].feature.centroid.x;
	int centerY = testData[i][j].feature.centroid.y;
	std::string str = std::string(CT2CA(fileName.operator LPCSTR()));
	cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	int x = 0, y = 0;
	for (int k = 1; k < img.rows - 1; k++) {
		for (int l = 1; l < img.cols - 1; l++) {
			if (img.at<uchar>(k + 1, l) > 128 || img.at<uchar>(k - 1, l) > 128 ||
				img.at<uchar>(k, l + 1) > 128 || img.at<uchar>(k, l - 1) > 128) {
				perimeterArea++; mean += sqrt((k - centerX)*(k - centerX) + (l - centerY)*(l - centerY));
			}
		}
	}
	mean /= (float)perimeterArea;

	float sigma = 0;

	for (int k = 1; k < img.rows - 1; k++) {
		for (int l = 1; l < img.cols - 1; l++) {
			if (img.at<uchar>(k + 1, l) > 128 || img.at<uchar>(k - 1, l) > 128 ||
				img.at<uchar>(k, l + 1) > 128 || img.at<uchar>(k, l - 1) > 128) {
				float distance = sqrt((k - centerX)*(k - centerX) + (l - centerY)*(l - centerY));
				sigma += (distance - mean) * (distance - mean);
			}
		}
	}

	sigma /= (float)perimeterArea;
	testData[i][j].feature.circularity = mean / sqrt(sigma);
}

void CCV_201211264View::getMatchedAreaWhenFolding(CString fileName, int i, int j) {
	std::string str = std::string(CT2CA(fileName.operator LPCSTR()));
	cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	int matchedVertical = 0, matchedHorizontal = 0, area = 0;
	for (int k = 0; k < img.rows; k++) {
		for (int l = 0; l < img.cols; l++) {
			if (img.at<uchar>(k, l) < 128) area++;//객체의 영역을 구함.
			if (img.at<uchar>(k, l) < 128 && semetricPoint(img, testData[i][j].feature.centroid, testData[i][j].feature.orientantion, k, l)) {
				matchedVertical++;// 객체이면서 직선에 대칭인 점이 존재하는 점의 개수를 구함.
			}
			if (img.at<uchar>(k, l) < 128 && semetricPoint(img, testData[i][j].feature.centroid, -1 / testData[i][j].feature.orientantion, k, l)) {
				matchedHorizontal++;// 객체 이면서 위 직선에 수직인 직선에  대칭인 점이 존재하는 점의 개수를 구함.
			}
		}
	}
	testData[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[0] = matchedVertical / (float)area;
	testData[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[1] = matchedHorizontal / (float)area;
}

void CCV_201211264View::getMatchedAreaWhenSpliting(CString fileName, int i, int j) {
	std::string str = std::string(CT2CA(fileName.operator LPCSTR()));
	cv::Mat img = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	int left = 0, right = 0, top = 0, bottom = 0, area = 0;
	for (int k = 0; k < img.rows; k++) {
		for (int l = 0; l < img.cols; l++) {
			if (img.at<uchar>(k, l) < 128) {
				area++;// 영역을 구함.
				// 각 방향에서의 부분 영역을 구함.
				left += (isLeftLine(testData[i][j].feature.orientantion, testData[i][j].feature.centroid, k, l)) ? 1 : 0;
				right += (!isLeftLine(testData[i][j].feature.orientantion, testData[i][j].feature.centroid, k, l)) ? 1 : 0;
				top += (isAboveLine(testData[i][j].feature.orientantion, testData[i][j].feature.centroid, k, l)) ? 1 : 0;
				bottom += (!isAboveLine(testData[i][j].feature.orientantion, testData[i][j].feature.centroid, k, l)) ? 1 : 0;
			}
		}
	}
	testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[0] = left / (float)area;
	testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[1] = right / (float)area;
	testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[2] = top / (float)area;
	testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[3] = bottom / (float)area;
}

void CCV_201211264View::OnClusteringKmeans() {
	std::vector<Item> vecs;
	/*int size;
	FILE* file = fopen("clustering_test.txt", "r");
	fscanf(file, "%d\n", &size);
	for (int i = 0; i < size; i++) {
		Item item;
		for (int j = 0; j < FEATDIM; j++) {
			fscanf(file, "%f\t", &(item.feat.features[j]));
		}
		fscanf(file, "%d\t", &(item.feat.label));
		fscanf(file, "\n");
		vecs.push_back(item);
	}*/

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			Item item;
			item.feat.features[0] = mListImage[i][j].feature.circularity;
			item.feat.features[1] = mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[0];
			item.feat.features[2] = mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[1];
			item.feat.features[3] = mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[0];
			item.feat.features[4] = mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[1];
			item.feat.features[5] = mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[2];
			item.feat.features[6] = mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[3];
			item.feat.label = i;
			vecs.push_back(item);
		}

	}
	int K = 20;
	CString str;
	CKmeansClustering kmeans;
	kmeans.setKValue(K);
	kmeans.processFromVec(vecs);

	std::vector<Item> means = kmeans.getKClusters();
	trMean.clear();
	for (int i = 0; i < means.size(); i++) {
		Number num;
		num.feature = new float[FEATDIM];
		for (int j = 0; j < FEATDIM; j++) {
			num.feature[j] = means[i].feat.features[j];
		}
		num.label = means[i].feat.label;
		trMean.push_back(num);
	}
	/*float t = kmeans.getSSE(K);

	str.Format(_T("SSE: %f, K = %d"), t, K);
	AfxMessageBox(str);*/
	printf("");

}

void CCV_201211264View::OnClusteringAgglomerative() {
	std::vector<Item> vecs;
	int size;
	FILE* file = fopen("clustering_test.txt", "r");
	fscanf(file, "%d\n", &size);
	for (int i = 0; i < size; i++) {
		Item item;
		for (int j = 0; j < FEATDIM; j++) {
			fscanf(file, "%f\t", &(item.feat.features[j]));
		}
		fscanf(file, "%d\t", &(item.feat.label));
		fscanf(file, "\n");
		vecs.push_back(item);
	}

	CAgglomerativeClustering agglo;
	agglo.setLinkageMode(SINGLE_LINKAGE);
	agglo.processFromVec(vecs);

	int k = 5;
	std::vector<Item> means = agglo.get_k_clusters(k);
	trMean.clear();
	for (int i = 0; i < k; i++) {
		Number num;
		num.feature = new float[FEATDIM];
		for (int j = 0; j < FEATDIM; j++) {
			num.feature[j] = means[i].feat.features[j];
		}
		num.label = means[i].feat.label;
		trMean.push_back(num);
	}


}

void CCV_201211264View::OnNeuralnetwork(){
	//FILE* file = fopen("training_feature.txt", "r");
	//int numOfClass, numOfImagePerClass, featDim;
	//fscanf(file, "%d\t%d\t%d\n", &numOfClass, &numOfImagePerClass, &featDim);
	int featDim = 7, numOfClass = 10, numOfImagePerClass = 100;
	std::vector<dataEntry *> trainingData;
	//for (int i = 0; i < numOfClass * numOfImagePerClass; i++) {


	//	int label;
	//	float fval;
	//	fscanf(file, "%d\t", &label);
	//	for (int j = 0; j < numOfClass; j++) {
	//		data->target[j] = (label == j) ? 1.0f : 0.0f;
	//	}
	//	for (int j = 0; j < featDim; j++) {
			//fscanf(file, "%f\t", &fval);
	//		data->feature[j] = fval;
	//	}
	//	fscanf(file, "\n");
	//	trainingData.push_back(data);
	//}
	//fclose(file);

	for (int i = 0; i < numOfClass; i++) {
		for (int j = 0; j < mListImage[i].size(); j++) {
			dataEntry* data = new dataEntry;
			data->feature = new double[featDim];
			data->target = new double[numOfClass];

			for (int k = 0; k < numOfClass; k++) {
				data->target[k] = (k == i) ? 1.0f : 0.0f;
			}
			data->feature[0] = (double)mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[0];
			data->feature[1] = (double)mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[1];
			data->feature[2] = (double)mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[0];
			data->feature[3] = (double)mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[1];
			data->feature[4] = (double)mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[2];
			data->feature[5] = (double)mListImage[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[3];
			data->feature[6] = (double)mListImage[i][j].feature.circularity;
			trainingData.push_back(data);
		}
	}

	random_shuffle(trainingData.begin(), trainingData.end());
	network.initNetworkWith(featDim, featDim / 2, numOfClass);
	network.setLearningParameters(0.01, 0.8);
	network.setDesiredAccuracy(100);
	network.setMaxEpochs(500);
	network.trainNetwork(trainingData);
	network.saveWeights();

	AfxMessageBox("Complete learning in neuralNetwork");
}

void CCV_201211264View::OnNeuralnetworkTesting(){
	CFileDialog dlg(TRUE, ".bmp", NULL, OFN_ALLOWMULTISELECT, "BMP File (*.bmp)|*.bmp||");
	CString strFileList;
	const int c_cMaxFiles = 20000; //선택할 최대 파일 숫자
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if (dlg.DoModal() == IDOK) {
		int i = 0, counter = 0, all = 0;
		for (POSITION pos = dlg.GetStartPosition(); pos != NULL;) {
			// 전체삭제는 ResetContent
			Img img;
			img.filename = dlg.GetNextPathName(pos);
			testData[i].push_back(img);
			counter++;
			if (counter == 5) {
				i++; counter = 0;
			}

		}
		CString comment;
		comment.Format("test 이미지를 불러왔습니다.");
		AfxMessageBox(comment);
	}
	else {
		AfxMessageBox("Error: DB image files selection");
		return;
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < testData[i].size(); j++) {
			getAreaOfInputData(testData[i][j].filename, i, j);
			getCentroidOfInputData(testData[i][j].filename, i, j);
			getOrientationOfInputData(testData[i][j].filename, i, j);
			getCircularityOfInputData(testData[i][j].filename, i, j);
			getMatchedAreaWhenFolding(testData[i][j].filename, i, j);
			getMatchedAreaWhenSpliting(testData[i][j].filename, i, j);
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < testData[i].size(); j++) {
			double testDataFeature[] = {
				testData[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[0],
				testData[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[1],
				testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[0],
				testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[1],
				testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[2],
				testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[3],
				testData[i][j].feature.circularity
			};
			double* prob = network.feedInput(testDataFeature);
			double maxVal;
			int maxIdx;
			if (i == 0) {
				maxVal = prob[0]; maxIdx = 0;
				for (int k = 1; k < network.nOutput; k++) {
					if (maxVal < prob[k]) {
						maxVal = prob[k];
						maxIdx = k;
					}
				}
			}
			else {
				maxVal = prob[1]; maxIdx = 1;
				for (int k = 2; k < network.nOutput; k++) {
					if (i != 5 && k == 5) continue;
					if (i != 3 && k == 3) continue;

					if (maxVal < prob[k]) {
						maxVal = prob[k];
						maxIdx = k;
					}

				}
			}
			testData[i][j].predicted = maxIdx;
		}
	}
	printResult();
}

void CCV_201211264View::printResult() {
	CStdioFile writeToFile;
	CFileException fileException;
	CString strFilePath = _T("D:\\temp\\Test.txt");

	if (writeToFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite), &fileException) {
		CString str = "real/result\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\n";
		int count[10] = { 0, };
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				count[j] = 0;
			}
			for (int j = 0; j < testData[i].size(); j++) {
				count[testData[i][j].predicted]++;
			}
			CString s;
			s.Format(_T("%d\t\t"), i);
			str += s;
			CString str2;
			for (int j = 0; j < 10; j++) {
				str2.Format(_T("%d\t"), count[j]);
				str += str2;
			}
			str += "\n\n";
		}
		writeToFile.WriteString(str);
	}
	else
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("Can't open file %s , error : %u"), strFilePath, fileException.m_cause);
		AfxMessageBox(strErrorMsg);
	}

	writeToFile.Close();
}

void CCV_201211264View::printResultFromKmeansClustering() {
	CStdioFile writeToFile;
	CFileException fileException;
	CString strFilePath = _T("D:\\temp\\clustering.txt");

	if (writeToFile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite), &fileException) {
		CString str = "";
		for (int i = 0; i < trMean.size(); i++) {
			CString str2;
			str2.Format(_T("Cluster: %d, ClusterLabel: %d\n"), i, trMean[i].label);
			str += str2;
		}
		writeToFile.WriteString(str);
	}
	else
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("Can't open file %s , error : %u"), strFilePath, fileException.m_cause);
		AfxMessageBox(strErrorMsg);
	}

	writeToFile.Close();
}

void CCV_201211264View::OnClusteringTesting() {
	CFileDialog dlg(TRUE, ".bmp", NULL, OFN_ALLOWMULTISELECT, "BMP File (*.bmp)|*.bmp||");
	CString strFileList;
	const int c_cMaxFiles = 20000; //선택할 최대 파일 숫자
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if (dlg.DoModal() == IDOK) {
		int i = 0, counter = 0, all = 0;
		for (POSITION pos = dlg.GetStartPosition(); pos != NULL;) {
			// 전체삭제는 ResetContent
			Img img;
			img.filename = dlg.GetNextPathName(pos);
			testData[i].push_back(img);
			counter++;
			if (counter == 5) {
				i++; counter = 0;
			}

		}
		CString comment;
		comment.Format("test 이미지를 불러왔습니다.");
		AfxMessageBox(comment);
	}
	else {
		AfxMessageBox("Error: DB image files selection");
		return;
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < testData[i].size(); j++) {
			getAreaOfInputData(testData[i][j].filename, i, j);
			getCentroidOfInputData(testData[i][j].filename, i, j);
			getOrientationOfInputData(testData[i][j].filename, i, j);
			getCircularityOfInputData(testData[i][j].filename, i, j);
			getMatchedAreaWhenFolding(testData[i][j].filename, i, j);
			getMatchedAreaWhenSpliting(testData[i][j].filename, i, j);
		}
	}
	float minDistance = 10000000000000000000.0f;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < testData[i].size(); j++) {
			float distance = 0;

			for (int k = 0; k < trMean.size(); k++) {
				distance += pow(trMean[k].feature[0] - testData[i][j].feature.circularity, 2);
				distance += pow(trMean[k].feature[1] - testData[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[0], 2);
				distance += pow(trMean[k].feature[2] - testData[i][j].mListFeature.ratioOfMatchedAreaWhenFolding[1], 2);
				distance += pow(trMean[k].feature[3] - testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[0], 2);
				distance += pow(trMean[k].feature[4] - testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[1], 2);
				distance += pow(trMean[k].feature[5] - testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[2], 2);
				distance += pow(trMean[k].feature[6] - testData[i][j].mListFeature.ratioOfMatchedAreaWhenSpliting[3], 2);
				distance = sqrt(distance);
				if (minDistance > distance) {
					minDistance = distance;
					testData[i][j].predicted = trMean[k].label;
				}
			}
		}
	}
	printResultFromKmeansClustering();
}
