#include "stdafx.h"
#include "CAgglomerativeClustering.h"
#include <iostream>

using namespace std;


CAgglomerativeClustering::CAgglomerativeClustering()
{	//생성자 함수 (초기화)
	mItemCount = 0;
	mLinkageMode = -1;
	mCluster = nullptr;
	mItems = nullptr;
	kclusters.clear();
}


CAgglomerativeClustering::~CAgglomerativeClustering()
{	//소멸자 함수 (메모리해제)
	mItemCount = 0;
	mLinkageMode = -1;
	delete[] mCluster;
	delete[] mItems;
	kclusters.clear();
}

void CAgglomerativeClustering::freeNeighbors(Neighbor *node)
{
	/*메모리 해제 함수*/
	/*노드가 주어졌을 때, 노드의 이웃하는 이웃 노드들 순서대로 해제*/
	Neighbor *t;
	while (node) {
		t = node->next;
		delete[] node;
		node = t;
	}
}

void CAgglomerativeClustering::freeClusterNodes(Cluster *cluster)
{
	/*클러스터가 주어졌을 때, 클러스터 내부의 노드들을 해제하는 함수*/
	for (int i = 0; i < cluster->num_nodes; ++i) {
		ClusterNode *node = &(cluster->nodes[i]);
		if (node->merged)
			delete[] node->merged;
		if (node->items)
			delete[] node->items;
		if (node->neighbors)
			freeNeighbors(node->neighbors);
	}

	delete[] cluster->nodes;
}

/*클러스터가 주어졌을 때, 클러스터 자체를 해제하는 함수*/
void CAgglomerativeClustering::freeCluster(Cluster * cluster)
{
	if (cluster) {
		if (cluster->nodes) {
			freeClusterNodes(cluster);
		}
		if (cluster->distances) {
			for (int i = 0; i < cluster->num_items; ++i)
			{
				delete[] cluster->distances[i];
			}
			delete[] cluster->distances;
		}
		delete[] cluster;
	}
}

/*1. 실행 및 설정함수*/
/*입력 데이터가 주어졌을 때, 군집화 알고리즘을 수행하는 함수*/
void CAgglomerativeClustering::processFromVec(std::vector<Item> items)
{
	mItemCount = items.size();
	mItems = new Item[mItemCount]();/*입력 데이터와 동일한 사이즈의 Item 배열 할당*/
	for (int i = 0; i < mItemCount; i++) {
		mItems[i].id = i + 1;
		for (int j = 0; j < FEATDIM; j++) {
			(mItems[i].feat.features[j]) = (items[i].feat.features[j]);
		}
		mItems[i].feat.label = items[i].feat.label;
	}
	mCluster = agglomerate();/*군집화 수행 함수 호출*/
}

/*군집화 수행 함수*/
Cluster* CAgglomerativeClustering::agglomerate()
{
	Cluster* cluster = new Cluster[1]();
	if (cluster) {
		cluster->nodes = new ClusterNode[2 * mItemCount - 1]();/*최대로 발생 가능한 노드 개수만큼 노드 할당*/
		if (cluster->nodes)
		{
			initCluster(cluster, mItemCount, mItems);/*클러스터 초기화 함수 호출*/
		}
		else {
			std::cout << "FAIL: cluster nodes" << std::endl;
			freeCluster(cluster);
			cluster = NULL;
		}
	}
	else
	{
		cout << "cluster" << endl;
	}
	return cluster;
}

void CAgglomerativeClustering::initCluster(Cluster* cluster, int num_items, Item* items)
{
	/*클러스터 초기화 함수*/
	cluster->distances = generateDistanceMatrix(num_items, items);
	if (!cluster->distances) {
		freeCluster(cluster);
	}
	else
	{
		cluster->num_items = num_items;
		cluster->num_nodes = 0;
		cluster->num_clusters = 0;
		if (addLeaves(cluster, items))
		{
			mergeClusters(cluster);
		}
		else
		{
			freeCluster(cluster);
		}
	}
}

Cluster* CAgglomerativeClustering::addLeaves(Cluster *cluster, Item *items)
{
	/*데이터 배열이 주어졌을 때, 클러스터에 leaf 노드들을 추가하는 함수*/
	for (int i = 0; i < cluster->num_items; ++i) {
		/*각 데이터에 대해 leaf 노드로 추가하는 함수 호출 */
		if (addLeaf(cluster, &items[i])) {
			updateNeighbors(cluster, i);/*현재 클러스터에 해당 데이터의 이웃 관계 업데이트*/
		}
		else {
			cluster = NULL;
			break;
		}
	}
	return cluster;
}

