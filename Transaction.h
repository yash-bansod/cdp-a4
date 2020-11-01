#ifndef TRANSACTION_H
#define TRANSACTION_H
class Transaction
{
    private :
        int tx_id;
        std::vector<std::pair <char, int*> > ops;
        bool res; //true if commit, false if abort
    public :
        Transaction(int id);
        void add_op(char opcode, int *var);
};

#endif
