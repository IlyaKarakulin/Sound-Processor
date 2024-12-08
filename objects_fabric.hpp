#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <utility>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

class MetaData
{
public:
    MetaData() = default;
    virtual ~MetaData() = default;
    virtual bool openWAVFile(string) = 0;

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
    Converter() = default;
    virtual ~Converter() = default;
    virtual void convert(string) = 0;
};

class Mute : public Converter
{
private:
    u_int32_t left;
    u_int32_t right;

public:
    Mute(u_int32_t, u_int32_t);
    ~Mute() = default;
    void convert(string) override;
};

class Mix : public Converter
{
private:
    string nameSrcFile;
    u_int32_t start_with;

public:
    Mix(string, u_int32_t);
    ~Mix() = default;
    void convert(string) override;
};

class Reverberation : public Converter
{
private:
    u_int32_t left;
    u_int32_t right;
    double koeff;

public:
    Reverberation(u_int32_t, u_int32_t, double);
    ~Reverberation() = default;
    void convert(string) override;
};

class Creater
{
public:
    Creater() = default;
    virtual ~Creater() = default;
    // virtual Converter *creatConverter() = 0;
};

class MuteCreater : public Creater
{
private:
public:
    MuteCreater() = default;
    Converter *creatConverter(u_int32_t, u_int32_t);
};

class MixCreater : public Creater
{
private:
public:
    MixCreater() = default;
    Converter *creatConverter(u_int32_t, string);
};

class ReverberationCreater : public Creater
{
private:
public:
    ReverberationCreater() = default;
    Converter *creatConverter(u_int32_t, u_int32_t, double);
};

class ParseCmdLineArg
{
private:
    vector<string> args;
    string confFileName;

public:
    ParseCmdLineArg(int, char **);
    ~ParseCmdLineArg() = default;
    string getConfFileName();
    string getInWAVFileName(int);
    string getOutWAVFileName();
    string getMainWAVFileName();
};

class ParseConfigFile
{
private:
    string confFileName;

public:
    ParseConfigFile(string);
    ~ParseConfigFile() = default;
    queue<Converter *> parsing(ParseCmdLineArg &parseArgs);
};