ClusterNode* CAgglomerativeClustering::addLeaf(Cluster *cluster, const Item *item)
{
	/*데이터가 주어졌을 때, 클러스터에 leaf 노드를 추가하는 함수*/
	/*leaf 노드 공간 할당*/
	ClusterNode *leaf = &(cluster->nodes[cluster->num_nodes]);
	leaf->items = new int[1]();
	if (leaf->items) {
		initLeaf(cluster, leaf, item);	/*leaf 노드 초기화 함수 호출*/
		cluster->num_clusters++;		/*Merge가 필요한 노드 개수 증가*/
	}
	else {
		cout << "FAIL: node items" << endl;
		leaf = NULL;
		return nullptr;
	}
	return leaf;
}

/*클러스터에 넣어야 데이터와 넣을 노드가 주어졌을 때 실제 삽입을 수행하는 함수*/
void CAgglomerativeClustering::initLeaf(Cluster* cluster, ClusterNode* node, const Item* item)
{
	node->id = item->id;	/*현재 leaf node 개별 ID*/
	node->center = item->feat;	/*현재 leaf node의 특징*/
	node->type = LEAF_NODE;	/*현재 노드가 leaf node임을 표시*/
	node->is_root = 1;		/*isRoot = 1로 초기화*/
	node->height = 0;		/*Leaf node이므로 height (leaf node)로부터의 높이는 0으로 초기화*/
	node->num_items = 1;	/*현재 하나의 데이터만 있으므려 item 개수 1로 초기화*/
	node->items[0] = cluster->num_nodes++;	/*Merge가 필요한 노드 개수 증가*/
}

/*Proximity matrix 계산하는 함수*/
float** CAgglomerativeClustering::generateDistanceMatrix(int num_items, const Item* items)
{
	float **matrix = new float*[num_items];
	if (matrix) {
		for (int i = 0; i < num_items; ++i) {
			matrix[i] = new float[num_items];
			if (!matrix[i]) {
				cout << "FAIL: distance matrix row" << endl;
				num_items = i;
				for (i = 0; i < num_items; ++i) {
					delete[] matrix[i];
				}
				delete[] matrix;
				matrix = NULL;
				break;
			}
		}
		if (matrix)
		{
			fillEuclideanDistances(matrix, num_items, items);
		}
	}
	else
	{
		cout << "FAIL: distance matrix" << endl;
	}
	return matrix;
}

/*두 특징 간의 유클리디안 거리값 계산 함수*/
float CAgglomerativeClustering::euclideanDistance(const Feat *a, const Feat *b)
{
	float distance = 0;
	for (int i = 0; i < FEATDIM; i++)
	{
		distance += pow((a->features[i] - b->features[i]), 2);
	}
	return sqrt(distance);
}

/*유클리디안 거리 계산 함수를 호출하여 매트릭스를 채움*/
void CAgglomerativeClustering::fillEuclideanDistances(float **matrix, int num_items, const Item* items)
{
	for (int i = 0; i < num_items; ++i) {
		for (int j = 0; j < num_items; ++j) {
			matrix[i][j] =
				euclideanDistance(&(items[i].feat),
					&(items[j].feat));
			matrix[j][i] = matrix[i][j];
		}
	}
}

Cluster* CAgglomerativeClustering::updateNeighbors(Cluster *cluster, int index)
{
	/*현재까지 삽입된 Leaf node들 중에서, Merge가 되어 하위노드가 되지 않은 노드인 경우에 대해서만 neighbor 업데이트*/
	ClusterNode *node = &(cluster->nodes[index]);
	if (node->type == NOT_USED)
	{
		cout << "FAIL: Invalid node: " << index << endl;
		cluster = NULL;
	}
	else
	{
		int root_clusters_seen = 1, target = index;
		while (root_clusters_seen < cluster->num_clusters)
		{
			ClusterNode *temp = &(cluster->nodes[--target]);
			if (temp->type == NOT_USED)
			{
				cout << "FAIL: Invalid node: " << index << endl;
				cluster = NULL;
				break;
			}
			if (temp->is_root)
			{
				++root_clusters_seen;
				addNeighbor(cluster, index, target);
			}
		}
	}
	return cluster;
}

