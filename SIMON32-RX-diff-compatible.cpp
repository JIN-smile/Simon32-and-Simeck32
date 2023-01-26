/**********************************************************************/
/* This file is an automated tool for finding RX-characteristics      */
/* in SIMON32 to the paper:  	                                      */
/* Improved Rotational-XOR Cryptanalysis of Simon-like Block Ciphers  */
/* by Jinyu Lu, Yunwen Liu, Tomer Ashur, Bing Sun and Chao Li.	 	  */
/**********************************************************************/


#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

void printhead()
{
    cout << "(set-logic QF_ABV)" << endl;
    cout << "(set-info :smt-lib-version 2.5)" << endl;
    cout << "(set-option :produce-models true)" <<endl;
}



void declare(int r)
{

    for(int i=1; i<=r; i++)
    {
        cout << "(declare-fun lft_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun rgt_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun key_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun d_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun varibits_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun doublebits_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun z_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun wr_" << i << " () (_ BitVec 16)) " <<endl;

    }


    cout << "(declare-fun lft_" << r+1 << " () (_ BitVec 16)) " ;
    cout << "(declare-fun rgt_" << r+1 << " () (_ BitVec 16)) " ;
    cout << endl;



    for(int i=1; i<=r; i++)
    {
        cout << "(declare-fun k_" << i << " () (_ BitVec 16)) " <<endl;

    }


    for(int i=1; i<=(r+1); i++)
    {
        cout << "(declare-fun value_k_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun value_kpie_" << i << " () (_ BitVec 16)) " <<endl;
    }

    for(int i=1; i<=(r+1); i++)
    {
        cout << "(declare-fun value_lft_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun value_rgt_" << i << " () (_ BitVec 16)) " <<endl;

        cout << "(declare-fun value_lftpie_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun value_rgtpie_" << i << " () (_ BitVec 16)) " <<endl;

        cout << "(declare-fun rxdiff_lft_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun rxdiff_rgt_" << i << " () (_ BitVec 16)) " <<endl;
        cout << "(declare-fun rxdiff_k_" << i << " () (_ BitVec 16)) " <<endl;
    }

    for(int i = 1; i <= (r-4); i++)
    {
        cout << "(declare-fun c_" << i << " () (_ BitVec 16)) " <<endl;
    }


    cout << "(declare-fun Weight () (_ BitVec 16)) " ;
    cout << endl;

    //define hamming weight
    cout <<"(define-fun w_H0 ((x (_ BitVec 16))) (_ BitVec 16)"<<endl;
    cout <<"      (bvadd (bvand x #x5555 )"<<endl;
    cout <<"             (bvand (bvlshr x #x0001) #x5555 )))"<<endl;
    cout <<"(define-fun w_H1 ((x (_ BitVec 16))) (_ BitVec 16)"<<endl;
    cout <<"      (bvadd (bvand x #x3333 )"<<endl;
    cout <<"             (bvand (bvlshr x #x0002) #x3333 )))"<<endl;
    cout <<"(define-fun w_H2 ((x (_ BitVec 16))) (_ BitVec 16)"<<endl;
    cout <<"      (bvadd (bvand x #x0f0f )"<<endl;
    cout <<"             (bvand (bvlshr x #x0004) #x0f0f)))"<<endl;
    cout <<"(define-fun w_H3 ((x (_ BitVec 16))) (_ BitVec 16)"<<endl;
    cout <<"      (bvadd (bvand x #x00ff )"<<endl;
    cout <<"             (bvand (bvlshr x #x0008) #x00ff )))"<<endl;
    cout <<"(define-fun w_H ((x (_ BitVec 16))) (_ BitVec 16) (w_H3 (w_H2 (w_H1 (w_H0 x)))))"<<endl;

}

