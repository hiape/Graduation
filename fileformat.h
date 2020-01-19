//
// Created by ape on 2020/1/13.
//

#ifndef FILE_FILEFORMAT_H
#define FILE_FILEFORMAT_H

#include <cstdint>

namespace  voip{
    //wav文件的 文件头内容，一共有44个字节
    typedef struct WavHeader
    {
        uint32_t ChunkId;     //4字节的RIFF标志
        uint32_t ChunkSize;     //下一地址到文件尾的大小
        uint32_t Format;        //"WAVE"  4个字节的标志

        //sub-chunk   "fmt"
        uint32_t SubChunk1Id;   //"fmt" 波形标志
        uint32_t SubChunk1Size; // 16 for pcm
        uint16_t AudioFormat;   //1表示线性PCM，  大于1表示有压缩
        uint16_t NumChannels;	/* Mono = 1, Stereo = 2, 声道数*/
        uint32_t SampleRate;	/* 8000, 44100, 采样率 */
        uint32_t ByteRate;	/* = SampleRate * NumChannels * BitsPerSample/8 */
        uint16_t BlockAlign;	/* = NumChannels * BitsPerSample/8  块对齐*/
        uint16_t BitsPerSample;	/* 8bits, 16bits, 样本数据位 8位量化或者是16位量化 */

        /* sub-chunk "data" */
        uint32_t SubChunk2ID;	/* "data"  数据标志 */
        uint32_t SubChunk2Size;	/* data size 数据的大小，也就是PCM裸流的大小*/

        // 获取通道数
        uint16_t  GetChannel() const    {return NumChannels;}

        //获取采样率
        uint32_t GetSampleRate() const  {return SampleRate;}

        //获取采样位数
        uint16_t GetBitsPerSample() const {return BitsPerSample;}

        //
    }WavHeader;
}
#endif