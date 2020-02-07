#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <string.h>	

#include "gnuplot-iostream.h"

using namespace std;
using namespace std::chrono; 

vector <pair <double,int>> heapify(vector <pair <double,int>> ratioList, int i);
vector <pair <double,int>> buildHeap(vector <pair <double,int>> ratioList);
vector <pair <double,int>> deleteMax(vector <pair <double,int>> ratioHeap);

vector<double> SetUpValues (string fileNum);
vector <pair <double,int>> GetRatio (int numItems);

int GetKnapValue(int i, int j);
int MFKnapsack (int i, int j);
vector <int> DynamicBacktrack (int i, int j);
int GreedyBuild (double kCap, vector <pair <double,int>>);
int GreedyHeap (double kCap, vector <pair <double,int>>); 

void plotPoints(vector<boost::tuple<double, double>>, int, int, string, string);

int gbuildOperations;
int heapOperations;
vector <pair <double,double>> weightToValues;

vector <vector <int>> arr;
vector <vector <int>> F;

ifstream inFile;
ifstream inFile2;

int main (int argc, char *argv[]){
	
	
	
	if (argc != 2)
	{
		cerr << "Not enough arguments in command or arguments invalid, format: {'executable' 'test#'}" << endl;
		exit(0);
	}
	
	string fileNum = argv[1];
	
	vector<boost::tuple<double, double> > gbuildPoints;
	vector<boost::tuple<double, double> > gheapPoints;
	//vector<boost::tuple<double, double> > dynPoints;
	//vector<boost::tuple<double, double> > memPoints;
	
	vector<double> vals; // 0 kcap 1 numItems 2 weight 3 value	
	
	vals = SetUpValues(fileNum);	
	vector <pair <double,int>> ratio = GetRatio(vals[1]);	// get value/weight ratios with item# in pair
	
	cout << "Knapsack capacity = " << vals[0] << ". Total number of items = " << vals[1] << endl << endl;

	// Task 1a: Dynamic Programming (traditional) approach
	
	auto tradDynstart = high_resolution_clock::now();
	
	
	for (int i = 1; i < vals[1]+1; i++)
		for (int j = 1; j < vals[0]+1; j++)
			arr[i][j] = GetKnapValue(i,j);
	
	int dynOpCount = arr[vals[1]][vals[0]];
		
	//dynPoints.push_back(boost::make_tuple(0,dynOpCount));
	vector <int> optDSet = DynamicBacktrack(vals[1],vals[0]);
	
	auto tradDynstop = high_resolution_clock::now();
	auto tradDynDuration = duration_cast<microseconds>(tradDynstop - tradDynstart);
	
	
	cout << "Traditional Dynamic Programming Optimal value: " << dynOpCount << endl;
	cout << "Traditional Dynamic Programming Optimal subset: {";
	
	
	for(int i = 0; i < optDSet.size(); i++)
	{
		cout << optDSet[i];
		if (i != optDSet.size()-1)
			cout << ",";
	}	
	cout << "}" <<endl;	
	
	cout << "Traditional Dynamic Programming Time Taken: " << tradDynDuration.count() << " microseconds." << endl << endl;
	
	
	// Task 1b: Dynamic Programming (space-efficient) approach using hash tables
	
	auto memDynstart = high_resolution_clock::now();
	
	int memOpCount = MFKnapsack(vals[1], vals[0]);
	//mememPoints.push_back(boost::make_tuple(0,dynOpCount));
	vector <int> optMSet = DynamicBacktrack(vals[1],vals[0]);
	
	auto memDynstop = high_resolution_clock::now();
	auto memDynDuration = duration_cast<microseconds>(memDynstop - memDynstart);
	
	
	cout << "Space-efficient Dynamic Programming Optimal value: " << memOpCount << endl;
	cout << "Space-efficient Dynamic Programming Optimal subset: {";
	
	
	for(int i = 0; i < optDSet.size(); i++)
	{
		cout << optDSet[i];
		if (i != optDSet.size()-1)
			cout << ",";
	}	
	cout << "}" <<endl;	
	
	cout << "Space-efficient Dynamic Programming Time Taken: " << tradDynDuration.count() << " microseconds." << endl;
	cout << "Space-efficient Dynamic Programming Space Taken: " << -1 << endl << endl;
	
	// Task 2a: Greedy approach using in-built sort
	GreedyBuild(vals[0], ratio);
	gbuildPoints.push_back(boost::make_tuple(0,gbuildOperations));
	cout << "Greedy Approach Number of Operations: " << gbuildOperations << endl << endl;

	// Task 2b: Greedy approach using max-heap
	GreedyHeap(vals[0], ratio);
	gheapPoints.push_back(boost::make_tuple(0,heapOperations));
	cout << "Heap-based Greedy Approach Number of Operations: " << heapOperations << endl << endl;
	
	
	int graphNum;

	cout << "All graphs produced will be placed in the knapsack_graphs folder" << endl;
	cout << "How many test files are present?" << endl;
	cin >> graphNum;
	
	int max1 = 0;
	int max2 = 0;
	int maxItems = 0;
	
	for (int i = 1; i < graphNum; i++)
	{
	
		vals = SetUpValues(to_string(i));	
		vector <pair <double,int>> ratio = GetRatio(vals[1]);
		cout << "Knapsack capacity = " << vals[0] << ". Total number of items = " << vals[1] << endl << endl;
		
		if (vals[1] > maxItems)
			maxItems = vals[1];
		
		
		GreedyBuild(vals[0], ratio);
		gbuildPoints.push_back(boost::make_tuple(vals[1],gbuildOperations));
		cout << "Greedy Approach Number of Operations: " << gbuildOperations << endl << endl;
		if (gbuildOperations > max1)
			max1 = gbuildOperations;
		GreedyHeap(vals[0], ratio);
		gheapPoints.push_back(boost::make_tuple(vals[1],heapOperations));
		cout << "Heap-based Greedy Approach Number of Operations: " << heapOperations << endl << endl;		
		
		if (heapOperations > max2)
			max2 = heapOperations;
		
	}
	
	plotPoints(gbuildPoints, maxItems, max1 + 10, "greedybuildG", "GreedyBuild");
	plotPoints(gheapPoints, maxItems, max2 + 10, "greedyheapG", "GreedyHeap");
	
	return 0;
}


