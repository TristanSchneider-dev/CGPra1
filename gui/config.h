#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief The global configuration
 *
 * This class contains global configuration parameters
 * that are used all over the program.
 *
 * Hint: Add as many parameters as you need.
 */
class Config{
public:

    static float animationSpeed;     /**< the number of steps per ms */

    static bool localRotation;       /**< rotate the planets locally */
};

#endif // CONFIG_H

