#include <cstdlib>
#include <fstream>
#include <vector>

using namespace std;

struct FormatChunk {

    uint16_t format;
    uint16_t channels;
    uint32_t frequency;
    uint32_t byteRate;
    uint16_t blockSize;
    uint16_t sampleBits;

};

int make_noisy() {

    uint32_t frequency = 44100;
    vector<uint8_t> data(frequency * 4);
    for(size_t i = 0; i < data.size(); ++i) data[i] = i % 64 < 32 ? 0 : 255;

    const char* riffID = "RIFF";
    const char* waveID = "WAVE";
    const char* fmtID = "fmt ";
    FormatChunk fmtData = {
        0x0001,
        1,
        frequency,
        frequency,
        1,
        8,
    };
    uint32_t fmtSize = sizeof(fmtData);
    const char* dataID = "data";
    uint32_t dataSize = data.size();
    uint32_t riffSize = 4 + 4 + 4 + fmtSize + 4 + 4 + dataSize;

    vector<pair<const void*, size_t>> v = {
        {riffID, 4}, {&riffSize, 4},
            {waveID, 4},
                {fmtID, 4}, {&fmtSize, 4}, {&fmtData, sizeof(fmtData)},
                {dataID, 4}, {&dataSize, 4}, {data.data(), data.size()},
    };
    ofstream out("output.wav", ios::binary);
    for(auto&& x : v) out.write(static_cast<const char*>(x.first), x.second);

    return 0;

}
