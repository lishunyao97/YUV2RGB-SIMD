/*
 ±¾Ä£°å½ö¹©²Î¿¼
 */
#include <iostream>
#include <fstream>
#include <ctime>
#include "xmmintrin.h"
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
    __m128 alpha = _mm_set_ps((float)t/256, (float)t/256, (float)t/256, (float)t/256);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=4){
//            tmpRGB[0][i][j]=(double)t/256*RGB[0][i][j];
//            tmpRGB[1][i][j]=(double)t/256*RGB[1][i][j];
//            tmpRGB[2][i][j]=(double)t/256*RGB[2][i][j];

            __m128 RGB_0 = _mm_set_ps((float)RGB[0][i][j+3], (float)RGB[0][i][j+2], (float)RGB[0][i][j+1], (float)RGB[0][i][j]);
            __m128 RGB_1 = _mm_set_ps((float)RGB[1][i][j+3], (float)RGB[1][i][j+2], (float)RGB[1][i][j+1], (float)RGB[1][i][j]);
            __m128 RGB_2 = _mm_set_ps((float)RGB[2][i][j+3], (float)RGB[2][i][j+2], (float)RGB[2][i][j+1], (float)RGB[2][i][j]);
            __m128 tmpRGB_0 = _mm_mul_ps(RGB_0, alpha);
            __m128 tmpRGB_1 = _mm_mul_ps(RGB_1, alpha);
            __m128 tmpRGB_2 = _mm_mul_ps(RGB_2, alpha);
            
            float f[4]={0};
            _mm_storer_ps(f,tmpRGB_0);
            tmpRGB[0][i][j]=f[3];
            tmpRGB[0][i][j+1]=f[2];
            tmpRGB[0][i][j+2]=f[1];
            tmpRGB[0][i][j+3]=f[0];
            
            _mm_storer_ps(f,tmpRGB_1);
            tmpRGB[1][i][j]=f[3];
            tmpRGB[1][i][j+1]=f[2];
            tmpRGB[1][i][j+2]=f[1];
            tmpRGB[1][i][j+3]=f[0];
            _mm_storer_ps(f,tmpRGB_2);
            tmpRGB[2][i][j]=f[3];
            tmpRGB[2][i][j+1]=f[2];
            tmpRGB[2][i][j+2]=f[1];
            tmpRGB[2][i][j+3]=f[0];
            
            
        }
    }
}
void YUV2RGB(){
    __m128 sub_0 = _mm_set_ps((float)16, (float)16, (float)16, (float)16);
    __m128 sub_1 = _mm_set_ps((float)128, (float)128, (float)128, (float)128);
    __m128 coe_0 = _mm_set_ps(1.164383, 1.164383, 1.164383, 1.164383);
    __m128 coe_1_G = _mm_set_ps(-0.391762,-0.391762,-0.391762,-0.391762);
    __m128 coe_1_B = _mm_set_ps(2.017232,2.017232,2.017232,2.017232);
    __m128 coe_2_R = _mm_set_ps(1.596027,1.596027,1.596027,1.596027);
    __m128 coe_2_G = _mm_set_ps(-0.812968,-0.812968,-0.812968,-0.812968);
    
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=4){
            // R = 1.164383 * (Y - 16) + 1.596027*(V - 128)
            // G = 1.164383 * (Y - 16) Ð 0.391762*(U - 128) Ð 0.812968*(V - 128)
            // B = 1.164383 * (Y - 16) + 2.017232*(U - 128)
//            float R,G,B;
//            R=1.164383*(YUV[0][i][j]-16)+1.596027*(YUV[2][i][j]-128);
//            G = 1.164383 * (YUV[0][i][j] - 16) - 0.391762*(YUV[1][i][j] - 128) - 0.812968*(YUV[2][i][j] - 128);
//            B=1.164383*(YUV[0][i][j]-16)+2.017232*(YUV[1][i][j]-128);
            __m128 YUV_0 = _mm_set_ps((float)YUV[0][i][j+3],(float)YUV[0][i][j+2],(float)YUV[0][i][j+1],(float)YUV[0][i][j]);
            __m128 YUV_1 = _mm_set_ps((float)YUV[1][i][j+3],(float)YUV[1][i][j+2],(float)YUV[1][i][j+1],(float)YUV[1][i][j]);
            __m128 YUV_2 = _mm_set_ps((float)YUV[2][i][j+3],(float)YUV[2][i][j+2],(float)YUV[2][i][j+1],(float)YUV[2][i][j]);
            YUV_0 = _mm_sub_ps(YUV_0,sub_0);
            YUV_1 = _mm_sub_ps(YUV_1,sub_1);
            YUV_2 = _mm_sub_ps(YUV_2,sub_1);
            __m128 RGB_Y = _mm_mul_ps(YUV_0,coe_0);
            __m128 G_U=_mm_mul_ps(YUV_1,coe_1_G);
            __m128 B_U=_mm_mul_ps(YUV_1,coe_1_B);
            __m128 R_V=_mm_mul_ps(YUV_2,coe_2_R);
            __m128 G_V=_mm_mul_ps(YUV_2,coe_2_G);
            __m128 rtR = _mm_add_ps(RGB_Y,R_V);
            __m128 rtG = _mm_add_ps(RGB_Y,G_U);
            rtG = _mm_add_ps(rtG,G_V);
            __m128 rtB = _mm_add_ps(RGB_Y,B_U);
            
            float f[4]={0};
            _mm_storer_ps(f,rtR);
            RGB[0][i][j]=f[3];
            RGB[0][i][j+1]=f[2];
            RGB[0][i][j+2]=f[1];
            RGB[0][i][j+3]=f[0];
            
            _mm_storer_ps(f,rtG);
            RGB[1][i][j]=f[3];
            RGB[1][i][j+1]=f[2];
            RGB[1][i][j+2]=f[1];
            RGB[1][i][j+3]=f[0];
            
            _mm_storer_ps(f,rtB);
            RGB[2][i][j]=f[3];
            RGB[2][i][j+1]=f[2];
            RGB[2][i][j+2]=f[1];
            RGB[2][i][j+3]=f[0];
//            if(R<0)
//                RGB[0][i][j] = 0;
//            else if(R>255)
//                RGB[0][i][j] = 255;
//            else
//                RGB[0][i][j]=R;
//
//            if(G<0)
//                RGB[1][i][j] = 0;
//            else if(G>255)
//                RGB[1][i][j] = 255;
//            else
//                RGB[1][i][j]=G;
//
//            if(B<0)
//                RGB[2][i][j] = 0;
//            else if(B>255)
//                RGB[2][i][j] = 255;
//            else
//                RGB[2][i][j]=B;

        }
    }
}
void RGB2YUV(){
    __m128 coe_00 = _mm_set_ps(0.256788,0.256788,0.256788,0.256788);
    __m128 coe_01 = _mm_set_ps(0.504129,0.504129,0.504129,0.504129);
    __m128 coe_02 = _mm_set_ps(0.097906,0.097906,0.097906,0.097906);
    __m128 coe_10 = _mm_set_ps(-0.148223,-0.148223,-0.148223,-0.148223);
    __m128 coe_11 = _mm_set_ps(-0.290993,-0.290993,-0.290993,-0.290993);
    __m128 coe_12 = _mm_set_ps(0.439216,0.439216,0.439216,0.439216);
    __m128 coe_20 = _mm_set_ps(0.439216,0.439216,0.439216,0.439216);
    __m128 coe_21 = _mm_set_ps(-0.367788,-0.367788,-0.367788,-0.367788);
    __m128 coe_22 = _mm_set_ps(-0.071427,-0.071427,-0.071427,-0.071427);
    __m128 YUV_0 = _mm_set_ps(16,16,16,16);
    __m128 YUV_1 = _mm_set_ps(128,128,128,128);
    
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=4){
            // Y = 0.256788*R + 0.504129*G + 0.097906*B + 16
            // U = -0.148223*R - 0.290993*G + 0.439216*B + 128
            // V = 0.439216*R - 0.367788*G - 0.071427*B + 128
//            resultYUV[0][i][j]= 0.256788*tmpRGB[0][i][j] + 0.504129*tmpRGB[1][i][j] + 0.097906*tmpRGB[2][i][j] + 16;
//            resultYUV[1][i][j]= -0.148223*tmpRGB[0][i][j] - 0.290993*tmpRGB[1][i][j] + 0.439216*tmpRGB[2][i][j] + 128;
//            resultYUV[2][i][j]= 0.439216*tmpRGB[0][i][j] - 0.367788*tmpRGB[1][i][j] - 0.071427*tmpRGB[2][i][j] + 128;
            
            __m128 RBG_0 = _mm_set_ps((float)tmpRGB[0][i][j+3],(float)tmpRGB[0][i][j+2],(float)tmpRGB[0][i][j+1],(float)tmpRGB[0][i][j]);
            __m128 RBG_1 = _mm_set_ps((float)tmpRGB[1][i][j+3],(float)tmpRGB[1][i][j+2],(float)tmpRGB[1][i][j+1],(float)tmpRGB[1][i][j]);
            __m128 RBG_2 = _mm_set_ps((float)tmpRGB[2][i][j+3],(float)tmpRGB[2][i][j+2],(float)tmpRGB[2][i][j+1],(float)tmpRGB[2][i][j]);
            __m128 RGB_00 = _mm_mul_ps(RBG_0,coe_00);
            __m128 RGB_10 = _mm_mul_ps(RBG_0,coe_10);
            __m128 RGB_20 = _mm_mul_ps(RBG_0,coe_20);
            __m128 RGB_01 = _mm_mul_ps(RBG_1,coe_01);
            __m128 RGB_11 = _mm_mul_ps(RBG_1,coe_11);
            __m128 RGB_21 = _mm_mul_ps(RBG_1,coe_21);
            __m128 RGB_02 = _mm_mul_ps(RBG_2,coe_02);
            __m128 RGB_12 = _mm_mul_ps(RBG_2,coe_12);
            __m128 RGB_22 = _mm_mul_ps(RBG_2,coe_22);
            
            __m128 rtYUV_0 = _mm_add_ps(_mm_add_ps(RGB_00,RGB_01),_mm_add_ps(RGB_02,YUV_0));
            __m128 rtYUV_1 = _mm_add_ps(_mm_add_ps(RGB_10,RGB_11),_mm_add_ps(RGB_12,YUV_1));
            __m128 rtYUV_2 = _mm_add_ps(_mm_add_ps(RGB_20,RGB_21),_mm_add_ps(RGB_22,YUV_1));
            
            
            float f[4]={0};
            _mm_storer_ps(f,rtYUV_0);
            resultYUV[0][i][j]=f[3];
            resultYUV[0][i][j+1]=f[2];
            resultYUV[0][i][j+2]=f[1];
            resultYUV[0][i][j+3]=f[0];
            _mm_storer_ps(f,rtYUV_1);
            resultYUV[1][i][j]=f[3];
            resultYUV[1][i][j+1]=f[2];
            resultYUV[1][i][j+2]=f[1];
            resultYUV[1][i][j+3]=f[0];
            _mm_storer_ps(f,rtYUV_2);
            resultYUV[2][i][j]=f[3];
            resultYUV[2][i][j+1]=f[2];
            resultYUV[2][i][j+2]=f[1];
            resultYUV[2][i][j+3]=f[0];
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


