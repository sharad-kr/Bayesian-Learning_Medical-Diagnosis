#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
// #include<bits/stdc++.h>
#include<map>

#include<chrono>

using namespace std;
float q = 1729.5;
vector<vector<string>> raw_data;		    // just fetched the initial data into this vector.
vector<vector<int>> running_data;	        // this will contain the most updated data while processing.
vector<map<string,int>> value_index; 	    // each element of this vector is mapping/indexing of the possible values of each node to a number.
map<string,int> node_index;        		    // this mapping indexes the nodes from 0 to 36.
vector<string> node_name(37); 			    // this is index ----> node mapping , reverse for node_index.
vector<int> wild_in_data;				    // at index i : contains the node whose value_assignment is missing in i_th data in raw_data.
vector<map<string,int>> value_assignment;	// this contains the value_assignment of each data except for the wild position.
map<string,int> valuesPerVariable;			// this contains the cardinality of set of values a node can have.
vector<vector<int>> cpt_for_missing_data;   


// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning



public:
	vector<float> working_CPT;
	map<string,int> value_index;
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
	
		nvalues=n;
		values=vals;
		

	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}

	void set_working_CPT(vector<float> cpt){
		working_CPT.clear();
		working_CPT = cpt;
	}


	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		working_CPT.clear();
		CPT=new_CPT;
		working_CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }



};


 // The whole network represted as a list of nodes
class network{

	

public:
	list <Graph_Node> Pres_Graph;

	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
// get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }
	

};

network read_network()
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile("alarm.bif"); 
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{

             		map<string,int> g;    
             		int counter = 0;   
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);

     					g[temp] = counter;
     					counter++;
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				valuesPerVariable[name] = values.size();
     				value_index.push_back(g);
     				// new_node.value_index = g;
     				int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<float> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                        
     					curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;
                       
                        

    				}
                    
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                
            }
     		
     		

    		
    		
    	}
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}


int cpt_index(int wild,int wild_value, vector<int> value_map , vector<string> parent){

    int index = 0;
    index = (index * valuesPerVariable[node_name[wild]]) + wild_value;
    for (int i = 0; i < parent.size(); i++) {
    	index = (index * valuesPerVariable[parent[i]]) + value_map[node_index[parent[i]]];
    }
    return index;

}

vector<int> cpt_pointer(network &Alarm,int &wild, vector<int> &value_map , vector<string> &parent){

		list<Graph_Node> :: iterator it;
		it = Alarm.search_node(node_name[wild]);
		vector<string> val = it->get_values();
		vector<int> ptr;
		for(int i = 0 ; i<val.size() ; i++){
			int idx = cpt_index(wild,value_index[wild][val[i]],value_map,parent);
			ptr.push_back(idx);

		}

		return ptr;
}


void process_data(network &Alarm){
	ifstream file("records.dat");
	string temp;
	string line;
	while(!file.eof()){
		vector<string> tempv;
		vector<int> tempv2;
		map<string,int> tempv3;
		int k = 0;
		stringstream ss;
		getline(file, line);
		ss.str(line);
		// int count_idx = 0;
		while(ss >> temp){
			tempv.push_back(temp);
			if(temp.compare("\"?\"")==0){
				wild_in_data.push_back(k);
				list<Graph_Node>::iterator it = Alarm.search_node(node_name[k]);
				tempv2.push_back(value_index[k][(it->get_values())[0]]);
			} 
			else{
				tempv3[node_name[k]] = value_index[k][temp]; 
				tempv2.push_back(value_index[k][temp]);
			}
			
			k++;
		}
		value_assignment.push_back(tempv3);
		raw_data.push_back(tempv);
		running_data.push_back(tempv2);

	}


	for(int i = 0 ; i<wild_in_data.size() ; i++){
		int wild = wild_in_data[i];
		map<string,int> tempv4;
		vector<string> par = Alarm.search_node(node_name[wild])->get_Parents();
		for(string parent : par){
			tempv4[parent] = value_assignment[i][parent];
		}
		value_assignment[i] = tempv4;
	}

file.close();
}



