#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

using namespace std;

class MetaData
{
public:
    virtual bool openWAVFile(string) = 0;
    MetaData() = default;
    virtual ~MetaData() = default;

protected:
    // WAV file header structure
    struct WAVHeader
    {
        char chunkID[4];        // "RIFF"
        uint32_t chunkSize;     // File size minus the first 8 bytes
        char format[4];         // "WAVE"
        char subchunk1ID[4];    // "fmt"
        uint32_t subchunk1Size; // Size of the "fmt" subchunk (typically 16 for PCM)
        uint16_t audioFormat;   //* Audio format code (1 = PCM)
        uint16_t numChannels;   //* Number of audio channels (1 = mono, 2 = stereo, etc.)
        uint32_t sampleRate;    //* Sampling rate in Hz (e.g., 44100 for CD-quality audio)
        uint32_t byteRate;      // Bytes per second (sampleRate * numChannels * bitsPerSample / 8)
        uint16_t blockAlign;    // Bytes per audio frame (numChannels * bitsPerSample / 8)
        uint16_t bitsPerSample; //* Bits per sample (e.g., 8, 16, 24, or 32)
        char subchunk2ID[4];    // "data" - Data subchunk identifier
        uint32_t subchunk2Size; // Size of the audio data in bytes
    };
    static struct WAVHeader *header;
};

class ReadWAV : public MetaData
{
private:
    ifstream file;
    string inputFileName;
    const int sizeOfUnit = 524288; // cout samples
    int remainingDataSize;

public:
    ReadWAV() = default;
    ~ReadWAV() = default;
    bool checkCorrect();
    void parseHead();
    // reads a certain amount of data and returns true if not the entire file has been read, and false otherwise
    bool getSamples(vector<int16_t> &);
    bool openWAVFile(string) override;
    bool closeWAVFile();
    int getUnitSize();
};

class WriteWAV : public MetaData
{
private:
    ofstream file;
    string outputFileName;

public:
    WriteWAV() = default;
    ~WriteWAV() = default;
    bool openWAVFile(string) override;
    bool closeWAVFile();
    void writeHead();
    void saveSamples(vector<int16_t> &);
};

class Converter
{
private:
public:
    Converter();
    virtual ~Converter() = default;
    virtual void convert() = 0;
};

class Mute : public Converter
{
public:
    Mute();
    void convert() override;
};

class Mix : public Converter
{
public:
    Mix();
    void convert() override;
};

class Pass : public Converter
{
public:
    Pass();
    void convert() override;
};

class Creater
{
public:
    Creater();
    virtual ~Creater() = default;
    virtual Converter *creatConverter() = 0;
};

class MuteCreater : public Creater
{
public:
    Converter *creatConverter() override;
};

class MixCreater : public Creater
{
public:
    Converter *creatConverter() override;
};

class PassCreater : public Creater
{
public:
    Converter *creatConverter() override;
};
