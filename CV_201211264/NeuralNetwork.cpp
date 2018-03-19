#include "stdafx.h"
#include "NeuralNetwork.h"

void CNeuralNetwork::initNetworkWith(int in, int hidden, int out){

	nInput = in; //입력노드 개수 입력
	nHidden = hidden; //은닉노드 개수 입력
	nOutput = out; //출력노드 개수 입력

	//레이어 구성
	inputNeurons = new(double[in + 1]); //입력 노드 할당
	for (int i = 0; i < in; i++) inputNeurons[i] = 0; //초기화
	inputNeurons[in] = 1;	//Bias 노드 1로 초기화

	hiddenNeurons = new(double[hidden + 1]); //은닉 노드 할당
	for (int i = 0; i < hidden; i++) hiddenNeurons[i] = 0; //초기화
	hiddenNeurons[hidden] = 1;	//Bias 노드 1로 초기화

	outputNeurons = new(double[out]); //출력 노드 할당
	for (int i = 0; i < out; i++) outputNeurons[i] = 0; //초기화

	//웨이트를 저장할 배열 할당 및 초기화
	wInputHidden = new(double*[in + 1]); //입력 => 은닉
	for (int i = 0; i <= in; i++)
	{
		wInputHidden[i] = new (double[hidden]);
		for (int j = 0; j < hidden; j++) wInputHidden[i][j] = 0;
	}

	wHiddenOutput = new(double*[hidden + 1]); //은닉 => 출력
	for (int i = 0; i <= hidden; i++)
	{
		wHiddenOutput[i] = new (double[out]);
		for (int j = 0; j < out; j++) wHiddenOutput[i][j] = 0;
	}

	//에러율에 따른 웨이트 변화량을 계산할 배열 할당 및 초기화
	deltaInputHidden = new(double*[in + 1]);
	for (int i = 0; i <= in; i++)
	{
		deltaInputHidden[i] = new (double[hidden]);
		for (int j = 0; j < hidden; j++) deltaInputHidden[i][j] = 0;
	}

	deltaHiddenOutput = new(double*[hidden + 1]);
	for (int i = 0; i <= hidden; i++)
	{
		deltaHiddenOutput[i] = new (double[out]);
		for (int j = 0; j < out; j++) deltaHiddenOutput[i][j] = 0;
	}

	//출력, 은닉층의 에러값을 저장하는 배열 할당 및 초기화
	hiddenErrorGradients = new(double[hidden + 1]);
	for (int i = 0; i <= hidden; i++) hiddenErrorGradients[i] = 0;

	outputErrorGradients = new(double[out + 1]);
	for (int i = 0; i <= out; i++) outputErrorGradients[i] = 0;

	//웨이트값 임의의 값으로 초기화하는 함수 호출
	initializeWeights();

	//학습 파라미터 설정(default값으로/추후 Set함수 호출하여 변경 가능)
	learningRate = LEARNING_RATE;
	momentum = MOMENTUM;

	//종료 조건 설정(default값으로/추후 Set함수 호출하여 변경 가능)
	maxEpochs = MAX_EPOCHS;
	desiredAccuracy = DESIRED_ACCURACY;
}

//소멸자
CNeuralNetwork::~CNeuralNetwork()
{
	//각 레이어 노드 해제
	if (inputNeurons != nullptr){
		delete[] inputNeurons;
		delete[] hiddenNeurons;
		delete[] outputNeurons;
	}

	//웨이트 저장 배열 해제
	if (wInputHidden != nullptr){
		for (int i = 0; i <= nInput; i++)
			delete[] wInputHidden[i];
		delete[] wInputHidden;

		for (int j = 0; j <= nHidden; j++)
			delete[] wHiddenOutput[j];
		delete[] wHiddenOutput;
	}

	//웨이트 변화량 저장 배열 해제
	if (deltaInputHidden != nullptr){
		for (int i = 0; i <= nInput; i++)
			delete[] deltaInputHidden[i];
		delete[] deltaInputHidden;

		for (int j = 0; j <= nHidden; j++)
			delete[] deltaHiddenOutput[j];
		delete[] deltaHiddenOutput;
	}

	//에러값 저장 배열 해제
	if (hiddenErrorGradients != nullptr){
		delete[] hiddenErrorGradients;
		delete[] outputErrorGradients;
	}

}

