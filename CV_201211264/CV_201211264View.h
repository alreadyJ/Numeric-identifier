
// CV_201211264View.h : CCV_201211264View 클래스의 인터페이스
//
#include "opencv\cv.h"
#include "opencv2\opencv.hpp"
#include <vector>
#include "NeuralNetwork.h"
#pragma once


enum Process { BMPLOAD = 0, BINARY, DILATION, EROSION, OPENING , CLOSING, CUSTOMOTSU, DILATIONSPECIAL,
	EROSIONSPECIAL, ROWBYROW, RUNLENGTH, BOUNDINGBOX};


typedef struct Feature {
	int area, color;
	float circularity, perimeter, orientantion;
	POINT centroid;
	cv::Rect r;
};
typedef struct RowInfo {
	int rowNum, startCol, endCol, label;
};
typedef struct mListFeature {
	float ratioOfMatchedAreaWhenFolding[2];
	float ratioOfMatchedAreaWhenSpliting[4];
};
typedef struct FeatureVector {
	float meanOfPointsOfMatchedAreaWhenFolding[2];
	float sigmaOfPointsOfMatchedAreaWhenFolding[2];
	float meanOfPointsOfMatchedAreaWhenSpliting[4];
	float sigmaOfPointsOfMatchedAreaWhenSpliting[4];
};

typedef struct Img {
	CString filename;
	Feature feature;
	mListFeature mListFeature;
	int predicted;
};

typedef struct Number {
	float* feature;
	int label;
	int predicted;
};

class CCV_201211264View : public CView
{
protected: // serialization에서만 만들어집니다.
	CCV_201211264View();
	DECLARE_DYNCREATE(CCV_201211264View)

// 특성입니다.
public:
	CCV_201211264Doc* GetDocument() const;

// 작업입니다.
public:
	const int GRAYUPPER = 256;
	int MASK_SIZE;
	RGBQUAD** rgbBuffer;
	int height, width, viewType, otsuThreshold;
	float** hue, **satu, **inten;
	int** binary, **mor, **opening, **closing, **dilationSpecial, **erosionSpecial;
	int* histogram, *otsuThresholds;
	bool openingFlag, closingFlag, dilationSpecialCase, erosionSpecialCase;
	int **labelBuffer;
	int *unionFind, *labels;
	float covariance[10][6][6];
	FeatureVector* featureVectors;
	std::vector<Feature> features;
	std::vector<Img>* mListImage;
	std::vector<Img>* testData;
	std::vector<Number> trData;
	std::vector<Number> trMean;
	int trNum;
	FeatureVector meanAndSigmaOfInput;
	Img input;
	CNeuralNetwork network;
	float SSE;
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CCV_201211264View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	int** OnProcessingDilation(int** input, int** structElement);
	int** OnProcessingErosion(int** input, int** structElement);
	void RGBToHSI();
	int ufMakeSet();
	int ufUnion(int x, int y);
	int ufFind(int x);
	float selectedArea(int x1, int y1, int x2, int y2);
	void getCentroid();
	void getArea();
	void getOrientation();
	void getCircularity();
	int getLabel(Img testData);
	void getCentroidOfInputData(CString fileName, int i, int j);
	void getAreaOfInputData(CString fileName, int i, int j);
	void getOrientationOfInputData(CString fileName, int i, int j);
	void getCircularityOfInputData(CString fileName, int i, int j);
	void getMatchedAreaWhenFolding();
	void getMatchedAreaWhenSpliting();
	void getMatchedAreaWhenFolding(CString fileName, int i, int j);
	void getMatchedAreaWhenSpliting(CString fileName, int i, int j);
	bool isLeftLine(float orientation, POINT centroid, int x, int y);
	bool isAboveLine(float orientation, POINT centroid, int x, int y);
	bool semetricPoint(cv::Mat img, POINT centroid, float orientation, int x, int y);
	void getCovariance();
	void WriteToFile();
	void printResult();
	void printResultFromKmeansClustering();
	afx_msg void OnImageloadBmpload();
	afx_msg void OnImageloadJpegload();
	afx_msg void OnProcessingOtsumethod();	
	afx_msg void OnProcessingMorDilation();
	afx_msg void OnProcessingMorerosion();
	afx_msg void OnProcessingOpening();
	afx_msg void OnProcessingClosing();
	afx_msg void OnProcessingCustermized();
	afx_msg void OnProcessingMordilationwithspecialcase();
	afx_msg void OnProcessingMorerosionwithspecialcase();
	afx_msg void OnLabelingRow();
	afx_msg void OnLabelingRun();
	afx_msg void OnLabelingLabelingwitharea();
	afx_msg void OnLabelingLabelingwithcentroid();
	afx_msg void OnLabelingLabelingwithboundingbox();
	afx_msg void OnLabelingLabelingwithcircularity();
	afx_msg void OnLabelingLabelingwithorientation();
	afx_msg void OnExtractfeatureExtract();
	afx_msg void OnNearestNeighbor();
	afx_msg void OnNearestMean();
	afx_msg void OnNeuralnetwork();
	afx_msg void OnClusteringKmeans();
	afx_msg void OnClusteringAgglomerative();
	afx_msg void OnNeuralnetworkTesting();
	afx_msg void OnClusteringTesting();
};

#ifndef _DEBUG  // CV_201211264View.cpp의 디버그 버전
inline CCV_201211264Doc* CCV_201211264View::GetDocument() const
   { return reinterpret_cast<CCV_201211264Doc*>(m_pDocument); }
#endif

