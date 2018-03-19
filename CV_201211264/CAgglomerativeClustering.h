#pragma once
#include <vector>
#define NOT_USED 0
#define LEAF_NODE 1
#define A_MERGER 2
#define AVERAGE_LINKAGE 'a'
#define CENTROID_LINKAGE 't'
#define COMPLETE_LINKAGE 'c'
#define SINGLE_LINKAGE 's'


class CAgglomerativeClustering {
private:
	int mItemCount;
	Cluster* mCluster;
	Item* mItems;
	std::vector<std::vector<int>> kclusters;
	int mLinkageMode;
public:
	CAgglomerativeClustering();
	~CAgglomerativeClustering();

	void processFromVec(std::vector<Item> items);
	void setLinkageMode(int _mode) { mLinkageMode = _mode; }
	Cluster* agglomerate();

	void initLeaf(Cluster* cluster, ClusterNode* node, const Item* item);
	ClusterNode* addLeaf(Cluster* cluster, const Item* item);
	Cluster* addLeaves(Cluster* cluster, Item* items);
	void initCluster(Cluster* cluster, int num_items, Item* items);
	
	Neighbor* addNeighbor(Cluster *cluster, int index, int target);
	Cluster* updateNeighbors(Cluster *cluster, int index);

	float** generateDistanceMatrix(int num_items, const Item* items);
	float euclideanDistance(const Feat *a, const Feat *b);
	void fillEuclideanDistances(float** matrix, int num_items, const Item* items);
	float getDistance(Cluster *cluster, int index, int target);

	Cluster* mergeClusters(Cluster* cluster);
	int findClustersToMerge(Cluster* cluster, int* first, int* second);
	void findBestDistanceNeighbor(ClusterNode* nodes, int nodeIdx, Neighbor* neighbor,
		float* best_distance, int* first, int* second);
	ClusterNode *merge(Cluster* cluster, int first, int second);
	void mergeItems(Cluster* cluster, ClusterNode* node, ClusterNode** toMerge);
	void mergeToOne(Cluster* clutser, ClusterNode* to_merge[], ClusterNode* node, int nodeIdx);

	float singleLinkage(float** distances, const int* a, const int* b, int m, int n);
	float completeLinkage(float** distances, const int* a, const int* b, int m, int n);
	float averageLinkage(float** distances, const int* a, const int* b, int m, int n);

	void insertBefore(Neighbor* current, Neighbor* neighbors, ClusterNode* node);
	void insertAfter(Neighbor* current, Neighbor* neighbors);
	void insertSorted(ClusterNode* node, Neighbor* neighbors);

	std::vector<Item> get_k_clusters(int k);
	void printCluster();
	void printClusterItems(Cluster* cluster, int index);
	void printClusterNode(Cluster* cluster, int index);
	int printRootChildren(Cluster* cluster, int i, int nodes_to_discard);

	void freeNeighbors(Neighbor *node);
	void freeClusterNodes(Cluster *cluster);
	void freeCluster(Cluster * cluster);
};

