#include<bits/stdc++.h>
#include "Transaction.h"
using namespace std;

map<string,int> sym_table;      //symbol table: variable name , value

void* initiate_tid(void* args)
{
    Transaction *tx = (Transaction*)args;
    int ret=tx->get_txid();
    cout<<ret<<endl;
    pthread_exit(&ret);
}

void begin_transactions(vector<Transaction>* TX)
{
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

    return 0;
}

