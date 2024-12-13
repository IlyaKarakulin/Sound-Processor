#include <gtest/gtest.h>
#include "./lib/sound_pr.hpp"

TEST(CmdParser, cmdParserCorrectInput)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./config.txt",
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    ParseCmdLineArg cmdParser(argc, argv);

    EXPECT_EQ(cmdParser.getConfFileName(), "./config.txt");
    EXPECT_EQ(cmdParser.getInWAVFileName(1), "./in1.wav");
    EXPECT_EQ(cmdParser.getInWAVFileName(2), "./in2.wav");

    try
    {
        EXPECT_EQ(cmdParser.getInWAVFileName(3), "./in2.wav");
    }
    catch (const std::out_of_range &e)
    {
        // std::cerr << e.what();
    }

    EXPECT_EQ(cmdParser.getMainWAVFileName(), "./in.wav");
    EXPECT_EQ(cmdParser.getMode(), 1);
    EXPECT_EQ(cmdParser.getOutWAVFileName(), "./output.wav");

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(CmdParser, cmdParserCorrectInput2)
{
    int argc = 2;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-h",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    ParseCmdLineArg cmdParser(argc, argv);

    EXPECT_EQ(cmdParser.getMode(), 0);

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(CmdParser, cmdParserIncorrectInput)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./config.tx", //! .tx
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    try
    {
        ParseCmdLineArg cmdParser(argc, argv);
    }
    catch (const std::invalid_argument &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(CmdParser, cmdParserIncorrectInput2)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./.txt", //! no name
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    try
    {
        ParseCmdLineArg cmdParser(argc, argv);
    }
    catch (const std::invalid_argument &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(CmdParser, cmdParserIncorrectInput3)
{
    int argc = 5;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    }; //! wos not found conf file

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    try
    {
        ParseCmdLineArg cmdParser(argc, argv);
    }
    catch (const std::runtime_error &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(CmdParser, cmdParserIncorrectInput4)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./config.txt",
        "./output.wav",
        "./in.wv", //!
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    try
    {
        ParseCmdLineArg cmdParser(argc, argv);
    }
    catch (const std::invalid_argument &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(ConfigParser, CorrectCommand)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./config.txt",
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    ParseCmdLineArg cmdParser(argc, argv);
    ParseConfigFile confPrs("./config.txt");

    try
    {
        confPrs.parsing(cmdParser);
    }
    catch (const std::invalid_argument &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(ConfigParser, IncorrectCommand)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./config_incr.txt",
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    ParseCmdLineArg cmdParser(argc, argv);
    ParseConfigFile confPrs(cmdParser.getConfFileName());

    try
    {
        confPrs.parsing(cmdParser);
    }
    catch (const std::invalid_argument &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}

TEST(ConfigParser, IncorrectCommand2)
{
    int argc = 7;
    char **argv = new char *[argc];
    const char *args[] = {
        "./build/sound_pr",
        "-c",
        "./config_incr2.txt",
        "./output.wav",
        "./in.wav",
        "./in1.wav",
        "./in2.wav",
    };

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[strlen(args[i]) + 1];
        strcpy(argv[i], args[i]);
    }

    ParseCmdLineArg cmdParser(argc, argv);
    ParseConfigFile confPrs(cmdParser.getConfFileName());

    try
    {
        confPrs.parsing(cmdParser);
    }
    catch (const std::invalid_argument &e)
    {
        // std::cerr << e.what();
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}