vector<double> SetUpValues (string fileNum)
{
	weightToValues.clear();
	
	if (fileNum.size() == 1)
		fileNum = "0" + fileNum;
	
	string fileC = "p" + fileNum + "_c.txt";
	string fileW = "p" + fileNum + "_w.txt";
	string fileV = "p" + fileNum + "_v.txt";

	cout << "File containing the capacity, weights, and values are: " << fileC << ", " <<  fileW << ", and " << fileV << endl << endl;

	double kCap;
	double numItems;
	double weight;
	double value;

	heapOperations = 0;
	gbuildOperations = 0;
	
	// in weightToValues, first is weight, second is value, index is item #


	inFile.open(fileC);
	
	if (!inFile) {
        cout << "Unable to open file" << endl;
        exit(1);
    }
	
	inFile >> kCap;
	
	inFile.close();
	
	inFile.open(fileW);
	inFile2.open(fileV);

	if (!inFile || !inFile2) {
        cout << "Unable to open file" << endl;
        exit(1);
    }
	
	pair<double,double> wtvItem;
	
	while (inFile >> weight && inFile2 >> value)								
	{
		
		wtvItem = make_pair (weight, value);
		numItems++;
		weightToValues.push_back(wtvItem);
	}	
	
	inFile.close();
	inFile2.close();
	
	
	for (int i = 0; i < numItems+1; i++)
	{
		vector<int> Fitem;
		for (int j = 0; j < kCap+1; j++)
		{
			if (i == 0 || j == 0)
				Fitem.push_back(0);
			else
				Fitem.push_back(-1);
		}
		F.push_back(Fitem);
		arr.push_back(Fitem);
	}


	
	vector<double> vals = {kCap, numItems};
	
	return vals;
	
}

