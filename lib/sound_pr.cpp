#include "./sound_pr.hpp"
// Implementation of ReadWAV class methods

int ReadWAV::getUnitSize()
{
    return this->sizeOfUnit; // Returns the size of a data unit
}

bool ReadWAV::openWAVFile(string inputFileName)
{
    // Open the WAV file and verify the path is correct
    this->inputFileName = inputFileName;
    this->file.open(this->inputFileName, ios::binary);
    if (!file.is_open())
        throw runtime_error("Failed to open the file. Please check the file name or path.\n");

    return this->file.is_open();
}

bool ReadWAV::closeWAVFile()
{
    // Closes the WAV file
    this->file.close();
    return !this->file.is_open();
}

void ReadWAV::parseHead()
{
    // Reads the WAV file header and fills the struct
    this->header = new WAVHeader;
    file.read((char *)this->header, sizeof(WAVHeader));
}

bool ReadWAV::checkCorrect()
{
    // Validates that the file is a proper WAV format with specific properties
    if (string(header->chunkID, 4) != "RIFF" || string(header->format, 4) != "WAVE")
        throw runtime_error("The file is not a valid WAV format!\n");

    else if (!((header->audioFormat == 1) && (header->numChannels == 1) &&
               (header->sampleRate == 44100) && (header->bitsPerSample == 16)))
        throw runtime_error("This program supports only PCM, mono audio, 16 bit, sampling rate 44100!\n");

    else
        return true;
}

bool ReadWAV::getSamples(vector<int16_t> &samples, int sec_st, int sec_end)
{
    // Reads a portion of audio samples from the WAV file
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
    // Returns the sample rate of the WAV file
    return this->header->sampleRate;
}

WAVHeader *ReadWAV::getHeader()
{
    // Returns the WAV file header
    return this->header;
}

int ReadWAV::getSizeFile()
{
    header->subchunk2Size = 31753078;
    return header->subchunk2Size / (2 * header->sampleRate);
}

// Implementation of WriteWAV class methods
bool WriteWAV::openWAVFile(string outputFileName)
{
    // Opens the output WAV file
    this->outputFileName = outputFileName;
    this->file.open(outputFileName, ios::in | ios::out | ios::binary);

    if (!this->file.is_open())
        throw runtime_error("Failed to open the file. Please check the file name or path.\n");

    return this->file.is_open();
}

bool WriteWAV::closeWAVFile()
{
    // Closes the output WAV file
    this->file.close();
    return !this->file.is_open();
}

void WriteWAV::writeHead(ReadWAV &reader)
{
    // Writes the WAV header from the source file to the output file
    file.write((const char *)(reader.getHeader()), sizeof(WAVHeader));
}

void WriteWAV::saveSamples(ReadWAV &reader, vector<int16_t> &samples, int sec_st)
{
    // Writes the audio samples to the output file at the specified time offset
    u_int64_t offset = 2 * (u_int64_t)reader.getSampleRate() * (u_int64_t)sec_st + (u_int64_t)sizeof(WAVHeader);

    streampos currentPos = file.tellp();

    if (currentPos < offset)
        this->file.seekp(offset, ios::beg);

    this->file.write((const char *)(samples.data()), samples.size() * sizeof(int16_t));
}

// ParseCmdLineArg class constructor and methods

ParseCmdLineArg::ParseCmdLineArg(int argv, char **argc)
{
    // Parses command line arguments and extracts necessary file names
    size_t index = 0;
    this->args.assign(argc, argc + argv);

    auto it = find(this->args.begin(), this->args.end(), "-h");

    if (it != this->args.end())
    {
        this->mode = 0;
    }
    else
    {
        it = find(this->args.begin(), this->args.end(), "-c");

        if ((it != this->args.end()) && (++it != this->args.end()))
        {
            this->confFileName = *it;
            if (!(this->confFileName.ends_with(".txt") && this->confFileName.length() > 6))
                throw invalid_argument("Invalid file format!\n");
        }
        else
        {
            throw runtime_error("The configuration file was not found!\n");
        }

        this->mode = 1;

        for (int i = 3; i < args.size(); ++i)
        {
            if (!(args.at(i).ends_with(".wav") && (args.at(i).length() > 4)))
            {
                throw invalid_argument("Invalid WAV file format!\n");
            }
        }
    }
}

