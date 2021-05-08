


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 956213

  This file contains the implementation of the Measure class. Measure is a
  very simple class that needs to contain a few member variables for its name,
  codename, and a Standard Library container for data. The data you need to 
  store is values, organised by year. I'd recommend storing the values as 
  doubles.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

#include "measure.h"

/*
  TODO: Measure::Measure(codename, label);

  Construct a single Measure, that has values across many years.

  All StatsWales JSON files have a codename for measures. You should convert 
  all codenames to lowercase.

  @param codename
    The codename for the measure

  @param label
    Human-readable (i.e. nice/explanatory) label for the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);
*/
Measure::Measure(std::string codename, std::string label) {
    std::transform(codename.begin(), codename.end(), codename.begin(), ::tolower);
    this->codename = codename;
    this->label = std::move(label);
}


/*
  TODO: Measure::getCodename()

  Retrieve the code for the Measure. This function should be callable from a 
  constant context and must promise to not modify the state of the instance or 
  throw an exception.

  @return
    The codename for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto codename2 = measure.getCodename();
*/
std::string & Measure::getCodename() {
    return this->codename;
}




/*
  TODO: Measure::getLabel()

  Retrieve the human-friendly label for the Measure. This function should be 
  callable from a constant context and must promise to not modify the state of 
  the instance and to not throw an exception.

  @return
    The human-friendly label for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto label = measure.getLabel();
*/
std::string & Measure::getLabel() {
    return this->label;
}



/*
  TODO: Measure::setLabel(label)

  Change the label for the Measure.

  @param label
    The new label for the Measure

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    ...
    measure.setLabel("New Population");
*/
void Measure::setLabel(std::string label) {
    this->label = std::move(label);
}


/*
  TODO: Measure::getValue(key)

  Retrieve a Measure's value for a given year.

  @param key
    The year to find the value for

  @return
    The value stored for the given year

  @throws
    std::out_of_range if year does not exist in Measure with the message
    No value found for year <year>

  @return
    The value

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto value = measure.getValue(1999); // returns 12345678.9
*/
double Measure::getValue(const unsigned int key) {
    if (this->values.find(key) != this->values.end()) {
        return this->values.at(key);
    }
    throw std::out_of_range ("No value found for year " + std::to_string(key));
}


/*
  TODO: Measure::setValue(key, value)

  Add a particular year's value to the Measure object. If a value already
  exists for the year, replace it.

  @param key
    The year to insert a value at

  @param value
    The value for the given year

  @return
    void

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
*/
void Measure::setValue(const unsigned int key, const double value) {
    if (this->values.find(key) != this->values.end()) {
        //exists
        this->values.at(key) = value;
    } else {
        //doesn't exist
        this->values.insert(std::pair<unsigned int, double>(key, value));
    }
}


/*
  TODO: Measure::size()

  Retrieve the number of years data we have for this measure. This function
  should be callable from a constant context and must promise to not change
  the state of the instance or throw an exception.

  @return
    The size of the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    auto size = measure.size(); // returns 1
*/
unsigned int Measure::size() {
    return this->values.size();
}


/*
  TODO: Measure::getDifference()

  Calculate the difference between the first and last year imported. This
  function should be callable from a constant context and must promise to not
  change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year, or 0 if it
    cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(2001, 12345679.9);
    auto diff = measure.getDifference(); // returns 1.0
*/
double Measure::getDifference() {
    return getValues().rbegin()->second - getValues().begin()->second;
}


/*
  TODO: Measure::getDifferenceAsPercentage()

  Calculate the difference between the first and last year imported as a 
  percentage. This function should be callable from a constant context and
  must promise to not change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year as a decminal
    value, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1990, 12345678.9);
    measure.setValue(2010, 12345679.9);
    auto diff = measure.getDifferenceAsPercentage();
*/
double Measure::getDifferenceAsPercentage() {
    double maxVal = getValues().rbegin()->second;
    double minVal = getValues().begin()->second;
    return (maxVal-minVal)/minVal*100;
}


/*
  TODO: Measure::getAverage()

  Calculate the average/mean value for all the values. This function should be
  callable from a constant context and must promise to not change the state of 
  the instance or throw an exception.

  @return
    The average value for all the years, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(2001, 12345679.9);
    auto diff = measure.getAverage(); // returns 12345678.4
*/

double Measure::getAverage() {
    double total = 0;
    for (auto & value : getValues())
    {
        total += value.second;
    }
    if (total != 0) {
        return total / getValues().size();
    }
    return 0.0;
}



/*
  TODO: operator<<(os, measure)

  Overload the << operator to print all of the Measure's imported data.

  We align the year and value outputs by padding the outputs with spaces,
  i.e. the year and values should be right-aligned to each other so they
  can be read as a table of numerical values.

  Years should be printed in chronological order. Three additional columns
  should be included at the end of the output, correspodning to the average
  value across the years, the difference between the first and last year,
  and the percentage difference between the first and last year.

  If there is no data in this measure, print the name and code, and 
  on the next line print: <no data>

  See the coursework specification for more information.

  @param os
    The output stream to write to

  @param measure
    The Measure to write to the output stream

  @return
    Reference to the output stream

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    std::cout << measure << std::end;
*/

std::ostream &operator<<(std::ostream &os, Measure &measure) {
    //label and codename
    os << measure.getLabel() << " (" << measure.getCodename() << ")" << std::endl;

    //get max length of a measure value
    //This ensures that if the number of decimal places for the values changes at a
    //later date the column headers will still align
    int maxLength = std::to_string(measure.getValues().begin()->second).size();
    for (auto elem : measure.getValues()) {
        int curLength = std::to_string(elem.second).size();
        if (curLength > maxLength) {
            maxLength = curLength;
        }
    }

    //column names
    for (auto code : measure.getValues()) {
        os << measure.alignValue(code.first, maxLength);
    }
    os << measure.alignValue("Average", maxLength)
    << measure.alignValue("Diff.", maxLength)
    << measure.alignValue("% Diff.", maxLength) << std::endl;

    //values
    if (measure.getValues().empty()) {
        return os << "<no data>" << std::endl;
    }
    for (auto value : measure.getValues()) {
        os << std::fixed << std::setprecision(6)<< value.second << ' ';
    }
    return os << measure.getAverage() << ' '
    << measure.getDifference() << ' '
    << measure.getDifferenceAsPercentage() << std::endl;

}

/**
 * This will return a padded and aligned value for the ostream.
 *
 * @param t - the value to align & pad
 * @param width - The maximum length of a column
 * @return - std::string aligned right and padded
 */
template<typename T>
std::string Measure::alignValue(T t, const int &width) {
    std::stringstream stream;
    stream << std::right << std::setw(width) << std::setfill(' ') << t;
    return stream.str();
}



/*
  TODO: operator==(lhs, rhs)

  Overload the == operator for two Measure objects. Two Measure objects
  are only equal when their codename, label and data are all equal.

  @param lhs
    A Measure object

  @param rhs
    A second Measure object

  @return
    true if both Measure objects have the same codename, label and data; false
    otherwise
*/

bool operator==(const Measure& lhs, const Measure& rhs) {
    return lhs.label == rhs.label &&
           lhs.codename == rhs.codename &&
           lhs.values == rhs.values;
}

/**
 * This function returns the entire map of values owned by a measure object
 *
 * @return
 *  A std::map object containing all the values with their corresponding year
 */
std::map<unsigned int, double> Measure::getValues() {
    return this->values;
}



