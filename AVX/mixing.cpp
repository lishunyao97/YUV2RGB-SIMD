/*
 ±¾Ä£°å½ö¹©²Î¿¼
 */
#include <iostream>
#include <fstream>
#include <ctime>
#include "immintrin.h"
#include "emmintrin.h"
#include "xmmintrin.h"
using namespace std;

#define width 1920
#define height 1080

// 0:Y, 1:U, 2:V
unsigned char YUV1[3][height][width] = {0};
unsigned char YUV2[3][height][width] = {0};
unsigned char resultYUV[3][height][width]={0};
unsigned char tmpY1[height][width]={0};
unsigned char tmpU1[height/2][width/2]={0};
unsigned char tmpV1[height/2][width/2]={0};
unsigned char tmpY2[height][width]={0};
unsigned char tmpU2[height/2][width/2]={0};
unsigned char tmpV2[height/2][width/2]={0};
// 0:R, 1:G, 2:B
unsigned char RGB1[3][height][width] = {0};
unsigned char RGB2[3][height][width] = {0};
unsigned char tmpRGB[3][height][width] = {0};
void YUV2RGB();
void RGB2YUV();
void RGBMixing(int t);
int main(){
    char dem1[50] = "../demo/dem1.yuv";
    char dem2[50] = "../demo/dem2.yuv";
    char mixing_name[50]="./alpha_mixing.yuv";
    FILE *fp1=fopen(dem1,"r");
    FILE *fp2=fopen(dem2,"r");
    if(fp1 == NULL || fp2==NULL){
        cout<<"cannot open file!"<<endl;
        exit(1);
    }
    fread(tmpY1,sizeof(tmpY1),1,fp1);
    fread(tmpU1,sizeof(tmpU1),1,fp1);
    fread(tmpV1,sizeof(tmpV1),1,fp1);
    
    fread(tmpY2,sizeof(tmpY2),1,fp2);
    fread(tmpU2,sizeof(tmpU2),1,fp2);
    fread(tmpV2,sizeof(tmpV2),1,fp2);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            YUV1[0][i][j]=tmpY1[i][j];
            YUV2[0][i][j]=tmpY2[i][j];
        }
    }
    for(int i=0;i<height;i+=2){
        for(int j=0;j<width;j+=2){
            YUV1[1][i][j]=tmpU1[i/2][j/2];
            YUV1[1][i][j+1]=YUV1[1][i][j];
            YUV1[1][i+1][j]=YUV1[1][i][j];
            YUV1[1][i+1][j+1]=YUV1[1][i][j];
            
            YUV2[1][i][j]=tmpU2[i/2][j/2];
            YUV2[1][i][j+1]=YUV2[1][i][j];
            YUV2[1][i+1][j]=YUV2[1][i][j];
            YUV2[1][i+1][j+1]=YUV2[1][i][j];
        }
    }
    for(int i=0;i<height;i+=2){
        for(int j=0;j<width;j+=2){
            YUV1[2][i][j]=tmpV1[i/2][j/2];
            YUV1[2][i][j+1]=YUV1[2][i][j];
            YUV1[2][i+1][j]=YUV1[2][i][j];
            YUV1[2][i+1][j+1]=YUV1[2][i][j];
            
            YUV2[2][i][j]=tmpV2[i/2][j/2];
            YUV2[2][i][j+1]=YUV2[2][i][j];
            YUV2[2][i+1][j]=YUV2[2][i][j];
            YUV2[2][i+1][j+1]=YUV2[2][i][j];
        }
    }
    fclose(fp1);
    fclose(fp2);
    ofstream out;
    out.open(mixing_name,ofstream::out);
    if(!out.is_open()){
        cout<<"cannot open try!"<<endl;
        return 0;
    }
    clock_t start;
    clock_t end;
    int time=0;
    start=clock();
    YUV2RGB();
    end=clock();
    time+=end-start;
    for(int t=1;t<255;t+=3){
        start=clock();
        RGBMixing(t);
        RGB2YUV();
        end=clock();
        time += end-start;
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
void RGBMixing(int t){
    __m256 alpha = _mm256_set_ps((float)t/256, (float)t/256, (float)t/256, (float)t/256,(float)t/256,(float)t/256,(float)t/256,(float)t/256);
    __m256 alpha_ = _mm256_set_ps((float)(256-t)/256, (float)(256-t)/256, (float)(256-t)/256, (float)(256-t)/256,(float)(256-t)/256,(float)(256-t)/256,(float)(256-t)/256,(float)(256-t)/256);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=8){
            //            tmpRGB[0][i][j]=(double)(t*RGB1[0][i][j]+(256-t)*RGB2[0][i][j])/256;
            //            tmpRGB[1][i][j]=(double)(t*RGB1[1][i][j]+(256-t)*RGB2[1][i][j])/256;
            //            tmpRGB[2][i][j]=(double)(t*RGB1[2][i][j]+(256-t)*RGB2[2][i][j])/256;
            __m256 RGB1_0 = _mm256_set_ps((float)RGB1[0][i][j+7], (float)RGB1[0][i][j+6], (float)RGB1[0][i][j+5], (float)RGB1[0][i][j+4], (float)RGB1[0][i][j+3], (float)RGB1[0][i][j+2], (float)RGB1[0][i][j+1], (float)RGB1[0][i][j]);
            __m256 RGB1_1 = _mm256_set_ps((float)RGB1[1][i][j+7],(float)RGB1[1][i][j+6],(float)RGB1[1][i][j+5],(float)RGB1[1][i][j+4],(float)RGB1[1][i][j+3], (float)RGB1[1][i][j+2], (float)RGB1[1][i][j+1], (float)RGB1[1][i][j]);
            __m256 RGB1_2 = _mm256_set_ps((float)RGB1[2][i][j+7],(float)RGB1[2][i][j+6],(float)RGB1[2][i][j+5],(float)RGB1[2][i][j+4],(float)RGB1[2][i][j+3], (float)RGB1[2][i][j+2], (float)RGB1[2][i][j+1], (float)RGB1[2][i][j]);
            __m256 RGB2_0 = _mm256_set_ps((float)RGB2[0][i][j+7], (float)RGB2[0][i][j+6], (float)RGB2[0][i][j+5], (float)RGB2[0][i][j+4], (float)RGB2[0][i][j+3], (float)RGB2[0][i][j+2], (float)RGB2[0][i][j+1], (float)RGB2[0][i][j]);
            __m256 RGB2_1 = _mm256_set_ps((float)RGB2[1][i][j+7],(float)RGB2[1][i][j+6],(float)RGB2[1][i][j+5],(float)RGB2[1][i][j+4],(float)RGB2[1][i][j+3], (float)RGB2[1][i][j+2], (float)RGB2[1][i][j+1], (float)RGB2[1][i][j]);
            __m256 RGB2_2 = _mm256_set_ps((float)RGB2[2][i][j+7],(float)RGB2[2][i][j+6],(float)RGB2[2][i][j+5],(float)RGB2[2][i][j+4],(float)RGB2[2][i][j+3], (float)RGB2[2][i][j+2], (float)RGB2[2][i][j+1], (float)RGB2[2][i][j]);
            
            __m256 tmpRGB1_0 = _mm256_mul_ps(RGB1_0, alpha);
            __m256 tmpRGB1_1 = _mm256_mul_ps(RGB1_1, alpha);
            __m256 tmpRGB1_2 = _mm256_mul_ps(RGB1_2, alpha);
            __m256 tmpRGB2_0 = _mm256_mul_ps(RGB2_0, alpha_);
            __m256 tmpRGB2_1 = _mm256_mul_ps(RGB2_1, alpha_);
            __m256 tmpRGB2_2 = _mm256_mul_ps(RGB2_2, alpha_);
            
            __m256 tmpRGB_0 = _mm256_add_ps(tmpRGB1_0, tmpRGB2_0);
            __m256 tmpRGB_1 = _mm256_add_ps(tmpRGB1_1, tmpRGB2_1);
            __m256 tmpRGB_2 = _mm256_add_ps(tmpRGB1_2, tmpRGB2_2);
            
            float f[8]={0};
            _mm256_store_ps(f,tmpRGB_0);
            tmpRGB[0][i][j]=f[0];
            tmpRGB[0][i][j+1]=f[1];
            tmpRGB[0][i][j+2]=f[2];
            tmpRGB[0][i][j+3]=f[3];
            tmpRGB[0][i][j+4]=f[4];
            tmpRGB[0][i][j+5]=f[5];
            tmpRGB[0][i][j+6]=f[6];
            tmpRGB[0][i][j+7]=f[7];
            _mm256_store_ps(f,tmpRGB_1);
            tmpRGB[1][i][j]=f[0];
            tmpRGB[1][i][j+1]=f[1];
            tmpRGB[1][i][j+2]=f[2];
            tmpRGB[1][i][j+3]=f[3];
            tmpRGB[1][i][j+4]=f[4];
            tmpRGB[1][i][j+5]=f[5];
            tmpRGB[1][i][j+6]=f[6];
            tmpRGB[1][i][j+7]=f[7];
            _mm256_store_ps(f,tmpRGB_2);
            tmpRGB[2][i][j]=f[0];
            tmpRGB[2][i][j+1]=f[1];
            tmpRGB[2][i][j+2]=f[2];
            tmpRGB[2][i][j+3]=f[3];
            tmpRGB[2][i][j+4]=f[4];
            tmpRGB[2][i][j+5]=f[5];
            tmpRGB[2][i][j+6]=f[6];
            tmpRGB[2][i][j+7]=f[7];
        }
    }
}
void YUV2RGB(){
    __m256 sub_0 = _mm256_set_ps((float)16, (float)16, (float)16, (float)16, (float)16, (float)16, (float)16, (float)16);
    __m256 sub_1 = _mm256_set_ps((float)128, (float)128, (float)128, (float)128, (float)128, (float)128, (float)128, (float)128);
    __m256 coe_0 = _mm256_set_ps(1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383);
    __m256 coe_1_G = _mm256_set_ps(-0.391762,-0.391762,-0.391762,-0.391762,-0.391762,-0.391762,-0.391762,-0.391762);
    __m256 coe_1_B = _mm256_set_ps(2.017232,2.017232,2.017232,2.017232,2.017232,2.017232,2.017232,2.017232);
    __m256 coe_2_R = _mm256_set_ps(1.596027,1.596027,1.596027,1.596027,1.596027,1.596027,1.596027,1.596027);
    __m256 coe_2_G = _mm256_set_ps(-0.812968,-0.812968,-0.812968,-0.812968,-0.812968,-0.812968,-0.812968,-0.812968);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=8){
            // R = 1.164383 * (Y - 16) + 1.596027*(V - 128)
            // G = 1.164383 * (Y - 16) Ð 0.391762*(U - 128) Ð 0.812968*(V - 128)
            // B = 1.164383 * (Y - 16) + 2.017232*(U - 128)
            //            int R,G,B;
            //            R=1.164383*(YUV1[0][i][j]-16)+1.596027*(YUV1[2][i][j]-128);
            //            G = 1.164383 * (YUV1[0][i][j] - 16) - 0.391762*(YUV1[1][i][j] - 128) - 0.812968*(YUV1[2][i][j] - 128);
            //            B=1.164383*(YUV1[0][i][j]-16)+2.017232*(YUV1[1][i][j]-128);
            __m256 YUV_0 = _mm256_set_ps((float)YUV1[0][i][j+7],(float)YUV1[0][i][j+6],(float)YUV1[0][i][j+5],(float)YUV1[0][i][j+4],(float)YUV1[0][i][j+3],(float)YUV1[0][i][j+2],(float)YUV1[0][i][j+1],(float)YUV1[0][i][j]);
            __m256 YUV_1 = _mm256_set_ps((float)YUV1[1][i][j+7],(float)YUV1[1][i][j+6],(float)YUV1[1][i][j+5],(float)YUV1[1][i][j+4],(float)YUV1[1][i][j+3],(float)YUV1[1][i][j+2],(float)YUV1[1][i][j+1],(float)YUV1[1][i][j]);
            __m256 YUV_2 = _mm256_set_ps((float)YUV1[2][i][j+7],(float)YUV1[2][i][j+6],(float)YUV1[2][i][j+5],(float)YUV1[2][i][j+4],(float)YUV1[2][i][j+3],(float)YUV1[2][i][j+2],(float)YUV1[2][i][j+1],(float)YUV1[2][i][j]);
            YUV_0 = _mm256_sub_ps(YUV_0,sub_0);
            YUV_1 = _mm256_sub_ps(YUV_1,sub_1);
            YUV_2 = _mm256_sub_ps(YUV_2,sub_1);
            __m256 RGB_Y = _mm256_mul_ps(YUV_0,coe_0);
            __m256 G_U=_mm256_mul_ps(YUV_1,coe_1_G);
            __m256 B_U=_mm256_mul_ps(YUV_1,coe_1_B);
            __m256 R_V=_mm256_mul_ps(YUV_2,coe_2_R);
            __m256 G_V=_mm256_mul_ps(YUV_2,coe_2_G);
            __m256 rtR = _mm256_add_ps(RGB_Y,R_V);
            __m256 rtG = _mm256_add_ps(RGB_Y,G_U);
            rtG = _mm256_add_ps(rtG,G_V);
            __m256 rtB = _mm256_add_ps(RGB_Y,B_U);
            
            float f[8]={0};
            _mm256_store_ps(f,rtR);
            RGB1[0][i][j]=f[0];
            RGB1[0][i][j+1]=f[1];
            RGB1[0][i][j+2]=f[2];
            RGB1[0][i][j+3]=f[3];
            RGB1[0][i][j+4]=f[4];
            RGB1[0][i][j+5]=f[5];
            RGB1[0][i][j+6]=f[6];
            RGB1[0][i][j+7]=f[7];
            _mm256_store_ps(f,rtG);
            RGB1[1][i][j]=f[0];
            RGB1[1][i][j+1]=f[1];
            RGB1[1][i][j+2]=f[2];
            RGB1[1][i][j+3]=f[3];
            RGB1[1][i][j+4]=f[4];
            RGB1[1][i][j+5]=f[5];
            RGB1[1][i][j+6]=f[6];
            RGB1[1][i][j+7]=f[7];
            _mm256_store_ps(f,rtB);
            RGB1[2][i][j]=f[0];
            RGB1[2][i][j+1]=f[1];
            RGB1[2][i][j+2]=f[2];
            RGB1[2][i][j+3]=f[3];
            RGB1[2][i][j+4]=f[4];
            RGB1[2][i][j+5]=f[5];
            RGB1[2][i][j+6]=f[6];
            RGB1[2][i][j+7]=f[7];
            //            if(RGB1[0][i][j]<0)
            //                RGB1[0][i][j] = 0;
            //            else if(R>255)
            //                RGB1[0][i][j] = 255;
            //            else
            //                RGB1[0][i][j]=R;
            //
            //            if(G<0)
            //                RGB1[1][i][j] = 0;
            //            else if(G>255)
            //                RGB1[1][i][j] = 255;
            //            else
            //                RGB1[1][i][j]=G;
            //
            //            if(B<0)
            //                RGB1[2][i][j] = 0;
            //            else if(B>255)
            //                RGB1[2][i][j] = 255;
            //            else
            //                RGB1[2][i][j]=B;
            
            //            R=1.164383*(YUV2[0][i][j]-16)+1.596027*(YUV2[2][i][j]-128);
            //            G = 1.164383 * (YUV2[0][i][j] - 16) - 0.391762*(YUV2[1][i][j] - 128) - 0.812968*(YUV2[2][i][j] - 128);
            //            B=1.164383*(YUV2[0][i][j]-16)+2.017232*(YUV2[1][i][j]-128);
            
            YUV_0 = _mm256_set_ps((float)YUV2[0][i][j+7],(float)YUV2[0][i][j+6],(float)YUV2[0][i][j+5],(float)YUV2[0][i][j+4],(float)YUV2[0][i][j+3],(float)YUV2[0][i][j+2],(float)YUV2[0][i][j+1],(float)YUV2[0][i][j]);
            YUV_1 = _mm256_set_ps((float)YUV2[1][i][j+7],(float)YUV2[1][i][j+6],(float)YUV2[1][i][j+5],(float)YUV2[1][i][j+4],(float)YUV2[1][i][j+3],(float)YUV2[1][i][j+2],(float)YUV2[1][i][j+1],(float)YUV2[1][i][j]);
            YUV_2 = _mm256_set_ps((float)YUV2[2][i][j+7],(float)YUV2[2][i][j+6],(float)YUV2[2][i][j+5],(float)YUV2[2][i][j+4],(float)YUV2[2][i][j+3],(float)YUV2[2][i][j+2],(float)YUV2[2][i][j+1],(float)YUV2[2][i][j]);
            YUV_0 = _mm256_sub_ps(YUV_0,sub_0);
            YUV_1 = _mm256_sub_ps(YUV_1,sub_1);
            YUV_2 = _mm256_sub_ps(YUV_2,sub_1);
            RGB_Y = _mm256_mul_ps(YUV_0,coe_0);
            G_U=_mm256_mul_ps(YUV_1,coe_1_G);
            B_U=_mm256_mul_ps(YUV_1,coe_1_B);
            R_V=_mm256_mul_ps(YUV_2,coe_2_R);
            G_V=_mm256_mul_ps(YUV_2,coe_2_G);
            rtR = _mm256_add_ps(RGB_Y,R_V);
            rtG = _mm256_add_ps(RGB_Y,G_U);
            rtG = _mm256_add_ps(rtG,G_V);
            rtB = _mm256_add_ps(RGB_Y,B_U);
            
            _mm256_store_ps(f,rtR);
            RGB2[0][i][j]=f[0];
            RGB2[0][i][j+1]=f[1];
            RGB2[0][i][j+2]=f[2];
            RGB2[0][i][j+3]=f[3];
            RGB2[0][i][j+4]=f[4];
            RGB2[0][i][j+5]=f[5];
            RGB2[0][i][j+6]=f[6];
            RGB2[0][i][j+7]=f[7];
            _mm256_store_ps(f,rtG);
            RGB2[1][i][j]=f[0];
            RGB2[1][i][j+1]=f[1];
            RGB2[1][i][j+2]=f[2];
            RGB2[1][i][j+3]=f[3];
            RGB2[1][i][j+4]=f[4];
            RGB2[1][i][j+5]=f[5];
            RGB2[1][i][j+6]=f[6];
            RGB2[1][i][j+7]=f[7];
            _mm256_store_ps(f,rtB);
            RGB2[2][i][j]=f[0];
            RGB2[2][i][j+1]=f[1];
            RGB2[2][i][j+2]=f[2];
            RGB2[2][i][j+3]=f[3];
            RGB2[2][i][j+4]=f[4];
            RGB2[2][i][j+5]=f[5];
            RGB2[2][i][j+6]=f[6];
            RGB2[2][i][j+7]=f[7];
            //            if(R<0)
            //                RGB2[0][i][j] = 0;
            //            else if(R>255)
            //                RGB2[0][i][j] = 255;
            //            else
            //                RGB2[0][i][j]=R;
            //
            //            if(G<0)
            //                RGB2[1][i][j] = 0;
            //            else if(G>255)
            //                RGB2[1][i][j] = 255;
            //            else
            //                RGB2[1][i][j]=G;
            //
            //            if(B<0)
            //                RGB2[2][i][j] = 0;
            //            else if(B>255)
            //                RGB2[2][i][j] = 255;
            //            else
            //                RGB2[2][i][j]=B;
            
        }
    }
}

