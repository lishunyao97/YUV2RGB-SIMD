/*
 ±¾Ä£°å½ö¹©²Î¿¼
 */
#include <iostream>
#include <fstream>
#include <ctime>
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
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            tmpRGB[0][i][j]=(double)(t*RGB1[0][i][j]+(256-t)*RGB2[0][i][j])/256;
            tmpRGB[1][i][j]=(double)(t*RGB1[1][i][j]+(256-t)*RGB2[1][i][j])/256;
            tmpRGB[2][i][j]=(double)(t*RGB1[2][i][j]+(256-t)*RGB2[2][i][j])/256;
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
            R=1.164383*(YUV1[0][i][j]-16)+1.596027*(YUV1[2][i][j]-128);
            G = 1.164383 * (YUV1[0][i][j] - 16) - 0.391762*(YUV1[1][i][j] - 128) - 0.812968*(YUV1[2][i][j] - 128);
            B=1.164383*(YUV1[0][i][j]-16)+2.017232*(YUV1[1][i][j]-128);
            if(R<0)
                RGB1[0][i][j] = 0;
            else if(R>255)
                RGB1[0][i][j] = 255;
            else
                RGB1[0][i][j]=R;
            
            if(G<0)
                RGB1[1][i][j] = 0;
            else if(G>255)
                RGB1[1][i][j] = 255;
            else
                RGB1[1][i][j]=G;
            
            if(B<0)
                RGB1[2][i][j] = 0;
            else if(B>255)
                RGB1[2][i][j] = 255;
            else
                RGB1[2][i][j]=B;
            
            R=1.164383*(YUV2[0][i][j]-16)+1.596027*(YUV2[2][i][j]-128);
            G = 1.164383 * (YUV2[0][i][j] - 16) - 0.391762*(YUV2[1][i][j] - 128) - 0.812968*(YUV2[2][i][j] - 128);
            B=1.164383*(YUV2[0][i][j]-16)+2.017232*(YUV2[1][i][j]-128);
            if(R<0)
                RGB2[0][i][j] = 0;
            else if(R>255)
                RGB2[0][i][j] = 255;
            else
                RGB2[0][i][j]=R;
            
            if(G<0)
                RGB2[1][i][j] = 0;
            else if(G>255)
                RGB2[1][i][j] = 255;
            else
                RGB2[1][i][j]=G;
            
            if(B<0)
                RGB2[2][i][j] = 0;
            else if(B>255)
                RGB2[2][i][j] = 255;
            else
                RGB2[2][i][j]=B;

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