//학습 파라미터 설정
void CNeuralNetwork::setLearningParameters(double lr, double m){
	learningRate = lr;
	momentum = m;
}

//파라미터 설정 함수
void CNeuralNetwork::setMaxEpochs(int max) {//학습 종료 조건 ==> 최대 반복횟수
	maxEpochs = max;
}

void CNeuralNetwork::setDesiredAccuracy(float d){//학습 종료 조건 ==> 목표 정확도
	desiredAccuracy = d;
}

//신경망 재설정시
void CNeuralNetwork::resetWeights(){
	//웨이트 초기화
	initializeWeights();
}

//데이터 입력받아서 output 결과를 리턴하는 함수
double* CNeuralNetwork::feedInput(double* inputs){
	//feed data into the network
	feedForward(inputs);
	//return results
	return outputNeurons;
}

void CNeuralNetwork::trainNetwork(vector<dataEntry*> trainingSet){
	//반복횟수 초기화
	epoch = 0;
	//종료조건을 만족할때까지 반복 (목표 정확도를 만족 & 최대 종료 횟수)
	while ((trainingSetAccuracy < desiredAccuracy) && epoch < maxEpochs){
		//학습 수행 (입력: 학습데이터)
		runTrainingEpoch(trainingSet);
		//학습 정확도 Textfile 출력
		ofstream  outFile;
		outFile.open("nn_train_result.txt", ios::app);
		outFile << epoch << "\t" << trainingSetAccuracy << "\%" <<endl;
		outFile.close();
		//반복횟수 증가
		epoch++;
	}
}

//학습한 네트워크 정보 저장
void CNeuralNetwork::saveWeights(){
	ofstream  outFile;
	outFile.open("network.txt", ios::out);
	outFile << nInput << "\t" << nHidden << "\t" << nOutput << endl;
	for (int i = 0; i <= nInput; i++){
		for (int j = 0; j < nHidden; j++){
			outFile << wInputHidden[i][j] << "\t";
		}
	}

	for (int i = 0; i <= nHidden; i++){
		for (int j = 0; j < nOutput; j++){
			outFile << wHiddenOutput[i][j] << "\t";
		}
	}

	outFile.close();
}