Neighbor* CAgglomerativeClustering::addNeighbor(Cluster *cluster, int index, int target)
{
	Neighbor *neighbour = new Neighbor[1]();
	if (neighbour) {
		neighbour->target = target;
		neighbour->distance = getDistance(cluster, index, target);
		ClusterNode *node = &(cluster->nodes[index]);
		if (node->neighbors) {
			insertSorted(node, neighbour);
		}
		else
		{
			node->neighbors = neighbour;
		}
	}
	else
	{
		cout << "FAIL: neighbour node" << endl;
	}
	return neighbour;
}

/*neighbor 노드 추가 관련 함수, 현재 노드(current)보다 neighbor를 이전에 추가*/
void CAgglomerativeClustering::insertBefore(Neighbor *current, Neighbor *neighbours,
	ClusterNode *node)
{
	neighbours->next = current;
	if (current->prev) {
		current->prev->next = neighbours;
		neighbours->prev = current->prev;
	}
	else
	{
		node->neighbors = neighbours;
	}
	current->prev = neighbours;
}

/*neighbor 노드 추가 관련 함수, 현재 노드(current)보다 neighbor를 다음에 추가*/
void CAgglomerativeClustering::insertAfter(Neighbor *current, Neighbor *neighbours)
{
	neighbours->prev = current;
	current->next = neighbours;
}
/*neighbor 노드 추가 관련 함수,현재 노드(current)의 neighbor간의 거리값을 순서대로 정렬하여 추가*/
void CAgglomerativeClustering::insertSorted(ClusterNode *node, Neighbor *neighbours)
{
	Neighbor *temp = node->neighbors;
	while (temp->next) {
		if (temp->distance >= neighbours->distance) {
			insertBefore(temp, neighbours, node);
			return;
		}
		temp = temp->next;
	}
	if (neighbours->distance < temp->distance)
	{
		insertBefore(temp, neighbours, node);
	}
	else
	{
		insertAfter(temp, neighbours);
	}
}

/*노드들 간의 Merge를 수행하여 생성된 클러스터를 return하는 함수*/
Cluster* CAgglomerativeClustering::mergeClusters(Cluster *cluster)
{
	int first, second;
	while (cluster->num_clusters > 1)
	{
		/*클러스터링을 수행할 node가 0이 될때까지 반복됨, 가까운 두 노드를 찾아서, merge 함수 호출*/
		if (findClustersToMerge(cluster, &first, &second) != -1)
		{
			merge(cluster, first, second);
		}
	}
	return cluster;
}

/*Merge할 두 노드를 찾아오는 함수*/
int CAgglomerativeClustering::findClustersToMerge(Cluster *cluster, int *first, int *second)
{
	float best_distance = 0.0;
	int root_clusters_seen = 0;
	int j = cluster->num_nodes; /* traverse hierarchy top-down */
	(*first) = -1;
	while (root_clusters_seen < cluster->num_clusters)
	{
		ClusterNode *node = &(cluster->nodes[--j]);
		if (node->type == NOT_USED || !node->is_root)
			continue;
		++root_clusters_seen;
		findBestDistanceNeighbor(cluster->nodes, j,
			node->neighbors,
			&best_distance,
			first, second);
	}
	return *first;
}

/*이웃 노드 중에서 가장 거리값이 작은 노드를 선택, leaf 노드 중에서 가장 거리값이 작은 neighbor를 찾아 first와 second에 삽입*/
void CAgglomerativeClustering::findBestDistanceNeighbor(ClusterNode *nodes, int node_idx, Neighbor *neighbour, float *best_distance, int *first, int *second)
{
	while (neighbour) {
		if (nodes[neighbour->target].is_root) {
			if (*first == -1 || neighbour->distance < *best_distance) {
				*first = node_idx;
				*second = neighbour->target;
				*best_distance = neighbour->distance;
			}
			break;
		}
		neighbour = neighbour->next;
	}
}