int GetKnapValue (int i, int j)
{
	int val;
	if (arr[i][j] != -1)
		return arr[i][j];
	else
	{
		double dweight = weightToValues[i-1].first;
		double dvalue = weightToValues[i-1].second;
		
		dweight = dweight + 0.5 - (dweight<0);
		dvalue = dvalue + 0.5 - (dvalue<0);
		
		int iWeight = (int) dweight;
		int iValue = (int) dvalue;

		int check1 = 0;
		
		if (j-iWeight >= 0)
			 check1 = iValue + GetKnapValue(i-1,j-iWeight);
			
		 
		int check2 = GetKnapValue(i-1,j);

		if (check1 > check2)
			val = check1;
		else
			val = check2;

	}
	return val;
}

int MFKnapsack (int i, int j)
{
	
	if (F[i][j] < 0)
	{
		int val;
		double dweight = weightToValues[i-1].first;
		double dvalue = weightToValues[i-1].second;
		
		dweight = dweight + 0.5 - (dweight<0);
		dvalue = dvalue + 0.5 - (dvalue<0);
		
		int iWeight = (int) dweight;
		int iValue = (int) dvalue;
		
		if (j <  iWeight)
		{
			val = MFKnapsack(i-1,j);
		}
		else
		{
			int check1 = iValue + MFKnapsack(i-1,j-iWeight);
			int check2 = MFKnapsack(i-1,j);

			if (check1 > check2)
				val = check1;
			else
				val = check2;
		}
		F[i][j] = val;
	}
	return F[i][j];
}

vector <int> DynamicBacktrack (int i, int j)
{
	vector <int> optVals;
	if (i > 0)
	{
		if (arr[i][j] > arr[i-1][j])
		{
			// include item i
			double dweight = weightToValues[i-1].first;
			dweight = dweight + 0.5 - (dweight<0);
			int iWeight = (int) dweight;

			optVals = DynamicBacktrack(i-1, j - iWeight);
			optVals.push_back(i);
		}
		else
		{
			// do not inclue item i
			optVals = DynamicBacktrack(i-1,j);
		}
	}
	return optVals;
}

vector <pair <double,int>> GetRatio (int numItems)
{
	vector <pair <double,int>> ratio;
	pair<double,double> ratioItem;
	
	for (int i = 1; i < numItems+1; i++)
	{
		ratioItem = make_pair (weightToValues[i-1].second/weightToValues[i-1].first, i);
		ratio.push_back(ratioItem);
	}
	
	return ratio;
}

vector <pair <double,int>> merge (vector <pair <double,int>> list1, vector <pair <double,int>> list2)
{

	
	
	if (list1.size() == 0)
		return list2;
	else if (list2.size() == 0)
		return list1;
	
	vector <pair <double,int>> endList;
	vector <pair <double,int>> tempList;
	vector <pair <double,int>> mergedList;
	
	gbuildOperations++;
	if (list1[0].first > list2[0].first)
	{
		tempList.push_back(list1[0]);
		list1.erase(list1.begin());
		mergedList = merge(list1,list2);
		
		endList.reserve(tempList.size() + mergedList.size());
		endList.insert(endList.end(), tempList.begin(), tempList.end());
		endList.insert(endList.end(), mergedList.begin(), mergedList.end());
		
	}
	else
	{
		tempList.push_back(list2[0]);
		list2.erase(list2.begin());
		mergedList = merge(list1,list2);
		
		endList.reserve(tempList.size() + mergedList.size());
		endList.insert(endList.end(), tempList.begin(), tempList.end());
		endList.insert(endList.end(), mergedList.begin(), mergedList.end());
		
	}

	return endList;
}

vector <pair <double,int>> mergeSort (vector <pair <double,int>> list)
{
	
	if (list.size() == 1)
		return list;
	
	size_t const half = list.size() / 2;
	vector<pair <double,int>> L1(list.begin(), list.begin() + half);
	vector<pair <double,int>> L2(list.begin() + half, list.end());

	L1 = mergeSort(L1);
	L2 = mergeSort(L2);
	list = merge(L1, L2);
	
	return list;
	
}

