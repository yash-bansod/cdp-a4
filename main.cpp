#include<bits/stdc++.h>
#include "Transaction.h"
using namespace std;

struct arg_struct {
    Transaction* tx;
    map<string,int>* sym_table;
};

void* initiate_tid(void* arguments)
{
    pthread_detach(pthread_self());
    struct arg_struct *args = (struct arg_struct *)arguments;
    cout<<args->tx->get_txid()<<endl<<flush;
    pthread_exit(NULL);
}

void begin_transactions(vector<Transaction>* TX, map<string,int>* sym_table)
{
    struct arg_struct args;
    args.sym_table=sym_table;
    //for(auto it=(*TX).begin();it!=(*TX).end();++it)
    //{
    //    //it->show_tx();
        args.tx=&(*(*TX).begin());
        //args.tx->show_tx();
    //    //cout<<"k"<<args.tx->get_txid()<<endl;
        pthread_t ptid;
    //    pthread_create(&ptid, NULL, &initiate_tid, (void*)&args);

    //}
        pthread_create(&ptid, NULL, &initiate_tid, (void*)&args);


    
}


int main(int argc, char *argv[])
{
    ifstream f_inp;     //input file
    f_inp.open(argv[1]);
    map<string,int> sym_table;      //symbol table: variable name , value
    string line;
    int i=0, num_tx;
    vector<Transaction> TX;     //All transactions

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
    begin_transactions(&TX, &sym_table);

    return 0;
}

