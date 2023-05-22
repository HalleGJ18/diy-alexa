#ifndef _speaker_h_
#define _speaker_h_

#include <string>

using namespace std;

class I2SOutput;
class WAVFileReader;

class Speaker
{
private:
    WAVFileReader *m_ok;
    WAVFileReader *m_cantdo;
    WAVFileReader *m_ready_ping;
    WAVFileReader *m_life;
    WAVFileReader *m_jokes[5];
    
    WAVFileReader *m_sunny;
    WAVFileReader *m_partly_cloudy;
    WAVFileReader *m_overcast;
    WAVFileReader *m_cloudy;
    WAVFileReader *m_clear;

    

    I2SOutput *m_i2s_output;

public:
    Speaker(I2SOutput *i2s_output);
    ~Speaker();
    void playOK();
    void playReady();
    void playCantDo();
    void playRandomJoke();
    void playLife();
    void playWeather(string condition);
};

#endif