/**
 * @file trafik.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-01-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iomanip>
#include <queue>
#include <fstream>
#include <sstream>
/**
 * @brief
 *  Enum class för att hålla koll på vilken färg som lyser i trafikljuset.
 */
enum class TrafficLightColor
{
    GREEN,
    RED,
    YELLOW
};
/**
 * @brief
 *
 * std::atomic<TrafficLightColor> som håller koll på vilken färg som lyser i trafikljuset.
 * @return std::atomic<TrafficLightColor>
 */
std::atomic<TrafficLightColor> currentState(TrafficLightColor::RED);
std::queue<bool> pushbutton;
std::atomic<bool> exitprogram(false);
std::mutex mtx;
std::condition_variable cv;
void trafficLightController(int greenTime, int redTime, int yellowTime, int extendedRedTime);
void userInput();
void logState(const std::string &event);
/**
 * @brief
 *  Main funktionen som innehåller config för ljusens tider, 2 threads med ljusen på en, och userinput på den andra.
 * @return int
 *  Returnerar 0
 */
int main()
{

    int greenTime = 8;
    int redTime = 8;
    int yellowTime = 3;
    int extendedRedTime = 8;

    std::thread userInputThread(userInput);
    std::thread trafficLightControllerThread(trafficLightController, greenTime, redTime, yellowTime, extendedRedTime);

    userInputThread.join();
    trafficLightControllerThread.join();

    return 0;
}
/**
 * @brief
 *  User input som kollar om q trycks så avslutar programmet, om p trycks så gör det att fotgängaren kan gå över övergångstället.
 * @return void
 *
 */
void userInput()
{
    while (!exitprogram)
    {
        char input;
        std::cin >> input;
        if (input == 'q')
        {
            exitprogram = true;
            cv.notify_all();
        }
        else if (input == 'p')
        {
            {
                std::unique_lock<std::mutex> lock(mtx);
                pushbutton.push(true);
                logState("Pedestrian Button Pushed, wait for extended red signal.");
            }
            cv.notify_one();
        }
        else
        {
            std::cout << "Invalid input" << std::endl;
        }
    }
}

std::ofstream logFile("traffic_light_log.txt", std::ios_base::app);
std::mutex logMutex;

/**
 * @brief
 *  Funktion som printar ut system_clock till terminalen för att hålla koll, samt loggar detta till en textfile för att spara informationen.
 *
 * @param event eventet som skall loggas
 */
void logState(const std::string &event)
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << "[" << std::put_time(localtime(&now_c), "%T") << "] " << event;
    std::string logEntry = oss.str();

    std::cout << logEntry << std::endl;

    std::lock_guard<std::mutex> guard(logMutex);
    if (logFile.is_open())
    {
        logFile << logEntry << std::endl;
    }
}

/**
 * @brief Traffic Light Controller
 *Här är där all magi händer. Funktionen som flippar mellan färgerna, tar emot pushbutton kön och ändrar färgerna därefter.
 *Här har vi använt oss av logstate funktionen som vi skrev för varje output, samt condition_variable och mutex för att se till
 *Att det inte uppstår race condition mellan trådarna.
 *
 * @param greenTime tid för grönt ljus
 * @param redTime tid för rött ljus
 * @param yellowTime tid för gult ljus
 * @param extendedRedTime förlängd tid för röd ljus
 */
void trafficLightController(int greenTime, int redTime, int yellowTime, int extendedRedTime)
{
    while (!exitprogram)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (!pushbutton.empty())
        {
            pushbutton.pop();

            logState("Traffic Light: Extended Red light for pedestrian");
            cv.wait_for(lock, std::chrono::seconds(extendedRedTime));
            continue;
        }

        currentState = TrafficLightColor::YELLOW;
        logState("Traffic Light: Yellow");
        cv.wait_for(lock, std::chrono::seconds(yellowTime));

        if (!pushbutton.empty())
        {
            currentState = TrafficLightColor::GREEN;
            logState("Traffic Light: Green");
            cv.wait_for(lock, std::chrono::seconds(greenTime - 3));
        }
        else
        {
            currentState = TrafficLightColor::GREEN;
            logState("Traffic Light: Green");
            cv.wait_for(lock, std::chrono::seconds(greenTime));
        }

        currentState = TrafficLightColor::YELLOW;
        logState("Traffic Light: Yellow");
        cv.wait_for(lock, std::chrono::seconds(yellowTime));

        currentState = TrafficLightColor::RED;
        logState("Traffic Light: Red");
        cv.wait_for(lock, std::chrono::seconds(redTime));
    }
}
