#pragma once

//신경망 코드 구현에 필요한 헤더파일
#include <iostream>
#include <math.h>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

//기본 학습 파라미터
#define LEARNING_RATE 0.001 //학습율
#define MOMENTUM 0.9 //모멘트
#define MAX_EPOCHS 1500 //최대 반복수
#define DESIRED_ACCURACY 90  //원하는 정확도

//학습 데이터를 저장할 구조체
typedef struct dataEntry
{
	double* feature; //특징벡터
	double* target; //라벨
} dataEntry;

class CNeuralNetwork
{
public:
	//학습 파라미터
	double learningRate;
	double momentum;

	//반복회수를 저장하는 변수
	long epoch;
	long maxEpochs;

	//목표 정확도
	double desiredAccuracy;

	//각 레이어에 해당하는 노드 갯수를 가짐
	int nInput, nHidden, nOutput;

	//각 레이어의 노드 값을 저장하는 배열
	double* inputNeurons;
	double* hiddenNeurons;
	double* outputNeurons;

	//weights
	double** wInputHidden;
	double** wHiddenOutput;

	//갱신해야 하는 weight 값을 저장하는 배열
	double** deltaInputHidden;
	double** deltaHiddenOutput;

	//에러율 계산
	double* hiddenErrorGradients;
	double* outputErrorGradients;

	//각 단계별 정확도를 계산
	double trainingSetAccuracy;

	//생성자
	CNeuralNetwork() :epoch(0), trainingSetAccuracy(0)
	{
		inputNeurons = nullptr;
		hiddenNeurons = nullptr;
		outputNeurons = nullptr;
		wInputHidden = nullptr;
		wHiddenOutput = nullptr;
		deltaInputHidden = nullptr;
		deltaHiddenOutput = nullptr;
		hiddenErrorGradients = nullptr;
		outputErrorGradients = nullptr;
	};
	~CNeuralNetwork(); //소멸자

	//신경망 구현에 필요한 멤버 함수들

	//노드 개수가 주어졌을 때 신경망을 초기화하는 함수
	void initNetworkWith(int in, int hidden, int out);
	//학습 파라미터(학습률, 모멘텀)을 설정하는 함수
	void setLearningParameters(double lr, double m);
	//최대 반복횟수를 설정하는 함수
	void setMaxEpochs(int max);
	//원하는 정확도를 입력하는 함수
	void setDesiredAccuracy(float d);
	//웨이트를 새로 초기화하는 함수
	void resetWeights();
	//반올림
	int getRoundedOutputValue(double x);
	//신경망 웨이트 저장 및 로드 함수
	void saveWeights();
	void loadWeights();


	//Prediction을 수행하여 예측 확률을 리턴하는 함수
	double* feedInput(double* inputs);
	//신경망 학습을 수행하는 함수 (runTrainEpoch 호출)
	void trainNetwork(vector<dataEntry*> trainingSet);
	//신경망 웨이트를 초기화하는 함수
	void initializeWeights();
	//학습데이터가 주어졌을 때 실제 학습을 수행하는 함수
	void runTrainingEpoch(vector<dataEntry*> trainingSet);
	//Forward 연산 수행하는 함수(입력→출력 계산)
	void feedForward(double *inputs);
	//Backword 연산 수행하는 함수(바꿔야 할 만큼의 값 계산)
	void backpropagate(double* desiredValues);
	//웨이트 업데이트하는 함수
	void updateWeights();
	//시그모이드 함수
	double activationFunction(double x);
	//출력층 에러값 계산하는 함수
	double getOutputErrorGradient(double desiredValue, double outputValue);
	//은닉층 에러값 계산하는 함수
	double getHiddenErrorGradient(int j);


};
