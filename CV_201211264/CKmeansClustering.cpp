#include "stdafx.h"
#include "CKmeansClustering.h"


CKmeansClustering::CKmeansClustering(){
	mK = 0;
	mItemCount = 0;
	mItems = nullptr;
	mMeans.clear();
}


CKmeansClustering::~CKmeansClustering(){
	delete[] mItems;
	mK = 0;
	mMeans.clear();
}

void CKmeansClustering::assignment() {
	for (int i = 0; i < mMeans.size(); i++) {
		mMeans[i].num_items = 0;
	}
	for (int i = 0; i < mItemCount; i++) {
		float minDistance = 10000000;
		for (int k = 0; k < mK; k++) {
			float distance = 0.0;
			for (int j = 0; j < FEATDIM; j++) {
				distance += pow((mMeans[k].center.features[j] - mItems[i].feat.features[j]), 2);
			}
			distance = sqrt(distance);
			if (minDistance > distance) {
				minDistance = distance;
				mItems[i].meanId = k;
			}
		}
		mMeans[mItems[i].meanId].num_items++;
	}

}
void CKmeansClustering::update() {
	for (int k = 0; k < mK; k++) {
		int freq[TR_CLS] = { 0 };
		if (mMeans[k].num_items > 0) {
			Feat avg;
			for (int j = 0; j < FEATDIM; j++) {
				avg.features[j] = 0;
			}
			for (int i = 0; i < mItemCount; i++) {
				if (mItems[i].meanId == k) {
					for (int j = 0; j < FEATDIM; j++) {
						avg.features[j] += mItems[i].feat.features[j];
					}
					freq[mItems[i].feat.label]++;
				}
			}
			for (int j = 0; j < FEATDIM; j++) {
				avg.features[j] /= mMeans[k].num_items;
			}

			int maxIdx = 0, maxVal = 0;
			for (int j = 0; j < TR_CLS; j++) {
				if (maxVal < freq[j]) {
					maxVal = freq[j];
					maxIdx = j;
				}
			}
			avg.label = maxIdx;

			for (int j = 0; j < FEATDIM; j++) {
				mMeans[k].center.features[j] = avg.features[j];
			}
			mMeans[k].center.label = avg.label;
		}else {
			int randIdx = rand() % mItemCount;
			for (int i = 0; i < FEATDIM; i++) {
				mMeans[k].center.features[i] = mItems[randIdx].feat.features[i];
			}
			mMeans[k].center.label = mItems[randIdx].feat.label;
		}
	}
}

void CKmeansClustering::processFromVec(std::vector<Item> items) {
	mItemCount = items.size();
	mItems = new Item[mItemCount]();
	for (int i = 0; i < mItemCount; i++) {
		mItems[i].id = i + 1;
		for (int j = 0; j < FEATDIM; j++) {
			mItems[i].feat.features[j] = items[i].feat.features[j];
		}
		mItems[i].feat.label = items[i].feat.label;
	}

	mMeans.clear();

	for (int i = 0; i < mK; i++) {
		ClusterNode node;
		int randIdx = rand() % mItemCount;
		for (int j = 0; j < FEATDIM; j++) {
			node.center.features[j] = mItems[randIdx].feat.features[j];
		}
		node.center.label = 0;
		node.num_items = 0;
		mMeans.push_back(node);
	}

	for (int i = 0; i < MAX_ITER_IN_KCLUSTER; i++) {
		assignment();
		update();
	}


}


std::vector<Item> CKmeansClustering::getKClusters() {
	std::vector<Item> means;
	means.clear();
	for (int i = 0; i < mMeans.size(); i++) {
		Item item;
		for (int j = 0; j < FEATDIM; j++) {
			item.feat.features[j] = mMeans[i].center.features[j];
		}
		item.feat.label = mMeans[i].center.label;
		means.push_back(item);
	}
	return means;
}

void CKmeansClustering::processFromTxt(const char* fileName) {

}

float CKmeansClustering::getSSE(int K) {
	float* distances = new float[K];
	for (int i = 0; i < K; i++) {
		distances[i] = 0;
	}
	for (int i = 0; i < mItemCount; i++) {
		for (int j = 0; j < K; j++) {
			if (mItems[i].feat.label == mMeans[j].center.label) {
				for (int k = 0; k < FEATDIM; k++) {
					distances[mItems[i].feat.label] = pow(mItems[i].feat.features[k] - mMeans[j].center.features[k], 2);
				}
			}
		}
	}
	float sum = 0;
	for (int i = 0; i < K; i++) {
		sum += distances[i];
	}
	return 1/ (100*sum);
}