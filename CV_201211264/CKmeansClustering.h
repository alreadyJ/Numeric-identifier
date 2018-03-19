#pragma once
#define MAX_ITER_IN_KCLUSTER 100
#include <vector>
class CKmeansClustering{
private:
	int mItemCount;
	int mK;
	Item* mItems;
	std::vector<ClusterNode> mMeans;
	void assignment();
	void update();
public:
	CKmeansClustering();
	~CKmeansClustering();
	std::vector<Item> getKClusters();
	void processFromVec(std::vector<Item> items);
	void setKValue(int _k) { mK = _k; }
	void processFromTxt(const char* fileName);
	float getSSE(int K);
};

