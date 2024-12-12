#include "./sound_pr.hpp"

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
    //? this->remainingDataSize = this->header->subchunk2Size / 2; // count remaining samples
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
bool ReadWAV::getSamples(vector<int16_t> &samples, int sec_st, int sec_end)
{
    u_int64_t offset = 2 * (u_int64_t)header->sampleRate * (u_int64_t)sec_st + (u_int64_t)sizeof(WAVHeader);
    streampos currentPos = file.tellg();

    if (currentPos <= offset)
    {
        this->file.seekg(offset, ios::beg);
        this->remainingDataSize = (u_int64_t)(sec_end - sec_st) * (u_int64_t)header->sampleRate;
    }

    if (this->remainingDataSize > 0)
    {
        size_t bytesToRead = min((u_int64_t)this->getUnitSize(), this->remainingDataSize);
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

uint32_t ReadWAV::getSampleRate()
{
    return this->header->sampleRate;
}

WAVHeader *ReadWAV::getHeader()
{
    return this->header;
}

int ReadWAV::getSizeFile()
{
    return header->subchunk2Size / (2 * header->sampleRate);
}

bool WriteWAV::openWAVFile(string outputFileName)
{
    // Open the WAV file that check that the path is correct
    this->outputFileName = outputFileName;
    // this->file.open(this->outputFileName, ios::binary);
    this->file.open(outputFileName, ios::in | ios::out | ios::binary);

    if (!this->file.is_open())
        throw runtime_error("Failed to open the file. Please check the file name or path.\n");

    return this->file.is_open();
}

bool WriteWAV::closeWAVFile()
{
    this->file.close();
    return !this->file.is_open();
}

void WriteWAV::writeHead(ReadWAV &reader)
{
    file.write((const char *)(reader.getHeader()), sizeof(WAVHeader));
}

void WriteWAV::saveSamples(ReadWAV &reader, vector<int16_t> &samples, int sec_st)
{
    u_int64_t offset = 2 * (u_int64_t)reader.getSampleRate() * (u_int64_t)sec_st + (u_int64_t)sizeof(WAVHeader);

    streampos currentPos = file.tellp();

    if (currentPos < offset)
        this->file.seekp(offset, ios::beg);

    this->file.write((const char *)(samples.data()), samples.size() * sizeof(int16_t));
}

ParseCmdLineArg::ParseCmdLineArg(int argv, char **argc)
{
    size_t index = 0;
    this->args.assign(argc, argc + argv);

    auto it = find(this->args.begin(), this->args.end(), "-c");

    if ((it != this->args.end()) && (++it != this->args.end()))
    {
        this->confFileName = *it;
        if (!(this->confFileName.ends_with(".txt") && this->confFileName.length() > 4))
            throw runtime_error("Invalid file format!\n");
    }
    else
    {
        throw runtime_error("The configuration file was not found!\n");
    }
}

string ParseCmdLineArg::getConfFileName()
{
    return this->confFileName;
}

string ParseCmdLineArg::getMainWAVFileName()
{
    return this->args.at(4);
}

string ParseCmdLineArg::getInWAVFileName(int n)
{
    return this->args.at(4 + n);
}

string ParseCmdLineArg::getOutWAVFileName()
{
    return this->args.at(3);
}

Mute::Mute(u_int32_t left, u_int32_t right)
{
    this->left = left;
    this->right = right;
}

void Mute::convert(string inFileName, string OutFileName, ReadWAV &reader, WriteWAV &writer)
{
    cout << "mute " << this->left << " " << this->right << endl;

    fs::copy(inFileName, OutFileName, fs::copy_options::overwrite_existing);

    writer.openWAVFile(OutFileName);
    int countSec = this->right - this->left;
    vector<int16_t> samples(reader.getUnitSize(), 0);

    while (countSec)
    {
        writer.saveSamples(reader, samples, left);
        --countSec;
    }

    writer.closeWAVFile();
}

Mix::Mix(string nameSrcFile, u_int32_t start_with)
{
    this->nameSrcFile = nameSrcFile;
    this->start_with = start_with;
}

void Mix::avg_samples(vector<int16_t> &samples, vector<int16_t> &scr_samples)
{
    auto it1 = samples.begin();
    auto it2 = scr_samples.begin();

    while (it1 != samples.end() && it2 != scr_samples.end())
    {
        *it1 = (*it1 + *it2) / 2;
        ++it1;
        ++it2;
    }
}

void Mix::convert(string inFileName, string OutFileName, ReadWAV &reader, WriteWAV &writer)
{
    cout << "mix " << this->start_with << " " << this->nameSrcFile << endl;

    fs::copy(inFileName, OutFileName, fs::copy_options::overwrite_existing);

    reader.openWAVFile(inFileName);
    reader.parseHead();
    reader.checkCorrect();

    ReadWAV src_reader;
    src_reader.openWAVFile(this->nameSrcFile);
    src_reader.parseHead();
    src_reader.checkCorrect();

    writer.openWAVFile(OutFileName);

    const int src_size = src_reader.getSizeFile();
    const int size = reader.getSizeFile();

    vector<int16_t> src_samples;
    src_samples.reserve(reader.getSampleRate());

    vector<int16_t> samples;
    samples.reserve(reader.getSampleRate());

    bool flag = true;

    while (src_reader.getSamples(src_samples, 0, src_size) && flag)
    {
        flag = reader.getSamples(samples, this->start_with, size);
        this->avg_samples(samples, src_samples);
        writer.saveSamples(reader, samples, this->start_with);
    }

    reader.closeWAVFile();
    src_reader.closeWAVFile();
    writer.closeWAVFile();
}

Reverberation::Reverberation(u_int32_t left, u_int32_t right, double koeff)
{
    this->left = left;
    this->right = right;
    this->koeff = koeff;
}

void Reverberation::convert(string inFileName, string outFileName, ReadWAV &reader, WriteWAV &writer)
{
    cout << "revb: " << this->left << " " << this->right << " " << this->koeff << endl;

    // Копируем исходный файл во временный для сохранения результата
    fs::copy(inFileName, outFileName, fs::copy_options::overwrite_existing);

    reader.openWAVFile(inFileName);
    reader.parseHead();
    reader.checkCorrect();

    writer.openWAVFile(outFileName);

    vector<int16_t> samples;
    vector<int16_t> delayedSamples;

    const uint32_t sampleRate = reader.getSampleRate();
    const size_t delaySamples = this->koeff * sampleRate;

    samples.reserve(reader.getUnitSize());
    delayedSamples.resize(delaySamples, 0);

    bool hasMoreData = true;

    while (reader.getSamples(samples, this->left, this->right))
    {
        for (size_t i = 0; i < samples.size(); ++i)
        {
            int16_t original = samples[i];
            int16_t delayed = delayedSamples[i % delaySamples];
            int16_t newSample = static_cast<int16_t>(original + this->koeff * delayed);

            samples[i] = max(min(newSample, static_cast<int16_t>(INT16_MAX)), static_cast<int16_t>(INT16_MIN));

            delayedSamples[i % delaySamples] = samples[i];
        }

        writer.saveSamples(reader, samples, this->left);
    }

    reader.closeWAVFile();
    writer.closeWAVFile();
}

Converter *MuteCreater::creatConverter(u_int32_t left, u_int32_t rigth)
{
    Mute *mute = new Mute(left, rigth);
    return mute;
}

Converter *MixCreater::creatConverter(u_int32_t start, string nameSrcFile)
{
    Mix *mix = new Mix(nameSrcFile, start);
    return mix;
}

Converter *ReverberationCreater::creatConverter(u_int32_t left, u_int32_t rigth, double koeff)
{
    Reverberation *reverberation = new Reverberation(left, rigth, koeff);
    return reverberation;
}

ParseConfigFile::ParseConfigFile(string name)
{
    this->confFileName = name;
}

queue<Converter *> ParseConfigFile::parsing(ParseCmdLineArg &parseArgs)
{
    ifstream fin(this->confFileName);
    queue<Converter *> conv_queue;
    string str;
    string tmp;
    char pass = 0;
    u_int32_t left = 0, rigth = 0;

    MuteCreater muteCreater;
    MixCreater mixCreater;
    ReverberationCreater revbCreater;

    while (fin >> str)
    {
        if (str == "mute")
        {
            fin >> left >> rigth;

            Mute *mute = (Mute *)muteCreater.creatConverter(left, rigth);
            conv_queue.push(mute);
        }
        else if (str == "mix")
        {
            int with = 0;
            fin >> tmp >> with;
            int num_file = 0;
            num_file = stoi(tmp.substr(1));

            Mix *mix = (Mix *)mixCreater.creatConverter(with, parseArgs.getInWAVFileName(num_file));
            conv_queue.push(mix);
        }
        else if (str == "reverberation")
        {
            double k = 0.0;
            fin >> left >> rigth >> k;

            Reverberation *revb = (Reverberation *)revbCreater.creatConverter(left, rigth, k);
            conv_queue.push(revb);
        }
        else
        {
            cerr << "Comand was not found!\n";
        }
    }

    return conv_queue;
}