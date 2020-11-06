#include<bits/stdc++.h>
#include<unistd.h>
#include "Transaction.h"
#include "LockMgr.h"
using namespace std;

map<string,int> sym_table;      //symbol table: variable name , value

LockMgr* LOCK;

void signal_callback_handler(int signum) {
	cout << endl << "Caught signal " << signum << endl;
	if(LOCK != nullptr){
		set<int> culprits = LOCK->getCulprits();
		cout << "Transactions involved in deadLock :" << endl;
		for(int txid : culprits) cout << txid << " ";
		cout << endl;
	}

	exit(signum);
}

void* initiate_tid(void* args)
{
	map<string,int> t_sym;
	Transaction *tx = (Transaction*)args;
	int tx_id=tx->get_txid();
	//cout<<ret<<endl;
	vector< pair<char, string> > ops=tx->get_ops();
	for(auto it=ops.begin();it!=ops.end();++it)
	{
		switch(it->first)
		{
			case 'R':
			{
				LOCK->acquireReadLock(tx_id, it->second);
				break;
			}
			case 'W':
			{
				LOCK->acquireWriteLock(tx_id, it->second);
				break;
			}
			case 'O':
			{
				string var_op=it->second;
				int ispos=1;
				int eqpos=var_op.find("=");
				string var1=var_op.substr(0,eqpos);
				string nvar=var_op.substr(eqpos+1);
				int oppos=nvar.find("+");
				if(oppos==string::npos)
				{
					oppos=nvar.find("-");
					ispos=0;
				}
				string var2=nvar.substr(0,oppos); //ab=ab+1 eqpos=2 oppos=5
				string var3=nvar.substr(oppos+1);
				
				auto itr1=t_sym.find(var1);
				if(itr1==t_sym.end())
				{
					t_sym.insert({sym_table.find(var1)->first, sym_table.find(var1)->second});
					itr1=t_sym.find(var1);
				}
				auto itr2=sym_table.find(var2);
				auto itr3=sym_table.find(var3);
				if(itr3==sym_table.end())
				{
					if(ispos==1)
						itr1->second=itr2->second + stoi(var3);
					else
						itr1->second=itr2->second - stoi(var3);
				}
				else
				{
					if(ispos==1)
						itr1->second=itr2->second + itr3->second;
					else
						itr1->second=itr2->second - itr3->second;
				}
				break;
			}
			case 'C':
			{
				tx->set_res();
				for(auto itr=t_sym.begin();itr!=t_sym.end();++itr)
				{
					auto found=sym_table.find(itr->first);
					found->second=itr->second;
				}
				LOCK->releaseAllLocks(tx_id);
				break;
			}
			case 'A':
			{
				LOCK->releaseAllLocks(tx_id);
				break;
			}
			default:
			break;
		}
	}
	pthread_exit(&tx_id);
}

void begin_transactions(vector<Transaction>* TX)
{
	vector<string> db_vars;
	for(auto itr=sym_table.begin();itr!=sym_table.end();++itr)
	{
		db_vars.push_back(itr->first);
	}
	LockMgr lck(db_vars);
	LOCK=&lck;
	int n=(*TX).size(); 
	pthread_t ptid[n];
	void *status;
	int i=0;
	for(auto it=(*TX).begin();it!=(*TX).end();++it)
	{
		pthread_create(&ptid[i], NULL, &initiate_tid, (void*)&(*it));
		i++;
	}
	for(int i=0;i<n;i++)
	{
		pthread_join(ptid[i], &status);
	}
}


int main(int argc, char *argv[])
{
	LOCK = nullptr;
	signal(SIGINT, signal_callback_handler);

	vector<Transaction> TX;     //All transactions
	ifstream f_inp;     //input file
	f_inp.open(argv[1]);
	string line;
	int i=0, num_tx;

	while(f_inp) {      //while EOF
		if(i==0) {      //first line of file = num_tx
			getline(f_inp, line);
			num_tx=stoi(line);      //Number of transactions
		}
		else if(i==1) {     //Second line initialization of vars
			getline(f_inp, line);
			string token;
			int j=0;
			pair<string,int> sym_val;
			stringstream ss(line);
			while(getline(ss, token, ' ')) {        //Tokenize
				if(j%2==0)
					sym_val.first=token;
				else {
					sym_val.second=stoi(token);
					sym_table.insert(sym_val);      //Append to symbol table
				}
				j++;
			}
		}
		else {      //Transaction descriptions
			for(int j=0;j<num_tx;j++)
			{
				if(!getline(f_inp, line))
					break;
				int id=stoi(line);
				Transaction tx(id);
				while(getline(f_inp, line)) {
					istringstream ss(line);
					string token, var;
					int j=0;
					char opcode;
					while(ss >> token)
					{
						if(j==0)
						{
							if(token!="R" && token!="W" && token!="C" && token!="A") {
								opcode='O';
								var=token;
							}
							else
								opcode=token[0];
							if(opcode=='C' || opcode=='A') {
								var="";
								break;
							}
						}
						else if(j==1)
							var=token;
						j++;
					}
					tx.add_op(opcode, var);
					if(opcode=='C' || opcode=='A')
						break;
				}
				TX.push_back(tx);
			}
		}
		i++;
	}
	f_inp.close();
	begin_transactions(&TX);

	cout << endl << "Successfully executed all the transactions" << endl;
	for(auto it=sym_table.begin();it!=sym_table.end();++it)
	{
		cout<<it->first<<" "<<it->second<<"\t";
	}
	cout << endl;
	
	return 0;
}

