#include "./sound_pr.hpp"

// Constructor for the Reverberation class, initializes the delay parameters
Reverberation::Reverberation(u_int32_t left, u_int32_t right, double koeff)
{
    this->left = left;
    this->right = right;
    this->koeff = koeff;
}

void Reverberation::convert(string inFileName, string outFileName, ReadWAV &reader, WriteWAV &writer)
{
    // Logs the reverberation operation details
    cout << "revb: " << this->left << " " << this->right << " " << this->koeff << endl;

    // Copies the input WAV file to the output path
    fs::copy(inFileName, outFileName, fs::copy_options::overwrite_existing);

    // Open the input WAV file
    reader.openWAVFile(inFileName);
    reader.parseHead();
    reader.checkCorrect();

    // Open the output WAV file for writing
    writer.openWAVFile(outFileName);

    vector<int16_t> samples;
    vector<int16_t> delayedSamples;

    // Calculate delay in samples based on the coefficient
    const uint32_t sampleRate = reader.getSampleRate();
    const size_t delaySamples = this->koeff * sampleRate;

    // Prepare buffers
    samples.reserve(reader.getUnitSize());
    delayedSamples.resize(delaySamples, 0);

    // Process the reverberation effect
    while (reader.getSamples(samples, this->left, this->right))
    {
        for (size_t i = 0; i < samples.size(); ++i)
        {
            int16_t original = samples[i];
            int16_t delayed = delayedSamples[i % delaySamples];
            int16_t newSample = static_cast<int16_t>(original + this->koeff * delayed);

            // Clamp the new sample to the valid range
            samples[i] = max(min(newSample, static_cast<int16_t>(INT16_MAX)), static_cast<int16_t>(INT16_MIN));

            // Update the delayed samples buffer
            delayedSamples[i % delaySamples] = samples[i];
        }

        writer.saveSamples(reader, samples, this->left);
    }

    // Close all WAV files
    reader.closeWAVFile();
    writer.closeWAVFile();
}

void Reverberation::help()
{
    cout << "\033[33m   The reverb\033[0m" << endl
         << "Adds an echo to the" << endl
         << "reverberation sound stream <n> <m> <koeff>" << endl
         << "n - m interval where the effect will be applied" << endl
         << "coefficient of delay [0, 1]" << endl
         << "example: reverberation 1 5 0.4" << endl
         << endl;
}

// Factory method for creating Reverberation converters
Converter *ReverberationCreater::creatConverter(u_int32_t left, u_int32_t rigth, double koeff)
{
    Reverberation *reverberation = new Reverberation(left, rigth, koeff);
    return reverberation;
}