/*
 ±¾Ä£°å½ö¹©²Î¿¼
 */
#include <iostream>
#include <fstream>
#include <ctime>
#include "mmintrin.h"
using namespace std;

#define width 1920
#define height 1080

// 0:Y, 1:U, 2:V
unsigned char YUV[3][height][width] = {0};
unsigned char resultYUV[3][height][width]={0};
unsigned char tmpY[height][width]={0};
unsigned char tmpU[height/2][width/2]={0};
unsigned char tmpV[height/2][width/2]={0};
// 0:R, 1:G, 2:B
unsigned char RGB[3][height][width] = {0};
unsigned char tmpRGB[3][height][width] = {0};
void YUV2RGB();
void RGB2YUV();
void RGBBlending(int t);
unsigned char getbit(__m64 addr, int s) {
    return (unsigned char)(((long long)addr >> s)&((1 << 16) - 1));
}
int main(){
    char filename[50] = "../demo/dem1.yuv";
    char blending_name[50]="./alpha_blending.yuv";
    FILE *fp=fopen(filename,"r");
    if(fp == NULL){
        cout<<"cannot open file!"<<endl;
        exit(1);
    }
    fread(tmpY,sizeof(tmpY),1,fp);
    fread(tmpU,sizeof(tmpU),1,fp);
    fread(tmpV,sizeof(tmpV),1,fp);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            YUV[0][i][j]=tmpY[i][j];
        }
    }
    for(int i=0;i<height;i+=2){
        for(int j=0;j<width;j+=2){
            YUV[1][i][j]=tmpU[i/2][j/2];
            YUV[1][i][j+1]=YUV[1][i][j];
            YUV[1][i+1][j]=YUV[1][i][j];
            YUV[1][i+1][j+1]=YUV[1][i][j];
        }
    }
    for(int i=0;i<height;i+=2){
        for(int j=0;j<width;j+=2){
            YUV[2][i][j]=tmpV[i/2][j/2];
            YUV[2][i][j+1]=YUV[2][i][j];
            YUV[2][i+1][j]=YUV[2][i][j];
            YUV[2][i+1][j+1]=YUV[2][i][j];
        }
    }
    fclose(fp);
    ofstream out;
    out.open(blending_name,ofstream::out);
    if(!out.is_open()){
        cout<<"cannot open try!"<<endl;
        return 0;
    }
    clock_t start;
    clock_t end;
    int time=0;
    start = clock();
    YUV2RGB();
    end = clock();
    time += end - start;
    for(int t=1;t<255;t+=3){
        start = clock();
        RGBBlending(t);
        RGB2YUV();
        end = clock();
        time += end - start;
        for(int i=0;i<height;i++){
            for(int j=0;j<width;j++){
                out<<resultYUV[0][i][j];
            }
        }
        for(int i=0;i<height;i+=2){
            for(int j=0;j<width;j+=2){
                out<<resultYUV[1][i][j];
            }
        }
        for(int i=0;i<height;i+=2){
            for(int j=0;j<width;j+=2){
                out<<resultYUV[2][i][j];
            }
        }
    }
    out.close();
    cout<<"time:"<<time/ (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
    return 0;
}
void RGBBlending(int t){
    __m64 alpha = _mm_set_pi16((int16_t)t, (int16_t)t, (int16_t)t, (int16_t)t);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=4){
//            tmpRGB[0][i][j]=(double)t/256*RGB[0][i][j];
//            tmpRGB[1][i][j]=(double)t/256*RGB[1][i][j];
//            tmpRGB[2][i][j]=(double)t/256*RGB[2][i][j];

            __m64 RGB_0 = _mm_set_pi16((int16_t)RGB[0][i][j+3], (int16_t)RGB[0][i][j+2], (int16_t)RGB[0][i][j+1], (int16_t)RGB[0][i][j]);
            __m64 RGB_1 = _mm_set_pi16((int16_t)RGB[1][i][j+3], (int16_t)RGB[1][i][j+2], (int16_t)RGB[1][i][j+1], (int16_t)RGB[1][i][j]);
            __m64 RGB_2 = _mm_set_pi16((int16_t)RGB[2][i][j+3], (int16_t)RGB[2][i][j+2], (int16_t)RGB[2][i][j+1], (int16_t)RGB[2][i][j]);
            __m64 tmpRGB_0 = _m_pmullw(RGB_0, alpha);
            __m64 tmpRGB_1 = _m_pmullw(RGB_1, alpha);
            __m64 tmpRGB_2 = _m_pmullw(RGB_2, alpha);
            tmpRGB_0 = _mm_srli_pi16(tmpRGB_0, 8);
            tmpRGB_1 = _mm_srli_pi16(tmpRGB_1, 8);
            tmpRGB_2 = _mm_srli_pi16(tmpRGB_2, 8);
            tmpRGB[0][i][j]=getbit(tmpRGB_0,0);
            tmpRGB[0][i][j+1]=getbit(tmpRGB_0,16);
            tmpRGB[0][i][j+2]=getbit(tmpRGB_0,32);
            tmpRGB[0][i][j+3]=getbit(tmpRGB_0,48);
            
            tmpRGB[1][i][j]=getbit(tmpRGB_1,0);
            tmpRGB[1][i][j+1]=getbit(tmpRGB_1,16);
            tmpRGB[1][i][j+2]=getbit(tmpRGB_1,32);
            tmpRGB[1][i][j+3]=getbit(tmpRGB_1,48);
            
            tmpRGB[2][i][j]=getbit(tmpRGB_2,0);
            tmpRGB[2][i][j+1]=getbit(tmpRGB_2,16);
            tmpRGB[2][i][j+2]=getbit(tmpRGB_2,32);
            tmpRGB[2][i][j+3]=getbit(tmpRGB_2,48);
            
            
        }
    }
}
void YUV2RGB(){
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            // R = 1.164383 * (Y - 16) + 1.596027*(V - 128)
            // G = 1.164383 * (Y - 16) Ð 0.391762*(U - 128) Ð 0.812968*(V - 128)
            // B = 1.164383 * (Y - 16) + 2.017232*(U - 128)
            int R,G,B;
            R=1.164383*(YUV[0][i][j]-16)+1.596027*(YUV[2][i][j]-128);
            G = 1.164383 * (YUV[0][i][j] - 16) - 0.391762*(YUV[1][i][j] - 128) - 0.812968*(YUV[2][i][j] - 128);
            B=1.164383*(YUV[0][i][j]-16)+2.017232*(YUV[1][i][j]-128);
            if(R<0)
                RGB[0][i][j] = 0;
            else if(R>255)
                RGB[0][i][j] = 255;
            else
                RGB[0][i][j]=R;
            
            if(G<0)
                RGB[1][i][j] = 0;
            else if(G>255)
                RGB[1][i][j] = 255;
            else
                RGB[1][i][j]=G;
            
            if(B<0)
                RGB[2][i][j] = 0;
            else if(B>255)
                RGB[2][i][j] = 255;
            else
                RGB[2][i][j]=B;

        }
    }
}
void RGB2YUV(){
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            // Y = 0.256788*R + 0.504129*G + 0.097906*B + 16
            // U = -0.148223*R - 0.290993*G + 0.439216*B + 128
            // V = 0.439216*R - 0.367788*G - 0.071427*B + 128
            resultYUV[0][i][j]= 0.256788*tmpRGB[0][i][j] + 0.504129*tmpRGB[1][i][j] + 0.097906*tmpRGB[2][i][j] + 16;
            resultYUV[1][i][j]= -0.148223*tmpRGB[0][i][j] - 0.290993*tmpRGB[1][i][j] + 0.439216*tmpRGB[2][i][j] + 128;
            resultYUV[2][i][j]= 0.439216*tmpRGB[0][i][j] - 0.367788*tmpRGB[1][i][j] - 0.071427*tmpRGB[2][i][j] + 128;
        }
    }
}

//ÏÂÃæµÄ4¸öº¯ÊýÓ¦¸ÃÍ³¼Æ³öÍ¼Ïñ´¦ÀíµÄÊ±¼ä;
//º¯Êý²ÎÊýºÍ·µ»ØÖµ¿ÉÒÔÐèÒª×Ô¼º¶¨.
int process_without_simd(){
    int time = 0;
    
    /*´¦Àí¹ý³Ì*/
    
    return time;
}

int process_with_mmx(){
    int time = 0;
    
    /*´¦Àí¹ý³Ì*/
    
    return time;
}

int process_with_sse(){
    int time = 0;
    
    /*´¦Àí¹ý³Ì*/
    
    return time;
}

int process_with_avx(){
    int time = 0;
    /*´¦Àí¹ý³Ì*/
    
    
    return time;
}


