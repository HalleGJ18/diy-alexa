#include "Speaker.h"
#include "I2SOutput.h"
#include "WAVFileReader.h"
#include <string>

using namespace std;

Speaker::Speaker(I2SOutput *i2s_output)
{
    m_i2s_output = i2s_output;
    m_ok = new WAVFileReader("/ok.wav");
    m_ready_ping = new WAVFileReader("/ready_ping.wav");
    m_cantdo = new WAVFileReader("/cantdo.wav");
    m_life = new WAVFileReader("/life.wav");
    m_jokes[0] = new WAVFileReader("/joke0.wav");

    m_sunny = new WAVFileReader("/sunny.wav");
    m_partly_cloudy = new WAVFileReader("/partly_cloudy.wav");
    m_overcast = new WAVFileReader("/overcast.wav");
    m_cloudy = new WAVFileReader("/cloudy.wav");
    m_clear = new WAVFileReader("/clear.wav");
}


Speaker::~Speaker()
{
    delete m_ok;
    delete m_ready_ping;
    delete m_cantdo;
    delete m_life;
    delete m_jokes[0];

    delete m_sunny;
    delete m_overcast;
    delete m_cloudy;
    delete m_clear;
    delete m_partly_cloudy;

}

void Speaker::playOK()
{
    m_ok->reset();
    m_i2s_output->setSampleGenerator(m_ok);
}

void Speaker::playReady()
{
    m_ready_ping->reset();
    m_i2s_output->setSampleGenerator(m_ready_ping);
}

void Speaker::playCantDo()
{
    m_cantdo->reset();
    m_i2s_output->setSampleGenerator(m_cantdo);
}

void Speaker::playRandomJoke()
{
    // int joke = random(5);
    m_i2s_output->setSampleGenerator(m_jokes[0]);
}

void Speaker::playLife()
{
    m_life->reset();
    m_i2s_output->setSampleGenerator(m_life);
}

void Speaker::playWeather(string condition)
{

    // Serial.printf(condition.c_str());
    // Serial.printf("\n");

    if (condition.find("Sunny")!= string::npos) {
        // sunny
        m_sunny->reset();
        m_i2s_output->setSampleGenerator(m_sunny);

    } else if (condition.find("Partly cloudy")!= string::npos) {
        // partly cloudy
        m_partly_cloudy->reset();
        m_i2s_output->setSampleGenerator(m_partly_cloudy);
        // m_cloudy->reset();
        // m_i2s_output->setSampleGenerator(m_cloudy);

    } else if (condition.find("Cloudy")!= string::npos){
        // cloudy
        m_cloudy->reset();
        m_i2s_output->setSampleGenerator(m_cloudy);

    } else if (condition.find("Clear")!= string::npos) {
        // clear
        m_clear->reset();
        m_i2s_output->setSampleGenerator(m_clear);

    } else if (condition.find("Overcast")!= string::npos) {
        // overcast
        m_overcast->reset();
        m_i2s_output->setSampleGenerator(m_overcast);

    }
    
}