bool ParseCmdLineArg::getMode()
{
    return mode;
}

string ParseCmdLineArg::getConfFileName()
{
    // Returns the configuration file name
    return this->confFileName;
}

string ParseCmdLineArg::getMainWAVFileName()
{
    // Returns the main WAV file name from command line arguments
    return this->args.at(4);
}

// Rest of the code follows similar patterns with appropriate comments.

string ParseCmdLineArg::getInWAVFileName(int n)
{
    return this->args.at(4 + n);
}

string ParseCmdLineArg::getOutWAVFileName()
{
    // Retrieves the name of the output WAV file from the command line arguments
    return this->args.at(3);
}

// Constructor for the Mute class, initializes the left and right boundaries of the mute section
Mute::Mute(u_int32_t left, u_int32_t right)
{
    this->left = left;
    this->right = right;
}

void Mute::convert(string inFileName, string OutFileName, ReadWAV &reader, WriteWAV &writer)
{
    // Logs the mute operation with start and end points
    cout << "mute " << this->left << " " << this->right << endl;

    // Copies the input WAV file to the output path
    fs::copy(inFileName, OutFileName, fs::copy_options::overwrite_existing);

    // Opens the output file for writing
    writer.openWAVFile(OutFileName);

    // Calculate the number of seconds to mute
    int countSec = this->right - this->left;

    // Create a vector of silent samples (all zeroes)
    vector<int16_t> samples(reader.getUnitSize(), 0);

    // Write silent samples for the specified duration
    while (countSec)
    {
        writer.saveSamples(reader, samples, left);
        --countSec;
    }

    // Close the output WAV file
    writer.closeWAVFile();
}

void Mute::help()
{
    cout << "\033[33m   Mute converter\033[0m" << endl
         << "Allows you to turn off the sound from <n> to <m> seconds" << endl
         << "Example: mute 1 4" << endl
         << endl;
}

// Constructor for the Mix class, initializes the source file and starting offset
Mix::Mix(string nameSrcFile, u_int32_t start_with)
{
    this->nameSrcFile = nameSrcFile;
    this->start_with = start_with;
}

// Averages samples from two vectors: modifies `samples` in-place
void Mix::avg_samples(vector<int16_t> &samples, vector<int16_t> &scr_samples)
{
    auto it1 = samples.begin();
    auto it2 = scr_samples.begin();

    // Iterates over both vectors and calculates the average of each sample pair
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

    // Copies the input WAV file to the output path
    fs::copy(inFileName, OutFileName, fs::copy_options::overwrite_existing);

    // Open the input and source WAV files
    reader.openWAVFile(inFileName);
    reader.parseHead();
    reader.checkCorrect();

    ReadWAV src_reader;
    src_reader.openWAVFile(this->nameSrcFile);
    src_reader.parseHead();
    src_reader.checkCorrect();

    // Open the output WAV file for writing
    writer.openWAVFile(OutFileName);

    // Get file durations
    const int src_size = src_reader.getSizeFile();
    const int size = reader.getSizeFile();

    cout << src_size << ' ' << size << endl;

    vector<int16_t> src_samples;
    src_samples.reserve(reader.getSampleRate());

    vector<int16_t> samples;
    samples.reserve(reader.getSampleRate());

    // Mix samples from source and input files
    bool flag = true;
    while (src_reader.getSamples(src_samples, 0, src_size) && flag)
    {
        flag = reader.getSamples(samples, this->start_with, size);
        this->avg_samples(samples, src_samples);
        writer.saveSamples(reader, samples, this->start_with);
        cout << 1;
    }
    cout << endl;

    // Close all WAV files
    reader.closeWAVFile();
    src_reader.closeWAVFile();
    writer.closeWAVFile();
}

void Mix::help()
{
    cout << "\033[33m   Mix converter\033[0m" << endl
         << "Mixes 2 audio streams by finding the arithmetic mean of a pair of samples" << endl
         << "from the first and second streams, respectively" << endl
         << "mix$<n> <s>" << endl
         << "n is the sequence number of the file, from the command line parameters," << endl
         << "which will be attached to the main file, s is the second from which to insert the file" << endl
         << "Example: mix $1 4" << endl
         << endl;
}