void learn(network &Alarm){
	list<Graph_Node> :: iterator it,it2;

	for(int i = 0 ; i<Alarm.netSize() ; i++){

		int total_size = 1;
		it = Alarm.search_node(node_name[i]);
		for( string s : it->get_Parents()){
			it2 = Alarm.search_node(s);
			total_size = total_size * (it2->get_nvalues());
		}

		int k = it->get_nvalues();
		total_size = total_size * k;
		vector<int> query(total_size,0);
		vector<int> evidence(total_size/k,1);
		vector<string> par = it->get_Parents();
		for(int j = 0 ; j<running_data.size() ; j++){
			int idx_query = cpt_index(i,running_data[j][i],running_data[j], par );
			int idx_evidence = idx_query % (total_size / k) ; 
			query[idx_query]++;
			evidence[idx_evidence]++;
		}

		vector<float> CPT_table = it->get_CPT();

		for(int i1 = 0 ; i1<CPT_table.size() ; i1++){
			if(CPT_table[i1]==-1){
				CPT_table[i1] = (float)query[i1]/evidence[i1 % (total_size/k)];
				}
			}
		it->set_working_CPT(CPT_table);
	}
}


void hard_infer(network &Alarm){
	for(int i = 0 ; i < wild_in_data.size() ; i++){
		int wildy = wild_in_data[i];
		list<Graph_Node>::iterator it;
		it = Alarm.search_node(node_name[wildy]);
		vector<float> cpt = it->working_CPT;
		vector<string> value = it->get_values();
		int k = 0 ;
		vector<int> prob = cpt_for_missing_data[i];
		float maxi = cpt[prob[0]];
		for(int j = 0 ; j<prob.size() ; j++){
			if(cpt[prob[j]]>maxi){
				maxi=cpt[prob[j]];
				k=j;
			}
		}
		// cout<<k<<endl;
		running_data[i][wildy] = value_index[wildy][value[k]];

	}
}

void soft_infer(network &Alarm){
	srand(time(0));
	for(int i = 0 ; i < wild_in_data.size() ; i++){
		int wildy = wild_in_data[i];
		list<Graph_Node>::iterator it;
		it = Alarm.search_node(node_name[wildy]);
		vector<float> cpt = it->working_CPT;
		vector<string> value = it->get_values();

		// vector<int> missing_index = cpt_for_missing_data[i];
		vector<float> prob;
		for( auto j : cpt_for_missing_data[i]){
			prob.push_back(cpt[j]);
		}
		int k = 0 ;
		float random_number = rand();
		float random_number2 = (float)random_number/RAND_MAX;
		// float random_number2 = 0.0001;

		float total_prob = 0;
		for(auto elm : prob){
			total_prob+=elm;
		}

		random_number2 = random_number2 * total_prob;
		// cout<<(random_number2<total_prob)<<endl;

		float running_sum = 0;
		while(running_sum<random_number2){
			running_sum+=prob[k];
			k++;
		}
		k--;
		// cout<< (k<prob.size()) << endl;
		running_data[i][wildy] = value_index[wildy][value[k]];

	}
}


int main()
{
	network Alarm;
	Alarm=read_network();
	
	list<Graph_Node> :: iterator it,it2;
	int idx = 0;
	list<Graph_Node> node_list = Alarm.Pres_Graph;
	for(it = node_list.begin() ; it != node_list.end() ; it++){
		node_index[it->get_name()] = idx;
		node_name[idx] = it->get_name();
		idx++;
	}


	auto start = chrono :: high_resolution_clock :: now();
	process_data(Alarm);
	cout<<"Data processed !"<<endl;

	auto end = chrono :: high_resolution_clock :: now();

	

	for(int i = 0 ; i<wild_in_data.size() ; i++){
		int wild_position = wild_in_data[i];
		list<Graph_Node> :: iterator it2;
		it2 = Alarm.search_node(node_name[wild_position]);
		vector<string> par = it2->get_Parents();
		
		vector<int> cpt_ptr = cpt_pointer(Alarm,wild_position,running_data[i],par);
		cpt_for_missing_data.push_back(cpt_ptr);
	}

	start = chrono :: high_resolution_clock :: now();

	while(chrono::duration_cast<chrono::milliseconds>(end-start).count()<40000){
		learn(Alarm);
		soft_infer(Alarm);
		end = chrono :: high_resolution_clock :: now();

	}





	for(int i = 0 ; i < Alarm.netSize() ; i++){
		it = Alarm.search_node(node_name[i]);
		vector<float> cpt = it->working_CPT;
		for(auto i : cpt){
			cout<< i << " ";
		}
		cout<<endl;
	}
}