void cczformula_delta(int r)
{
    //key schedule
    for(int i=1; i<= (r-4);i++)
    {
        if((i == 1) || (i == 2) || (i == 3) || (i == 4)
           || (i == 5) || (i == 7) || (i == 11)||
           (i == 14) || (i == 16) || (i == 18) ||
           (i == 19) || (i == 24) || (i == 25) ||
           (i == 26))
        {
            cout<< " (assert (= c_"<<i<<" #xfffd))"<<endl;
        }
        else
        {
            cout<< " (assert (= c_"<<i<<" #xfffc))"<<endl;
        }

    }


    for(int i=1; i<=(r-4);i++)
    {

        cout<< " (assert (= k_"<< i+4 <<"  (bvxor ((_ rotate_right 1)(bvxor ((_ rotate_right 3) k_"<< i+3<<") k_"<< i+1<<"))(bvxor (bvxor k_"<< i<<"  (bvxor ((_ rotate_left 1) c_"<< i<<") c_"<< i<<")) (bvxor ((_ rotate_right 3) k_"<< i+3<<") k_"<< i+1<<")))))"<<endl;

    }


    //data part

    for(int i = 1; i <= r;i++)
    {
        cout<< " (assert (= key_"<<i<<"  k_"<<i<<"))"<<endl;

        cout<< " (assert (= rgt_"<< i+1 <<"  lft_"<< i<<" ))"<<endl;

        cout<< " (assert (= varibits_"<< i <<"  (bvor  ((_ rotate_left 8) lft_"<< i <<") ((_ rotate_left 1)lft_"<< i <<"))))"<<endl;

        cout<< " (assert (= doublebits_"<< i <<"  (bvand  (bvand (bvxor ((_ rotate_left 8) lft_"<< i <<") #xffff)  ((_ rotate_left 1)lft_"<< i <<")) ((_ rotate_left 15) lft_"<< i <<"))))"<<endl;

        cout<< " (assert (= #x0000 (bvand  (bvxor varibits_"<< i <<" #xffff)  d_"<< i <<"))) "<<endl;

        cout<< " (assert (= #x0000 (bvand  (bvxor ((_ rotate_left 7) d_"<< i <<") d_"<< i <<") doublebits_"<< i <<"))) "<<endl;

        cout<< " (assert (= lft_"<< i+1 <<"  (bvxor (bvxor rgt_"<<i<<" d_"<<i<<") (bvxor ((_ rotate_left 2) lft_"<<i<<") key_"<<i<<"))))"<<endl;

        cout<< " (assert (= z_"<< i <<" (bvxor varibits_"<< i <<"  doublebits_"<< i <<")))"<<endl;

    }


}

void cczformula_value(int r)
{


    //key schedule
    for(int i=1; i<=(r-4);i++)
    {

        cout<< " (assert (= value_k_"<< i+4 <<"  (bvxor ((_ rotate_right 1)(bvxor ((_ rotate_right 3) value_k_"<< i+3<<") value_k_"<< i+1<<"))(bvxor (bvxor value_k_"<< i<<"  c_"<< i<<") (bvxor ((_ rotate_right 3) value_k_"<< i+3<<") value_k_"<< i+1<<")))))"<<endl;

    }


    for(int i=1; i<=(r-4);i++)
    {

        cout<< " (assert (= value_kpie_"<< i+4 <<"  (bvxor ((_ rotate_right 1)(bvxor ((_ rotate_right 3) value_kpie_"<< i+3<<") value_kpie_"<< i+1<<"))(bvxor (bvxor value_kpie_"<< i<<"  c_"<< i<<") (bvxor ((_ rotate_right 3) value_kpie_"<< i+3<<") value_kpie_"<< i+1<<")))))"<<endl;

    }

    for(int i=1; i<=r;i++)
    {

        cout<< " (assert (= rxdiff_k_"<< i <<" (bvxor ((_ rotate_left 1) value_k_"<< i <<") value_kpie_"<< i <<")))"<<endl;


    }

    for(int i=1; i<=r;i++)
    {

        cout<< " (assert (= rxdiff_k_"<< i <<"  key_"<<i<<"))"<<endl;


    }

    //data part
    for(int i=1; i<=r;i++)
    {
        cout<< " (assert (= value_rgt_"<< i+1 <<"  value_lft_"<< i <<"))"<<endl;

        cout<< " (assert (= value_lft_"<< i+1 <<"  (bvxor (bvxor (bvxor (bvand ((_ rotate_left 8) value_lft_"<< i <<") ((_ rotate_left 1) value_lft_"<< i <<")) ((_ rotate_left 2) value_lft_"<< i <<")) value_rgt_"<< i <<") value_k_"<< i <<")))"<<endl;

    }

    for(int i=1; i<=r;i++)
    {
        cout<< " (assert (= value_rgtpie_"<< i+1 <<"  value_lftpie_"<< i <<"))"<<endl;

        cout<< " (assert (= value_lftpie_"<< i+1 <<"  (bvxor (bvxor (bvxor (bvand ((_ rotate_left 8) value_lftpie_"<< i <<") ((_ rotate_left 1) value_lftpie_"<< i <<")) ((_ rotate_left 2) value_lftpie_"<< i <<")) value_rgtpie_"<< i <<") value_kpie_"<< i <<")))"<<endl;

    }

    for(int i=1; i<=(r+1);i++)
    {

        cout<< " (assert (= rxdiff_lft_"<< i <<" (bvxor ((_ rotate_left 1) value_lft_"<< i <<") value_lftpie_"<< i <<")))"<<endl;
        cout<< " (assert (= rxdiff_rgt_"<< i <<" (bvxor ((_ rotate_left 1) value_rgt_"<< i <<") value_rgtpie_"<< i <<")))"<<endl;


    }

    for(int i=1; i<=(r+1);i++)
    {

        cout<< " (assert (= rxdiff_lft_"<< i <<"  lft_"<<i<<"))"<<endl;
        cout<< " (assert (= rxdiff_rgt_"<< i <<"  rgt_"<<i<<"))"<<endl;

    }
}



