#include <SFML/System.hpp>

namespace ARO {
    class Timer {
    public:
        ~Timer() {
            delete timeElapsed;
        };
        Timer(sf::Time t)
        void setTime(sf::Time t) {
            time = t;
        }
        void start() {
            if (!clockCalled)
                timeElapsed = new sf::Clock;
            clockCalled = true;
        }
        sf::Time getElapsedTime() {
            if (!clockCalled)
                return 0;
            return timeElapsed.getElapsedTime();
        }
        sf::Time getTimeLeft() {
            if (!clockCalled)
                return 0;
            return time-timeElapsed.getElapsedTime();
        }
        bool isFinished() {
            if (timeElapsed.getElapsedTime()>time) {
                return true;
            }
            return false;
        }
    protected:
        sf::Time time;
        sf::Clock* timeElapsed;
        bool clockCalled = false;
    };
}
