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

    EXPECT_THROW(cmdParser.getInWAVFileName(3), std::out_of_range);

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

    EXPECT_THROW(ParseCmdLineArg cmdParser(argc, argv), std::invalid_argument);

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

    EXPECT_THROW(ParseCmdLineArg cmdParser(argc, argv), std::invalid_argument);

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

    EXPECT_THROW(ParseCmdLineArg cmdParser(argc, argv), std::runtime_error);

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

    EXPECT_THROW(ParseCmdLineArg cmdParser(argc, argv), std::invalid_argument);

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

    EXPECT_THROW(confPrs.parsing(cmdParser), std::invalid_argument);

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    delete[] argv;
}
