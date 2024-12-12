#include <iostream>
#include "./lib/sound_pr.hpp"
using namespace std;

int main(int argc, char **argv)
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

    return 0;
}