/*Merge를 수행하여 merge된 노드를 return*/
ClusterNode* CAgglomerativeClustering::merge(Cluster *cluster, int first, int second)
{
	int new_idx = cluster->num_nodes;
	ClusterNode *node = &(cluster->nodes[new_idx]);
	node->merged = new int[2]();
	if (node->merged) {
		ClusterNode *to_merge[2] = {
			&(cluster->nodes[first]),
			&(cluster->nodes[second])
		};
		node->merged[0] = first;
		node->merged[1] = second;
		mergeToOne(cluster, to_merge, node, new_idx);
	}
	else {
		cout << "FAIL: array of merged nodes" << endl;
		node = NULL;
	}
	return node;
}

/*merge할 노드가 주어졌을 때, 노드끼리 merge를 수행, 이웃 노드 업데이트*/
void CAgglomerativeClustering::mergeToOne(Cluster* cluster, ClusterNode* to_merge[], ClusterNode* node, int node_idx)
{
	node->num_items = to_merge[0]->num_items +
		to_merge[1]->num_items;
	node->items = new int[node->num_items]();
	if (node->items) {
		mergeItems(cluster, node, to_merge);
		cluster->num_nodes++;
		cluster->num_clusters--;
		updateNeighbors(cluster, node_idx);
	}
	else {
		cout << "FAIL: array of merged items" << endl;
		delete[] node->merged;
		node = NULL;
	}
}

/*실질적인 노드간의 Merge를 수행*/
void CAgglomerativeClustering::mergeItems(Cluster *cluster, ClusterNode *node,
	ClusterNode **to_merge)
{
	node->type = A_MERGER;
	node->is_root = 1;
	node->height = -1;
	int k = 0, idx;
	Feat center;
	for (int i = 0; i < FEATDIM; i++)
		center.features[i] = 0;
	for (int i = 0; i < 2; ++i) {
		ClusterNode *t = to_merge[i];

		t->is_root = 0;
		if (node->height == -1 || node->height < t->height)
			node->height = t->height;
		for (int j = 0; j < t->num_items; ++j) {
			idx = t->items[j];
			node->items[k++] = idx;
		}
		for (int j = 0; j < FEATDIM; j++)
			center.features[j] += t->num_items * t->center.features[j];
	}
	for (int j = 0; j < FEATDIM; j++)
		node->center.features[j] = center.features[j] / k;
	node->height++;
}

/*두 노드(혹은 클러스터 간의) 거리값을 가져오는 함수, 둘다 leaf 노드인 경우, 클러스터간 거리를 재지 않고 distance값 이용*/
float CAgglomerativeClustering::getDistance(Cluster *cluster, int index, int target)
{

	float result = 0;
	if (index < cluster->num_items && target < cluster->num_items)
		result = cluster->distances[index][target];
	else
	{
		ClusterNode *a = &(cluster->nodes[index]);
		ClusterNode *b = &(cluster->nodes[target]);
		if (mLinkageMode == CENTROID_LINKAGE) {
			result = euclideanDistance(&(a->center),
				&(b->center));
		}
		else
		{
			switch (mLinkageMode)
			{
			case SINGLE_LINKAGE:
				result = singleLinkage(cluster->distances,
					a->items, b->items,
					a->num_items, b->num_items);
				break;
			case COMPLETE_LINKAGE:
				result = completeLinkage(cluster->distances,
					a->items, b->items,
					a->num_items, b->num_items);
				break;
			case AVERAGE_LINKAGE:
				result = averageLinkage(cluster->distances,
					a->items, b->items,
					a->num_items, b->num_items);
				break;
			}
		}
	}
	return result;
}

/*클러스터 간의 거리값을 계산,
클러스터 내부의 데이터간의 거리값 중 최소값 리턴*/
float CAgglomerativeClustering::singleLinkage(float **distances, const int* a, const int* b, int m, int n)
{
	float min = FLT_MAX, d;
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			d = distances[a[i]][b[j]];
			if (d < min)
				min = d;
		}
	}
	return min;
}
float CAgglomerativeClustering::completeLinkage(float **distances, const int* a, const int* b, int m, int n)
{
	/*클러스터 내부의 데이터간의 거리값 중 최대값 리턴*/
	float d, max = 0.0;
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			d = distances[a[i]][b[j]];
			if (d > max)
				max = d;
		}
	}
	return max;
}
/*클러스터 내부의 데이터간의 거리들의 평균값 리턴*/
float CAgglomerativeClustering::averageLinkage(float **distances, const int* a, const int* b, int m, int n)
{

	float total = 0.0;
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			total += distances[a[i]][b[j]];
		}
	}
	return total / (m * n);
}

