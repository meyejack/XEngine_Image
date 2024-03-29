﻿#include "pch.h"
#include "ImageRecognition_Help.h"
/********************************************************************
//    Created:     2022/04/26  10:59:50
//    File Name:   D:\IEngine_Image\IEngine_Source\IEngine_ImageRecognition\ImageRecognition_Help\ImageRecognition_Help.cpp
//    File Path:   D:\IEngine_Image\IEngine_Source\IEngine_ImageRecognition\ImageRecognition_Help
//    File Base:   ImageRecognition_Help
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     图像识别帮助库
//    History:
*********************************************************************/
CImageRecognition_Help::CImageRecognition_Help()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
}
CImageRecognition_Help::~CImageRecognition_Help()
{
}
//////////////////////////////////////////////////////////////////////////
//                        公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：ImageRecognition_Help_QREncodec
函数功能：二维码生成器
 参数.一：lpszFileName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要生成的图片位置
 参数.二：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要编码的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CImageRecognition_Help::ImageRecognition_Help_QREncodec(LPCTSTR lpszFileName, LPCTSTR lpszMsgBuffer)
{
	Image_IsErrorOccur = FALSE;

	if ((NULL == lpszFileName) || (NULL == lpszMsgBuffer))
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_PARAMENT;
		return FALSE;
	}
	//使用qrencode进行字符串编码
#ifdef _UNICODE
	USES_CONVERSION;
	QRcode* pSt_QRCodec = QRcode_encodeString(W2A(lpszMsgBuffer), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
#else
	QRcode* pSt_QRCodec = QRcode_encodeString(lpszMsgBuffer, 0, QR_ECLEVEL_H, QR_MODE_8, 1);
#endif
	if (NULL == pSt_QRCodec)
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_FAILED;
		return FALSE;
	}
	cv::Mat m_SrcFrame;
	//转换数据
	m_SrcFrame = cv::Mat(pSt_QRCodec->width, pSt_QRCodec->width, CV_8U);
	for (int i = 0; i < pSt_QRCodec->width; ++i)
	{
		for (int j = 0; j < pSt_QRCodec->width; ++j)
		{
			m_SrcFrame.at<uchar>(i, j) = (pSt_QRCodec->data[i * pSt_QRCodec->width + j] & 0x01) == 0x01 ? 0 : 255;
		}
	}
	//调整大小
	cv::resize(m_SrcFrame, m_SrcFrame, cv::Size(m_SrcFrame.rows * 10, m_SrcFrame.cols * 10), 0, 0, cv::INTER_NEAREST);
	//转换成彩色
	cv::cvtColor(m_SrcFrame, m_SrcFrame, cv::COLOR_GRAY2BGR);
#ifdef _UNICODE
	//USES_CONVERSION;
	if (!cv::imwrite(W2A(lpszFileName), m_SrcFrame))
#else
	if (!cv::imwrite(lpszFileName, m_SrcFrame))