int GreedyBuild (double kCap, vector <pair <double,int>> ratio)
{
	
	ratio = mergeSort(ratio);
	
	double optVal;
	vector <int> optSet;
	int numOps = 0;
	
	for(auto& x: ratio)
	{
		if ((kCap - weightToValues[x.second-1].first) >= 0)
		{
			kCap -= weightToValues[x.second-1].first;
			optVal += weightToValues[x.second-1].second; 
			optSet.push_back(x.second);	
		}
		else
			break;
	}
	
	cout << "Greedy Approach Optimal Value: " << optVal << endl;
	
	cout << "Greedy Approach Optimal subset: {";
	
	sort (optSet.begin(), optSet.end());
	
	for(int i = 0; i < optSet.size(); i++)
	{
		cout << optSet[i];
		if (i != optSet.size()-1)
			cout << ",";
	}	
	cout << "}" <<endl;

	return numOps;
	
}


int GreedyHeap (double kCap, vector <pair <double,int>> ratio)
{
	ratio = buildHeap(ratio);
	
	double optVal;
	vector <int> optSet;

	while (ratio.size() > 0)
	{
		heapOperations++;
		if ((kCap - weightToValues[ratio[0].second-1].first) >= 0)
		{
			kCap -= weightToValues[ratio[0].second-1].first;
			optVal += weightToValues[ratio[0].second-1].second; 
			optSet.push_back(ratio[0].second);
		}
		else
			break;
		ratio = deleteMax(ratio);
	}

	cout << "Heap-based Greedy Approach Optimal value: " << optVal << endl;
	
	cout << "Heap-based Greedy Approach Optimal subset: {";
	sort (optSet.begin(), optSet.end());
	for(int i = 0; i < optSet.size(); i++)
	{
		cout << optSet[i];
		if (i != optSet.size()-1)
			cout << ",";
	}	
	cout << "}" <<endl;	
}



vector <pair <double,int>> heapify(vector <pair <double,int>> ratioList, int i) 
{ 
    int maxNode = i;
    int leftNode = (2 * i) + 1; 
    int rightNode = (2 * i) + 2; 
	int heapSize = ratioList.size();
  
	heapOperations += 2; // basic operation, does comparisons with left and right nodes
  
    if (leftNode < heapSize && ratioList[leftNode].first > ratioList[maxNode].first) 
        maxNode = leftNode;
	
    if (leftNode < heapSize && ratioList[rightNode].first > ratioList[maxNode].first) 
        maxNode = rightNode;

    if (maxNode != i) { 
        swap(ratioList[i], ratioList[maxNode]); 
        heapify(ratioList, maxNode); 
    }
	return ratioList;
} 
  
vector <pair <double,int>> buildHeap(vector <pair <double,int>> ratioList) 
{ 

    int startNode = (ratioList.size() / 2) - 1; 

    for (int i = startNode; i >= 0; i--)
        ratioList = heapify(ratioList, i); 

	return ratioList;
} 

vector <pair <double,int>> deleteMax(vector <pair <double,int>> ratioHeap) 
{ 
	
    swap(ratioHeap[0], ratioHeap[ratioHeap.size()-1]);
	ratioHeap.pop_back();
	ratioHeap = buildHeap(ratioHeap);
	
	return ratioHeap;
}



void plotPoints(vector<boost::tuple<double, double>> allPoints, int x, int y, string name, string title)
{
		Gnuplot gp;
		gp << "set term png\n";
		gp << "set out 'knapsack_graphs/" << name <<".png'\n";

		gp << "set title '" << title << "'\n";
		gp << "set xrange [0:" << x + 10 << "]\nset yrange [0:" << y << "]\n";
		gp << "plot '-' with points notitle\n";
		gp.send1d(allPoints);	
		
		gp << "set out";
}