/*트리 구조에서 상위 k의 노드만을 가져오는 함수*/
std::vector<Item> CAgglomerativeClustering::get_k_clusters(int k)
{
	std::vector<Item> clusterMeans;
	clusterMeans.clear();
	if (k < 1)
		return clusterMeans;

	kclusters.clear();
	if (k > mCluster->num_items)
		k = mCluster->num_items;

	int i = mCluster->num_nodes - 1;
	int roots_found = 0;
	int nodes_to_discard = mCluster->num_nodes - k + 1;
	while (k) {
		if (i < nodes_to_discard) {
			printClusterItems(mCluster, i);
			roots_found = 1;
		}
		else {
			roots_found = printRootChildren(mCluster, i, nodes_to_discard);
		}
		k -= roots_found;
		--i;
	}

	for (int i = 0; i < kclusters.size(); i++)
	{
		Item item;
		for (int j = 0; j < FEATDIM; j++)
			item.feat.features[j] = 0;
		int freq[TR_CLS] = { 0 };
		for (int k = 0; k < kclusters[i].size(); k++)
		{
			for (int j = 0; j < FEATDIM; j++)
			{
				item.feat.features[j] += mItems[kclusters[i][k] - 1].feat.features[j];
			}
			freq[mItems[kclusters[i][k] - 1].feat.label]++;
		}
		for (int j = 0; j < FEATDIM; j++)
		{
			item.feat.features[j] /= kclusters[i].size();
		}
		int maxIdx = 0;
		int maxVal = 0;
		for (int j = 0; j < TR_CLS; j++)
		{
			if (maxVal < freq[j])
			{
				maxVal = freq[j];
				maxIdx = j;
			}
		}
		item.feat.label = maxIdx;
		clusterMeans.push_back(item);
	}
	return clusterMeans;
}

/*현재 노드의 포함된 이웃 노드들의 데이터 출력*/
void CAgglomerativeClustering::printClusterItems(Cluster *cluster, int index)
{
	std::vector<int> vec;
	vec.clear();
	ClusterNode *node = &(cluster->nodes[index]);
	cout << "Items: ";
	if (node->num_items > 0) {
		vec.push_back(cluster->nodes[node->items[0]].id);
		cout << cluster->nodes[node->items[0]].id;
		for (int i = 1; i < node->num_items; ++i)
		{
			cout << ", " << cluster->nodes[node->items[i]].id;
			vec.push_back(cluster->nodes[node->items[i]].id);
		}
	}
	kclusters.push_back(vec);
	cout << endl;
}

/*현재 노드의 포함된 이웃 노드들을 찾아 데이터 출력 함수 호출*/
void CAgglomerativeClustering::printClusterNode(Cluster *cluster, int index)
{
	ClusterNode *node = &(cluster->nodes[index]);
	cout << "Node " << index << " - height: " << node->height << endl;
	cout << "centroid: " << endl;
	for (int i = 0; i < FEATDIM; i++)
		cout << node->center.features[i] << "\t";
	cout << endl;
	if (!node->merged)
		cout << "Leaf: " << node->id << endl;
	else
		cout << "Merged: " << node->merged[0] << ", " << node->merged[1] << endl;
	printClusterItems(cluster, index);
	cout << "Neighbours: " << endl;
	Neighbor *t = node->neighbors;
	while (t) {
		cout << t->target << "\t" << t->distance << endl;
		t = t->next;
	}
	cout << endl;
}

/*현재 노드의 하위 노드들을 찾아 출력함수 호출*/
int CAgglomerativeClustering::printRootChildren(Cluster *cluster, int i, int nodes_to_discard)
{
	ClusterNode *node = &(cluster->nodes[i]);
	int roots_found = 0;
	if (node->type == A_MERGER) {
		for (int j = 0; j < 2; ++j) {
			int t = node->merged[j];
			if (t < nodes_to_discard) {
				printClusterItems(cluster, t);
				++roots_found;
			}
		}
	}
	return roots_found;
}

void CAgglomerativeClustering::printCluster()
{
	/*클러스터 정보 출력*/
	for (int i = 0; i < mCluster->num_nodes; ++i)
		printClusterNode(mCluster, i);
}
