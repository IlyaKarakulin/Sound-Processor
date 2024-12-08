#include <iostream>
#include "objects_fabric.hpp"
using namespace std;

MetaData::WAVHeader *MetaData::header = nullptr;

int ReadWAV::getUnitSize()
{
    return this->sizeOfUnit;
}

bool ReadWAV::openWAVFile(string inputFileName)
{
    // Open the WAV file that check that the path is correct
    this->inputFileName = inputFileName;
    this->file.open(this->inputFileName, ios::binary);
    if (!file.is_open())
        throw runtime_error("Failed to open the file. Please check the file name or path.\n");

    return this->file.is_open();
}

bool ReadWAV::closeWAVFile()
{
    this->file.close();
    return !this->file.is_open();
}

void ReadWAV::parseHead()
{
    // Read the WAV header, feiling struct
    this->header = new WAVHeader;

    file.read((char *)this->header, sizeof(WAVHeader));
    this->remainingDataSize = this->header->subchunk2Size / 2; // count remaining samples
}

bool ReadWAV::checkCorrect()
{
    if (string(header->chunkID, 4) != "RIFF" || string(header->format, 4) != "WAVE")
        throw runtime_error("The file is not a valid WAV format!\n");

    else if (!((header->audioFormat == 1) && (header->numChannels == 1) && (header->sampleRate == 44100) && (header->bitsPerSample == 16)))
        throw runtime_error("This program supports only PCM, mono audio, 16 bit, sampling rate 44100!\n");

    else
        return true;
}

// reads a certain amount of data and returns true if not the entire file has been read, and false otherwise
bool ReadWAV::getSamples(vector<int16_t> &samples)
{
    if (this->remainingDataSize > 0)
    {
        size_t bytesToRead = min(this->getUnitSize(), this->remainingDataSize);
        samples.resize(bytesToRead);
        file.read((char *)samples.data(), bytesToRead * sizeof(int16_t));
        this->remainingDataSize -= bytesToRead;
        return true;
    }
    else
    {
        return false;
    }
}

bool WriteWAV::openWAVFile(string outputFileName)
{
    // Open the WAV file that check that the path is correct
    this->outputFileName = outputFileName;
    this->file.open(this->outputFileName, ios::binary);

    if (!this->file.is_open())
        throw runtime_error("Failed to open the file. Please check the file name or path.\n");

    return this->file.is_open();
}

bool WriteWAV::closeWAVFile()
{
    this->file.close();
    return !this->file.is_open();
}

void WriteWAV::writeHead()
{
    file.write((const char *)(this->header), sizeof(WAVHeader));
}

void WriteWAV::saveSamples(vector<int16_t> &samples)
{
    file.write((const char *)(samples.data()), samples.size() * sizeof(int16_t));
}

int main()
{
    ReadWAV reader;
    reader.openWAVFile("./exp_music/el_guitar_16.wav");
    reader.parseHead();

    cout << reader.checkCorrect()
         << endl;

    vector<int16_t> samples;
    samples.reserve(reader.getUnitSize());

    WriteWAV writer;
    writer.openWAVFile("./el_guitar_16_out.wav");
    writer.writeHead();

    while (reader.getSamples(samples))
    {
        writer.saveSamples(samples);
    }

    writer.closeWAVFile();
    reader.closeWAVFile();
    return 0;
}