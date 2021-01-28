/**********************************************************************/
/* This file is an auxiliary key-recovery attack experimental         */
/* verification code to the paper:  	                              */
/* Improved Rotational-XOR Cryptanalysis of Simon-like Block Ciphers  */
/* by Jinyu Lu, Yunwen Liu, Tomer Ashur, Bing Sun and Chao Li.	 	  */
/* It is used to verify the key-recovery attack on 12-round Simeck32  */
/* with a 9-round RX-distinguisher presented in this paper.		      */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define ROL(x,i) (((x<<i)|(x>>(16-i)))&0xffff)//Circular left shift of x by i bits

#define TAKE_bit(x,i) ((x >> i) & 0x0001)//Take out the i-th bit of x


#ifndef SIMECK32_H
#define SIMECK32_H

#define ROUND32(key, lft, rgt, tmp) do { \
    tmp = (lft); \
    lft = ((lft) & ROL((lft), 5)) ^ ROL((lft), 1) ^ (rgt) ^ (key); \
    rgt = (tmp); \
} while (0)



uint16_t  transfer_byte_bit16(uint16_t x, uint16_t x_bit[])//x_bit[0] is the the least significant one of x
{

    for(int i = 0 ; i < 16; i++)
    {

        x_bit[i] = TAKE_bit(x,i);

    }

}



void simeck_32_64(
        const uint16_t master_key[],
        const uint16_t plaintext[],
        uint16_t ciphertext[],
        uint16_t lft_bit[][16],
        uint16_t rgt_bit[][16],
        uint16_t round_key_bit[][16])
{
    const int NUM_ROUNDS = 12;
    int idx;

    uint16_t keys[4] =
    {
        master_key[0],
        master_key[1],
        master_key[2],
        master_key[3],
    };

    ciphertext[0] = plaintext[0];
    ciphertext[1] = plaintext[1];

    uint16_t temp;

    uint16_t constant = 0xFFFC;
    uint32_t sequence = 0x9A42BB1F;

    for (idx = 0; idx < NUM_ROUNDS; idx++)
    {

        transfer_byte_bit16(keys[0],round_key_bit[idx]);


        transfer_byte_bit16(ciphertext[1],lft_bit[idx]);
        transfer_byte_bit16(ciphertext[0],rgt_bit[idx]);

        ROUND32(keys[0],ciphertext[1],ciphertext[0],temp);

        constant &= 0xFFFC;
        constant |= sequence & 1;
        sequence >>= 1;
        ROUND32(constant,keys[1],keys[0],temp);

        // rotate the LFSR of keys
        temp = keys[1];
        keys[1] = keys[2];
        keys[2] = keys[3];
        keys[3] = temp;
    }


    transfer_byte_bit16(ciphertext[1],lft_bit[NUM_ROUNDS]);
    transfer_byte_bit16(ciphertext[0],rgt_bit[NUM_ROUNDS]);
}

#endif  // SIMECK32_H


uint32_t M_right = 1;//for random numbers
int kui;
uint32_t Jl = 0xaa7197bb, Jr = 0xe693983c;
//If you want change this set of random numbers, you can change Jl or Jr to take success.
uint32_t M_left;


uint32_t Random()
{
	kui = M_right & 0x1;
	M_right = (M_right >>1) ^ (M_left <<31) ;
	M_left >>=1;

	if(kui)
	{
		M_right ^= Jr;
		M_left ^= Jl;
	}

	return M_right & 0xffffffff;
} //random numbers