void objectivefun(int r,int bound_plain)
{

    for(int i=1; i<=r;i++)
    {
        cout<< " (assert (= wr_"<<i<<" (w_H z_"<<i<<")))"<<endl;
    }

    cout << " (assert (= ";

    for(int i=1; i<=r; i++)
    {
        if(i<r)
        {
            cout << "(bvadd  wr_"<<i<<" ";
        }

        else
        {
            cout << "wr_"<< r <<" ";
        }
    }

    for(int i=1; i<r; i++)
    {
        cout << ")";
    }

    cout <<" (_ bv"<<bound_plain<<" 16)))"<<endl;
}

void tail(int r)
{

	cout << "(check-sat)" << endl;


	for(int i=1;i<=(r+1);i++)
	{

	    cout << "(get-value (value_lft_"<< i <<"))" << endl;
	    cout << "(get-value (value_rgt_"<< i <<"))" << endl;

	    cout << "(get-value (value_lftpie_"<< i <<"))" << endl;
	    cout << "(get-value (value_rgtpie_"<< i <<"))" << endl;

	    cout << "(get-value (rxdiff_lft_"<< i <<"))" << endl;
	    cout << "(get-value (lft_"<< i <<"))" << endl;

	    cout << "(get-value (rxdiff_rgt_"<< i <<"))" << endl;
	    cout << "(get-value (rgt_"<< i <<"))" << endl;
	}

	for(int i=1;i<=r;i++)
	{

	    cout << "(get-value (value_k_"<< i <<"))" << endl;
	    cout << "(get-value (value_kpie_"<< i <<"))" << endl;

	    cout << "(get-value (rxdiff_k_"<< i <<"))" << endl;

	    cout << "(get-value (key_"<< i <<"))" << endl;
	}



	for(int i=1;i<=r;i++)
	{

	    cout << "(get-value (k_"<< i <<"))" << endl;

	}


	for(int i=1;i<=r;i++)
	{

	    cout << "(get-value (lft_"<< i <<"))" << endl;
	    cout << "(get-value (rgt_"<< i <<"))" << endl;
	    cout << "(get-value (key_"<< i <<"))" << endl;
	    cout << "(get-value (d_"<< i <<"))" << endl;
	    cout << "(get-value (z_"<< i <<"))" << endl;
	    cout << "(get-value (wr_"<< i <<"))" << endl;


	}

	cout << "(get-value (lft_"<< r+1 <<"))" << endl;
    cout << "(get-value (rgt_"<< r+1 <<"))" << endl;

	cout << "(exit)" << endl;


}

int main(int argc, char** argv)
{
    int ROUND = atoi(argv[1]);
    int BOUND_PLAIN = atoi(argv[2]);
    printhead();

    declare(ROUND);

    cczformula_delta(ROUND);
    cczformula_value(ROUND);

    objectivefun(ROUND,BOUND_PLAIN);

    tail(ROUND);

    return 0;
}