//학습한 네트워크 정보 불러오기
void CNeuralNetwork::loadWeights(){
	//이미 할당된 데이터 메모리 해제
	if (wInputHidden != nullptr)
	{
		for (int j = 0; j < nInput; j++)
			delete[] wInputHidden[j];
		delete[] wInputHidden;
	}

	if (wHiddenOutput != nullptr)
	{
		for (int j = 0; j < nHidden; j++)
			delete[] wHiddenOutput[j];
		delete[] wHiddenOutput;
	}

	if (inputNeurons != nullptr)
		delete[] inputNeurons;
	if (outputNeurons != nullptr)
		delete[] outputNeurons;

	//텍스트로부터 정보 얻기
	ifstream inFile;
	inFile.open("network.txt", ios::out);
	inFile >> nInput >> nHidden >> nOutput;

	//레이어 구성
	inputNeurons = new(double[nInput + 1]); //입력 노드 할당
	for (int i = 0; i < nInput; i++)
		inputNeurons[i] = 0; //초기화
	//Bias 노드 1로 초기화
	inputNeurons[nInput] = 1;

	hiddenNeurons = new(double[nHidden + 1]); //은닉 노드 할당
	for (int i = 0; i < nHidden; i++)
		hiddenNeurons[i] = 0; //초기화

	//Bias 노드 1로 초기화
	hiddenNeurons[nHidden] = 1;

	outputNeurons = new(double[nOutput]); //출력 노드 할당
	for (int i = 0; i < nOutput; i++)
		outputNeurons[i] = 0; //초기화

	//웨이트를 저장할 배열 할당 및 초기화
	wInputHidden = new(double*[nInput + 1]); //입력 => 은닉
	for (int i = 0; i <= nInput; i++){
		wInputHidden[i] = new (double[nHidden]);
		for (int j = 0; j < nHidden; j++)
			wInputHidden[i][j] = 0;
	}

	wHiddenOutput = new(double*[nHidden + 1]); //은닉 => 출력
	for (int i = 0; i <= nHidden; i++){
		wHiddenOutput[i] = new (double[nOutput]);
		for (int j = 0; j < nOutput; j++)
			wHiddenOutput[i][j] = 0;
	}

	//순서대로 weight 정보 가져오기
	for (int i = 0; i <= nInput; i++){
		for (int j = 0; j < nHidden; j++){
			inFile >> wInputHidden[i][j];
		}
	}

	for (int i = 0; i <= nHidden; i++){
		for (int j = 0; j < nOutput; j++){
			inFile >> wHiddenOutput[i][j];
		}
	}
	inFile.close();
}

//웨이트 초기화 함수
void CNeuralNetwork::initializeWeights()
{
	//random number 생성
	srand((unsigned int)time(0));

	//-0.5 and 0.5 사이의 랜덤값으로 웨이트 초기화 (입력 ==> 은닉층 웨이트)
	for (int i = 0; i <= nInput; i++){
		for (int j = 0; j < nHidden; j++){
			//웨이트 설정
			wInputHidden[i][j] = (double)rand() / (RAND_MAX + 1) - 0.5;
			//0으로 초기화
			deltaInputHidden[i][j] = 0;
		}
	}
	//-0.5 and 0.5 사이의 랜덤값으로 웨이트 초기화 (은닉 ==> 출력층 웨이트)
	for (int i = 0; i <= nHidden; i++){
		for (int j = 0; j < nOutput; j++){
			//웨이트 설정
			wHiddenOutput[i][j] = (double)rand() / (RAND_MAX + 1) - 0.5;
			//0으로 초기화
			deltaHiddenOutput[i][j] = 0;
		}
	}
}

//1회 학습 (Forward & Backward) 수행
void CNeuralNetwork::runTrainingEpoch(vector<dataEntry*> trainingSet)
{
	//틀린 데이터 개수 측정을 위한 변수 선언
	double incorrectfeatures = 0;
	//각 학습데이터에 대해 연산 수행
	for (int tp = 0; tp < (int)trainingSet.size(); tp++){
		//forward / backward 연산 수행
		feedForward(trainingSet[tp]->feature);
		backpropagate(trainingSet[tp]->target);

		//목표값과 결과 값의 일치 여부 확인
		bool featureCorrect = true;
		for (int k = 0; k < nOutput; k++){
			//목표값과 결과값이 하나라도 일치하지 않으면 false
			if (getRoundedOutputValue(outputNeurons[k]) != trainingSet[tp]->target[k]) featureCorrect = false;
		}

		//일치하지 않는 데이터 갯수 세기
		if (!featureCorrect) incorrectfeatures++;

	}

	//학습 정확도 계산
	trainingSetAccuracy = 100 - (incorrectfeatures / trainingSet.size() * 100);
}



