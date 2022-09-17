/**
 * @file Location.h
 * @brief Header for class Location
 * @version 0.1
 * 
 */
#pragma once
#include <cstddef> // size_t

namespace pink {
    /**
     * @brief represents a single textual location
     * 
     * if we have a line of text within an array starting at 0:
     * (and we just started parsing the n'th line of text)
     * 
     *  0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
     *  --|---|---|---|---|---|---|---|
     *  s | y | m | b | o | l | ~ | ~ |
     * 
     * then the textual location of the term 'symbol' would be
     * firstLine   == n
     * firstColumn == 0
     * lastLine    == n
     * lastColumn  == 6
     *      
     */
    class Location {
    public:
        /**
         * @brief holds the number of the first line that this Location appears on.
         * 
         */
        size_t firstLine;

        /**
         * @brief holds the number of the first column that this Location appears on.
         * 
         */
        size_t firstColumn;

        /**
         * @brief holds the number of the last line that this Location appears on
         * 
         */
        size_t lastLine;
        
        /**
         * @brief holds the number of the last column that this Location appears on
         * 
         */
        size_t lastColumn;
        
        /**
         * @brief Construct a new Location
         * 
         * the default Location is (0, 0, 0, 0)
         */
        Location();

        /**
         * @brief Construct a new Location from another Location
         * 
         * @param other the other Location to copy
         */
        Location(const Location& other);

        /**
         * @brief Construct a new Location
         * 
         * @param fl the firstLine
         * @param fc the firstColumn 
         * @param ll the lastLine
         * @param lc the lastColumn
         */
        Location(size_t fl, size_t fc, size_t ll, size_t lc);

        /**
         * @brief Assigns this Location to the value of another Location
         * 
         * @param other 
         * @return Location& 
         */
        Location& operator=(const Location& other);
        
        /**
         * @brief compares two locations for equality
         * 
         * two locations are equal if and only if each member of each location is equal.
         * 
         * @param other the Location to compare against this Location
         * @return true iff this Location is equal to the other Location 
         * @return false iff this Location is not equal to the other Location
         */
        bool operator==(const Location& other);
    };

}
