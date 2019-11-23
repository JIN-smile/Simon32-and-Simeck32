#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

struct A{
    uint16_t out_put[2];
};

uint16_t ROL(uint16_t x, int r)
{
    uint16_t result;

    result = (x << r) | (x >> (16 - r));

    return result;
}

struct A ROUND32(uint16_t constant,uint16_t input_lft,uint16_t input_rgt)
{

    struct A s;

    s.out_put[0] = (ROL(input_lft,5) & input_lft) ^ input_rgt ^ ROL(input_lft,1) ^constant;
    s.out_put[1] = input_lft;
    return s;

}

int main()
{
    struct A s;
    uint16_t temp;

    int count = 0;
    uint16_t key[15][4],key_diff[15][4];

    uint16_t lft[16],rgt[16];

    uint16_t lft_diff[16],rgt_diff[16];
    uint16_t chafen_plain_lft[16];
    uint16_t chafen_plain_rgt[16];

    FILE *fp = fopen("15-26-18-RX-trail-everyround_1.txt","w");

    //key schedule
    uint16_t constant[14] = {0xfffd,0xfffd,0xfffd,0xfffd,
                            0xfffd,0xfffc,0xfffc,0xfffc,

                            0xfffd,0xfffd,0xfffc,0xfffd,
                            0xfffd,0xfffd};

    key[0][0] = 0x60ed;
    key[0][1] = 0x1c3d;
    key[0][2] = 0x9c31;
    key[0][3] = 0x32ad;

    key_diff[0][0] = ROL(key[0][0],1) ^ 0x0014;

    key_diff[0][1] = ROL(key[0][1],1) ^ 0x0008;
    key_diff[0][2] = ROL(key[0][2],1) ^ 0x0004;
    key_diff[0][3] = ROL(key[0][3],1) ^ 0x0001;

    for(int r = 0; r < 14 ;r++)
    {
        s = ROUND32(constant[r],key[r][1],key[r][0]);
        key[r+1][1] = s.out_put[0];
        key[r+1][0] = s.out_put[1];

        temp = key[r+1][1];
        key[r+1][1] = key[r][2];
        key[r+1][2] = key[r][3];
        key[r+1][3] = temp;

    }

    for(int r = 0; r < 14 ;r++)
    {

        s = ROUND32(constant[r],key_diff[r][1],key_diff[r][0]);
        key_diff[r+1][1] = s.out_put[0];
        key_diff[r+1][0] = s.out_put[1];

        temp = key_diff[r+1][1];
        key_diff[r+1][1] = key_diff[r][2];
        key_diff[r+1][2] = key_diff[r][3];
        key_diff[r+1][3] = temp;
    }


    //data part
    for(int i = 0; i < 65536; i++)
    {
        printf("i:%d\n",i);
        printf("count:%d\n",count);

        for(int j = 0; j < 65536;j++)
        {
            lft[0] = i;
            rgt[0] = j;

            lft_diff[0] = ROL(lft[0],1) ^ 0x0000;
            rgt_diff[0] = ROL(rgt[0],1) ^ 0x0010;

            for(int r = 0 ; r < 15 ;r++)
            {

                s = ROUND32(key[r][0],lft[r],rgt[r]);
                lft[r+1] = s.out_put[0];
                rgt[r+1] = s.out_put[1];

            }

            for(int r = 0 ; r < 15 ;r++)
            {
                s = ROUND32(key_diff[r][0],lft_diff[r],rgt_diff[r]);
                lft_diff[r+1] = s.out_put[0];
                rgt_diff[r+1] = s.out_put[1];
            }

            for(int r = 0; r < 16; r++)
            {
                chafen_plain_lft[r] = ROL(lft[r],1) ^ lft_diff[r];
                chafen_plain_rgt[r] = ROL(rgt[r],1) ^ rgt_diff[r];
            }



           if((chafen_plain_lft[0] == 0x0000)
               &&(chafen_plain_rgt[0] == 0x0010)

               &&(chafen_plain_lft[1] == 0x0004)
               &&(chafen_plain_rgt[1] == 0x0000)

               &&(chafen_plain_lft[2] == 0x0000)
               &&(chafen_plain_rgt[2] == 0x0004)

               &&(chafen_plain_lft[3] == 0x0000)
               &&(chafen_plain_rgt[3] == 0x0000)

               &&(chafen_plain_lft[4] == 0x0001)
               &&(chafen_plain_rgt[4] == 0x0000)

               &&(chafen_plain_lft[5] == 0x0001)
               &&(chafen_plain_rgt[5] == 0x0001)

               &&(chafen_plain_lft[6] == 0x0000)
               &&(chafen_plain_rgt[6] == 0x0001)

               &&(chafen_plain_lft[7] == 0x0001)
               &&(chafen_plain_rgt[7] == 0x0000)

               &&(chafen_plain_lft[8] == 0x0000)
               &&(chafen_plain_rgt[8] == 0x0001)

               &&(chafen_plain_lft[9] == 0x0003)
               &&(chafen_plain_rgt[9] == 0x0000)

               &&(chafen_plain_lft[10] == 0x0000)
               &&(chafen_plain_rgt[10] == 0x0003)

               &&(chafen_plain_lft[11] == 0x0002)
               &&(chafen_plain_rgt[11] == 0x0000)

               &&(chafen_plain_lft[12] == 0x0004)
               &&(chafen_plain_rgt[12] == 0x0002)

               &&(chafen_plain_lft[13] == 0x0002)
               &&(chafen_plain_rgt[13] == 0x0004)

               &&(chafen_plain_lft[14] == 0x0000)
               &&(chafen_plain_rgt[14] == 0x0002)

               &&(chafen_plain_lft[15] == 0x0002)
               &&(chafen_plain_rgt[15] == 0x0000))
            {
                fprintf(fp,"%04x",i);
                fprintf(fp," %04x\n",j);
                count = count + 1;
            }

        }
    }

    fprintf(fp,"\ncount:%d\n",count);
    fclose(fp);
    printf("测试结束");
    printf("\ncount:%d",count);

    return 0;

}



