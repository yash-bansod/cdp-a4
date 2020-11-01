#include<utility>
#include<vector>
#include "Transaction.h"
using namespace std;

Transaction::Transaction(int id)
{
    this->tx_id = id;
}

void Transaction::add_op(char opcode, int *var)
{
    pair<char, int*> op;
    op.first = opcode;
    op.second = var;
    this->ops.push_back(op);
}