#endif
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_WRITE;
		return FALSE;
	}
	return TRUE;
}
/********************************************************************
函数名称：ImageRecognition_Help_QRDecodecFile
函数功能：从文件解码二维码
 参数.一：lpszFileName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的图片地址
 参数.二：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出识别到的内容
 参数.三：lpszDetectProto
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
 参数.四：lpszDetectModel
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
 参数.五：lpszSrProto
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
 参数.六：lpszSrModel
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CImageRecognition_Help::ImageRecognition_Help_QRDecodecFile(LPCTSTR lpszFileName, TCHAR* ptszMsgBuffer, LPCTSTR lpszDetectProto, LPCTSTR lpszDetectModel, LPCTSTR lpszSrProto, LPCTSTR lpszSrModel)
{
	Image_IsErrorOccur = FALSE;

	if ((NULL == lpszFileName) || (NULL == ptszMsgBuffer))
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_PARAMENT;
		return FALSE;
	}
	cv::Mat m_Frame;
	vector<cv::Mat> m_MatPoint;
	cv::Ptr<cv::wechat_qrcode::WeChatQRCode> m_QRDetector;
	
#ifdef _UNICODE
	USES_CONVERSION;
	m_Frame = cv::imread(W2A(lpszFileName));
#else
	m_Frame = cv::imread(lpszFileName);
#endif // _UNICODE
	//是否成功
	if (m_Frame.empty())
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_EMPTY;
		return FALSE;
	}
#ifdef _UNICODE
	m_QRDetector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(W2A(lpszDetectProto), W2A(lpszDetectModel), W2A(lpszSrProto), W2A(lpszSrModel));
#else
	m_QRDetector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(lpszDetectProto, lpszDetectModel, lpszSrProto, lpszSrModel);
#endif
	vector<string> stl_VectorQRList = m_QRDetector->detectAndDecode(m_Frame, m_MatPoint);

	if (stl_VectorQRList.empty())
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_NOTQR;
		return FALSE;
	}
#ifdef _UNICODE
	wcscpy(ptszMsgBuffer, A2W(stl_VectorQRList[0].c_str()));
#else
	strcpy(ptszMsgBuffer, stl_VectorQRList[0].c_str());
#endif
	return TRUE;
}
/********************************************************************
函数名称：ImageRecognition_Help_QRDecodecMemory
函数功能：从内存解析二维码
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的图片缓冲区
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：输入缓冲区大小
 参数.三：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出识别到的内容
 参数.四：lpszDetectProto
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
 参数.五：lpszDetectModel
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
 参数.六：lpszSrProto
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
 参数.七：lpszSrModel
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：学习模型地址,参考示例
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CImageRecognition_Help::ImageRecognition_Help_QRDecodecMemory(LPCSTR lpszMsgBuffer, int nMsgLen, TCHAR* ptszMsgBuffer, LPCTSTR lpszDetectProto, LPCTSTR lpszDetectModel, LPCTSTR lpszSrProto, LPCTSTR lpszSrModel)
{
	Image_IsErrorOccur = FALSE;

	if ((NULL == lpszMsgBuffer) || (NULL == ptszMsgBuffer))
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_PARAMENT;
		return FALSE;
	}
	cv::Mat m_Frame;
	vector<cv::Mat> m_MatPoint;
	cv::Ptr<cv::wechat_qrcode::WeChatQRCode> m_QRDetector;

	cv::_InputArray m_InputArray(lpszMsgBuffer, nMsgLen);
	m_Frame = cv::imdecode(m_InputArray, cv::IMREAD_UNCHANGED);
	//是否成功
	if (m_Frame.empty())
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_EMPTY;
		return FALSE;
	}
#ifdef _UNICODE
	USES_CONVERSION;
	m_QRDetector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(W2A(lpszDetectProto), W2A(lpszDetectModel), W2A(lpszSrProto), W2A(lpszSrModel));
#else
	m_QRDetector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(lpszDetectProto, lpszDetectModel, lpszSrProto, lpszSrModel);
#endif
	vector<string> stl_VectorQRList = m_QRDetector->detectAndDecode(m_Frame, m_MatPoint);

	if (stl_VectorQRList.empty())
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_NOTQR;
		return FALSE;
	}
#ifdef _UNICODE
	wcscpy(ptszMsgBuffer, A2W(stl_VectorQRList[0].c_str()));
#else
	strcpy(ptszMsgBuffer, stl_VectorQRList[0].c_str());
#endif
	return TRUE;
}
/********************************************************************
函数名称：ImageRecognition_Help_QRCamera
函数功能：识别摄像头的二维码
 参数.一：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出识别到的内容
 参数.二：nDeviceID
  In/Out：In
  类型：整数型
  可空：Y
  意思：输入摄像头的索引,一个就填写0
 参数.三：nTimeout
  In/Out：In
  类型：整数型
  可空：Y
  意思：超时时间,单位秒
返回值
  类型：逻辑型
  意思：是否成功
备注：此函数会死锁直到识别或者超时
*********************************************************************/
BOOL CImageRecognition_Help::ImageRecognition_Help_QRCamera(TCHAR* ptszMsgBuffer, LPCTSTR lpszDetectProto, LPCTSTR lpszDetectModel, LPCTSTR lpszSrProto, LPCTSTR lpszSrModel, int nDeviceID /* = 0 */, int nTimeout /* = 3 */)
{
	Image_IsErrorOccur = FALSE;

	if (NULL == ptszMsgBuffer)
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_PARAMENT;
		return FALSE;
	}
	cv::VideoCapture m_VideoCapture;
	m_VideoCapture.open(nDeviceID);
	//是否打开
	if (!m_VideoCapture.isOpened())
	{
		Image_IsErrorOccur = TRUE;
		Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_OPEN;
		return FALSE;
	}
	cv::Mat m_SrcFrame;
	time_t nTimeStart = time(NULL);
	int64_t nTimeCheck = clock();
	while (m_VideoCapture.read(m_SrcFrame))
	{
		if (m_SrcFrame.empty())
		{
			Image_IsErrorOccur = TRUE;
			Image_dwErrorCode = ERROR_XENGINE_IMAGE_RECOGNITION_HELP_EMPTY;
			return FALSE;
		}
		//每500毫秒读取一次
		if ((nTimeCheck - clock()) > 500)
		{
			std::vector<uchar> stl_VectorBuffer;
			std::vector<int> stl_VectorParam(2);
			stl_VectorParam[0] = cv::IMWRITE_JPEG_QUALITY;
			stl_VectorParam[1] = 80;        

			cv::imencode(".jpg", m_SrcFrame, stl_VectorBuffer, stl_VectorParam);
			if (ImageRecognition_Help_QRDecodecMemory((LPCSTR)stl_VectorBuffer.data(), stl_VectorBuffer.size(), ptszMsgBuffer, lpszDetectProto, lpszDetectModel, lpszSrProto, lpszSrModel))
			{
				break;
			}
			nTimeCheck = clock();
		}
		//是否超时
		time_t nTimeEnd = time(NULL);
		if ((nTimeEnd - nTimeStart) > nTimeout)
		{
			break;
		}
	}
	m_VideoCapture.release();
	return TRUE;
}