// Factory method for creating Mute converters
Converter *MuteCreater::creatConverter(u_int32_t left, u_int32_t rigth)
{
    Mute *mute = new Mute(left, rigth);
    return mute;
}

// Factory method for creating Mix converters
Converter *MixCreater::creatConverter(u_int32_t start, string nameSrcFile)
{
    Mix *mix = new Mix(nameSrcFile, start);
    return mix;
}

// Constructor for parsing configuration file paths
ParseConfigFile::ParseConfigFile(string name)
{
    this->confFileName = name;
}

// Parses the configuration file and builds a queue of converters
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

    // Read and parse each command in the config file
    while (fin >> str)
    {
        if (str == "mute")
        {
            // Add mute operation to the queue
            fin >> left >> rigth;
            if ((left < 0) || (left >= rigth))
            {
                throw invalid_argument("Invalid parameters!\n");
            }
            Mute *mute = (Mute *)muteCreater.creatConverter(left, rigth);
            conv_queue.push(mute);
        }
        else if (str == "mix")
        {
            // Add mix operation to the queue
            int with = 0;
            fin >> tmp >> with;
            if (with < 0)
            {
                throw invalid_argument("Invalid parameters!\n");
            }
            int num_file = 0;
            num_file = stoi(tmp.substr(1));
            Mix *mix = (Mix *)mixCreater.creatConverter(with, parseArgs.getInWAVFileName(num_file));
            conv_queue.push(mix);
        }
        else if (str == "reverberation")
        {
            // Add reverberation operation to the queue
            double k = 0.0;
            fin >> left >> rigth >> k;

            if ((left < 0) || (left >= rigth) || (k < 0.0) || (k > 1.0))
            {
                throw invalid_argument("Invalid parameters!\n");
            }

            Reverberation *revb = (Reverberation *)revbCreater.creatConverter(left, rigth, k);
            conv_queue.push(revb);
        }
        else
        {
            // Handle unknown commands
            // cerr << "Command was not found!\n";
        }
    }

    return conv_queue;
}

void Main::soundProcessing(int argc, char **argv)
{

    ReadWAV reader;
    WriteWAV writer;
    ParseCmdLineArg parserCmdLine(argc, argv);

    string confFileName = parserCmdLine.getConfFileName();

    ParseConfigFile parserConfFile(confFileName);
    queue<Converter *> convs = parserConfFile.parsing(parserCmdLine);

    reader.openWAVFile(parserCmdLine.getMainWAVFileName());
    reader.parseHead();
    reader.checkCorrect();
    reader.closeWAVFile();

    const string mainFileName = parserCmdLine.getMainWAVFileName();
    const string outFileName = parserCmdLine.getOutWAVFileName();

    fs::copy(mainFileName, "./tmp1.wav", fs::copy_options::overwrite_existing);
    pair<string, string> names{"./tmp1.wav", "./tmp2.wav"};

    if (fs::exists(outFileName))
        fs::remove(outFileName);

    while (!convs.empty())
    {
        Converter *conv = convs.front();
        convs.pop();
        conv->convert(names.first, names.second, reader, writer);
        swap(names.first, names.second);
    }

    fs::remove(names.second);
    rename(names.first.c_str(), outFileName.c_str());
}

void Main::helpPrint()
{
    MuteCreater muteCreater;
    MixCreater mixCreater;
    ReverberationCreater revbCreater;
    queue<Converter *> convs;

    Mute *mute = (Mute *)muteCreater.creatConverter(0, 1);
    convs.push(mute);
    Mix *mix = (Mix *)mixCreater.creatConverter(0, "tmp.wav");
    convs.push(mix);
    Reverberation *revb = (Reverberation *)revbCreater.creatConverter(0, 1, 0.5);
    convs.push(revb);

    while (!convs.empty())
    {
        Converter *conv = convs.front();
        convs.pop();
        conv->help();
    }
}

void Main::processing(int argc, char **argv)
{
    ParseCmdLineArg parserCmdLine(argc, argv);

    if (parserCmdLine.getMode())
    {
        this->soundProcessing(argc, argv);
    }
    else
    {
        this->helpPrint();
    }
}