void RGB2YUV(){
    __m256 coe_00 = _mm256_set_ps(0.256788,0.256788,0.256788,0.256788,0.256788,0.256788,0.256788,0.256788);
    __m256 coe_01 = _mm256_set_ps(0.504129,0.504129,0.504129,0.504129,0.504129,0.504129,0.504129,0.504129);
    __m256 coe_02 = _mm256_set_ps(0.097906,0.097906,0.097906,0.097906,0.097906,0.097906,0.097906,0.097906);
    __m256 coe_10 = _mm256_set_ps(-0.148223,-0.148223,-0.148223,-0.148223,-0.148223,-0.148223,-0.148223,-0.148223);
    __m256 coe_11 = _mm256_set_ps(-0.290993,-0.290993,-0.290993,-0.290993,-0.290993,-0.290993,-0.290993,-0.290993);
    __m256 coe_12 = _mm256_set_ps(0.439216,0.439216,0.439216,0.439216,0.439216,0.439216,0.439216,0.439216);
    __m256 coe_20 = _mm256_set_ps(0.439216,0.439216,0.439216,0.439216,0.439216,0.439216,0.439216,0.439216);
    __m256 coe_21 = _mm256_set_ps(-0.367788,-0.367788,-0.367788,-0.367788,-0.367788,-0.367788,-0.367788,-0.367788);
    __m256 coe_22 = _mm256_set_ps(-0.071427,-0.071427,-0.071427,-0.071427,-0.071427,-0.071427,-0.071427,-0.071427);
    __m256 YUV_0 = _mm256_set_ps(16,16,16,16,16,16,16,16);
    __m256 YUV_1 = _mm256_set_ps(128,128,128,128,128,128,128,128);
    
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j+=8){
            // Y = 0.256788*R + 0.504129*G + 0.097906*B + 16
            // U = -0.148223*R - 0.290993*G + 0.439216*B + 128
            // V = 0.439216*R - 0.367788*G - 0.071427*B + 128
            //            resultYUV[0][i][j]= 0.256788*tmpRGB[0][i][j] + 0.504129*tmpRGB[1][i][j] + 0.097906*tmpRGB[2][i][j] + 16;
            //            resultYUV[1][i][j]= -0.148223*tmpRGB[0][i][j] - 0.290993*tmpRGB[1][i][j] + 0.439216*tmpRGB[2][i][j] + 128;
            //            resultYUV[2][i][j]= 0.439216*tmpRGB[0][i][j] - 0.367788*tmpRGB[1][i][j] - 0.071427*tmpRGB[2][i][j] + 128;
            
            __m256 RBG_0 = _mm256_set_ps((float)tmpRGB[0][i][j+7],(float)tmpRGB[0][i][j+6],(float)tmpRGB[0][i][j+5],(float)tmpRGB[0][i][j+4],(float)tmpRGB[0][i][j+3],(float)tmpRGB[0][i][j+2],(float)tmpRGB[0][i][j+1],(float)tmpRGB[0][i][j]);
            __m256 RBG_1 = _mm256_set_ps((float)tmpRGB[1][i][j+7],(float)tmpRGB[1][i][j+6],(float)tmpRGB[1][i][j+5],(float)tmpRGB[1][i][j+4],(float)tmpRGB[1][i][j+3],(float)tmpRGB[1][i][j+2],(float)tmpRGB[1][i][j+1],(float)tmpRGB[1][i][j]);
            __m256 RBG_2 = _mm256_set_ps((float)tmpRGB[2][i][j+7],(float)tmpRGB[2][i][j+6],(float)tmpRGB[2][i][j+5],(float)tmpRGB[2][i][j+4],(float)tmpRGB[2][i][j+3],(float)tmpRGB[2][i][j+2],(float)tmpRGB[2][i][j+1],(float)tmpRGB[2][i][j]);
            __m256 RGB_00 = _mm256_mul_ps(RBG_0,coe_00);
            __m256 RGB_10 = _mm256_mul_ps(RBG_0,coe_10);
            __m256 RGB_20 = _mm256_mul_ps(RBG_0,coe_20);
            __m256 RGB_01 = _mm256_mul_ps(RBG_1,coe_01);
            __m256 RGB_11 = _mm256_mul_ps(RBG_1,coe_11);
            __m256 RGB_21 = _mm256_mul_ps(RBG_1,coe_21);
            __m256 RGB_02 = _mm256_mul_ps(RBG_2,coe_02);
            __m256 RGB_12 = _mm256_mul_ps(RBG_2,coe_12);
            __m256 RGB_22 = _mm256_mul_ps(RBG_2,coe_22);
            
            __m256 rtYUV_0 = _mm256_add_ps(_mm256_add_ps(RGB_00,RGB_01),_mm256_add_ps(RGB_02,YUV_0));
            __m256 rtYUV_1 = _mm256_add_ps(_mm256_add_ps(RGB_10,RGB_11),_mm256_add_ps(RGB_12,YUV_1));
            __m256 rtYUV_2 = _mm256_add_ps(_mm256_add_ps(RGB_20,RGB_21),_mm256_add_ps(RGB_22,YUV_1));
            
            float f[8]={0};
            _mm256_store_ps(f,rtYUV_0);
            resultYUV[0][i][j]=f[0];
            resultYUV[0][i][j+1]=f[1];
            resultYUV[0][i][j+2]=f[2];
            resultYUV[0][i][j+3]=f[3];
            resultYUV[0][i][j+4]=f[4];
            resultYUV[0][i][j+5]=f[5];
            resultYUV[0][i][j+6]=f[6];
            resultYUV[0][i][j+7]=f[7];
            _mm256_store_ps(f,rtYUV_1);
            resultYUV[1][i][j]=f[0];
            resultYUV[1][i][j+1]=f[1];
            resultYUV[1][i][j+2]=f[2];
            resultYUV[1][i][j+3]=f[3];
            resultYUV[1][i][j+4]=f[4];
            resultYUV[1][i][j+5]=f[5];
            resultYUV[1][i][j+6]=f[6];
            resultYUV[1][i][j+7]=f[7];
            _mm256_store_ps(f,rtYUV_2);
            resultYUV[2][i][j]=f[0];
            resultYUV[2][i][j+1]=f[1];
            resultYUV[2][i][j+2]=f[2];
            resultYUV[2][i][j+3]=f[3];
            resultYUV[2][i][j+4]=f[4];
            resultYUV[2][i][j+5]=f[5];
            resultYUV[2][i][j+6]=f[6];
            resultYUV[2][i][j+7]=f[7];
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



