# **Audio Processor**
*This is a simple learning project aimed at learning how to work with WAV files. The code is written in the OOP style with the "object factory" design template*
**Audio Processor** is a tool for processing WAV files, offering various audio effects such as **mute**, **mix**, and **reverberation**. This project showcases a flexible architecture using the **Factory Method** design pattern, allowing for easy addition of new audio effects. 

---

## **Features**
- **Mute:** Removes audio signal within a specified time range.
- **Mix:** Overlays one audio file onto another by averaging their amplitude values.
- **Reverberation:** Adds echo effects with a customizable delay coefficient.

---

## **Requirements**
- **Compiler:** C++20 or higher.

---

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
- ./output.wav