int main()
{

    FILE *fp = fopen("Simeck32-RX-diff-key-recovery-9-12round-6guess-1.txt","w");
    //the key-recovery attack detail data can be find in "Simeck32-RX-diff-key-recovery-9-12round-6guess-1.txt" file.


    const uint16_t key64[] =
    {
        0x9c61,
        0x4e01,
        0x221f,
        0xe56f,
    };//Collect the encrypted pairs after 12 rounds under key64[] and key64_pie[]

    const uint16_t key64_pie[] =
    {
        0x38d7,
        0x9c0a,
        0x4438,
        0xcadf,
    };



    uint16_t plain[2];
    uint16_t plain_pie[2];
    uint32_t temp;

    uint16_t cipher[2];
    uint16_t cipher_pie[2];

    uint16_t chafen_cipher[2];//the RX-difference of cipher

    uint16_t tmp_chafen_cipher[2];


    uint16_t cipher_lft[13];
    uint16_t cipher_rgt[13];

    uint16_t cipher_lft_pie[13];
    uint16_t cipher_rgt_pie[13];

    uint16_t chafen_cipher_lft[13][16];
    uint16_t chafen_cipher_rgt[13][16];


    int count = 0;



    uint16_t lft_bit[13][16];
    uint16_t rgt_bit[13][16];

    uint16_t round_key_bit[12][16];


    uint16_t lft_bit_pie[13][16];
    uint16_t rgt_bit_pie[13][16];

    uint16_t round_key_bit_pie[12][16];


    uint16_t guess_key_bit[3][16];
    uint16_t guess_key_bit_pie[3][16];


    uint16_t guess_key[3] = {0};
    uint16_t guess_key_pie[3] = {0};


    uint16_t guess_lft_bit[3][16];
    uint16_t guess_lft_bit_pie[3][16];


    uint16_t guess_rgt_bit[3][16];
    uint16_t guess_rgt_bit_pie[3][16];


    uint16_t guess_lft[3] = {0};
    uint16_t guess_lft_pie[3] = {0};


    uint16_t guess_rgt[3] = {0};
    uint16_t guess_rgt_pie[3] = {0};

    uint16_t chafen_guess_lft_bit[3][16];
    uint16_t chafen_guess_rgt_bit[3][16];


    uint16_t chafen_guess_lft[3] = {0};
    uint16_t chafen_guess_rgt[3] = {0};



    int num_guess_key = 0;
    int sum[64] = {0};//Traverse 6-bit guess key, i.e., 2^{6} candidate key.

    int sum_not0 = 0;




    for(int i = 0; i < 2048; i++)//Initialize random()
    {
        temp = Random();
    }



    for(int i = 0 ; i < 32768; i++)//Taking 2^{15} pairs of plaintexts with the input RX-difference (0020,0464)
    {

        temp = Random();


        plain[1] = (temp >> 16) & 0xffff;
        plain[0] = temp & 0xffff;


        plain_pie[1] = ROL(plain[1], 1) ^ 0x0020;
        plain_pie[0] = ROL(plain[0], 1) ^ 0x0464;


        simeck_32_64(key64, plain, cipher, lft_bit, rgt_bit, round_key_bit);


        simeck_32_64(key64_pie, plain_pie, cipher_pie, lft_bit_pie, rgt_bit_pie, round_key_bit_pie);


        chafen_cipher[1] = ROL(cipher[1], 1) ^ cipher_pie[1];
        chafen_cipher[0] = ROL(cipher[0], 1) ^ cipher_pie[0];


        tmp_chafen_cipher[1] = chafen_cipher[1] & 0xe210;//delta-lft=000* **0* ***1 ****

        tmp_chafen_cipher[0] = chafen_cipher[0] & 0xff18;//delta-rgt=0000 0000 ***0 1***


        if((tmp_chafen_cipher[0] == 0x0008) && (tmp_chafen_cipher[1] == 0x0010))// Collect the encrypted pairs after 12 rounds.
        {

            count = count + 1;


            //Start traverse the 6-bit guess key
            for(int i12 = 0 ; i12 < 2; i12++)
            {
                for(int i11 = 0; i11 < 2; i11++)
                {
                    for(int i10 = 0; i10 < 2; i10++)
                    {
                        for(int i6 = 0; i6 < 2; i6++)
                        {
                            for(int i5 = 0; i5 < 2; i5++)
                            {
                                for(int i4 = 0; i4 < 2; i4++)
                                {
                                    //initialization
                                    guess_key[0] = 0;
                                    guess_key[1] = 0;
                                    guess_key[2] = 0;


                                    guess_key_pie[0] = 0;
                                    guess_key_pie[1] = 0;
                                    guess_key_pie[2] = 0;


                                    guess_lft[0] = 0;
                                    guess_lft[1] = 0;
                                    guess_lft[2] = 0;


                                    guess_rgt[0] = 0;
                                    guess_rgt[1] = 0;
                                    guess_rgt[2] = 0;


                                    guess_lft_pie[0] = 0;
                                    guess_lft_pie[1] = 0;
                                    guess_lft_pie[2] = 0;


                                    guess_rgt_pie[0] = 0;
                                    guess_rgt_pie[1] = 0;
                                    guess_rgt_pie[2] = 0;


                                    num_guess_key = num_guess_key + 1;


                                    guess_key_bit[0][0] = Random() & 0x1;
                                    guess_key_bit[0][1] = Random() & 0x1;
                                    guess_key_bit[0][2] = Random() & 0x1;
                                    guess_key_bit[0][3] = Random() & 0x1;
                                    guess_key_bit[0][4] = i4;
                                    guess_key_bit[0][5] = i5;
                                    guess_key_bit[0][6] = i6;
                                    guess_key_bit[0][7] = Random() & 0x1;
                                    guess_key_bit[0][8] = Random() & 0x1;
                                    guess_key_bit[0][9] = Random() & 0x1;
                                    guess_key_bit[0][10] = i10;
                                    guess_key_bit[0][11] = i11;
                                    guess_key_bit[0][12] = i12;
                                    guess_key_bit[0][13] = Random() & 0x1;
                                    guess_key_bit[0][14] = Random() & 0x1;
                                    guess_key_bit[0][15] = Random() & 0x1;


                                    guess_key_bit[1][0] = Random() & 0x1;
                                    guess_key_bit[1][1] = Random() & 0x1;
                                    guess_key_bit[1][2] = Random() & 0x1;
                                    guess_key_bit[1][3] = Random() & 0x1;
                                    guess_key_bit[1][4] = Random() & 0x1;
                                    guess_key_bit[1][5] = Random() & 0x1;
                                    guess_key_bit[1][6] = Random() & 0x1;
                                    guess_key_bit[1][7] = Random() & 0x1;
                                    guess_key_bit[1][8] = Random() & 0x1;
                                    guess_key_bit[1][9] = Random() & 0x1;
                                    guess_key_bit[1][10] = Random() & 0x1;
                                    guess_key_bit[1][11] = Random() & 0x1;
                                    guess_key_bit[1][12] = Random() & 0x1;
                                    guess_key_bit[1][13] = Random() & 0x1;
                                    guess_key_bit[1][14] = Random() & 0x1;
                                    guess_key_bit[1][15] = Random() & 0x1;


                                    guess_key_bit[2][0] = Random() & 0x1;
                                    guess_key_bit[2][1] = Random() & 0x1;
                                    guess_key_bit[2][2] = Random() & 0x1;
                                    guess_key_bit[2][3] = Random() & 0x1;
                                    guess_key_bit[2][4] = Random() & 0x1;
                                    guess_key_bit[2][5] = Random() & 0x1;
                                    guess_key_bit[2][6] = Random() & 0x1;
                                    guess_key_bit[2][7] = Random() & 0x1;
                                    guess_key_bit[2][8] = Random() & 0x1;
                                    guess_key_bit[2][9] = Random() & 0x1;
                                    guess_key_bit[2][10] = Random() & 0x1;
                                    guess_key_bit[2][11] = Random() & 0x1;
                                    guess_key_bit[2][12] = Random() & 0x1;
                                    guess_key_bit[2][13] = Random() & 0x1;
                                    guess_key_bit[2][14] = Random() & 0x1;
                                    guess_key_bit[2][15] = Random() & 0x1;


                                    for(int m = 0 ; m < 16; m++)
                                    {

                                        guess_key[0] = (guess_key_bit[0][m] << m ) ^ guess_key[0];

                                        guess_key[1] = (guess_key_bit[1][m] << m ) ^ guess_key[1];

                                        guess_key[2] = (guess_key_bit[2][m] << m ) ^ guess_key[2];

                                    }


                                    guess_key_pie[0] = ROL(guess_key[0],1) ^ 0x0005;
                                    guess_key_pie[1] = ROL(guess_key[1],1) ^ 0x0005;
                                    guess_key_pie[2] = ROL(guess_key[2],1) ^ 0x0007;




                                    transfer_byte_bit16(guess_key_pie[0],guess_key_bit_pie[0]);
                                    transfer_byte_bit16(guess_key_pie[1],guess_key_bit_pie[1]);
                                    transfer_byte_bit16(guess_key_pie[2],guess_key_bit_pie[2]);




                                    for(int p = 0; p < 16; p++)//Decrypt the last round
                                    {
                                        guess_lft_bit[0][p] = rgt_bit[12][p];
                                        guess_rgt_bit[0][p] = rgt_bit[12][p] & rgt_bit[12][(p-5+16)%16] ^ rgt_bit[12][(p-1+16)%16] ^ lft_bit[12][p] ^ guess_key_bit[0][p];

                                    }



                                    for(int p1 = 0; p1 < 16; p1++)//Decrypt the penultimate round
                                    {

                                        guess_lft_bit[1][p1] = guess_rgt_bit[0][p1];
                                        guess_rgt_bit[1][p1] = guess_rgt_bit[0][p1] & guess_rgt_bit[0][(p1+11)%16] ^ guess_rgt_bit[0][(p1+15)%16] ^ guess_lft_bit[0][p1] ^ guess_key_bit[1][p1];


                                    }


                                    for(int p2 = 0; p2 < 16; p2++)//Decrypt the ante-penultimate round
                                    {

                                        guess_lft_bit[2][p2] = guess_rgt_bit[1][p2];
                                        guess_rgt_bit[2][p2] = guess_rgt_bit[1][p2] & guess_rgt_bit[1][(p2-5+16)%16] ^ guess_rgt_bit[1][(p2-1+16)%16] ^ guess_lft_bit[1][p2] ^ guess_key_bit[2][p2];


                                    }



                                    for(int q = 0; q < 16; q++)//Decrypt the last round
                                    {
                                        guess_lft_bit_pie[0][q] = rgt_bit_pie[12][q];
                                        guess_rgt_bit_pie[0][q] = rgt_bit_pie[12][q] & rgt_bit_pie[12][(q-5+16)%16] ^ rgt_bit_pie[12][(q-1+16)%16] ^ lft_bit_pie[12][q] ^ guess_key_bit_pie[0][q];

                                    }


                                    for(int q1 = 0 ; q1 < 16; q1++) //Decrypt the penultimate round
                                    {

                                        guess_lft_bit_pie[1][q1] = guess_rgt_bit_pie[0][q1];
                                        guess_rgt_bit_pie[1][q1] = guess_rgt_bit_pie[0][q1] & guess_rgt_bit_pie[0][(q1-5+16)%16] ^ guess_rgt_bit_pie[0][(q1-1+16)%16] ^ guess_lft_bit_pie[0][q1] ^ guess_key_bit_pie[1][q1];


                                    }




                                    for(int q2 = 0 ; q2 < 16; q2++) //Decrypt the ante-penultimate round
                                    {

                                        guess_lft_bit_pie[2][q2] = guess_rgt_bit_pie[1][q2];
                                        guess_rgt_bit_pie[2][q2] = guess_rgt_bit_pie[1][q2] & guess_rgt_bit_pie[1][(q2-5+16)%16] ^ guess_rgt_bit_pie[1][(q2-1+16)%16] ^ guess_lft_bit_pie[1][q2] ^ guess_key_bit_pie[2][q2];

                                    }


                                    for(int n = 0 ; n < 16; n++)
                                    {
                                        guess_lft[0] = (guess_lft_bit[0][n] << n ) ^ guess_lft[0];
                                        guess_rgt[0] = (guess_rgt_bit[0][n] << n ) ^ guess_rgt[0];


                                        guess_lft_pie[0] = (guess_lft_bit_pie[0][n] << n ) ^ guess_lft_pie[0];
                                        guess_rgt_pie[0] = (guess_rgt_bit_pie[0][n] << n ) ^ guess_rgt_pie[0];

                                    }



                                    for(int n = 0 ; n < 16; n++)
                                    {
                                        guess_lft[1] = (guess_lft_bit[1][n] << n ) ^ guess_lft[1];
                                        guess_rgt[1] = (guess_rgt_bit[1][n] << n ) ^ guess_rgt[1];


                                        guess_lft_pie[1] = (guess_lft_bit_pie[1][n] << n ) ^ guess_lft_pie[1];
                                        guess_rgt_pie[1] = (guess_rgt_bit_pie[1][n] << n ) ^ guess_rgt_pie[1];

                                    }



                                    for(int n = 0 ; n < 16; n++)
                                    {
                                        guess_lft[2] = (guess_lft_bit[2][n] << n ) ^ guess_lft[2];
                                        guess_rgt[2] = (guess_rgt_bit[2][n] << n ) ^ guess_rgt[2];


                                        guess_lft_pie[2] = (guess_lft_bit_pie[2][n] << n ) ^ guess_lft_pie[2];
                                        guess_rgt_pie[2] = (guess_rgt_bit_pie[2][n] << n ) ^ guess_rgt_pie[2];

                                    }



                                    chafen_guess_lft[0] = ROL(guess_lft[0],1) ^ guess_lft_pie[0];//Calculate the RX-difference

                                    chafen_guess_rgt[0] = ROL(guess_rgt[0],1) ^ guess_rgt_pie[0];


                                    chafen_guess_lft[1] = ROL(guess_lft[1],1) ^ guess_lft_pie[1];

                                    chafen_guess_rgt[1] = ROL(guess_rgt[1],1) ^ guess_rgt_pie[1];


                                    chafen_guess_lft[2] = ROL(guess_lft[2],1) ^ guess_lft_pie[2];

                                    chafen_guess_rgt[2] = ROL(guess_rgt[2],1) ^ guess_rgt_pie[2];





                                    if((chafen_guess_lft[2] == 0x0000) && (chafen_guess_rgt[2] == 0x0000)
                                        )
                                    {


                                        sum[num_guess_key] = sum[num_guess_key] + 1;


                                    }

                                }

                            }

                        }

                    }

                }

            }

            num_guess_key = 0; //Initialization

        }

    }


    fprintf(fp,"########################################################################\n");
    fprintf(fp,"### READ ME: The probabilities in the encryption part with the 9-round                            ###\n");
    fprintf(fp,"### RX-distinguisher used in this paper is 2^{-12}.                                                             ###\n");
    fprintf(fp,"### So we take 2^{15} pairs of plaintexts with the input RX-difference (0020,0464).          ###\n");
    fprintf(fp,"### Collect the encrypted pairs after 12 rounds under a pair of keys:                                 ###\n");
    fprintf(fp,"### K =(e56f,221f,4e01,9c61) and K_pie=(cadf,4438,9c0a,38d7).                                      ###\n");
    fprintf(fp,"### To three-round partial decryption, we need guess six key bits in k^{11},                     ###\n");
    fprintf(fp,"### and set the other key bits in k^{11}, k^{10} and k^{9} being random.                         ###\n");
    fprintf(fp,"### The prediction number of right pairs for the correct key should 8 pairs.                      ###\n");
    fprintf(fp,"### Noticed that if you want change a set of plaintext pairs,                                             ###\n");
    fprintf(fp,"### you can change the code Jl or Jr for another parameter in line 105 to take success.    ###\n");
    fprintf(fp,"########################################################################\n\n\n");


    fprintf(fp,"Output the number of plaintext pairs remaining after the filtering step is:\n");
    fprintf(fp,"count: %d\n\n",count);//Output the number of plaintext pairs remaining after the filtering step.


    fprintf(fp,"Output the number of right pairs for the candidate key is:\n");
    for(int i = 0; i < 64; i++)
    {
        fprintf(fp,"sum[%d]:  %d\n", i,sum[i]);
    }


    fprintf(fp,"\nOutput the number of right pairs for the correct key is:\n");
    fprintf(fp,"sum[36]: %d\n",sum[36]);


    fclose(fp);

    printf("--- The result is generated in a txt file ---\n");

    printf("--- The End ---\n");

    return 0;

}

