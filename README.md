# **Audio Processor**
*This is a simple learning project aimed at learning how to work with WAV files.* \
**Audio Processor** is a tool for processing WAV files, offering various audio effects such as **mute**, **mix**, and **reverberation**. This project showcases a flexible architecture using the **Factory Method** design pattern, allowing for easy addition of new audio effects. 

## **Features**
- **Mute:** Removes audio signal within a specified time range.
- **Mix:** Overlays one audio file onto another by averaging their amplitude values.
- **Reverberation:** Adds echo effects with a customizable delay coefficient.

## **Requirements**
- **Compiler:** C++20 or higher.
- **WAV files:** 
    - Sample rate = 44100
    - mono sound
    - PCM 
    - sound depth = 16 bit

*To edit a WAV file, you can use the sox utility, for example, to view information about wav file use **soxi file.wav**, to convert to 16-bit sound depth, use **sox infile.wav -b 16 outfile.wav***


## **Installation and Execution**
1. **Clone the repository:**
   ```bash
   git clone https://github.com/IlyaKarakulin/Sound-Processor.git
   cd Sound-Processor
   cmake -B ./build -S ./
   ./build/sound_pr -c config.txt ./output.wav ./in.wav ./in1.wav ./in2.wav
   ```
2. **Using**

- config.txt - the configuration file contains commands, for example
    ```bash
    mute 1 2
    mix $1 3
    mix $2 7
    reverberation 5 10 0.5
    ```
- output.wav - the file where the result of the program will be saved
- in.wav - the input file to be edited
- in1.wav, in2.wav ... - the auxiliary files that the mix command will use, the main file will be merged with them

3. **Testing**
You can enable testing of command line argument parsers and configuration file
    ``bash
    cmake -DENABLE_TESTING=<ON/OFF> ..
    ```