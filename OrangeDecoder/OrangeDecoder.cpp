//  
//  OrangeDecoder.cpp
//  libencoderqr (https://github.com/josephholsten/libdecodeqr)を用いた
//  WebカメラQRデコーダーの例です．
//
//  Created by Layui (15/09/04). Copyright 2015 All rights reserved.
//

#include "stdafx.h"
#include "Form1.h"
#include <windows.h>

// C:\opencv\build\include\... を参照
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

// 自分のプロジェクトを参照
#include "QRDetector.h"
#include "decodeqr.h"
#pragma comment(lib, "WS2_32.LIB")
#pragma comment(lib, "libdecodeqr.lib")


using namespace OrangeDecoder;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{

	// カメラからのビデオキャプチャを初期化
    CvCapture *videoCapture = NULL;

	// Webカメラを取得す
    for(int i = 0;videoCapture == NULL;i++){
        videoCapture = cvCreateCameraCapture(i);
        if(5 < i) return -1;
    }


    // ウインドウを作成
    char windowName[] = "camera";
    cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

	// Decoderを用意(decodeqr.h)
	QrDecoderHandle decoder = qr_decoder_open();

	IplImage *qrimg;
	int c;

    while(1)
    {
		// Escキーが押されたら終了
        c = cvWaitKey(2);
        if(c == '\x1b') break;

        // カメラから1フレームを取得
        IplImage *img = cvQueryFrame(videoCapture);

		if(img != 0){

            try {
                // QRコードのデコード処理
                qr_decoder_set_image_buffer(decoder, img);

                if(!qr_decoder_is_busy(decoder))
                {
                    short sz = 25;
                    short stat = qr_decoder_decode(decoder, sz);

                    for(sz = 25, stat = 0; (sz >= 3) && ((stat & QR_IMAGEREADER_DECODED) == 0); sz -= 2)
                        stat = qr_decoder_decode(decoder, sz);
                }

                // QRコードからテキストを取得
                int text_size = 0;
                unsigned char* text = new unsigned char[text_size];

                QrCodeHeader header;
                if(qr_decoder_get_header(decoder, &header))
                {
                    if(text_size < header.byte_size + 1)
                    {
                        if(text) delete[] text;
                        text_size = header.byte_size + 1;
                        text = new unsigned char[text_size];
                    }
                    qr_decoder_get_body(decoder, text, text_size);
                    
					// 画像に取得テキストを描画
                    CvFont dfont;
                    float hscale = 1.0f;
                    float vscale = 1.0f;
                    cvInitFont (&dfont, CV_FONT_HERSHEY_COMPLEX, hscale, vscale);
                    cvPutText(img, (char*)text, cvPoint(65, 60), &dfont, CV_RGB(0, 255, 0));
                }
                delete[] text;

            } catch(...){

				// 画像にErrorを描画
                CvFont dfont;
                float hscale = 1.0f;
                float vscale = 1.0f;
                cvInitFont (&dfont, CV_FONT_HERSHEY_COMPLEX, hscale, vscale);
                cvPutText(img, "Error", cvPoint(65, 60), &dfont, CV_RGB(255, 0, 0));
            }
            
            // ウィンドウに画像を表示
            cvShowImage(windowName, img);
		}
    }
    qr_decoder_close(decoder);

	// 解放
	cvReleaseImage(&qrimg);
    cvReleaseCapture(&videoCapture);
	
    // ウィンドウを破棄
    cvDestroyWindow(windowName);

	return 0;
}


