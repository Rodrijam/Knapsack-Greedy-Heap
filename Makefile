knapsack.x: main.cpp
	g++ -o knapsack main.cpp -lboost_iostreams -lboost_system -lboost_filesystem

clean:
	rm -fr knapsack_graphs/*.png knapsack