void CNeuralNetwork::feedForward(double *inputs)
{
	//입력층에 현재 입력값 삽입
	for (int i = 0; i < nInput; i++) inputNeurons[i] = inputs[i];

	//웨이트 값을 이용하여 forward 연산 수행
	for (int j = 0; j < nHidden; j++) {//입력 => 은닉
		hiddenNeurons[j] = 0;//은닉노드 초기화

		//Bias 값을 포함한 forward 연산 수행
		for (int i = 0; i <= nInput; i++) hiddenNeurons[j] += inputNeurons[i] * wInputHidden[i][j];

		//활성화 함수를 통해 얻어진 값 저장
		hiddenNeurons[j] = activationFunction(hiddenNeurons[j]);
	}

	for (int k = 0; k < nOutput; k++){ //은닉 => 출력
		outputNeurons[k] = 0;//출력 노드 초기화

		//Bias 값을 포함한 forward 연산 수행
		for (int j = 0; j <= nHidden; j++) outputNeurons[k] += hiddenNeurons[j] * wHiddenOutput[j][k];

		//활성화 함수를 통해 얻어진 값 저장
		outputNeurons[k] = activationFunction(outputNeurons[k]);
	}
}

void CNeuralNetwork::backpropagate(double* desiredValues)
{
	//목표값과 출력값을 이용하여 히든 ~ 출력 웨이트 웨이트 수정할 값 계산
	for (int k = 0; k < nOutput; k++){
		//목표값과 출력층간의 에러값 계산
		outputErrorGradients[k] = getOutputErrorGradient(desiredValues[k], outputNeurons[k]);

		//히든 노드 ~ 출력노드 사이의 웨이트 변화량 계산
		for (int j = 0; j <= nHidden; j++){
			deltaHiddenOutput[j][k] = learningRate * hiddenNeurons[j] * outputErrorGradients[k] + momentum * deltaHiddenOutput[j][k];
		}
	}

	//입력 ~ 히든 웨이트 웨이트 수정할 값 계산
	for (int j = 0; j < nHidden; j++){
		//각 히든 노드의 에러값 계산
		hiddenErrorGradients[j] = getHiddenErrorGradient(j);

		//입력 노드 ~ 히든노드 사이의 웨이트 변화량 계산
		for (int i = 0; i <= nInput; i++){
			deltaInputHidden[i][j] = learningRate * inputNeurons[i] * hiddenErrorGradients[j] + momentum * deltaInputHidden[i][j];
		}
	}
	//변화량 값을 이용하여 웨이트 업데이트
	updateWeights();
}

//update weights
void CNeuralNetwork::updateWeights()
{
	//입력 ~ 은닉층 웨이트 갱신
	for (int i = 0; i <= nInput; i++){
		for (int j = 0; j < nHidden; j++){
			//변화량만큼 더함
			wInputHidden[i][j] += deltaInputHidden[i][j];
		}
	}

	//은닉 ~ 출력층 웨이트 갱신
	for (int j = 0; j <= nHidden; j++){
		for (int k = 0; k < nOutput; k++){
			//변화량만큼 더함
			wHiddenOutput[j][k] += deltaHiddenOutput[j][k];
		}
	}
}

//활성화 함수
double CNeuralNetwork::activationFunction(double x){
	return 1 / (1 + exp(-x));
}

//Output 레이어의 에러 차이값 계산
double CNeuralNetwork::getOutputErrorGradient(double desiredValue, double outputValue){
	//목표치와 출력치간의 차이값 계산
	return outputValue * (1 - outputValue) * (desiredValue - outputValue);
}

//Hidden 레이어의 에러 차이값 계산
double CNeuralNetwork::getHiddenErrorGradient(int j){
	//출력 노드의 에러값을 이용하여 히든 노드의 에러값 계산
	double weightedSum = 0;
	for (int k = 0; k < nOutput; k++) weightedSum += wHiddenOutput[j][k] * outputErrorGradients[k];

	return hiddenNeurons[j] * (1 - hiddenNeurons[j]) * weightedSum;
}



//값이 0.1보다 적으면 0/0.9보다 크면 1/ 그 외의 값은 -1를 리턴하는 함수
int CNeuralNetwork::getRoundedOutputValue(double x){
	if (x < 0.1) return 0;
	else if (x > 0.9) return 1;
	else return -1;
}
